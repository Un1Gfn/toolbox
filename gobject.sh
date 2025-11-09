#!/bin/bash

# https://stackoverflow.com/questions/15184358/how-to-avoid-bash-command-substitution-to-remove-the-newline-character

#   H0="$H"; cat <<__EOF | sed 's/^  //g' | IFS= read -rd '' H; H="$H0$H"
# __EOF

#     H0="$H"; cat <<____EOF | sed 's/^    //g' | IFS= read -rd '' H; H="$H0$H"
# ____EOF

# https://stackoverflow.com/questions/15184358/how-to-avoid-bash-command-substitution-to-remove-the-newline-character
shopt -s lastpipe

# namespace, type, type instance, class (base type)
BN=""; BT=""; BTI=""; BC="";

# namespace, type, type instance, class
N=""; T=""; TI=""; C="";

# true if final, false if derivable
F=true

# methods
M_pub=(); M_priv=(); M_virtpub=(); M_virtpriv=();

# H to emit header, C to emit source
O=""

function help2 {
  1>&2 echo "$(basename "$0") -H|-C <namespace>::<type> [-f] [-b <basetype>] [--pub|priv <method>] ..."
  1>&2 echo "$(basename "$0") -H|-C <namespace>::<type>  -d  [-b <basetype>] [--pub|priv|virtpub|virtpriv <method>] ..."
  1>&2 echo "$(basename "$0") -h|--help"
  1>&2 echo "$(basename "$0") --demo"
  exit 1
}

function nonexist {
  [ -e "$1" ] && { echo "err: $1 exists"; exit 1; }
}

function demo {
  # nonexist ns_dict.h
  # nonexist ns_dict.c
  # "$0" -H ns::dict -f --pub cksum 1>|ns_dict.h
  # "$0" -C ns::dict -f --pub cksum 1>|ns_dict.c
  # "$0" -H ns::dict -d --pub cksum 1>|ns_dict.h
  # "$0" -C ns::dict -d --pub cksum 1>|ns_dict.c
  nonexist viewer_file.h
  nonexist viewer_file.c
  # "$0" -H viewer::file -f --pub openPub 1>|viewer_file.h
  # "$0" -C viewer::file -f --pub openPub 1>|viewer_file.c
  "$0" -H viewer::file -d --pub openPub --virtpub openVirtPub --virtpub closeVirtPub --virtpriv openVirtPriv --priv openVirtPriv 1>|viewer_file.h
  "$0" -C viewer::file -d --pub openPub --virtpub openVirtPub --virtpub closeVirtPub --virtpriv openVirtPriv --priv openVirtPriv 1>|viewer_file.c
  # "$0" -H viewer::file -d --pub openPub 1>|ns_dict.h
  # "$0" -C viewer::file -d --pub openPub 1>|ns_dict.c
}

function nt2nt {
  [[ "$1" =~ ^(([0-9A-Za-z]+)::([0-9A-Za-z]+))$ ]] || help2
  [ -n "${BASH_REMATCH[2]}" ] || help2
  [ -n "${BASH_REMATCH[3]}" ] || help2
  printf -v "$2" '%s' "${BASH_REMATCH[2],,}"
  printf -v "$3" '%s' "${BASH_REMATCH[3],,}"
  printf -v "$4" '%s' "${!2^}${!3^}"
  printf -v "$5" '%s' "${!2^}${!3^}Class"
}

function show {
  if "$F"; then echo "final"; else echo "derivable"; fi
  echo
  echo "pub      - ${M_pub[*]/%/()}"
  echo "priv     - ${M_priv[*]/%/()}"
  echo "virtpub  - ${M_virtpub[*]/%/()}"
  echo "virtpriv - ${M_virtpriv[*]/%/()}"
  echo
}

