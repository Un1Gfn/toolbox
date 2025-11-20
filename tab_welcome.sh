#!/bin/bash
cat <<EOF
#include <gtk/gtk.h>
#include "tabs.h"

static const char text[] = { $({ sed "s/^/| /g" | xxd -i; } <<"EOF2"

Tips

Tab popup menu

 right click any tab label

Quick switch to tab N:

 env TAB=N ./toolbox

Lorem ipsum:

 dolor sit amet

EOF2
) };

GtkWidget *tab_welcome() {
	//return gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	return gtk_label_new(text);
}
EOF
