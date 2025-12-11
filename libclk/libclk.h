#pragma once

#ifndef GLIB_MAJOR_VERSION
	#ifdef G_LOG_DOMAIN
		#include <stdio.h>
		#define G_DEBUG_HERE() { printf("(custom) %s: %s: %d\n", __FILE__, __func__, __LINE__); }
	#else
		#define G_DEBUG_HERE() ;
	#endif
#endif

typedef void *New(void (*)(void*), void*);
typedef void Destroy(void**);

#define DECL(x) \
	New tick_##x##_new; \
	Destroy tick_##x##_destroy;

DECL(nanosleep)
//DECL(libev)
//DECL(libevent)

