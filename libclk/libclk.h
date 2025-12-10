#pragma once

#ifndef GLIB_MAJOR_VERSION
	#ifdef G_LOG_DOMAIN
		#include <stdio.h>
		#define G_DEBUG_HERE() { printf("(custom) %s: %s: %d\n", __FILE__, __func__, __LINE__); }
	#else
		#define G_DEBUG_HERE() ;
	#endif
#endif

typedef struct TickNanosleep TickNanosleep;
TickNanosleep *tick_nanosleep_new(void (*)(void*), void*);
void tick_nanosleep_destroy(TickNanosleep**);

typedef struct TickLibev TickLibev;
TickLibev *tick_libev_new(void (*)(void*), void*);
void tick_libev_destroy(TickLibev**);

typedef struct TickLibevent TickLibevent;
TickLibevent *tick_libevent_new(void (*)(void*), void*);
void tick_libevent_destroy(TickLibevent**);
