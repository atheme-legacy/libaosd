PROG_NOINST = image

SRCS = image.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${AUX_CFLAGS} -I../../libaosd -I../shared
CFLAGS +=
LDFLAGS += ${AUX_LDFLAGS} ../shared/example-shared.a -L../../libaosd -laosd
