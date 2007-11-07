PROG_NOINST = image

SRCS = image.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${LIB_CPPFLAGS} ${AUX_CFLAGS} -I../../libaosd -I../shared
CFLAGS += ${LIB_CFLAGS}
LDFLAGS += ${LIB_LDFLAGS} ${AUX_LDFLAGS} ../shared/example-shared.a -L../../libaosd -laosd
