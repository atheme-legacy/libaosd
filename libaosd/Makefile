LIB = ${LIB_PREFIX}aosd${LIB_SUFFIX}
LIB_MAJOR = 1
LIB_MINOR = 0

SRCS = aosd.c \
       aosd-main.c \
       aosd-text.c

INCLUDES = aosd.h \
	   aosd-text.h

include ../buildsys.mk
include ../extra.mk

CPPFLAGS += ${LIB_CPPFLAGS} ${AUX_CFLAGS} -I..
CFLAGS += ${LIB_CFLAGS}
LIBS += ${AUX_LDFLAGS}
