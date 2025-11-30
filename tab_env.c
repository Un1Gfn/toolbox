#include <gtk/gtk.h>
#include "tabs.h"

#define E g_getenv("E")
#define EE G_STRINGIFY(E)

static void clicked(GtkWidget*, gpointer) {
	g_message("%s", E);
}

GtkWidget *tab_env() {
	//auto button = gtk_button_new_with_mnemonic(EE);
	auto button = gtk_button_new_with_label(EE);
	gtk_widget_set_tooltip_text(button, EE);
	g_signal_connect(button, "clicked", G_CALLBACK(clicked), NULL);
	return button;
	//return gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
}
