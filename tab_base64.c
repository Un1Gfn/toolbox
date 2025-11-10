#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <openssl/evp.h>
#include <gtk/gtk.h>
//#include "viewer_file.h"

#define SZ 128

static GtkEntryBuffer *buffer = NULL;

static GtkWidget *label = NULL;

static void clicked(GtkWidget*, gpointer) {
	gtk_label_set_text(GTK_LABEL(label), gtk_entry_buffer_get_text(buffer));
}

GtkWidget *tab_base64() {
	// output
	label = gtk_label_new(NULL);
	// input
	buffer = gtk_entry_buffer_new(NULL, -1);
	GtkWidget *entry = gtk_entry_new_with_buffer(buffer);
	// button
	GtkWidget *button = gtk_button_new_with_mnemonic("_base64");
	g_signal_connect(button, "clicked", G_CALLBACK(clicked), NULL);
	// page
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
	gtk_box_append(GTK_BOX(box), button);
	gtk_box_append(GTK_BOX(box), entry);
	gtk_box_append(GTK_BOX(box), label);
	return box;
}

// caller free
static int base64() {

  puts("encoding...");

  EVP_ENCODE_CTX *ctx=EVP_ENCODE_CTX_new();
  assert(ctx);
  EVP_EncodeInit(ctx);

  const char raw[]="hello\n";
  char enc[SZ]={};
  const int r=EVP_EncodeBlock((unsigned char*)enc,(const unsigned char*)raw,strlen(raw));
  assert(r>=4&&r%4==0);

  assert(0==EVP_ENCODE_CTX_num(ctx));
  EVP_ENCODE_CTX_free(ctx);
  ctx=NULL;

  printf("%.*s\n",SZ,enc);

  return 0;

}
