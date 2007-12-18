PROG_NOINST = scroller

SRCS = scroller.c

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${PANGOCAIRO_CFLAGS} -I../../
LDFLAGS += ${PANGOCAIRO_LIBS} -L../../libaosd -laosd -L../../libaosd-text -laosd-text
