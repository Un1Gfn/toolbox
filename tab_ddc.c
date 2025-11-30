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
	gpointer func(gpointer) {
		//g_usleep(g_random_int_range(1000000, 3000000));
		G_DEBUG_HERE();
		return NULL;
	}
	func(NULL);
	//GThread *thread = g_thread_new(NULL, func, NULL);
	//g_assert_true(thread);
	//g_thread_join(thread);
}

GtkWidget *tab_ddc() {
	auto box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append(GTK_BOX(box), flexiblespace());
	gtk_box_append(GTK_BOX(box), gtk_label_new("~/.local/bin/ddcutil_contrast.c"));
  gtk_box_append(GTK_BOX(box), flexiblespace());
  gtk_box_append(GTK_BOX(box), flexiblespace());
	gtk_box_append(GTK_BOX(box), gtk_label_new("brightness"));
  gtk_box_append(GTK_BOX(box), flexiblespace());
	gtk_box_append(GTK_BOX(box), gtk_label_new("contrast"));
  gtk_box_append(GTK_BOX(box), flexiblespace());

	g_signal_connect(box, "destroy", delete, NULL);

	/*
	#define CONCAT_IMPL(a, b) a##b
	#define CONCAT(a, b) CONCAT_IMPL(a, b)
	#define C(I, S) C0(I, S, CONCAT(s_, __COUNTER__))
	#define C0(I, S, N) { \
		void N(GtkWidget *widget, gpointer) { \
			g_debug("+ %s::%s", G_OBJECT_TYPE_NAME(widget), S); \
			g_debug("- %s::%s", G_OBJECT_TYPE_NAME(widget), S); \
		} \
		g_signal_connect(I, S, G_CALLBACK(N), NULL); \
	}
	C(box, "realize");
	C(box, "map");
	C(box, "unmap");
	C(box, "unrealize");
	void _(GtkWidget*, gpointer) { g_assert_true(FALSE); }
	g_signal_connect(box, "move-focus", G_CALLBACK(_), NULL);
	g_signal_connect(box, "show", G_CALLBACK(_), NULL);
	g_signal_connect(box, "hide", G_CALLBACK(_), NULL);
	*/

	return box;
}

