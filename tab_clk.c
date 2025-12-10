//#undef G_LOG_DOMAIN

#include <gtk/gtk.h>
#include "tabs.h"
#include "util.h"
#include "libclk/libclk.h"

#define SZ 1024

#define A(X) { if (X) ; else { alert(G_STRLOC); return; } }

static GtkEntryBuffer *buffer;
static GDateTime *until;
static GMutex change_state;
static gboolean running;

// order!
typedef struct {
	const char *name;
	GtkWidget *label;
	void *tick;
	New *new;
	Destroy *destroy;
} Clk;

// order!
static Clk clk[] = {
	{ "nanosleep", NULL, NULL, &tick_nanosleep_new, &tick_nanosleep_destroy },
	//{ "libev", NULL, NULL, &tick_libev_new, &tick_libev_destroy },
	//{ "libevent", NULL, NULL, &tick_libevent_new, &tick_libevent_destroy },
	{ }
};

// loop
#define Foreach() for (Clk *c = clk; c->name; c++)

static void alert(const char *const message) {
	g_warning(message);
	static const char *labels[] = {
		[0] = "default",
		[1] = "cancel",
		[2] = "...",
		NULL
	};
	/*
	auto dialog = gtk_alert_dialog_new("");
	gtk_alert_dialog_set_message(dialog, message);
	gtk_alert_dialog_set_detail(dialog, message):
	*/
	auto dialog = gtk_alert_dialog_new("%s", message);
	gtk_alert_dialog_set_modal(dialog, true);
	gtk_alert_dialog_set_buttons(dialog, labels);
	gtk_alert_dialog_set_default_button(dialog, 0);
	gtk_alert_dialog_set_cancel_button(dialog, 1);
	gtk_alert_dialog_show(dialog, window);
}

static void callback(void *userdata) {

	g_assert_true(userdata);
	auto label = GTK_LABEL(userdata);

	auto now = g_date_time_new_now_local();
	auto diff = g_date_time_difference(until, now);
	g_date_time_unref(g_steal_pointer(&now));

	auto text = g_strdup_printf("%ld", diff);
	g_assert_true(text && text[0]);

	g_debug("tick %p", label);
	gtk_label_set_text(GTK_LABEL(label), text);
	g_free(g_steal_pointer(&text));

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

	g_mutex_lock(&change_state);

	if (running) {
		alert(G_STRLOC);
		goto err;
	}

	// now iso8601
	auto now = g_date_time_new_now_local();
	g_assert_true(now);
	gchar *iso = g_date_time_format_iso8601(now);
	g_assert_true(iso && 29 == strnlen(iso, 30));
	g_date_time_unref(g_steal_pointer(&now));

	// until iso8601
	//g_debug("%s", iso);
	g_assert_true(memcpy(iso + 11, s, 5) == iso + 11);
	//g_debug("%s", iso);
	g_assert_true(memcpy(iso + 16, ":00.000000", 9) == iso + 16);
	//g_debug("%s", iso);

	// until datetime
	auto default_tz = g_time_zone_new_local();
	g_assert_true(default_tz);
	until = g_date_time_new_from_iso8601(iso, default_tz);
	g_assert_true(until);
	g_free(g_steal_pointer(&iso));

	// until iso8601 verify
	iso = g_date_time_format_iso8601(until);
	g_debug("%s", iso);
	g_free(g_steal_pointer(&iso));

	// timezone destroy
	g_time_zone_unref(g_steal_pointer(&default_tz));

	// tick new
	Foreach() {
		g_assert_true(!c->tick);
		c->tick = c->new(&callback, c->label);
		g_assert_true(c->tick);
	}

	g_message("running...");

	err:
	g_mutex_unlock(&change_state);

}

static void stop() {

	if(!g_mutex_trylock(&change_state))
		return;

	if (!running) {
		alert(G_STRLOC);
		goto stop_err;
	}

	Foreach() {
		g_assert_true(c->tick);
		c->destroy(&(c->tick));
		g_assert_true(!c->tick);
	}

	g_assert_true(until);
	g_date_time_unref(g_steal_pointer(&until));
	g_message("stoped");

	stop_err:
		g_mutex_unlock(&change_state);

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

	auto box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
	gtk_box_append(box, flexiblespace());
	gtk_box_append(box, entry);

	Foreach() {
		gtk_box_append(box, flexiblespace());
		c->label = gtk_label_new(c->name);
		gtk_box_append(box, c->label);
	}

	gtk_box_append(box, flexiblespace());

	return GTK_WIDGET(box);

}
