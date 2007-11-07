LIB = ${LIB_PREFIX}aosd${LIB_SUFFIX}
LIB_MAJOR = 1
LIB_MINOR = 0

SRCS = aosd.c \
       aosd-main.c

INCLUDES = aosd.h

include ../extra.mk

SRCS += ${AUX_PANGO}.c
INCLUDES += ${AUX_PANGO}.h

include ../buildsys.mk

CPPFLAGS += ${LIB_CPPFLAGS} ${AUX_CFLAGS} -I..
LIBS += ${AUX_LDFLAGS}
