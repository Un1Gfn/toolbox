//#undef G_LOG_DOMAIN

#include <gtk/gtk.h>
#include <assert.h>
#include "tabs.h"

_Static_assert(4 == GTK_MAJOR_VERSION, "");

static GtkWidget *notebook = NULL;

typedef struct {
	GtkWidget* (*f)();
	const char* const l;
} Tab;

// null-terminated
static Tab tabs[] = {
	//{ &tab_welcome, "Welcome" },
	//{ &tab_base64, "Base64" },
	//{ &tab_env, "Env" },
	//{ &tab_ddc, "DDC/CI" },
	{ &tab_pdf, "PDF" },
	{ }
};
static const gint N0 = sizeof(tabs)/sizeof(Tab);
static const gint N = N0 - 1;

static gboolean list = FALSE;
static gint tab = -2;
static GOptionEntry entries[] = {
	#define DSZ 128
	{ "list-tabs", 'l', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &list, (gchar[DSZ+1]){}, NULL },
	{ "tab", 't', G_OPTION_FLAG_NONE, G_OPTION_ARG_INT, &tab, "switch to tab n, -1 for last tab", (gchar[DSZ+1]){}},
	{ }
};
static void init_entries() {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
	gchar *s = NULL;
	gint c = -1;
	c = 0;
	s = entries[0].description;
	c += g_snprintf(s+c, DSZ-c, "list %d available tabs", N);
	g_assert_true(DSZ > c);
	c = 0;
	s = entries[1].arg_description;
	c += g_snprintf(s+c, DSZ-c, "[0-%d-]", N-1);
	g_assert_true(DSZ > c);
	#pragma GCC diagnostic pop
}

static void s_switch_page(GtkNotebook*, GtkWidget*, guint, gpointer) {
	//g_debug("switch from page %u to page %u", gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)), page_num);
	return;
}

// fully instantiate a tab
static void th_func(gpointer data, gpointer userdata) {
	g_assert_true(!userdata);
	g_assert_true(data);
	auto t = (Tab*)data;
	gint n = t - tabs;
	auto stack = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), n);
	g_assert_true(0 == g_strcmp0("GtkStack", g_type_name(G_OBJECT_TYPE(stack))));
	g_assert_true(G_TYPE_CHECK_INSTANCE_TYPE(stack, GTK_TYPE_STACK));
	auto w = (*(t->f))();
	g_assert_true(w);
	gtk_stack_add_child(GTK_STACK(stack), w);
}

static void s_activate(GtkApplication* app, gpointer) {

	// run once
	static GMutex m = {};
	if(!g_mutex_trylock(&m))
		return;

	// font
	auto settings = gtk_settings_get_default();
	g_object_set(settings, "gtk-font-name", "Dejavu Sans Mono 18", NULL);
	g_object_unref(settings);

	// main window
  auto window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Toolbox");
  gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);
  gtk_widget_set_size_request(window, 1024, 768);

	// notebook stub
	notebook = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
	for (auto t = tabs; t->f; t++) {
		assert((long long)(t-tabs) == (long long)gtk_notebook_append_page(
			GTK_NOTEBOOK(notebook),
			gtk_stack_new(),
			gtk_label_new(t->l)
		));
	}
	g_signal_connect(notebook, "switch-page", G_CALLBACK(s_switch_page), NULL);
	gtk_window_set_child(GTK_WINDOW(window), notebook);
	
	// show
  //gtk_widget_set_visible(window, true);
	gtk_window_present(GTK_WINDOW(window));

	// notebook quick switch
	if (0 <= tab) {
		g_assert_true((long long)(N) > tab);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), tab);
	} else if (-1 == tab) {
		_Static_assert(1 <= N, "");
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), N-1);
	}

	// notebook full instantiate
	auto pool = g_thread_pool_new(&th_func, NULL, -1, FALSE, NULL);
	for (auto t = tabs; t->f; t++) {
		g_assert_true(g_thread_pool_push(pool, t, NULL));
	}
	//g_thread_pool_free(pool, FALSE, TRUE);

}

static gint s_handle_local_options(GApplication*, GVariantDict*, gpointer user_data) {
	g_assert_true(!user_data);
	if (list) {
		for (auto t = tabs; t->f; t++)
			g_print("%ld %s\n", t-tabs, t->l);
		return 0;
	}
	return -1;
}

int main(int argc, char **argv) {
  g_set_application_name("toolbox_2");
  auto app = gtk_application_new("io.github.Un1Gfn.toolbox_3",
		G_APPLICATION_DEFAULT_FLAGS
		| G_APPLICATION_HANDLES_OPEN
		| G_APPLICATION_CAN_OVERRIDE_APP_ID
		//| G_APPLICATION_ALLOW_REPLACEMENT
		//| G_APPLICATION_REPLACE
	);
	g_application_set_version(G_APPLICATION(app), "0.1");

	// args - manual
	//auto context = g_option_context_new("@parameter_string@");
	//g_option_context_add_main_entries(context, entries, NULL);
	//g_option_context_set_help_enabled(context, TRUE);
	//g_assert_true(g_option_context_get_help_enabled(context));
	//g_assert_true(g_option_context_parse(context, &argc, &argv, NULL));
	//g_debug("%d %p %p %s", argc, argv, argv[0], argv[0]);
  ////g_set_prgname("toolbox");
	//g_option_context_free(g_steal_pointer(&context));

	// args - signal
	init_entries();
	g_application_add_main_option_entries(G_APPLICATION(app), entries);
	g_signal_connect(app,"handle-local-options",  G_CALLBACK(s_handle_local_options), NULL);

  g_signal_connect(app, "activate", G_CALLBACK(s_activate), NULL);
  auto const status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;

}

