// https://mw.abxhg8.xyz/index.php?title=Glib#Memory_Safety
// https://mw.abxhg8.xyz/index.php?title=C#Automain_Test

// ./automain-test.sh

#include <glib.h>
#define SZ 1024

extern const int argc;
extern const char *const *const argv;

void init() { }

typedef struct {
	gpointer *buffer;
} O;

O *o_new() {
	O *o = g_new0(O, 1);
	o->buffer = g_malloc0(SZ);
	return o;
}

void o_access(O *o) {
	bzero(o->buffer, SZ);
}

void o_destroy(O *o) {
	free(o->buffer);
	free(o);
}

G_DEFINE_AUTOPTR_CLEANUP_FUNC(O, o_destroy);

// leak
void mmm_0() {
	O *o = o_new();
	o = NULL;
}

// ok
void mmm_1() {
	O *o = o_new();
	o_destroy(o);
	o = NULL;
}

// ok
void mmm_2() {
	g_autoptr(O) o = o_new();
}

// crash - use after auto free
void mmm_3() {
	O *o_new2() {
		g_autoptr(O) o = o_new();
		return o;
	}
	O *oo = o_new2();
	o_access(oo);
}

// leak - auto to manual then neglect
void mmm_4() {
	O *o_new2() {
		g_autoptr(O) o = o_new();
		return g_steal_pointer(&o);
	}
	O *oo = o_new2();
}

// ok - inherit auto
void mmm_5() {
	O *o_new2() {
		g_autoptr(O) o = o_new();
		return g_steal_pointer(&o);
	}
	g_autoptr(O) oo = o_new2();
	o_access(oo);
}

// ok - auto to manual then free
void mmm_6() {
	O *o_new2() {
		g_autoptr(O) o = o_new();
		return g_steal_pointer(&o);
	}
	O *oo = o_new2();
	o_destroy(oo);
}

// ok - primitive auto
static void freep(void **p) {
	free(*p);
	*p = nullptr;
}
#ifdef _cleanup_
#error ""
#else
#define _cleanup_ __attribute__((cleanup(freep)))
#endif
void mmm_7() {
	_cleanup_ void *x = calloc(1, SZ);
}

// leak
void mmm_8() {
	void *x = calloc(1, SZ);
}
