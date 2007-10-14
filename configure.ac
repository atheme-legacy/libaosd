AC_PREREQ(2.54)

AC_INIT(libaosd, 0.0.1, [bugs+libaosd@atheme.org])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.7])

LT_CURRENT=0
LT_REVISION=0
LT_AGE=0

VERSION_INFO="$LT_CURRENT:$LT_REVISION:$LT_AGE"
AC_SUBST(VERSION_INFO)

if test "x$GCC" = "xyes"; then
    CFLAGS+=" -Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes"
    CFLAGS+=" -Wmissing-declarations -Wnested-externs -fno-strict-aliasing"
fi

AC_PROG_LIBTOOL

LIBAOSD_PACKAGES=cairo
PKG_CHECK_MODULES(CAIRO, cairo,
    [
     LIBAOSD_CFLAGS=$CAIRO_CFLAGS
     LIBAOSD_LIBS=$CAIRO_LIBS
    ],
    [
     AC_MSG_ERROR(can't find cairo package)
    ]
)

AC_ARG_ENABLE(pango,
    [AC_HELP_STRING([--enable-pango], [use pango (default on)])],
    [enable_pango=$enableval], [enable_pango="yes"]
)

if test "$enable_pango" = "yes"; then
    PKG_CHECK_MODULES(PANGO, pango,
	[
	 LIBAOSD_PACKAGES+=" pango"
	 LIBAOSD_CFLAGS+=" $PANGO_CFLAGS"
	 LIBAOSD_LIBS+=" $PANGO_LIBS"
	],
	[
	 AC_MSG_ERROR(can't find pango package)
	]
    )

    PKG_CHECK_MODULES(PANGOCAIRO, pangocairo,
	[
	 LIBAOSD_PACKAGES+=" pangocairo"
	 LIBAOSD_CFLAGS+=" $PANGOCAIRO_CFLAGS"
	 LIBAOSD_LIBS+=" $PANGOCAIRO_LIBS"
	],
	[
	 AC_MSG_ERROR(can't find pangocairo package)
	]
    )

    AC_DEFINE([HAVE_PANGO], [1], [Both pango and pangocairo are available])
else
    enable_pango="no"
fi
AM_CONDITIONAL([PANGO], test "$enable_pango" = "yes")

AC_ARG_ENABLE(xcomposite,
    [AC_HELP_STRING([--enable-xcomposite], [use X composite (default on)])],
    [enable_xcomposite=$enableval], [enable_xcomposite="yes"]
)

if test "$enable_xcomposite" = "yes"; then
    PKG_CHECK_MODULES(XCOMPOSITE, xcomposite,
	[
	 LIBAOSD_PACKAGES+=" xcomposite"
	 LIBAOSD_CFLAGS+=" $XCOMPOSITE_CFLAGS"
	 LIBAOSD_LIBS+=" $XCOMPOSITE_LIBS"
	],
	[
	 AC_MSG_ERROR(can't find xcomposite package)
	]
    )

    AC_DEFINE([HAVE_XCOMPOSITE], [1], [X Composite extension available])
else
    enable_xcomposite="no"
fi

AC_SUBST(LIBAOSD_PACKAGES)
AC_SUBST(LIBAOSD_CFLAGS)
AC_SUBST(LIBAOSD_LIBS)

AC_OUTPUT(
    [
     Makefile
     src/Makefile
     libaosd.pc
    ]
)
