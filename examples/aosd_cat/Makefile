PROG = aosd_cat

SRCS = aosd_cat.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${AUX_CFLAGS} ${TEXT_CFLAGS} -I../../
LDFLAGS += ${AUX_LDFLAGS} ${TEXT_LDFLAGS} -L../../libaosd -laosd
