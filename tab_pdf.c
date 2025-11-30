#include <gtk/gtk.h>
#include <poppler.h>
#include <unistd.h>
#include "tabs.h"

#define MMD "/home/darren/mermaid/mindmap.mmd"
//#define PDF "/home/darren/mermaid/mindmap.pdf"

static gdouble w = 0;
static gdouble h = 0;
static PopplerPage *page = NULL;
static GMutex mux = {};
static GtkWidget *area = NULL;

static gpointer load(gpointer do_draw) {
	g_debug("L0");

	// already locked by caller
	g_assert_true(!g_mutex_trylock(&mux));

	static PopplerDocument *document = NULL;
	static GBytes *bytes = NULL;

	// cleanup
	if (page) {
		g_assert_true(page && document && bytes);
		g_object_unref(g_steal_pointer(&page));
		g_object_unref(g_steal_pointer(&document));
		g_bytes_unref(g_steal_pointer(&bytes));
		//page = NULL;
		//document = NULL;
		//bytes = NULL;
	}

	// run
	auto subprocess = g_subprocess_new(
		G_SUBPROCESS_FLAGS_STDOUT_PIPE,
		NULL,
		"/usr/bin/mmdc",
		"-i", MMD,
		"-e", "pdf",
		"-f",
		"-o", "-",
		NULL
	);
	g_subprocess_communicate(subprocess, NULL, NULL, &bytes, NULL, NULL);
	g_subprocess_wait(subprocess, NULL, NULL);
	document = poppler_document_new_from_bytes(bytes, NULL, NULL);
	page = poppler_document_get_page(document, 0);
	g_assert_true(page);
	poppler_page_get_size(page, &w, &h);

	// callee unlocks
	g_mutex_unlock(&mux);

	if((intptr_t)do_draw) {
		g_debug("LD");
		gtk_widget_queue_draw(area);
	}

	g_debug("LZ");
	return NULL;

}

static void draw(GtkDrawingArea*, cairo_t *cr, int w0, int h0, gpointer) {
	g_debug("D0");
	g_mutex_lock(&mux);
	g_assert_true(page);
	g_debug("D1");
	gdouble m = 0;
	gdouble sx = (gdouble)w0/w;
	gdouble sy = (gdouble)h0/h;
	if (sx > sy) {
		m = w0 - (w * sy);
		cairo_translate(cr, m/2, 0);
		cairo_scale(cr, sy, sy);
	} else {
		m = h0 - (h * sx);
		cairo_translate(cr, 0, m/2);
		cairo_scale(cr, sx, sx);
	}
	poppler_page_render(page, cr);
	g_mutex_unlock(&mux);
	g_debug("DZ");
}

static void s_pressed(GtkGestureClick*, gint, gdouble, gdouble, gpointer) {
	g_debug("P0");
	if(!g_mutex_trylock(&mux)) {
		g_debug("P_");
		return;
	}
	g_thread_new(NULL, load, (gpointer)1);
	g_debug("PZ");
}

GtkWidget *tab_pdf() {

	// init
	area = gtk_drawing_area_new();
	auto click = gtk_gesture_click_new();

	// pen

	// trigger draw - file change
	// refer to mmwait() in /home/darren/kountdown/src/countdown.c

	// initial draw
	//gtk_widget_queue_draw(area);
	//s_pressed(NULL, 0, 0, 0, area);

	g_assert_true(g_mutex_trylock(&mux));
	load((gpointer)0);

  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw, NULL, NULL);

	g_signal_connect(click, "pressed", G_CALLBACK(s_pressed), area);
	gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(click));

	return area;

}

// info
//gchar *tooltip = info_2();
//GtkWidget *label = gtk_label_new(G_STRLOC);
//gtk_widget_set_tooltip_text(label, tooltip);

//static gchar *info_gslist() {
//	GSList *l = NULL;
//	g_assert_false(poppler_document_get_author(document));
//	g_assert_false(poppler_document_get_metadata(document));
//	l = g_slist_prepend(l, poppler_document_get_creator(document));
//	l = g_slist_prepend(l, poppler_document_get_producer(document));
//	char *tooltip = NULL;
//	char *s = NULL;
//	void func(gpointer data, gpointer userdata) {
//		G_DEBUG_HERE();
//		g_assert_false(userdata);
//		s = tooltip;
//		//g_strdup_printf
//		tooltip = g_strconcat(s?s:"", "\n\n", (char*)data, NULL);
//		g_free(g_steal_pointer(&s));
//		g_assert_false(s);
//	}
//	g_slist_foreach(l, func, NULL);
//	G_DEBUG_HERE();
//	s = tooltip;
//	tooltip = g_strconcat(s, "\n\n", NULL);
//	g_free(g_steal_pointer(&s));
//	G_DEBUG_HERE();
//	g_slist_free_full(g_steal_pointer(&l), &g_free);
//	return tooltip;
//}

//static gchar *info_2() {
//	char *tooltip = NULL;
//	void f(char *s) {
//		g_assert_true(s);
//		char *new = g_strconcat(tooltip?tooltip:"", "\n\n", s, NULL);
//		g_free(g_steal_pointer(&s));
//		g_free(g_steal_pointer(&tooltip));
//		tooltip = new;
//	}
//	g_assert_false(poppler_document_get_author(document));
//	f(poppler_document_get_creator(document));
//	g_assert_false(poppler_document_get_keywords(document));
//	g_assert_false(poppler_document_get_metadata(document));
//	g_assert_false(poppler_document_get_pdf_subtype_string(document));
//	f(poppler_document_get_pdf_version_string(document));
//	f(poppler_document_get_producer(document));
//	g_assert_false(poppler_document_get_subject(document));
//	f(poppler_document_get_title(document));
//	f(strdup(""));
//	return tooltip;
//}
