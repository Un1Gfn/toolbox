#include <gtk/gtk.h>
#include <stdint.h>
#include "util.h"

_Static_assert(4 == sizeof(GtkTextDirection));
_Static_assert(4 == sizeof(GtkStateFlags));
_Static_assert(4 == sizeof(int32_t));
_Static_assert(4 == sizeof(gboolean));
_Static_assert(4 == sizeof(gint));

GtkWidget *flexiblespace(void) {
	auto _ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	_Static_assert(_Generic((_), GtkWidget*: 1, default: 0), "");
	gtk_widget_set_vexpand(_, TRUE);
	return _;
}

static void s2(GtkWidget *self, gpointer userdata) {
	g_assert_true(userdata);
	const gchar *const s = userdata;
	g_assert_true(s[0]);
	g_debug("%p %s::%s", self, G_OBJECT_TYPE_NAME(self), s);
}

static void s3(GtkWidget *self, int32_t, gpointer userdata) {
	s2(self, userdata);
}

static gboolean s2b(GtkWidget *self, gpointer userdata) {
	s2(self, userdata);
	return false;
}

static gboolean s3b(GtkWidget *self, int32_t, gpointer userdata) {
	return s2b(self, userdata);
}

static gboolean s6b(GtkWidget* self, gint, gint, gboolean, GtkTooltip*, gpointer userdata) {
	return s2b(self, userdata);
}

void signals(GtkWidget *widget) {

	g_assert_true(GTK_IS_WIDGET(widget));

	#define C(H, S) g_signal_connect(widget, S, G_CALLBACK(H), S);

	C(s2, "destroy");
	C(s3, "direction-changed");
	C(s2, "hide");
	C(s3b, "keynav-failed");
	C(s2, "map");
	C(s3b, "mnemonic-activate");
	C(s3, "move-focus");
	C(s6b, "query-tooltip");
	C(s2, "realize");
	C(s2, "show");
	C(s3, "state-flags-changed");
	C(s2, "unmap");
	C(s2, "unrealize");

}
