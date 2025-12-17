#pragma once

#ifndef GLIB_MAJOR_VERSION
	#ifdef G_LOG_DOMAIN
		#include <stdio.h>
		#define G_DEBUG_HERE() { printf("(custom) %s: %s: %d\n", __FILE__, __func__, __LINE__); }
	#else
		#define G_DEBUG_HERE() ;
	#endif
#endif

// immediate
#define ONESEC \
	(struct itimerspec){ \
		.it_interval = { .tv_sec = 1, .tv_nsec = 0 }, \
		.it_value = { .tv_sec = 0, .tv_nsec = 1 } \
	}

typedef void Callback(void*);
typedef void *New(Callback*, void*);
typedef void Destroy(void**);

#define DECL(x) \
	New tick_##x##_new; \
	Destroy tick_##x##_destroy;

DECL(nanosleep);
DECL(timer);
DECL(timerfd);
//DECL(libev);
//DECL(libevent);
