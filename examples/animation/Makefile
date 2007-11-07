SRCS = animation.c

include ../../buildsys.mk
include ../../extra.mk

PROG_NOINST = animation

CPPFLAGS += ${LIB_CPPFLAGS} ${AUX_CFLAGS} -I../../src
LDFLAGS += -L../../src -laosd ${AUX_LDFLAGS} ../shared/example-shared.a
