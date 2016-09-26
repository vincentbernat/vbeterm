#                                               -*- Autoconf -*-
AC_PREREQ([2.64])

AC_INIT([vbeterm],
        [m4_esyscmd_s([./get-version])],
        [bernat@luffy.cx])
AC_CONFIG_SRCDIR([src/term.c])
AC_CONFIG_HEADER([config.h])
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
AC_PROG_CC_C99
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_EGREP

# Check some compiler flags
AX_CFLAGS_GCC_OPTION([-fdiagnostics-show-option])
AX_CFLAGS_GCC_OPTION([-pipe])
AX_CFLAGS_GCC_OPTION([-Wall])
AX_CFLAGS_GCC_OPTION([-W])
AX_CFLAGS_GCC_OPTION([-Wextra])
AX_CFLAGS_GCC_OPTION([-Wformat])
AX_CFLAGS_GCC_OPTION([-Wformat-security])
AX_CFLAGS_GCC_OPTION([-Wfatal-errors])
AX_CFLAGS_GCC_OPTION([-Wcast-align])
AX_CFLAGS_GCC_OPTION([-Winline])
AX_CFLAGS_GCC_OPTION([-fstack-protector])
AX_CFLAGS_GCC_OPTION([-D_FORTIFY_SOURCE=2])
AX_CFLAGS_GCC_OPTION([-Wno-unused-parameter])
AX_CFLAGS_GCC_OPTION([-Wno-missing-field-initializers])
AX_LDFLAGS_OPTION([-Wl,-z,relro])
AX_LDFLAGS_OPTION([-Wl,-z,now])
AX_LDFLAGS_OPTION([-Wl,--push-state,--no-as-needed -lSegFault -Wl,--pop-state])

AC_CACHE_SAVE

PKG_CHECK_MODULES([GTK], [gtk+-3.0 gdk-3.0])
PKG_CHECK_MODULES([VTE], [vte-2.91])

AC_CACHE_SAVE

AC_OUTPUT

cat <<EOF

------------------ Summary ------------------
 $PACKAGE_NAME version $PACKAGE_VERSION
  Prefix.........: $prefix
  C Compiler.....: $CC $CFLAGS $CPPFLAGS
  Linker.........: $LD $LDFLAGS $LIBS
---------------------------------------------

Check the above options and compile with:
 ${MAKE-make}

EOF
