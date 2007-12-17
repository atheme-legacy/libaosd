LIB = ${LIB_PREFIX}aosd${LIB_SUFFIX}
LIB_MAJOR = 1
LIB_MINOR = 0

SRCS = aosd.c \
       aosd-internal.c \
       aosd-main.c

INCLUDES = aosd.h

include ../extra.mk

SRCS += ${TEXT_SRCS}
INCLUDES += ${TEXT_INCLUDES}

include ../buildsys.mk

CPPFLAGS += ${LIB_CPPFLAGS} ${X_CFLAGS} ${CAIRO_CFLAGS} ${PANGOCAIRO_CFLAGS} -I..
CFLAGS += ${LIB_CFLAGS}
LIBS += ${X_LIBS} ${CAIRO_LIBS} ${PANGOCAIRO_LIBS}
