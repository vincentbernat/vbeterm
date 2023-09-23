#                                               -*- Autoconf -*-
AC_PREREQ([2.71])

AC_INIT([vbeterm],[m4_esyscmd_s([./get-version])],[bernat@luffy.cx])
AC_CONFIG_SRCDIR([src/term.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror tar-ustar])
AM_MAINTAINER_MODE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES(yes)])
m4_pattern_allow([AM_PROG_AR])
AM_PROG_AR

# Configure libtool
LT_INIT

#######################
### Checks

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
LT_INIT
AC_PROG_LN_S
AC_PROG_EGREP

# Check some compiler flags
AX_APPEND_COMPILE_FLAGS([-fdiagnostics-show-option],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-pipe],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-Wall],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-W],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-Wextra],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-Wformat],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-Wformat-security],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-Wfatal-errors],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-Wcast-align],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-Winline],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-fstack-protector],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-D_FORTIFY_SOURCE=2],[MORE_CPPFLAGS])
AX_APPEND_COMPILE_FLAGS([-Wno-unused-parameter],[MORE_CFLAGS])
AX_APPEND_COMPILE_FLAGS([-Wno-missing-field-initializers],[MORE_CFLAGS])
AX_APPEND_LINK_FLAGS([-Wl,-z,relro],[MORE_LDFLAGS])
AX_APPEND_LINK_FLAGS([-Wl,-z,now],[MORE_LDFLAGS])

AC_CACHE_SAVE

PKG_CHECK_MODULES([GTK], [gtk+-3.0 gdk-3.0])
PKG_CHECK_MODULES([X11], [x11])
PKG_CHECK_MODULES([VTE], [vte-2.91])

AC_CACHE_SAVE

AC_SUBST([MORE_CFLAGS])
AC_SUBST([MORE_CPPFLAGS])
AC_SUBST([MORE_LDFLAGS])
AC_OUTPUT

cat <<EOF

------------------ Summary ------------------
 $PACKAGE_NAME version $PACKAGE_VERSION
  Prefix.........: $prefix
  C Compiler.....: $CC $MORE_CFLAGS $MORE_CPPFLAGS $CFLAGS $CPPFLAGS
  Linker.........: $LD $MORE_LDFLAGS $LDFLAGS $LIBS
---------------------------------------------

Check the above options and compile with:
 ${MAKE-make}

EOF
