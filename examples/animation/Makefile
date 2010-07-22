PROG_NOINST = animation

SRCS = animation.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${CAIRO_CFLAGS} -I../.. -I../../libaosd
LDFLAGS += ${CAIRO_LIBS} -L../../libaosd -laosd
