#include <gio/gunixinputstream.h>
#include <gio/gunixoutputstream.h>
#include <json-glib/json-glib.h>
#include <gtk/gtk.h>
#include "tabs.h"
#include "util.h"
#include "secrets.h"

// GtkCheckButton
#define SERVICE "mihomo.service"
//static GtkWidget *erase;
//static GtkWidget *restart;
static GtkWidget *path;

// secure global variable
static JsonObject *o0 = NULL;
static JsonObject *const *const o = &o0;

// k must not exist
static gboolean add(const char *const k, const char *const v) {
	if (!(k && k[0])) return FALSE;
	auto n = json_object_get_member(*o, k);
	if (n) return FALSE;
	auto n2 = json_node_alloc();
	auto n3 = json_node_init_string(n2, v);
	if (!(n2 && n2 == n3)) return FALSE;
	json_object_set_member(*o, k, n3);
	return TRUE;
}

// k must exist
// v0   v     action
// NULL NULL  remove member whether string or other type
// NULL STR   change string unconditionally
// STR  NULL  assert string
// STR  STR   assert and change string
static gboolean conf(const char *const k, const char *const v0, const char *const v) {
	if (!(k && k[0])) return FALSE;
	auto n = json_object_get_member(*o, k);
	if (!n) return FALSE;
	if (!v0 && !v) {
		json_object_remove_member(*o, k);
		return TRUE;
	}
	if (!(
		JSON_NODE_VALUE == json_node_get_node_type(n) &&
		G_TYPE_STRING == json_node_get_value_type(n) &&
		(!v0 || 0 == g_strcmp0(v0, json_node_get_string(n)))
	)) return FALSE;
	if (v) {
		json_node_init_string(n, v);
	}
	return TRUE;
}

void convert() {

	// init
	gboolean _ = FALSE;
	g_autoptr(GInputStream) is = g_unix_input_stream_new(STDIN_FILENO, TRUE);
	g_assert_true(is);
	g_autoptr(JsonParser) p = json_parser_new();
	g_autoptr(GOutputStream) os = g_unix_output_stream_new(STDOUT_FILENO, TRUE);
	g_autoptr(JsonGenerator) g = json_generator_new();

	// parse
	json_parser_set_strict(p, TRUE);
	_ = json_parser_load_from_stream(p, is, NULL, NULL);
	g_assert_true(_);

	//g_printerr("%d\n", json_node_get_node_type(r));

	// anchor
	auto const r = json_parser_get_root(p);
	g_assert_true(JSON_NODE_OBJECT == json_node_get_node_type(r));
	o0 = json_node_get_object(r);
	g_assert_true(o0);

	// modify - global conf
	g_assert_true(conf("mode", "Rule", "Global"));
	g_assert_true(conf("external-controller", NULL, "0.0.0.0:9090"));
	g_assert_true(add("external-ui", "/var/lib/mihomo/metacubexd/"));

	// modify - remove rules
	g_assert_true(conf("proxy-groups", NULL, NULL));
	g_assert_true(conf("rules", NULL, NULL));

	// write
	json_generator_set_root(g, r);
	json_generator_to_stream(g, os, NULL, NULL);

}

GtkWidget *tab_ssrcloud() {

	////gtk_file_dialog_new()

	//// file path

	auto bv = gtk_list_box_new();
	gtk_list_box_set_selection_mode(GTK_LIST_BOX(bv), GTK_SELECTION_NONE);
	gtk_list_box_set_show_separators(GTK_LIST_BOX(bv), FALSE);
	gtk_widget_set_valign(bv, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand(bv, TRUE);

	auto url = gtk_label_new(nullptr);
	//gtk_label_set_markup(GTK_LABEL(url), SUBSCRIPTION);
	//gtk_label_set_markup(GTK_LABEL(url), "<a href=\""SUBSCRIPTION"\">link</a>");
	gtk_label_set_markup(GTK_LABEL(url), "<a href=\"" SUBSCRIPTION "\">" SUBSCRIPTION "</a>");
	gtk_list_box_append(GTK_LIST_BOX(bv), url);

	GtkWidget *x(gchar *s) {
		auto label = gtk_label_new(s);
		gtk_widget_set_halign(label, GTK_ALIGN_END);
		gtk_widget_set_hexpand(label, TRUE);

		auto row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_box_append(GTK_BOX(row), label);

		// leak
		auto toggle = gtk_switch_new();
		gtk_switch_set_active(GTK_SWITCH(toggle), TRUE);
		gtk_widget_set_sensitive(toggle, FALSE);
		gtk_box_append(GTK_BOX(row), toggle);

		// no leak
		//auto sdlfkj = gtk_label_new("ksks");
		//gtk_box_append(GTK_BOX(row), sdlfkj);

		return row;

	}
	gtk_list_box_append(GTK_LIST_BOX(bv), x("erase "));
	gtk_list_box_append(GTK_LIST_BOX(bv), x("restart "));

	auto bh = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	path = gtk_entry_new();
	gtk_widget_set_hexpand(path, TRUE);
	gtk_box_append(GTK_BOX(bh), path);
	gtk_box_append(GTK_BOX(bh), gtk_button_new_with_label("choose"));
	gtk_list_box_append(GTK_LIST_BOX(bv), GTK_WIDGET(bh));

	gtk_list_box_append(GTK_LIST_BOX(bv), gtk_button_new_with_label("run"));

	auto b = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_append(GTK_BOX(b), bv);
	return b;

}

