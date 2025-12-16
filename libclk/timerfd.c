// https://cppreference.com/w/c/header/threads.html
#include <threads.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include "libclk.h"

typedef struct {
	int timerfd;
	void *userdata;
	void (*callback)(void*);
} TickTimerfd;

void *tick_timerfd_new(void (*callback)(void*), void *userdata) {
	
	TickTimerfd *tick = calloc(1, sizeof(TickTimerfd));
	if (!tick)
		return nullptr;
	tick->userdata = userdata;
	tick->callback = callback;

	if (-1 != (tick->timerfd = timerfd_create(CLOCK_REALTIME, 0)))
			goto err;

	timerfd_settime(tick->timerfd, 0, &ONESEC, nullptr);

	err:
		free(tick);
		return nullptr;
	
}

//void tick_timerfd_destroy(void** p) { }
