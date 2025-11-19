#!/bin/bash
cat <<EOF
#include <gtk/gtk.h>
#include "tabs.h"

static const char text[] = { $(xxd -i <<"EOF2"

tips 

quick switch to tab N
 $ env TAB=N ./toolbox

...
 $ ...
 
EOF2
) };

GtkWidget *tab_welcome() {
	//return gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	return gtk_label_new(text);
}
EOF
