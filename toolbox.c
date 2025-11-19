#include <gtk/gtk.h>
#include <assert.h>
#include "tabs.h"

_Static_assert(4 == GTK_MAJOR_VERSION, "");

static GtkWidget *notebook = NULL;

typedef struct {
	const guint n;
	GtkWidget* (*f)();
	const char* const l;
	const bool lazy;// lazy - call a second time to fully instantiate
} Tab;

static Tab tabs[] = {
	{ 0, &tab_ddc, "DDC", false },
	{ 1, &tab_base64, "Base64", true }
};

static void switch_page(GtkNotebook*, GtkWidget*, guint page_num, gpointer) {
	// lazy - call a second time to fully instantiate
	for (size_t i = 0; i < sizeof(tabs)/sizeof(Tab); i++) {
		Tab *_ = &tabs[i];
		if (_->lazy && page_num == _->n) {
			(*(_->f))();
			break;
		}
	}
}

static void activate(GtkApplication* app, gpointer) {

	// main window
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Toolbox");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
	notebook = gtk_notebook_new();
	gtk_window_set_child(GTK_WINDOW(window), notebook);

	// tabs
	for (size_t i = 0; i < sizeof(tabs)/sizeof(Tab); i++) {
		Tab *_ = &tabs[i];
		assert((long long)_->n == (long long)gtk_notebook_append_page(GTK_NOTEBOOK(notebook), (*(_->f))(), gtk_label_new(_->l)));
	}
	g_signal_connect(notebook, "switch-page", G_CALLBACK(switch_page), NULL);
	
	// show
  //gtk_widget_set_visible(window, true);
	gtk_window_present(GTK_WINDOW(window));

}

int main(int argc, char **argv) {
  g_set_prgname("toolbox");
  GtkApplication *app = gtk_application_new("io.github.Un1Gfn.toolbox", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
