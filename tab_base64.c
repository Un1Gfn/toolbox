// disable g_debug
#undef G_LOG_DOMAIN

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <openssl/evp.h>
#include <gtk/gtk.h>
#include "tabs.h"
#include "util.h"

#define SZO (1024*1024*1024)
#define SZI (1024*1024*768+1)

// encoded string
static gchar enc[SZO];

// input
static GtkEntryBuffer *buffer;

// output
static GtkWidget *label;

// g_autoptr
void EVP_ENCODE_CTX_free2(EVP_ENCODE_CTX *ctx) {
	assert(0 == EVP_ENCODE_CTX_num(ctx));
	EVP_ENCODE_CTX_free(ctx);
}
G_DEFINE_AUTOPTR_CLEANUP_FUNC(EVP_ENCODE_CTX, EVP_ENCODE_CTX_free2);

static void base64() {

	// check
	gsize _ = gtk_entry_buffer_get_bytes(buffer);
	gint n = _;
	assert((long long)n == (long long)_);
	if(n >= SZI) { g_warning(G_STRLOC); return; }
	if(n == 0) { g_warning(G_STRLOC); return; }
	assert(n > 0);

	// init
	g_autoptr(EVP_ENCODE_CTX) ctx = EVP_ENCODE_CTX_new();
	assert(ctx);
	EVP_EncodeInit(ctx);

	// calculate
	bzero(enc, SZO);
	const int r = EVP_EncodeBlock(
		(unsigned char*)enc,
		(const unsigned char*)gtk_entry_buffer_get_text(buffer),
		n
	);
	assert(r >= 4 && r % 4 == 0);

}

static void clicked(GtkWidget*, gpointer) {
	base64();
	gtk_label_set_text(GTK_LABEL(label), enc);
}

GtkWidget *tab_base64() {

	// run once
	static GMutex m;
	assert(g_mutex_trylock(&m));

	// input
	buffer = gtk_entry_buffer_new("animal", -1);
	auto entry = gtk_entry_new_with_buffer(buffer);
	gtk_widget_set_valign(entry, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand(entry, TRUE);
	//assert(gtk_widget_grab_focus(entry));

	// button
	auto button = gtk_button_new_with_mnemonic("_base64");
	gtk_widget_set_valign(button, GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand(button, TRUE);
	g_signal_connect(button, "clicked", G_CALLBACK(clicked), nullptr);

	// output
	label = gtk_label_new(nullptr);
	gtk_label_set_wrap(GTK_LABEL(label), TRUE);
	gtk_label_set_wrap_mode(GTK_LABEL(label), PANGO_WRAP_CHAR);
	gtk_widget_set_vexpand(label, TRUE);

	// box of 3
	auto box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0 /*30*/ );
	gtk_box_append(GTK_BOX(box), entry);
	gtk_box_append(GTK_BOX(box), button);
	gtk_box_append(GTK_BOX(box), label);
	//gtk_widget_grab_focus(entry);
	//gtk_entry_grab_focus_without_selecting(GTK_ENTRY(entry));

	return box;

}

