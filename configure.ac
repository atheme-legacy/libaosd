AC_PREREQ(2.54)

AC_INIT([libaosd], [0.0.1], [bugs+libaosd@atheme.org])
AC_CONFIG_HEADER([config.h])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_ISC_POSIX

if test "x$GCC" = "xyes"; then
    CFLAGS+=" -Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes"
    CFLAGS+=" -Wmissing-declarations -Wnested-externs -fno-strict-aliasing"
fi

# XXX workaround
PACKAGE="libaosd"
AC_SUBST(PACKAGE)

# Checks for libraries.
BUILDSYS_SHARED_LIB
LIBS="$LIBS $DYNAMIC_LD_LIBS"

PKG_CHECK_MODULES(CAIRO, cairo,
    [
     PACKAGES=cairo
     LIB_CFLAGS=$CAIRO_CFLAGS
     LIB_LDFLAGS=$CAIRO_LIBS
    ],
    [
     AC_MSG_ERROR(can't find cairo package)
    ]
)

AC_ARG_ENABLE(pango,
    [AC_HELP_STRING([--disable-pango], [avoid using pango (default=autodetect)])],
    [enable_pango=$enableval], [enable_pango="yes"]
)

if test "$enable_pango" = "yes"; then
    PKG_CHECK_MODULES(PANGO, pango,
	[
	 PKG_CHECK_MODULES(PANGOCAIRO, pangocairo,
	    [
	     PACKAGES+=" pango pangocairo"
	     LIB_CFLAGS+=" $PANGO_CFLAGS $PANGOCAIRO_CFLAGS"
	     LIB_LDFLAGS+=" $PANGO_LIBS $PANGOCAIRO_LIBS"
	     AC_DEFINE([HAVE_PANGO], [1], [Both pango and pangocairo are available])
	     AUX_PANGO_SRCS=aosd-text.c
	    ],
	    [
	     AC_MSG_WARN(can't find pangocairo package)
	     enable_pango="no"
	    ]
	 )
	],
	[
	 AC_MSG_WARN(can't find pango package)
	 enable_pango="no"
	]
    )
else
    enable_pango="no"
fi

AC_ARG_ENABLE(xcomposite,
    [AC_HELP_STRING([--disable-xcomposite], [avoid using X composite (default=autodetect)])],
    [enable_xcomposite=$enableval], [enable_xcomposite="yes"]
)

if test "$enable_xcomposite" = "yes"; then
    PKG_CHECK_MODULES(XCOMPOSITE, xcomposite,
	[
	 PACKAGES+=" xcomposite"
	 LIB_CFLAGS+=" $XCOMPOSITE_CFLAGS"
	 LIB_LDFLAGS+=" $XCOMPOSITE_LIBS"
	 AC_DEFINE([HAVE_XCOMPOSITE], [1], [X Composite extension available])
	],
	[
	 AC_MSG_WARN(can't find xcomposite package)
	]
    )
else
    enable_xcomposite="no"
fi

AC_SUBST(PACKAGES)
AC_SUBST(LIB_CFLAGS)
AC_SUBST(LIB_LDFLAGS)
AC_SUBST(AUX_PANGO_SRCS)

AC_CONFIG_FILES(
    [
     buildsys.mk
     extra.mk
     libaosd.pc
    ]
)
AC_OUTPUT
