PROG = aosd_cat

SRCS = aosd_cat.c

MAN = aosd_cat.1

DISTCLEAN = aosd_cat.1

include ../../buildsys.mk
include ../../extra.mk

CPPFLAGS += ${PANGOCAIRO_CFLAGS} ${GLIB_CFLAGS} -I../../ -I../../libaosd -I../../libaosd-text
LDFLAGS += ${PANGOCAIRO_LIBS} ${GLIB_LIBS} -L../../libaosd -laosd -L../../libaosd-text -laosd-text
