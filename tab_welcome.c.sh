#!/bin/bash

# AddressSanitizer: global-buffer-overflow
# manually feed a trailing "\0" to xxd

cat <<EOF
#include <gtk/gtk.h>
#include "tabs.h"

static const gchar text[] = {
$({ { sed "s/^/| /g"; printf "\0"; } | xxd -i; } <<"EOF2"

= TIPS =

Tab popup menu:

 right click any tab label

Help:

 make run_debug A="-h"

Lorem ipsum:

 dolor sit amet

EOF2
)
};

GtkWidget *tab_welcome() {
	return gtk_label_new(text);
}
EOF
