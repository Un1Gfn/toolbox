#pragma once

// mimic
#ifndef GLIB_MAJOR_VERSION
	#define g_printerr(...) fprintf(stderr, __VA_ARGS__)
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

// for tab_clk.c
typedef void Callback(void*);
typedef void *New(Callback, void*);
typedef void Destroy(void**);

// for both tab_clk.c and libclk/*.c
#define NEW(x) void *tick_##x##_new (Callback callback, void *userdata)
#define DESTROY(x) void tick_##x##_destroy(void **p)
#define DECL(x) NEW(x); DESTROY(x);
DECL(nanosleep);
//DECL(timer); // race freeze bug
DECL(timerfd);
DECL(libev);
