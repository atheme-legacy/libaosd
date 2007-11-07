SRCS = image.c

include ../../buildsys.mk
include ../../extra.mk

PROG_NOINST = image

CPPFLAGS += ${LIB_CPPFLAGS} ${AUX_CFLAGS} -I../../src -I../shared
LDFLAGS += ${LIB_LDFLAGS} ${AUX_LDFLAGS} ../shared/example-shared.a -L../../src -laosd
