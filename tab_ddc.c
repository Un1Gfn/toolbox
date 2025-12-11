#undef G_LOG_DOMAIN

#include <gtk/gtk.h>
#include "tabs.h"
#include "util.h"

// acquire ddc handle
//static void new() {
//	;
//}

// release ddc handle
static void delete() {
	//g_usleep(g_random_int_range(1000000, 3000000));
	G_DEBUG_HERE();
}

GtkWidget *tab_ddc() {
	auto w = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	auto b = GTK_BOX(w);
	gtk_box_append(b, flexiblespace());
	gtk_box_append(b, gtk_label_new("~/.local/bin/ddcutil_contrast.c"));
	gtk_box_append(b, flexiblespace());
	gtk_box_append(b, flexiblespace());
	gtk_box_append(b, gtk_label_new("brightness"));
	gtk_box_append(b, flexiblespace());
	gtk_box_append(b, gtk_label_new("contrast"));
	gtk_box_append(b, flexiblespace());
	g_signal_connect(w, "destroy", delete, NULL);
	return w;
}

