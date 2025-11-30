#include <gtk/gtk.h>
#include "util.h"

GtkWidget *flexiblespace() {
	auto _ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	_Static_assert(_Generic((_), GtkWidget*: 1, default: 0), "");
  gtk_widget_set_vexpand(_, TRUE);
	return _;
}
