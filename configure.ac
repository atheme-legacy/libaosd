AC_PREREQ(2.54)

AC_INIT(libaosd, 0.0.1, [eugene@raptor.kiev.ua])
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE([1.7])

WARN_CFLAGS=""
if test "x$GCC" = "xyes"; then
    WARN_CFLAGS="-Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes " \
    "-Wmissing-declarations -Wnested-externs -fno-strict-aliasing"
fi

CFLAGS="$CFLAGS $WARN_CFLAGS"

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
    PKG_CHECK_MODULES(pango, pango pangocairo,
	[
	 LIBAOSD_PACKAGES+=" pangocairo"
	 LIBAOSD_CFLAGS+=" $pango_CFLAGS"
	 LIBAOSD_LIBS+=" $pango_LIBS"
	],
	[
	 AC_MSG_ERROR(can't find pango)
	]
    )

    AC_DEFINE([HAVE_PANGO], [], [we have pangocairo])
else
    enable_pango="no"
fi
AM_CONDITIONAL([PANGO], test "$enable_pango" = "yes")

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
