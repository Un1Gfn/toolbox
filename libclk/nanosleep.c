//#undef G_LOG_DOMAIN

#include <pthread.h>
#include <stdlib.h>
#include "libclk.h"

typedef struct {
	void *userdata;
	Callback *callback;
	pthread_t pth;
} TickNanosleep;

static void *start_routine(TickNanosleep *t){
	for (;;) {

		// immediate
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, nullptr);
		t->callback(t->userdata);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);

		struct timespec ts = {};
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec = 0;
		ts.tv_nsec = -ts.tv_nsec + 1000000000;
		nanosleep(&ts, nullptr);

	}
	_Static_assert(PTHREAD_CANCELED == (void*)-1, "");
	return (void*)-2;
}

DESTROY(nanosleep) {

	TickNanosleep *t = *p;

	if (0 != pthread_cancel(t->pth))
		return;

	void *_ = nullptr;
	if (0 != pthread_join(t->pth, &_))
		return;

	if (PTHREAD_CANCELED != _)
		return;

	free(*p);
	*p = nullptr;

}

typedef void *T;
static void d2(TickNanosleep **p) {
	if (p && *p)
		tick_nanosleep_destroy((void**)p);
}

NEW(nanosleep) {

	__attribute__((cleanup(d2))) TickNanosleep *t = calloc(1, sizeof(TickNanosleep));

	if (!t)
		return nullptr;

	t->userdata = userdata;
	t->callback = callback;

	if (0 != pthread_create(&(t->pth), nullptr, (void*(*)(void*))start_routine, t))
		return nullptr;

	return ({void *_ = t; t = nullptr; _;});

}
