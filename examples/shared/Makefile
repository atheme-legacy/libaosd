SRCS = example-shared.c

STATIC_LIB_NOINST = example-shared.a

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${LIB_CPPFLAGS} ${AUX_CFLAGS} -I../../src
LDFLAGS += -L../../src
LIBS += ${AUX_LDFLAGS} -laosd