function H_emit {

  cat <<__EOF | sed 's/^  //g'
  // ${N,,}_${T,,}.h

  // $(basename "$0") ${ARGSBAK[*]}

  // #pragma once
  #ifndef ${N^^}_${T^^}_H
  #define ${N^^}_${T^^}_H

  #include <glib-object.h>

  G_BEGIN_DECLS

  #define ${N^^}_TYPE_${T^^} ${N,,}_${T,,}_get_type()
__EOF

  if "$F"; then
    echo "G_DECLARE_FINAL_TYPE(${TI}, ${N,,}_${T,,}, ${N^^}, ${T^^}, ${BTI})"$'\n'
  else
    cat <<____EOF | sed 's/^    //g'
    G_DECLARE_DERIVABLE_TYPE(${TI}, ${N,,}_${T,,}, ${N^^}, ${T^^}, ${BTI})

    // type instance structure of a derivable class should be defined in its H/header file
    struct _${C} {
      ${BC} parent_class;
      // virtual public methods
____EOF
    for f in "${M_virtpub[@]}"; do
      echo "  void (*${f})(${TI} *${T,,}, GError **error);"
    done
    cat <<____EOF | sed 's/^    //g'
      // padding to allow adding up to 12 new virtual functions without breaking ABI
      gpointer padding[12];
    };
____EOF
    echo
  fi
  cat <<__EOF | sed 's/^  //g'
  ${TI} *${N,,}_${T,,}_new();
__EOF
  echo
  echo "// non-virtual public methods"
  for f in "${M_pub[@]}"; do
    echo "void ${N,,}_${T,,}_${f}(${TI} *self, GError **error);"
  done
  echo
  echo "// virtual public methods"
  for f in "${M_virtpub[@]}"; do
    echo "void ${N,,}_${T,,}_${f}(${TI} *self, GError **error);"
  done
  echo
  cat <<__EOF | sed 's/^  //g'
  G_END_DECLS

  #endif
__EOF

}

