#include <gtk/gtk.h>
#include <assert.h>
#include "tabs.h"

_Static_assert(4 == GTK_MAJOR_VERSION, "");

static guint n_tab_base64 = 99;

static void switch_page(GtkNotebook*, GtkWidget*, guint page_num, gpointer) {
	// call a second time to fully instantiate
	if (n_tab_base64 == page_num) tab_base64();
}

static void activate(GtkApplication* app, gpointer) {

	// main window
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Toolbox");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
	GtkWidget *notebook = gtk_notebook_new();
	gtk_window_set_child(GTK_WINDOW(window), notebook);

	// tabs
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tab_ddc(), gtk_label_new("DDC"));
	n_tab_base64 = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tab_base64(), gtk_label_new("Base64"));
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
