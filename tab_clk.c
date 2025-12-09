//#undef G_LOG_DOMAIN

#include <gtk/gtk.h>
#include "tabs.h"
#include "util.h"

#define SZ 1024

static GtkEntryBuffer *buffer;
static GtkWidget *label_et, *label_ev, *label_uv;

GtkWidget *tab_clk() {

	buffer = gtk_entry_buffer_new("21:00", -1); // OK
	auto entry = gtk_entry_new_with_buffer(buffer);
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "alarm-symbolic");

	label_et = gtk_label_new("libevent =");
	label_ev = gtk_label_new("libev =");
	label_uv = gtk_label_new("libuv =");

	auto box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
	gtk_box_append(box, flexiblespace());
	gtk_box_append(box, entry);
	gtk_box_append(box, flexiblespace());
	gtk_box_append(box, label_et);
	gtk_box_append(box, flexiblespace());
	gtk_box_append(box, label_ev);
	gtk_box_append(box, flexiblespace());
	gtk_box_append(box, label_uv);
	gtk_box_append(box, flexiblespace());
	return GTK_WIDGET(box);

}