function C_emit {

  cat <<__EOF | sed 's/^  //g'
  // ${N,,}_${T,,}.c

  // $(basename "$0") ${ARGSBAK[*]}

  #include "${N,,}_${T,,}.h"
  #include <gio/gio.h>
__EOF
  echo

  if "$F"; then
    cat <<____EOF | sed 's/^    //g'
    // type instance structure of a final class should be defined in its C/source file
    struct _${TI} {
      ${BTI} parent_instance;
      // private data of a final class should be placed in the instance structure
      void *data;
      gchar *filename;
      guint zoom_level;
      GInputStream *input_stream;
    };

    G_DEFINE_TYPE(${TI}, ${N,,}_${T,,}, ${BN^^}_TYPE_${BT^^})
____EOF
  else
    cat <<____EOF | sed 's/^    //g'
    // (A) derivable without private
    // G_DEFINE_TYPE(${TI}, ${N,,}_${T,,}, ${BN^^}_TYPE_${BT^^})

    // (B) derivable with private
    // private data for a derivable class must be included in a private structure, and G_DEFINE_TYPE_WITH_PRIVATE must be used
    typedef struct {
      void *data;
      gchar *filename;
      guint zoom_level;
      GInputStream *input_stream;
    } ${TI}Private;
    //
    G_DEFINE_TYPE_WITH_PRIVATE(${TI}, ${N,,}_${T,,}, ${BN^^}_TYPE_${BT^^})
____EOF
  fi

  echo
  echo "// object destruction phase 1 - dispose"
  echo "static void ${N,,}_${T,,}_dispose(${BTI} *${BTI,,}){"
  if "$F"; then
    echo "  g_clear_object(&(${N^^}_${T^^}(${BTI,,})->input_stream));"
  else
    echo "  ${TI}Private *priv = ${N,,}_${T,,}_get_instance_private(${N^^}_${T^^}(${BTI,,}));"
    echo "  g_clear_object(&priv->input_stream);"
  fi
  echo "  ${BN^^}_${BT^^}_CLASS(${N,,}_${T,,}_parent_class)->dispose(${BTI,,});"
  echo "}"

  echo
  echo "// object destruction phase 2 - finalize"
  echo "static void ${N,,}_${T,,}_finalize(${BTI} *${BTI,,}){"
  if "$F"; then
    echo "  g_free(${N^^}_${T^^}(${BTI,,})->filename);"
  else
    echo "  ${TI}Private *priv = ${N,,}_${T,,}_get_instance_private(${N^^}_${T^^}(${BTI,,}));"
    echo "  g_free(priv->filename);"
  fi
  echo "  ${BN^^}_${BT^^}_CLASS(${N,,}_${T,,}_parent_class)->finalize(${BTI,,});"
  echo "}"

  for f in "${M_virtpub[@]}"; do
    echo
    cat <<____EOF | sed 's/^    //g'
    // default implementation of virtual public method ${N}::${T}::${f}()
    static void ${N,,}_${T,,}_real_${f}(${TI} *self, GError **error){
      // ...
    }
____EOF
  done
  echo

  cat <<__EOF | sed 's/^  //g'
  // forward declarations
  // ...

  enum {
    RESERVED = 0,
    PROP_FILENAME,
    PROP_ZOOM_LEVEL,
    N_PROPERTIES
  };

  static GParamSpec *obj_properties[N_PROPERTIES] = {};
__EOF
  echo
  cat <<__EOF | sed 's/^  //g'
  static void ${N,,}_${T,,}_class_init(${C} *klass){

    ${BC} *${BT,,}_class = ${BN^^}_${BT^^}_CLASS(klass);
    ${BT,,}_class->dispose = ${N,,}_${T,,}_dispose;
    ${BT,,}_class->finalize = ${N,,}_${T,,}_finalize;
__EOF
  echo
  echo "  // virtual public methods"
  for f in "${M_virtpub[@]}"; do
    echo "  // klass->${f} = NULL; // pure virtual - mandates implementation in derived class"
    echo "  klass->${f} = ${N,,}_${T,,}_real_${f}; // semi-virtual"
  done
  echo
  cat <<__EOF | sed 's/^  //g'
    // ${BT,,}_class->set_property = ${N,,}_${T,,}_set_property;
    // ${BT,,}_class->get_property = ${N,,}_${T,,}_get_property;
    obj_properties[PROP_FILENAME] = g_param_spec_string(
      "filename",
      "Filename",
      "Name of the file to load and display from.",
      NULL  /* default */,
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    obj_properties[PROP_ZOOM_LEVEL] = g_param_spec_uint(
      "zoom-level",
      "Zoom level",
      "Zoom level to view the file at.",
      0   /* min */,
      10 /* max */,
      2  /* default */,
      G_PARAM_READWRITE
    );
    g_object_class_install_properties(${BT,,}_class, N_PROPERTIES, obj_properties);

  }
__EOF
  echo

  echo "static void ${N,,}_${T,,}_init(${TI} *self){"
  if "$F"; then
    cat <<____EOF | sed 's/^    //g'
      // initialize private members (final type cannot have public member)
      self->data = NULL;
      self->input_stream = g_object_new(G_TYPE_INPUT_STREAM, NULL);
      self->filename = NULL;
____EOF
  else
    cat <<____EOF | sed 's/^    //g'
      // initialize public members
      // ...
      // initialize private members
      ${TI}Private *priv = ${N,,}_${T,,}_get_instance_private(self);
      priv->data = NULL;
      priv->input_stream = g_object_new(G_TYPE_INPUT_STREAM, NULL);
      priv->filename = NULL;
____EOF
  fi
  echo "}"
  echo
  echo "// non-virtual public methods"
  for f in "${M_pub[@]}"; do
    echo
    cat <<____EOF | sed 's/^    //g'
    void ${N,,}_${T,,}_${f}(${TI} *self, GError **error){
      g_return_if_fail(${N^^}_IS_${T^^}(self));
      g_return_if_fail(error == NULL || *error == NULL);
    }
____EOF
  done
  echo
  echo "// virtual public methods"
  for f in "${M_virtpub[@]}"; do
    echo
    cat <<____EOF | sed 's/^    //g'
    void ${N,,}_${T,,}_${f}(${TI} *self, GError **error){

      g_return_if_fail(${N^^}_IS_${T^^}(self));
      g_return_if_fail(error == NULL || *error == NULL);
      ${C} *klass = ${N^^}_${T^^}_GET_CLASS(self);

      // either one
      /* (A) ignore it silently */ // if (klass->${f} == NULL) return;
      /* (B) warn the user      */ g_return_if_fail(klass->${f} != NULL);

      // redirect ${N,,}::${T,,}::${f}() call to the relevant implementation
      klass->${f}(self, error);

    }
____EOF
  done

}

{

  (($#>=1)) || help2

  ARGSBAK=("$@")
  TEMP=$(/bin/getopt -n 'gen.sh' -o 'b:''fd''HC' -l 'help,demo,pub:,virtpub:,virtpriv:,priv:' -- "$@") || help2
  eval set -- "$TEMP"
  unset -v TEMP

  nt2nt "g::object" BN BT BTI BC

  while true; do
    case "x$1" in
      x-h|x--help) help2; exit;;
      x--demo) demo; exit;;
      x--pub|x--priv|x--virtpub|x--virtpriv) declare -n A="M_${1:2}"; shift; A+=("$1"); shift;;
      x-b) shift; nt2nt "$1" BN BT BTI BC; shift;;
      x-f) F=true; shift;;
      x-d) F=false; shift;;
      x-H|x-C) O="${1:1:1}"; shift;;
      x--) shift; ((1==$#)) || { echo "err: there should be exactly one positional parameter"; exit 1; }; break;;
      *) echo "err: invalid parameter '$1'"; exit 1;;
    esac
  done

  [ H == "$O" ] || [ C == "$O" ] || { echo "err: do yout want the header or the source?"; exit 1; }

  if "$F"; then
    ((0==${#M_virtpub[@]}))  || { echo "err: virtual method not allowed in final (non-derivable) type"; exit 1; }
    ((0==${#M_virtpriv[@]})) || { echo "err: virtual method not allowed in final (non-derivable) type"; exit 1; }
  fi

  nt2nt "$1" N T TI C

  # pygmentize -l c <<<"$H"
  "$O"_emit

}; exit
