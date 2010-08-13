AC_PREREQ(2.54)

AC_INIT([libaosd], [0.2.7], [http://jira.atheme.org/])
AC_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_ISC_POSIX

AC_PATH_PROG(AR, ar)

if test "x$GCC" = "xyes"; then
    CFLAGS+=" -Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes"
    CFLAGS+=" -Wmissing-declarations -Wnested-externs -fno-strict-aliasing"
fi

# XXX workaround
PACKAGE="libaosd"
AC_SUBST(PACKAGE)

# Init the buildsys
BUILDSYS_INIT

# Checks for libraries.
BUILDSYS_SHARED_LIB

PKG_CHECK_MODULES(X11, x11)
PKG_CHECK_MODULES(XRENDER, xrender)

X_CFLAGS="$X11_CFLAGS $XRENDER_CFLAGS"
X_LIBS="$X11_LIBS $XRENDER_LIBS"

PKG_CHECK_MODULES(CAIRO, cairo)
PACKAGES="cairo"

AC_ARG_ENABLE(xcomposite,
    [AC_HELP_STRING([--disable-xcomposite], [avoid using X composite (default=autodetect)])],
    [enable_xcomposite=$enableval], [enable_xcomposite="yes"]
)

if test "$enable_xcomposite" = "yes"; then
    PKG_CHECK_MODULES(XCOMPOSITE, xcomposite,
	[
	 PACKAGES+=" xcomposite"
	 X_CFLAGS+=" $XCOMPOSITE_CFLAGS"
	 X_LIBS+=" $XCOMPOSITE_LIBS"
	 AC_DEFINE([HAVE_XCOMPOSITE], [1], [X Composite extension available])
	],
	[
	 AC_MSG_WARN(can't find xcomposite package, real transparency won't be supported)
	 enable_xcomposite="no"
	]
    )
else
    enable_xcomposite="no"
fi

EXAMPLES="animation"

AC_ARG_ENABLE(pangocairo,
    [AC_HELP_STRING([--disable-pangocairo], [avoid using Pango-Cairo (default=autodetect)])],
    [enable_pangocairo=$enableval], [enable_pangocairo="yes"]
)

if test "$enable_pangocairo" = "yes"; then
    PKG_CHECK_MODULES(PANGOCAIRO, pangocairo,
	[
	 EXAMPLES+=" scroller"
	 TEXT_DIR="libaosd-text"
	 TEXT_PKGCONF="libaosd-text.pc"
	],
	[
	 AC_MSG_WARN(can't find pangocairo package, textual helpers won't be built)
	 enable_pangocairo="no"
	]
    )
else
    enable_pangocairo="no"
fi

AC_ARG_ENABLE(glib,
    [AC_HELP_STRING([--disable-glib], [avoid using Glib-2.0 (default=autodetect)])],
    [enable_glib=$enableval], [enable_glib="yes"]
)

if test "$enable_glib" = "yes"; then
    PKG_CHECK_MODULES(GLIB, glib-2.0,
	[
	 EXAMPLES+=" image"
	 if test "$enable_pangocairo" = "yes"; then
	     EXAMPLES+=" aosd_cat"
	 fi
	],
	[
	 enable_glib="no"
	]
    )
else
    enable_glib="no"
fi

# touch the .deps
BUILDSYS_TOUCH_DEPS

AC_SUBST(PACKAGES)
AC_SUBST(PACKAGE_BUGREPORT)
AC_SUBST(EXAMPLES)
AC_SUBST(X_CFLAGS)
AC_SUBST(X_LIBS)
AC_SUBST(CAIRO_CFLAGS)
AC_SUBST(CAIRO_LIBS)
AC_SUBST(PANGOCAIRO_CFLAGS)
AC_SUBST(PANGOCAIRO_LIBS)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
AC_SUBST(TEXT_DIR)
AC_SUBST(TEXT_PKGCONF)

AC_CONFIG_FILES(
    [
     buildsys.mk
     extra.mk
     libaosd.pc
     examples/aosd_cat/aosd_cat.1
     ${TEXT_PKGCONF}
    ]
)
AC_OUTPUT

cat << _EOF_

Configuration:
AC_HELP_STRING([X Composite], [${enable_xcomposite}])
AC_HELP_STRING([Pango-Cairo], [${enable_pangocairo}])
AC_HELP_STRING([Glib-2.0], [${enable_glib}])
AC_HELP_STRING([Examples], [${EXAMPLES}])

Now type "make" to build, and "make install" to install.
Thank you for using libaosd.

_EOF_
