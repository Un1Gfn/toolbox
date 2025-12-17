//#undef G_LOG_DOMAIN

#include <gtk/gtk.h>
#include "tabs.h"
#include "util.h"
#include "libclk/libclk.h"

#define SZ 1024

#define UEPOCH "1970-01-01T00:00:00.000000Z"
#define SEPOCH "1970-01-01T00:00:00Z"

#define A(X) { if (X) ; else { alert(G_STRLOC); return; } }

#define Foreach() for (Clk *c = clk; c->name; c++)

#define xstr(s) str(s)
#define str(s) #s

#define CLK(X) { \
	.name = xstr(X), \
	.label = nullptr, \
	.tick = nullptr, \
	.new = &tick_##X##_new, \
	.destroy = &tick_##X##_destroy \
}

typedef struct {
	const char *name;
	GtkWidget *label;
	void *tick;
	New *new;
	Destroy *destroy;
} Clk;

static Clk clk[] = {
	CLK(timerfd),
	CLK(nanosleep),
	CLK(timer),
	//CLK(libev),
	//CLK(libevent),
	{ }
};

static GtkEntryBuffer *buffer;
static GDateTime *epoch;
static GDateTime *until;
static GMutex change_state;
static gboolean running;

static void alert(const char *const message) {
	//g_warning(message);
	static const char *labels[] = {
		[0] = "default",
		[1] = "cancel",
		[2] = "...",
		nullptr
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
	g_object_unref(g_steal_pointer(&dialog));
}


typedef struct {
	GtkLabel *label;
	gchar *text;
} I;

static void idle(gpointer userdata) {
	I *i = userdata;
	//g_debug("tick %p", i->label);
	gtk_label_set_text(i->label, i->text);
	g_free(i->text);
	g_free(i);
}

static void callback(void *userdata) {

	g_assert_true(userdata);

	I *i = g_malloc0(sizeof(I));
	i->label = GTK_LABEL(userdata);

	// diff
	auto now = g_date_time_new_now_local();
	GTimeSpan delta = g_date_time_difference(until, now);
	auto diff = g_date_time_add(epoch, delta);

	// human readable diff
	auto s = g_date_time_format_iso8601(diff);
	g_assert_true(27 == strnlen(s, 28));

	// fill hud
	i->text = g_strdup_printf(
		"%.0lfm ^ "
		"%.5s"
		//" %.0lfs"
	"%s",
		(double)delta/60000000.0,
		s + 11,
		//(double)delta/1000000.0,
	"");
	g_assert_true(i->text && i->text[0]);

	// cleanup
	g_free(g_steal_pointer(&s));
	g_date_time_unref(g_steal_pointer(&diff));
	g_date_time_unref(g_steal_pointer(&now));

	// render hud
	// (toolbox:...): GLib-GObject-CRITICAL **: ...: g_object_unref: assertion 'G_IS_OBJECT (object)' failed
	// (toolbox:...): Pango-WARNING **: ...: Invalid UTF-8 string passed to pango_layout_set_text()
	g_idle_add_once(&idle, i);

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

	running = true;

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

	// epoch datetime

	void epoch_test() {
		auto _ = g_date_time_format_iso8601(epoch);
		g_assert_true(0 == g_strcmp0(SEPOCH, _));
		g_free(_);
	}

	epoch = g_date_time_new_from_iso8601(UEPOCH, nullptr);
	epoch_test();
	g_date_time_unref(epoch);

	epoch = g_date_time_new_from_iso8601(SEPOCH, nullptr);
	epoch_test();
	g_date_time_unref(epoch);

	epoch = g_date_time_new_from_unix_utc(0);
	epoch_test();

	// foreach start
	Foreach() {
		gtk_label_set_text(GTK_LABEL(c->label), "...");
	}
	Foreach() {
		g_assert_true(!c->tick);
		c->tick = c->new(&callback, c->label);
		g_assert_true(c->tick);
	}

	//g_message("running...");

	err:
	g_mutex_unlock(&change_state);

}

static void stop() {

	if (!g_mutex_trylock(&change_state))
		return;

	if (!running) {
		alert(G_STRLOC);
		goto stop_err;
	}

	running = false;

	// foreach stop
	Foreach() {
		g_assert_true(c->tick);
		c->destroy(&(c->tick));
		g_assert_true(!c->tick);
		//g_message("%s", c->name);
		I *i = g_malloc0(sizeof(I));
		i->label = GTK_LABEL(c->label);
		i->text = g_strdup(c->name);
		g_idle_add_once(&idle, i);
	}

	g_assert_true(until);
	g_assert_true(epoch);
	g_date_time_unref(g_steal_pointer(&until));
	g_date_time_unref(g_steal_pointer(&epoch));

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

	auto box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
	gtk_box_append(box, flexiblespace());

	buffer = gtk_entry_buffer_new("21:00", -1); // OK
	auto entry = gtk_entry_new_with_buffer(buffer);
	g_object_unref(buffer); // crash?

	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "alarm-symbolic");
	gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_PRIMARY, "media-playback-stop-symbolic");
	g_signal_connect(entry, "activate", G_CALLBACK(start), nullptr);
	g_signal_connect(entry, "icon-press", G_CALLBACK(s_icon_press), nullptr);
	gtk_box_append(box, entry);

	// foreach label
	Foreach() {
		gtk_box_append(box, flexiblespace());
		c->label = gtk_label_new(c->name);
		gtk_box_append(box, c->label);
	}

	// crash?
	//start(GTK_ENTRY(entry), nullptr);

	gtk_box_append(box, flexiblespace());

	return GTK_WIDGET(box);

}
