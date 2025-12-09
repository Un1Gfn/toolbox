//#undef G_LOG_DOMAIN

#include <gtk/gtk.h>
#include "tabs.h"
#include "util.h"

#define SZ 1024

#define A(X) { if (X) ; else { alert(G_STRLOC); return; } }

static GtkEntryBuffer *buffer;
static GtkWidget *label_et, *label_ev, *label_uv;
static GMutex running;

static void alert(const char *const message) {

	g_warning(message);

	static const char *labels[] = {
		[0] = "default",
		[1] = "cancel",
		[2] = "...",
		NULL
	};

	//auto dialog = gtk_alert_dialog_new("");
	//gtk_alert_dialog_set_message(dialog, message);
	//gtk_alert_dialog_set_detail(dialog, message);

	auto dialog = gtk_alert_dialog_new("%s", message);

	gtk_alert_dialog_set_modal(dialog, true);
	gtk_alert_dialog_set_buttons(dialog, labels);
	gtk_alert_dialog_set_default_button(dialog, 0);
	gtk_alert_dialog_set_cancel_button(dialog, 1);

	gtk_alert_dialog_show(dialog, window);

}

static void start(GtkEntry*, gpointer) {

	// min 20:00^8:00pm tonight
	// max 29:59^5:59am the next day

	const char *s = gtk_entry_buffer_get_text(buffer);
	g_assert_true(s);

	A(5 == strnlen(s, 6));
	A('2' == s[0]);
	A('0' <= s[1] && s[1] <= '9');
	A(':' == s[2]);
	A('0' <= s[3] && s[3] <= '5');
	A('0' <= s[4] && s[4] <= '9');
	A(g_mutex_trylock(&running));

	g_message("running...");

}

static void stop() {
	A(!g_mutex_trylock(&running));
	g_mutex_unlock(&running);
	g_message("stoped");
}

static void s_icon_press(GtkEntry* entry, GtkEntryIconPosition pos, gpointer userdata) {
	if (GTK_ENTRY_ICON_SECONDARY == pos)
		start(entry, userdata);
	else if (GTK_ENTRY_ICON_PRIMARY == pos)
		stop();
	else
		g_assert_true(0);
}

GtkWidget *tab_clk() {

	buffer = gtk_entry_buffer_new("21:00", -1); // OK
	auto entry = gtk_entry_new_with_buffer(buffer);
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "alarm-symbolic");
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_PRIMARY, "media-playback-stop-symbolic");
	g_signal_connect(entry, "activate", G_CALLBACK(start), NULL);
	g_signal_connect(entry, "icon-press", G_CALLBACK(s_icon_press), NULL);

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
