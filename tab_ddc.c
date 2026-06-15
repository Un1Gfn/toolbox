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

static void addlb(GtkBox* b, const gchar *const s) {
	g_assert_true(s);
	g_assert_true(b);
	auto l = gtk_label_new(s);
	gtk_widget_set_vexpand(l, TRUE);
	gtk_box_append(b, l);
}

GtkWidget *tab_ddc() {
	auto w = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	auto b = GTK_BOX(w);
	addlb(b, "~/.local/bin/ddcutil_contrast.c");
	addlb(b, "brightness");
	addlb(b, "contrast");
	g_signal_connect(w, "destroy", delete, nullptr);
	return w;
}

