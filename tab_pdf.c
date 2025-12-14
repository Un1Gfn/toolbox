/*******************************************************************************

(1) th_tab > tab_pdf()
(2) th_tab > load()
(3) bind - &draw - GtkDrawingArea
(4) bind - &s_press - GtkGestureClick - GtkEventController - GtkDrawingArea
(5) g_application_run
(6) th_pd > draw()
(7) th_pd > s_pressed()
(8) th_load > load() > queue_draw()
(9) th_pd > draw()

th_tab   tab_pdf()+load(-)

th_pd    ... draw() ... s_pressed()                         draw() ...
                                   \                       /
                                   mux                   mux
                                     \                   /
dynamic                               load(+queue_draw())

********************************************************************************/

#undef G_LOG_DOMAIN

#include <gtk/gtk.h>
#include <poppler.h>
#include <unistd.h>
#include "tabs.h"

#define MMD "/home/darren/mermaid/mindmap.mmd"

static GdkCursor *cursor;
static GSubprocess *subprocess;
static PopplerPage *page;

static gdouble w;
static gdouble h;
static GtkWidget *area;

// tab_pdf() and the first load()
static GThread *th_tab;

// subsequent load() different thread id each time

// s_press() and draw() always in the same thread
static GThread *th_pd;

// for th_load and th_pd
static GMutex mux;

//static void status(GSubprocess *p) { g_debug("%d %d %d %d",
//	//g_subprocess_get_exit_status(p), // gint
//	g_subprocess_get_if_exited(p), // gboolean
//	g_subprocess_get_if_signaled(p), // gboolean
//	g_subprocess_get_successful(p) // gboolean
//); }

// load caller lock
// load callee unlock
static gpointer load(gpointer do_draw) {

	g_debug("%s() 0", G_STRFUNC);

	// must be already locked by caller
	g_assert_true(!g_mutex_trylock(&mux));

	// check
	static GMutex _;
	if (g_mutex_trylock(&_)) {
		// first
		g_assert_true(th_tab == g_thread_self());
	} else {
		// subsequent
		g_assert_true(subprocess);
		g_assert_true(G_IS_SUBPROCESS(subprocess));
		g_object_unref(g_steal_pointer(&subprocess));
	}

	// chained
	static GBytes *bytes;
	static PopplerDocument *document;
	if (page) {
		g_assert_true(page && document && bytes);
	} else {
		g_assert_true(!page && !document && !bytes);
	}

	// spawn
	gtk_widget_set_cursor(area, cursor);
	//g_printerr(" "); g_print("\n");
	subprocess = g_subprocess_new(
		G_SUBPROCESS_FLAGS_STDOUT_PIPE,
		nullptr,
		"/usr/bin/mmdc",
		"-i", MMD,
		"-e", "pdf",
		"-f",
		"-o", "-",
		nullptr
	);
	GBytes *bytes2 = nullptr;
	g_subprocess_communicate(subprocess, nullptr, nullptr, &bytes2, nullptr, nullptr);
	g_subprocess_wait(subprocess, nullptr, nullptr);
	g_assert_true(bytes2);
	//g_printerr(" "); g_print("\n");

	// status
	//status(subprocess);
	typedef gint R[4];
	R r = {
		// g_subprocess_get_exit_status(subprocess),
		g_subprocess_get_if_exited(subprocess),
		g_subprocess_get_if_signaled(subprocess),
		g_subprocess_get_successful(subprocess)
	};
	const gboolean killed  = (0 == memcmp((R){/*1,*/ 0, 1, 0}, r, sizeof(r)));
	const gboolean fail    = (0 == memcmp((R){/*1,*/ 1, 0, 0}, r, sizeof(r)));
	const gboolean success = (0 == memcmp((R){/*0,*/ 1, 0, 1}, r, sizeof(r)));

	// discard new invalid pdf
	if (fail) {
		g_bytes_unref(g_steal_pointer(&bytes2));
	}

	// discard old outdated pdf
	g_debug("%s @%ld k=%d f=%d s=%d", G_STRLOC, (intptr_t)page, killed, fail, success);
	if (page && (killed || success)) {
		g_assert_true(page && document && bytes);
		g_object_unref(g_steal_pointer(&page));
		g_object_unref(g_steal_pointer(&document));
		g_bytes_unref(g_steal_pointer(&bytes));
	}
	g_debug("%s @%ld", G_STRLOC, (intptr_t)page);

	// update pdf
	if (success) {
		bytes = bytes2;
		g_assert_true((document = poppler_document_new_from_bytes(bytes, nullptr, nullptr)));
		g_assert_true((page = poppler_document_get_page(document, 0)));
		poppler_page_get_size(page, &w, &h);
	}
	g_debug("%s @%ld", G_STRLOC, (intptr_t)page);

	// callee unlock
	gtk_widget_set_cursor(area, nullptr);
	g_mutex_unlock(&mux);

	// optional draw
	if(!killed && (intptr_t)do_draw) {
		gtk_widget_queue_draw(area);
	}

	g_debug("%s() Z", G_STRFUNC);
	return nullptr;

}

