#include <gtk/gtk.h>
#include <poppler.h>
#include <unistd.h>
#include "tabs.h"

#define MMD "/home/darren/mermaid/mindmap.mmd"
//#define PDF "/home/darren/mermaid/mindmap.pdf"

static void f_draw(GtkDrawingArea*, cairo_t *cr, int w0, int h0, gpointer) {

	// def
	GSubprocess *subprocess = NULL;
	GBytes *buf = NULL;
	PopplerDocument *document = NULL;
	PopplerPage *page = NULL;
	double w = 0;
	double h = 0;
	double sx = 0;
	double sy = 0;
	double m = 0;

	// exec
	subprocess = g_subprocess_new(
		G_SUBPROCESS_FLAGS_STDOUT_PIPE,
		NULL,
		"/usr/bin/mmdc",
		"-i", MMD,
		"-e", "pdf",
		"-f",
		"-o", "-",
		NULL
	);
	g_subprocess_communicate(subprocess, NULL, NULL, &buf, NULL, NULL);
	g_subprocess_wait(subprocess, NULL, NULL);

	// load
	document = poppler_document_new_from_bytes(buf, NULL, NULL);
	page = poppler_document_get_page(document, 0);

	// print
	cairo_save(cr);
	poppler_page_get_size(page, &w, &h);
	sx = (double)w0/w;
	sy = (double)h0/h;
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

	// cleanup
	cairo_restore(cr);
	g_object_unref(document);
	g_bytes_unref(buf);
	g_object_unref(subprocess);

}

GtkWidget *tab_pdf() {

	// init
	GtkWidget *area = gtk_drawing_area_new();
	GtkGesture *click = gtk_gesture_click_new();
	void s_pressed(GtkGestureClick*, gint, gdouble, gdouble, gpointer x) { gtk_widget_queue_draw(GTK_WIDGET(x)); }

	// pen
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), f_draw, NULL, NULL);

	// trigger draw - click
	g_signal_connect(click, "pressed", G_CALLBACK(s_pressed), area);
	gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(click));

	// trigger draw - file change
	// refer to mmwait() in /home/darren/kountdown/src/countdown.c

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
