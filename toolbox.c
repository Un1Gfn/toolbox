//#undef G_LOG_DOMAIN

const char *__asan_default_options() {
	return "log_path=/tmp/toolbox_asan";
}

#include <gtk/gtk.h>
#include <assert.h>
#include "tabs.h"
#include "util.h"

_Static_assert(4 == GTK_MAJOR_VERSION, "");

GtkWindow *window = nullptr;
static GtkNotebook *notebook;

// notebook tab type
typedef struct {
	GtkWidget* (*f)();
	const char* const l;
} Tab;

// notebook slot nullptr-terminated
static Tab tabs[] = {
	{ &tab_welcome, "Welcome" },
	{ &tab_base64, "Base64" },
	{ &tab_env, "Env" },
	{ &tab_ddc, "DDC/CI" },
	{ &tab_pdf, "PDF" },
	{ &tab_clk, "Clock" },
	{ }
};

// notebook tab count
static const gint N0 = sizeof(tabs)/sizeof(Tab);
static const gint N = N0 - 1;

// arg stub
static gboolean list;
static gint to_tab = -2;
static GOptionEntry entries[] = {
	#define DSZ 128
	{ "list-tabs", 'l', G_OPTION_FLAG_NONE, G_OPTION_ARG_NONE, &list, (gchar[DSZ+1]){}, nullptr },
	{ "tab", 't', G_OPTION_FLAG_NONE, G_OPTION_ARG_INT, &to_tab, (gchar[DSZ+1]){}, ""},
	{ }
};

// arg fill
static void init_entries() {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
	static gchar *s;
	static gint c;

	c = 0;
	s = entries[0].description;
	c += g_snprintf(s+c, DSZ-c, "list %d available tabs", N);
	g_assert_true(DSZ > c);

	c = 0;
	s = entries[1].description;
	c += g_snprintf(s+c, DSZ-c, "switch to tab 0..%d(-1)", N-1);
	g_assert_true(DSZ > c);

	//c = 0;
	//s = entries[1].arg_description;
	//c += g_snprintf(s+c, DSZ-c, "[0-%d-]", N-1);
	//g_assert_true(DSZ > c);

	#pragma GCC diagnostic pop
}

// notebook tab full

typedef struct { GtkWidget* stack; GtkWidget *widget; } SW;

static void idle_stack(gpointer userdata) {
	auto sw = (SW*)userdata;
	gtk_stack_add_child(GTK_STACK(sw->stack), sw->widget);
	g_free(sw);
}

static void th_func(gpointer data, gpointer userdata) {

	// prepare
	g_assert_true(!userdata);
	g_assert_true(data);
	auto t = (Tab*)data;
	SW *sw = g_malloc0(sizeof(SW));

	sw->widget = (*(t->f))();
	g_assert_true(sw->widget);

	sw->stack = gtk_notebook_get_nth_page(notebook, t - tabs);
	g_assert_true(sw->stack);
	g_assert_true(G_TYPE_CHECK_INSTANCE_TYPE(sw->stack, GTK_TYPE_STACK));
	g_assert_true(0 == g_strcmp0("GtkStack", g_type_name(G_OBJECT_TYPE(sw->stack))));

	g_idle_add_once(&idle_stack, sw);

}

// do not dispatch nested function to g_idle_add_once
// avoid out of scope calling
void idle_switch(gpointer userdata) {
	gtk_notebook_set_current_page(notebook, (intptr_t)userdata);
}

static void s_activate(GtkApplication* app, gpointer) {

	// run once
	static GMutex m;
	if(!g_mutex_trylock(&m))
		return;

	// font
	auto settings = gtk_settings_get_default();
	g_object_set(settings, "gtk-font-name", "Dejavu Sans Mono 18", nullptr);
	g_object_unref(settings);

	// main window
	window = GTK_WINDOW(gtk_application_window_new(app));
	gtk_window_set_title(window, "Toolbox");
	gtk_window_set_default_size(window, 1024, 768);
	gtk_widget_set_size_request(GTK_WIDGET(window), 1024, 768);

	// notebook tab stub
	notebook = GTK_NOTEBOOK(gtk_notebook_new());
	gtk_notebook_set_scrollable(notebook, TRUE);
	gtk_notebook_popup_enable(notebook);
	for (auto t = tabs; t->f; t++) {
		assert((long long)(t-tabs) == (long long)gtk_notebook_append_page(
			notebook,
			gtk_stack_new(),
			gtk_label_new(t->l)
		));
	}
	gtk_window_set_child(window, GTK_WIDGET(notebook));
	//gtk_widget_set_visible(window, true);
	gtk_window_present(window);

	// notebook tab full
	auto pool = g_thread_pool_new(&th_func, nullptr, -1, FALSE, nullptr);
	for (auto t = tabs; t->f; t++) {
		g_assert_true(g_thread_pool_push(pool, t, nullptr));
	}

	// notebook arg switch
	// idle avoid pango segmentation fault
	g_assert_true(1 <= N);
	if (0 <= to_tab) {
		g_assert_true((long long)(N) > to_tab);
		g_idle_add_once(&idle_switch, (gpointer)(intptr_t)to_tab);
	} else if (-1 == to_tab) {
		g_idle_add_once(&idle_switch, (gpointer)(intptr_t)(N-1));
	} else {
		g_assert_true(-2 == to_tab);
	}

}

// arg handler
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

	// https://github.com/google/sanitizers/wiki/AddressSanitizerSupportedPlatforms/dd6e4bfd9189239ff1003002786421408fcc9190#gcc
#ifdef __SANITIZE_ADDRESS__
	g_print("[pid=%u AddressSanitizer %s]\n", getpid(), __asan_default_options());
#endif

	// app metadata
	g_set_application_name("toolbox_2");
	auto app = gtk_application_new("io.github.Un1Gfn.toolbox_3",
		G_APPLICATION_DEFAULT_FLAGS
		| G_APPLICATION_HANDLES_OPEN
		| G_APPLICATION_CAN_OVERRIDE_APP_ID
		//| G_APPLICATION_ALLOW_REPLACEMENT
		//| G_APPLICATION_REPLACE
	);
	g_application_set_version(G_APPLICATION(app), "0.1");

	// arg manual
	//auto context = g_option_context_new("@parameter_string@");
	//g_option_context_add_main_entries(context, entries, nullptr);
	//g_option_context_set_help_enabled(context, TRUE);
	//g_assert_true(g_option_context_get_help_enabled(context));
	//g_assert_true(g_option_context_parse(context, &argc, &argv, nullptr));
	//g_debug("%d %p %p %s", argc, argv, argv[0], argv[0]);
	////g_set_prgname("toolbox");
	//g_option_context_free(g_steal_pointer(&context));

	// arg signal
	init_entries();
	g_application_add_main_option_entries(G_APPLICATION(app), entries);
	g_signal_connect(app,"handle-local-options",  G_CALLBACK(s_handle_local_options), nullptr);

	// app run
	g_signal_connect(app, "activate", G_CALLBACK(s_activate), nullptr);
	auto status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;

}

