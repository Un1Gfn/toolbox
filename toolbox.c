#include <gtk/gtk.h>
#include <assert.h>
#include "tabs.h"

_Static_assert(4 == GTK_MAJOR_VERSION, "");

static GtkWidget *notebook = NULL;

typedef struct {
	GtkWidget* (*f)();
	const char* const l;
	const bool lazy;// lazy - call a second time to fully instantiate
} Tab;

static Tab tabs[] = {
	{ &tab_welcome, "Welcome", false },
	{ &tab_base64, "Base64", true },
	{ &tab_env, "Env", false },
	{ NULL, NULL, false }
};

static void switch_page(GtkNotebook*, GtkWidget*, guint page_num, gpointer) {
	// lazy - call a second time to fully instantiate
	for (const Tab *t = tabs; t->f; t++) {
		if (t->lazy && (long long)page_num == (long long)(t-tabs)) {
			(*(t->f))();
			break;
		}
	}
}

static void activate(GtkApplication* app, gpointer) {

	// a second activate takes no effect
	static int called = 0;
	if(called++) return;

	// font
	GtkSettings *settings = gtk_settings_get_default();
	g_object_set(settings, "gtk-font-name", "Dejavu Sans Mono 18", NULL);

	// main window
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Toolbox");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
	notebook = gtk_notebook_new();
	gtk_window_set_child(GTK_WINDOW(window), notebook);

	// tabs
	for (const Tab *t = tabs; t->f; t++) {
		assert((long long)(t-tabs) ==
			(long long)gtk_notebook_append_page(GTK_NOTEBOOK(notebook), (*(t->f))(), gtk_label_new(t->l))
		);
	}
	g_signal_connect(notebook, "switch-page", G_CALLBACK(switch_page), NULL);
	
	// show
  //gtk_widget_set_visible(window, true);
	gtk_window_present(GTK_WINDOW(window));

	const char *e = g_getenv("TAB");
	if (e && e[0]) {
		gchar *p = NULL;
		gint64 n = g_ascii_strtoll(e, &p, 10);
		g_assert_false(errno);
		g_assert_false(e == p);
		g_assert_true(0 <= n);
		g_assert_true((long long)(sizeof(tabs)/sizeof(Tab)) > n);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), n);
	}

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
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}

