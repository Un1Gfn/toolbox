#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "tab_base64"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <openssl/evp.h>
#include <gtk/gtk.h>
#include "tabs.h"

#define SZO (1024*1024*1024)
#define SZI (1024*1024*768+1)

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__) " "

// encoded string
static char enc[SZO]={};

// input
static GtkEntryBuffer *buffer = NULL;

// output
static GtkWidget *label = NULL;

static void base64() {

	// check
	gsize _ = gtk_entry_buffer_get_bytes(buffer);
	int n = _;
	assert((long long)n == (long long)_);
	if(n >= SZI) { g_warning(AT); return; }
	if(n == 0) { g_warning(AT); return; }
	assert(n > 0);

	// init
  EVP_ENCODE_CTX *ctx=EVP_ENCODE_CTX_new();
  assert(ctx);
  EVP_EncodeInit(ctx);

	// calculate
	bzero(enc, SZO);
  const int r = EVP_EncodeBlock(
		(unsigned char*)enc,
		(const unsigned char*)gtk_entry_buffer_get_text(buffer),
		n
	);
  assert(r>=4&&r%4==0);

	// cleanup
  assert(0==EVP_ENCODE_CTX_num(ctx));
  EVP_ENCODE_CTX_free(ctx);
  ctx=NULL;

}

static void clicked(GtkWidget*, gpointer) {
	base64();
	gtk_label_set_text(GTK_LABEL(label), enc);
}

static GtkWidget *flexiblespace() {
	GtkWidget *_ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_vexpand(_, TRUE);
	return _;
}

GtkWidget *tab_base64() {

	static GtkWidget *box = NULL;

	static int nth_call = 1;
	switch (nth_call++) {
		case 1: G_DEBUG_HERE();
			box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
			return box;
		case 2: G_DEBUG_HERE();
			break;
		default: G_DEBUG_HERE();
			return box;
	}

	// input
	buffer = gtk_entry_buffer_new(NULL, -1);
	GtkWidget *entry = gtk_entry_new_with_buffer(buffer);

	// button
	GtkWidget *button = gtk_button_new_with_mnemonic("_base64");
	g_signal_connect(button, "clicked", G_CALLBACK(clicked), NULL);

	// output
	label = gtk_label_new(NULL);

	// page
  gtk_box_append(GTK_BOX(box), flexiblespace());
	gtk_box_append(GTK_BOX(box), entry);
  gtk_box_append(GTK_BOX(box), flexiblespace());
	gtk_box_append(GTK_BOX(box), button);
  gtk_box_append(GTK_BOX(box), flexiblespace());
	gtk_box_append(GTK_BOX(box), label);
  gtk_box_append(GTK_BOX(box), flexiblespace());

	return box;

}

