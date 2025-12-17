// https://cppreference.com/w/c/header/threads.html
#include <threads.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdint.h>
#include "libclk.h"

typedef struct {
	int timerfd;
	Callback *callback;
	void *userdata;
	thrd_t cth;
	mtx_t mutex;
} TickTimerfd;

static int cth_start(void *p) {

	if (!p) return -1;
	TickTimerfd *tick = p;

	for (;;) {

		// buffer 128 bit
		uint64_t buf[2] = {};
		static_assert(16 == sizeof(buf));

		// read 64 bit
		if (8 != read(tick->timerfd, buf, 16)) return -1;
		if (1 != buf[0]) return -1;

		// terminate
		int _ = mtx_trylock(&(tick->mutex));
		if (thrd_busy == _) return 0;
		if (thrd_success != _) return -1;
		(*(tick->callback))(tick->userdata);
		if (thrd_success != mtx_unlock(&(tick->mutex))) return -1;

	}

}

void *tick_timerfd_new(void (*callback)(void*), void *userdata) {

	TickTimerfd *tick = calloc(1, sizeof(TickTimerfd));
	if (!tick) goto err;

	if (-1 == (tick->timerfd = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC))) goto errF;
	if (0 != timerfd_settime(tick->timerfd, 0, &ONESEC, nullptr)) goto errC;

	tick->callback = callback;
	tick->userdata = userdata;
	if (thrd_success != mtx_init(&(tick->mutex), mtx_plain)) goto errC;
	if (thrd_success != thrd_create(&(tick->cth), cth_start, tick)) goto errD;

	return tick;

	errD:
		mtx_destroy(&(tick->mutex));
	errC:
		close(tick->timerfd);
	errF:
		free(tick);
	err:
		return nullptr;

}

void tick_timerfd_destroy(void** p) {

	if (!p || !*p) return;
	TickTimerfd *tick = *p;

	int _ = 0;
	if (thrd_success != mtx_lock(&(tick->mutex))) return;
	if (thrd_success != thrd_join(tick->cth, &_) || 0 != _)	return;
	if (thrd_success != mtx_unlock(&(tick->mutex))) return;
	mtx_destroy(&(tick->mutex));

	if (0 != close(tick->timerfd)) return;

	free(*p);
	*p = nullptr;

}
