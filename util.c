#include <gtk/gtk.h>
#include "util.h"

GtkWidget *flexiblespace() {
	auto _ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	_Static_assert(_Generic((_), GtkWidget*: 1, default: 0), "");
  gtk_widget_set_vexpand(_, TRUE);
	return _;
}

void signals(GtkWidget *widget) {

	g_assert_true(GTK_IS_WIDGET(widget));

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

	//C(widget, "destroy");
	C(widget, "direction-changed");
	C(widget, "hide");
	C(widget, "keynav-failed");
	C(widget, "map");
	C(widget, "mnemonic-activate");
	C(widget, "move-focus");
	C(widget, "query-tooltip");
	C(widget, "realize");
	C(widget, "show");
	C(widget, "state-flags-changed");
	C(widget, "unmap");
	C(widget, "unrealize");


	//void _(GtkWidget*, gpointer) { g_assert_true(0); }
	//g_signal_connect(widget, "move-focus", G_CALLBACK(_), NULL);
	//g_signal_connect(widget, "show", G_CALLBACK(_), NULL);
	//g_signal_connect(widget, "hide", G_CALLBACK(_), NULL);

}
