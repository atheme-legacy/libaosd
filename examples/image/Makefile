PROG_NOINST = image

SRCS = image.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${CAIRO_CFLAGS} ${GLIB_CFLAGS} -I../../ -I../../libaosd
LDFLAGS += ${GLIB_LIBS} -L../../libaosd -laosd
