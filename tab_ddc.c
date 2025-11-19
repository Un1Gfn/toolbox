#include <gtk/gtk.h>
#include "tabs.h"
#include "util.h"

GtkWidget *tab_ddc() {
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	//GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
  gtk_box_append(GTK_BOX(box), flexiblespace());
	gtk_box_append(GTK_BOX(box), gtk_label_new("~/.local/bin/ddcutil_contrast.c"));
  gtk_box_append(GTK_BOX(box), flexiblespace());
  gtk_box_append(GTK_BOX(box), flexiblespace());
	gtk_box_append(GTK_BOX(box), gtk_label_new("brightness"));
  gtk_box_append(GTK_BOX(box), flexiblespace());
	gtk_box_append(GTK_BOX(box), gtk_label_new("contrast"));
  gtk_box_append(GTK_BOX(box), flexiblespace());
	return box;
}

