#include <gtk/gtk.h>
#include "util.h"

GtkWidget *flexiblespace() {
	GtkWidget *_ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_vexpand(_, TRUE);
	return _;
}
