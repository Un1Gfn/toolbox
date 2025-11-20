#undef G_LOG_DOMAIN

#include <gtk/gtk.h>
#include <assert.h>
#include "tabs.h"

_Static_assert(4 == GTK_MAJOR_VERSION, "");

static GtkWidget *notebook = NULL;

typedef struct {
	GtkWidget* (*f)();
	const char* const l;
} Tab;

static Tab tabs[] = {
	{ &tab_welcome, "Welcome" },
	{ &tab_base64, "Base64" },
	{ &tab_env, "Env" },
	{ &tab_ddc, "DDC/CI" },
	{ &tab_pdf, "PDF" },
	{ NULL, NULL }
};

static void s_switch_page(GtkNotebook*, GtkWidget*, guint page_num, gpointer) {
	g_debug("switch from page %u to page %u", gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)), page_num);
	return;
}

// fully instantiate a tab
static void th_func(gpointer data, gpointer userdata) {
	g_assert_true(!userdata);
	g_assert_true(data);
	Tab *t = (Tab*)data;
	int n = t - tabs;
	g_debug("+ [%d] = '%s'", n, t->l);
	GtkWidget *stack = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), n);
	g_assert_true(0 == g_strcmp0("GtkStack", g_type_name(G_OBJECT_TYPE(stack))));
	g_assert_true(G_TYPE_CHECK_INSTANCE_TYPE(stack, GTK_TYPE_STACK));
	GtkWidget *w = (*(t->f))();
	g_assert_true(w);
	gtk_stack_add_child(GTK_STACK(stack), w);
	//g_usleep(g_random_int_range(0, 5000000));
	g_debug("- [%d] = '%s'", n, t->l);
}

static void s_activate(GtkApplication* app, gpointer) {

	// run once
	static GMutex m = {};
	if(!g_mutex_trylock(&m))
		return;

	// font
	GtkSettings *settings = gtk_settings_get_default();
	g_object_set(settings, "gtk-font-name", "Dejavu Sans Mono 18", NULL);
	//g_free(settings);

	// main window
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Toolbox");
  gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);

	// notebook stub
	notebook = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
	for (const Tab *t = tabs; t->f; t++) {
		assert((long long)(t-tabs) == (long long)gtk_notebook_append_page(
			GTK_NOTEBOOK(notebook),
			//(*(t->f))(),
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
	const char *e = g_getenv("TAB");
	if (e && e[0]) {
		gchar *p = NULL;
		errno = 0;
		gint64 n = g_ascii_strtoll(e, &p, 10);
		g_assert_true(0==errno);
		g_assert_true(e != p);
		g_assert_true(0 <= n);
		g_assert_true((long long)(sizeof(tabs)/sizeof(Tab)) > n);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), n);
	}

	// notebook full instantiate
	GThreadPool *pool = g_thread_pool_new(&th_func, NULL, -1, FALSE, NULL);
	for (Tab *t = tabs; t->f; t++) {
		g_assert_true(g_thread_pool_push(pool, t, NULL));
	}
	//g_thread_pool_free(pool, FALSE, TRUE);

}

int main(int argc, char **argv) {
  g_set_prgname("toolbox");
  g_set_application_name("toolbox_2");
  GtkApplication *app = gtk_application_new("io.github.Un1Gfn.toolbox_3",
		G_APPLICATION_DEFAULT_FLAGS
		| G_APPLICATION_HANDLES_OPEN
		| G_APPLICATION_CAN_OVERRIDE_APP_ID
		//| G_APPLICATION_ALLOW_REPLACEMENT
		//| G_APPLICATION_REPLACE
	);
	g_application_set_version(G_APPLICATION(app), "0.1");
  g_signal_connect(app, "activate", G_CALLBACK(s_activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}