// callee lock mux
// callee unlock mux
static void draw(GtkDrawingArea*, cairo_t *cr, int w0, int h0, gpointer) {

	g_debug("%s() 0", G_STRFUNC);

	static GMutex first;
	if (g_mutex_trylock(&first)) {
		g_assert_true(!th_pd);
		g_assert_true((th_pd = g_thread_self()));
		g_assert_true(th_tab != th_pd);
	} else {
		g_assert_true(th_pd && (th_pd == g_thread_self()));
	}

	// callee lock mux
	g_mutex_lock(&mux);

	// render
	static gboolean history_success;
	if (page) {
		history_success = true;
		gdouble m = 0;
		gdouble sx = (gdouble)w0/w;
		gdouble sy = (gdouble)h0/h;
		if (sx > sy) {
			m = w0 - (w * sy);
			cairo_translate(cr, m/2, 0);
			cairo_scale(cr, sy, sy);
		} else {
			m = h0 - (h * sx);
			cairo_translate(cr, 0, m/2);
			cairo_scale(cr, sx, sx);
		}
		poppler_page_render(page, cr);
	} else {
		// page never null after first success
		g_assert_true(!history_success);
	}

	// callee unlock mux
	g_mutex_unlock(&mux);
	g_debug("%s() Z", G_STRFUNC);

}

static void s_pressed(GtkGestureClick*, gint, gdouble, gdouble, gpointer) {
	g_debug("%s() 0", G_STRFUNC);
	g_assert_true(th_pd && (th_pd == g_thread_self()));
	if(!g_mutex_trylock(&mux)) {
		g_debug("%s() _", G_STRFUNC);
		return;
	}
	g_assert_true(th_pd && (th_pd == g_thread_self()));

	static GThread *th_load;
	static GMutex _;
	if (g_mutex_trylock(&_)) {
		g_assert_true(!th_load);
	} else {
		g_assert_true(th_load);
		g_thread_join(g_steal_pointer(&th_load));
	}
	g_assert_true((th_load = g_thread_new("th_load", load, (gpointer)true)));
	g_debug("%s() Z", G_STRFUNC);
}

static void s_destroy(GtkWidget*, gpointer) {
	g_debug("%s()", G_STRFUNC);
	g_subprocess_force_exit(subprocess);
	g_mutex_lock(&mux);
	g_mutex_unlock(&mux);
}

GtkWidget *tab_pdf() {

	g_assert_true((th_tab = g_thread_self()));

	// load without draw
	g_assert_true((area = gtk_drawing_area_new()));
	g_assert_true(g_mutex_trylock(&mux));
	load((gpointer)false);

	// load and draw
	g_assert_true((cursor = gdk_cursor_new_from_name("wait", nullptr)));
	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw, nullptr, nullptr);
	auto click = gtk_gesture_click_new();
	g_signal_connect(click, "pressed", G_CALLBACK(s_pressed), area);
	gtk_widget_add_controller(area, GTK_EVENT_CONTROLLER(click));

	// cleanup
	g_signal_connect(area, "destroy", G_CALLBACK(s_destroy), nullptr);

	// trigger draw - file change
	// refer to mmwait() in /home/darren/kountdown/src/countdown.c

	return area;

}
