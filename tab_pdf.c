#include <gtk/gtk.h>
#include <poppler.h>
#include "tabs.h"

#define FILE "/home/darren/mermaid/mindmap.pdf"

static PopplerDocument *document = NULL;
static PopplerPage *page = NULL;

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

static gchar *info_2() {
	char *tooltip = NULL;
	void f(char *s) {
		g_assert_true(s);
		char *new = g_strconcat(tooltip?tooltip:"", "\n\n", s, NULL);
		g_free(g_steal_pointer(&s));
		g_free(g_steal_pointer(&tooltip));
		tooltip = new;
	}
	g_assert_false(poppler_document_get_author(document));
	f(poppler_document_get_creator(document));
	g_assert_false(poppler_document_get_keywords(document));
	g_assert_false(poppler_document_get_metadata(document));
	g_assert_false(poppler_document_get_pdf_subtype_string(document));
	f(poppler_document_get_pdf_version_string(document));
	f(poppler_document_get_producer(document));
	g_assert_false(poppler_document_get_subject(document));
	f(poppler_document_get_title(document));
	f(strdup(""));
	return tooltip;
}

static void c_draw(GtkDrawingArea*, cairo_t *cr, int w0, int h0, gpointer) {
	double w = 0;
	double h = 0;
	poppler_page_get_size(page, &w, &h);
	double s = MIN((double)w0/w, (double)h0/h);
	cairo_scale(cr, s, s);
	//cairo_translate(cr, 100, 0);
	poppler_page_render(page, cr);
}

GtkWidget *tab_pdf() {

	// PapersView fail
  //PpsDocument *document = pps_document_factory_get_document(g_filename_to_uri(FILE, NULL, NULL), NULL);
	//g_debug("%p", document);
	//g_assert_true(document);
	//PpsDocumentModel *model = pps_document_model_new_with_document(document);
	//g_assert_true(model);
	//PpsView *view = pps_view_new();
	//g_assert_true(view);
	//pps_view_set_model(view, model);
	//return GTK_WIDGET(view);

	// load document
	gchar *uri = g_filename_to_uri(FILE, NULL, NULL);
	g_assert_true(uri);
	document = poppler_document_new_from_file(uri, NULL, NULL);
	g_assert_true(document);
	g_free(uri);

	// info
	gchar *tooltip = info_2();
	GtkWidget *label = gtk_label_new(G_STRLOC);
	gtk_widget_set_tooltip_text(label, tooltip);

	// page
	g_assert_true(1 == poppler_document_get_n_pages(document));
	page = poppler_document_get_page(document, 0);
	g_assert_true(page);

	GtkWidget *area = gtk_drawing_area_new();
	//gtk_drawing_area_set_content_width (GTK_DRAWING_AREA (area), 50);
  //gtk_drawing_area_set_content_height (GTK_DRAWING_AREA (area), 50);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), c_draw, NULL, NULL);

	// end
	return area;

}

