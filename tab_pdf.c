#include <gtk/gtk.h>
#include <poppler.h>
#include "tabs.h"

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

static void f_draw(GtkDrawingArea*, cairo_t *cr, int w0, int h0, gpointer) {

	//G_DEBUG_HERE();

	// no parallel
	static GMutex mutex = {};
	if(!g_mutex_trylock(&mutex)) {
		g_error("gtk_widget_queue_draw() cutting in line!!!");
		g_assert_true(0);
		return;
	}

	//G_DEBUG_HERE();

	// compile
	//mmdc -i mindmap.mmd -f -o mindmap.pdf

	// load document
	gchar *uri = g_filename_to_uri("/home/darren/mermaid/mindmap.pdf", NULL, NULL);
	g_assert_true(uri);
	PopplerDocument *document = poppler_document_new_from_file(uri, NULL, NULL); // unref
	g_assert_true(document);
	g_free(g_steal_pointer(&uri));

	// info
	//gchar *tooltip = info_2();
	//GtkWidget *label = gtk_label_new(G_STRLOC);
	//gtk_widget_set_tooltip_text(label, tooltip);

	// page
	g_assert_true(1 == poppler_document_get_n_pages(document));
	PopplerPage *page = poppler_document_get_page(document, 0); // unref
	g_assert_true(page);

	// geometry
	double w = 0;
	double h = 0;
	poppler_page_get_size(page, &w, &h);
	double sx = (double)w0/w;
	double sy = (double)h0/h;
	if (sx > sy) {
		double m = w0 - (w * sy);
		cairo_translate(cr, m/2, 0);
		cairo_scale(cr, sy, sy);
	} else {
		double m = h0 - (h * sx);
		cairo_translate(cr, 0, m/2);
		cairo_scale(cr, sx, sx);
	}

	// render
	poppler_page_render(page, cr);

	// cleanup
	//g_object_unref(page); // segmentation fault
	g_object_unref(document);
	g_mutex_unlock(&mutex);

}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void s_pressed(GtkGestureClick*, gint n_press, gdouble x, gdouble y, gpointer user_data) {
#pragma GCC diagnostic pop
	g_assert_true(user_data);
	//g_debug("(%lf, %lf) %d", x, y, n_press);
	gtk_widget_queue_draw(GTK_WIDGET(user_data));
}

GtkWidget *tab_pdf() {

	// pen
	GtkWidget *area = gtk_drawing_area_new();
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), f_draw, NULL, NULL);

	// trigger draw - click
	GtkGesture *click = gtk_gesture_click_new();
	g_signal_connect(click, "pressed", G_CALLBACK(s_pressed), area);
	gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(click));

	// trigger draw - file change
	// refer to mmwait() in /home/darren/kountdown/src/countdown.c

	return area;

}

