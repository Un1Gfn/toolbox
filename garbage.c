
// info
//gchar *tooltip = info_2();
//GtkWidget *label = gtk_label_new(G_STRLOC);
//gtk_widget_set_tooltip_text(label, tooltip);

//static gchar *info_gslist() {
//	GSList *l = nullptr;
//	g_assert_false(poppler_document_get_author(document));
//	g_assert_false(poppler_document_get_metadata(document));
//	l = g_slist_prepend(l, poppler_document_get_creator(document));
//	l = g_slist_prepend(l, poppler_document_get_producer(document));
//	char *tooltip = nullptr;
//	char *s = nullptr;
//	void func(gpointer data, gpointer userdata) {
//		G_DEBUG_HERE();
//		g_assert_false(userdata);
//		s = tooltip;
//		//g_strdup_printf
//		tooltip = g_strconcat(s?s:"", "\n\n", (char*)data, nullptr);
//		g_free(g_steal_pointer(&s));
//		g_assert_false(s);
//	}
//	g_slist_foreach(l, func, nullptr);
//	G_DEBUG_HERE();
//	s = tooltip;
//	tooltip = g_strconcat(s, "\n\n", nullptr);
//	g_free(g_steal_pointer(&s));
//	G_DEBUG_HERE();
//	g_slist_free_full(g_steal_pointer(&l), &g_free);
//	return tooltip;
//}

//static gchar *info_2() {
//	char *tooltip = nullptr;
//	void f(char *s) {
//		g_assert_true(s);
//		char *new = g_strconcat(tooltip?tooltip:"", "\n\n", s, nullptr);
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

