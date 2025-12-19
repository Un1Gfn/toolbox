#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <threads.h>

#include "libclk.h"

#define EMPTY ((void*)(intptr_t)(-1))

_Static_assert(sizeof(void*) == sizeof(timer_t), "");
static timer_t timerid = EMPTY;

static Callback *cb;
static void *d;
static mtx_t cmutex;

static void handler(int) {
	int _ = mtx_trylock(&cmutex);
	if (thrd_success != _) return;
	(*cb)(d);
	mtx_unlock(&cmutex);
}

// singleton
// signal handler cannot take userdata
// there can be one tick_timer instance only
NEW(timer) {

	g_printerr("timer.c: warning: race freeze bug\n");

	// create timer
	if (EMPTY != timerid)
		return nullptr;
	if (0 != timer_create(CLOCK_REALTIME, nullptr, &timerid))
		return nullptr;
	cb = callback;
	d = userdata;

	if (thrd_success != mtx_init(&cmutex, mtx_plain)) return nullptr;

	// handle sigalarm
	if (0 != sigaction(SIGALRM, &(struct sigaction){
		.sa_handler = &handler,
		.sa_mask = {},
		.sa_flags = 0
	}, nullptr))
		return nullptr;

	// generate sigalarm
	if (0 != timer_settime(timerid, 0, &ONESEC, nullptr))
		return nullptr;

	return (void*)(intptr_t)(1);

}

DESTROY(timer) {
	if (EMPTY == timerid)
		return;
	if (1 != (intptr_t)(*p))
		return;
	if (0 != timer_delete(timerid))
		return;
	timerid = EMPTY;
	*p = nullptr;
	cb = nullptr;
	d = nullptr;
}
