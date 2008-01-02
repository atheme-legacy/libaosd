PROG_NOINST = scroller

SRCS = scroller.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${PANGOCAIRO_CFLAGS} -I../../ -I../../libaosd -I../../libaosd-text
LDFLAGS += -L../../libaosd -laosd -L../../libaosd-text -laosd-text
