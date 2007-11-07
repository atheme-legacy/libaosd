PROG_NOINST = text

SRCS = text.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${LIB_CPPFLAGS} ${AUX_CFLAGS} -I../../libaosd
LDFLAGS += -L../../libaosd -laosd ${AUX_LDFLAGS} ../shared/example-shared.a
