AC_PREREQ(2.54)

AC_INIT([libaosd], [0.1.1], [http://bugzilla.atheme.org/])
AC_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_ISC_POSIX

AC_PATH_PROG(AR, ar)

if test "x$GCC" = "xyes"; then
    CFLAGS+=" -Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes"
    CFLAGS+=" -Wmissing-declarations -Wnested-externs -fno-strict-aliasing"
fi

# XXX workaround
PACKAGE="libaosd"
AC_SUBST(PACKAGE)

# Checks for libraries.
BUILDSYS_SHARED_LIB

PKG_CHECK_MODULES(X11, x11)
PKG_CHECK_MODULES(XRENDER, xrender)

X_LIBS="$X11_LIBS $XRENDER_LIBS"

PKG_CHECK_MODULES(CAIRO, cairo)
PKG_CHECK_MODULES(PANGO, pango)
PKG_CHECK_MODULES(PANGOCAIRO, pangocairo)

PACKAGES="cairo pango pangocairo"
AUX_CFLAGS="$CAIRO_CFLAGS $PANGO_CFLAGS $PANGOCAIRO_CFLAGS"
AUX_LDFLAGS="$CAIRO_LIBS $PANGO_LIBS $PANGOCAIRO_LIBS"

AC_ARG_ENABLE(xcomposite,
    [AC_HELP_STRING([--disable-xcomposite], [avoid using X composite (default=autodetect)])],
    [enable_xcomposite=$enableval], [enable_xcomposite="yes"]
)

if test "$enable_xcomposite" = "yes"; then
    PKG_CHECK_MODULES(XCOMPOSITE, xcomposite,
	[
	 PACKAGES+=" xcomposite"
	 AUX_CFLAGS+=" $XCOMPOSITE_CFLAGS"
	 AUX_LDFLAGS+=" $XCOMPOSITE_LIBS"
	 AC_DEFINE([HAVE_XCOMPOSITE], [1], [X Composite extension available])
	],
	[
	 AC_MSG_WARN(can't find xcomposite package)
	]
    )
else
    enable_xcomposite="no"
fi

AC_SUBST(X_LIBS)
AC_SUBST(PACKAGES)
AC_SUBST(AUX_CFLAGS)
AC_SUBST(AUX_LDFLAGS)

AC_CONFIG_FILES(
    [
     buildsys.mk
     extra.mk
     libaosd.pc
    ]
)
AC_OUTPUT

cat << _EOF_

Configuration:
	X Composite:	$enable_xcomposite

Now type "make" to build, and "make install" to install.
Thank you for using libaosd.

_EOF_
