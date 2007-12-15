PROG_NOINST = animation

SRCS = animation.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${CAIRO_CFLAGS} -I../..
LDFLAGS += ${CAIRO_LIBS} -L../../libaosd -laosd
