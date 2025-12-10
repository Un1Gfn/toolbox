//#undef G_LOG_DOMAIN

#include <pthread.h>
#include <stdlib.h>
#include "libclk.h"

/*
// opaque
typedef struct TickNanosleep TickNanosleep;
struct TickNanosleep {
	void *userdata;
	void (*callback)(void*);
	pthread_t thread;
};
*/

// private
typedef struct {
	void *userdata;
	void (*callback)(void*);
	pthread_t thread;
} TickNanosleep;

static void *start_routine(TickNanosleep *t){
	for (;;) {

		struct timespec ts = {};
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec = 0;
		ts.tv_nsec = -ts.tv_nsec + 1000000000;
		nanosleep(&ts, NULL);

		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		t->callback(t->userdata);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	}
	_Static_assert(PTHREAD_CANCELED == (void*)-1, "");
	return (void*)-2;
}

void *tick_nanosleep_new(void (*callback)(void*), void *userdata) {

	TickNanosleep *t = calloc(1, sizeof(TickNanosleep));

	if (!t)
		goto error;

	G_DEBUG_HERE();

	t->userdata = userdata;
	t->callback = callback;

	if (0 != pthread_create(&(t->thread), NULL, (void*(*)(void*))start_routine, t))
		goto error;

	G_DEBUG_HERE();

	return t;

	error:
		free(t);
		return NULL;

}

void tick_nanosleep_destroy(void** p) {

	TickNanosleep *t = *p;

	if (0 != pthread_cancel(t->thread))
		return;

	void *_ = NULL;

	if (0 != pthread_join(t->thread, &_))
		return;

	if (PTHREAD_CANCELED != _)
		return;

	free(*p);
	*p = NULL;

}
