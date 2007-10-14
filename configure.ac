AC_PREREQ(2.54)

AC_INIT(libaosd, 0.0.1, [bugs+libaosd@atheme.org])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.7])

if test "x$GCC" = "xyes"; then
    CFLAGS+=" -Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes"
    CFLAGS+=" -Wmissing-declarations -Wnested-externs -fno-strict-aliasing"
fi

AC_PROG_LIBTOOL

LIBAOSD_PACKAGES=cairo
PKG_CHECK_MODULES(cairo, cairo,
    [
     LIBAOSD_CFLAGS=$cairo_CFLAGS
     LIBAOSD_LIBS=$cairo_LIBS
    ],
    [
     AC_MSG_ERROR(can't find cairo)
    ]
)

AC_ARG_ENABLE(pango,
    [AC_HELP_STRING([--enable-pango], [use pango (default on)])],
    [enable_pango=$enableval], [enable_pango="yes"]
)

if test "$enable_pango" = "yes"; then
    PKG_CHECK_MODULES(pangoNcairo, pango pangocairo,
	[
	 LIBAOSD_PACKAGES+=" pango pangocairo"
	 LIBAOSD_CFLAGS+=" $pangoNcairo_CFLAGS"
	 LIBAOSD_LIBS+=" $pangoNcairo_LIBS"
	],
	[
	 AC_MSG_ERROR(can't find pango or pangocairo)
	]
    )

    AC_DEFINE([HAVE_PANGO], [], [we have pangocairo])
else
    enable_pango="no"
fi
AM_CONDITIONAL([PANGO], test "$enable_pango" = "yes")

PKG_CHECK_MODULES(XCOMPOSITE, xcomposite,
    [
     AC_DEFINE([HAVE_XCOMPOSITE], [1], [X Composite extension available])
    ],
    [
     AC_MSG_WARN([X Composite extension not found, composite support will not be compiled])
    ]
)

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
