#include <ev.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>
#include "libclk.h"

typedef struct {
	EV_P;
	ev_timer *timer;
	thrd_t cth;
	Callback *callback;
	void *userdata;
} TickLibev;

static void my_cb(EV_P_ ev_timer *w, int/*revents*/) {
	//EV_TIMER == revents
	TickLibev *tick = w->data;
	(*(tick->callback))(tick->userdata);
}

static int cth_start(void *p) {
	if (!p) return -1;
	TickLibev *tick = p;
	ev_run(tick->loop, 0);
	G_DEBUG_HERE();
	return 0;
}

void *tick_libev_new(Callback* callback, void *userdata) {

	// fill
	TickLibev *tick = calloc(1, sizeof(TickLibev));
	if (!tick) return nullptr;
	tick->callback = callback;
	tick->userdata = userdata;

	// timer
	tick->timer = calloc(1, sizeof(ev_timer));
	if (!tick->timer) goto err;
	ev_timer_init(tick->timer, &my_cb, 0.0, 1.0);
	tick->timer->data = tick; // self-referential structure

	// loop
	tick->loop = EV_DEFAULT;
	ev_timer_start(tick->loop, tick->timer);
	if (thrd_success != thrd_create(&(tick->cth), &cth_start, tick)) goto err;

	// return
	return tick;
	err:
		free(tick);
		return nullptr;

}

void tick_libev_destroy(void **p) {

	if (!*p) return;
	TickLibev *tick = *p;

	if (!tick->timer) return;
	ev_timer_stop(tick->loop, tick->timer);
	free(tick->timer);

	if (!tick->loop) return;
	ev_loop_destroy(tick->loop);

	int _ = -1;
	if (thrd_success != thrd_join(tick->cth, &_) || 0 != _) return;
	
	free(*p);
	*p = nullptr;

}

