/*
export A=toolbox L=gtk4; B() { gcc -std=gnu23 -Wall -Wextra -O0 -Og -g $(pkg-config --cflags $L) $A.c $(pkg-config --libs $L) -o $A.out; }; R() { ./$A.out; };
*/

#include <gtk/gtk.h>

static void activate(GtkApplication* app, gpointer) {
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Toolbox");
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
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
