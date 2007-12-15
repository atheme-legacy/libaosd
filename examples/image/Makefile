PROG_NOINST = image

SRCS = image.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${AUX_CFLAGS} ${GLIB_CFLAGS} -I../../
LDFLAGS += ${AUX_LDFLAGS} ${GLIB_LDFLAGS} -L../../libaosd -laosd
