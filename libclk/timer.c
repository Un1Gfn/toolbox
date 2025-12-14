#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#include "libclk.h"

#define EMPTY ((void*)(intptr_t)(-1))

_Static_assert(sizeof(void*) == sizeof(timer_t), "");
static timer_t timerid = EMPTY;

static void (*callback)(void*);
static void *userdata;

static void handler(int) {
	(*callback)(userdata);
}

// singleton
// signal handler cannot take userdata
// there can be one tick_timer instance only
void *tick_timer_new(void (*cb)(void*), void *d) {

	// create timer
	if (EMPTY != timerid)
		return NULL;
	if (0 != timer_create(CLOCK_REALTIME, NULL, &timerid))
		return NULL;
	callback = cb;
	userdata = d;

	// configure timeout
	if (0 != timer_settime(timerid, 0, &(struct itimerspec){
		.it_interval = { .tv_sec = 1, .tv_nsec = 0 },
		.it_value = { .tv_sec = 1, .tv_nsec = 0 }
	}, NULL))
		return NULL;

	// arm
	if (0 != sigaction(SIGALRM, &(struct sigaction){
		.sa_handler = &handler,
		.sa_mask = {},
		.sa_flags = 0
	}, NULL))
		return NULL;

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
	*p = NULL;
	callback = NULL;
	userdata = NULL;
}
