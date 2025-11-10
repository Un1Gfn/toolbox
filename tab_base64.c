#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <openssl/evp.h>
#include <gtk/gtk.h>
//#include "viewer_file.h"

#define SZ 128

GtkWidget *tab_base64() {
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 30);
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
