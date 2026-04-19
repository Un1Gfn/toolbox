#include <gio/gunixinputstream.h>
#include <gio/gunixoutputstream.h>
#include <json-glib/json-glib.h>
#include <gtk/gtk.h>
#include "tabs.h"
#include "util.h"

// GtkCheckButton
#define SERVICE "mihomo.service"
static GtkWidget *erase;
static GtkWidget *restart;
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
	auto is = g_unix_input_stream_new(STDIN_FILENO, TRUE);
	g_assert_true(is);
	auto p = json_parser_new();
	auto os = g_unix_output_stream_new(STDOUT_FILENO, TRUE);
	auto g = json_generator_new();

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

	// cleanup
	g_object_unref(g_steal_pointer(&g));
	_ = g_output_stream_close(os, NULL, NULL);
	g_assert_true(_);
	g_object_unref(g_steal_pointer(&os));
	g_object_unref(g_steal_pointer(&p));
	_ = g_input_stream_close(is, NULL, NULL);
	g_assert_true(_);
	g_object_unref(g_steal_pointer(&is));

}

GtkWidget *tab_ssrcloud() {

	erase = gtk_check_button_new_with_label("erase");
	restart = gtk_check_button_new_with_label("restart "SERVICE);
	path = gtk_entry_new();

	//gtk_file_dialog_new()
	auto choose = gtk_button_new_with_label("choose");
	auto run = gtk_button_new_with_label("run");

	auto bh = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
	gtk_box_append(bh, choose);
	gtk_box_append(bh, path);
	gtk_box_append(bh, run);

	auto bv = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
	gtk_box_append(bv, flexiblespace());
	gtk_box_append(bv, erase);
	gtk_box_append(bv, flexiblespace());
	gtk_box_append(bv, restart);
	gtk_box_append(bv, flexiblespace());
	gtk_box_append(bv, GTK_WIDGET(bh));
	gtk_box_append(bv, flexiblespace());
	return GTK_WIDGET(bv);

}

