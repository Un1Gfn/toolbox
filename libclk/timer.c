#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#include "libclk.h"

#define EMPTY ((void*)(intptr_t)(-1))

_Static_assert(sizeof(void*) == sizeof(timer_t), "");
static timer_t timerid = EMPTY;

static Callback *callback;
static void *userdata;

static void handler(int) {
	(*callback)(userdata);
}

// singleton
// signal handler cannot take userdata
// there can be one tick_timer instance only
void *tick_timer_new(Callback *cb, void *d) {

	// create timer
	if (EMPTY != timerid)
		return nullptr;
	if (0 != timer_create(CLOCK_REALTIME, nullptr, &timerid))
		return nullptr;
	callback = cb;
	userdata = d;

	// configure timeout
	if (0 != timer_settime(timerid, 0, &ONESEC, nullptr))
		return nullptr;

	// arm
	if (0 != sigaction(SIGALRM, &(struct sigaction){
		.sa_handler = &handler,
		.sa_mask = {},
		.sa_flags = 0
	}, nullptr))
		return nullptr;

	return (void*)(intptr_t)(1);

}

void tick_timer_destroy(void **p) {
	if (EMPTY == timerid)
		return;
	if (1 != (intptr_t)(*p))
		return;
	if (0 != timer_delete(timerid))
		return;
	timerid = EMPTY;
	*p = nullptr;
	callback = nullptr;
	userdata = nullptr;
}
