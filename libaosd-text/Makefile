LIB = ${LIB_PREFIX}aosd-text${LIB_SUFFIX}
LIB_MAJOR = 2
LIB_MINOR = 0

SRCS = aosd-text.c
INCLUDES = aosd-text.h

include ../buildsys.mk
include ../extra.mk

CPPFLAGS += ${PANGOCAIRO_CFLAGS} -I.. -I../libaosd
CFLAGS += ${LIB_CFLAGS}
LIBS += ${PANGOCAIRO_LIBS} -L../libaosd -laosd
