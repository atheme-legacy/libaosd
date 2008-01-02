PROG = aosd_cat

SRCS = aosd_cat.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${PANGOCAIRO_CFLAGS} -I../../ -I../../libaosd
LDFLAGS += ${PANGOCAIRO_LIBS} -L../../libaosd -laosd
