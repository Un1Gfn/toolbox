#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <openssl/evp.h>
#include <gtk/gtk.h>
//#include "viewer_file.h"

#define SZO (1024*1024*1024)
#define SZI (1024*1024*768+1)

// base64
static char enc[SZO]={};

// input
static GtkEntryBuffer *buffer = NULL;

// output
static GtkWidget *label = NULL;

static void base64() {

  EVP_ENCODE_CTX *ctx=EVP_ENCODE_CTX_new();
  assert(ctx);
  EVP_EncodeInit(ctx);
	bzero(enc, SZO);

	gsize _ = gtk_entry_buffer_get_bytes(buffer);
	int n = _;
	assert((long long)n == (long long)_);
	assert(n > 0);
	assert(n < SZI);
  const int r = EVP_EncodeBlock(
		(unsigned char*)enc,
		(const unsigned char*)gtk_entry_buffer_get_text(buffer),
		n
	);
  assert(r>=4&&r%4==0);

  assert(0==EVP_ENCODE_CTX_num(ctx));
  EVP_ENCODE_CTX_free(ctx);
  ctx=NULL;

}

static void clicked(GtkWidget*, gpointer) {
	base64();
	gtk_label_set_text(GTK_LABEL(label), enc);
	//gtk_label_set_text(GTK_LABEL(label), gtk_entry_buffer_get_text(buffer));
}

GtkWidget *tab_base64() {

	// input
	buffer = gtk_entry_buffer_new(NULL, -1);
	GtkWidget *entry = gtk_entry_new_with_buffer(buffer);

	// button
	GtkWidget *button = gtk_button_new_with_mnemonic("_base64");
	g_signal_connect(button, "clicked", G_CALLBACK(clicked), NULL);

	// output
	label = gtk_label_new(NULL);

	// page
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
	//gtk_box_append(GTK_BOX(box), gtk_label_new(NULL));
	gtk_box_append(GTK_BOX(box), entry);
	gtk_box_append(GTK_BOX(box), button);
	gtk_box_append(GTK_BOX(box), label);
	return box;

}

