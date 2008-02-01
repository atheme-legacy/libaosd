LIB = ${LIB_PREFIX}aosd${LIB_SUFFIX}
LIB_MAJOR = 2
LIB_MINOR = 0

SRCS = aosd.c \
       aosd-internal.c \
       aosd-main.c

INCLUDES = aosd.h

include ../buildsys.mk
include ../extra.mk

CPPFLAGS += ${LIB_CPPFLAGS} ${X_CFLAGS} ${CAIRO_CFLAGS} -I..
CFLAGS += ${LIB_CFLAGS}
LIBS += ${X_LIBS} ${CAIRO_LIBS} -lpthread
