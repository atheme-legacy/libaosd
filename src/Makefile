LIB = ${LIB_PREFIX}aosd${LIB_SUFFIX}
LIB_MAJOR = 1
LIB_MINOR = 0

SRCS = aosd.c \
       aosd-main.c

include ../extra.mk

SRCS += ${AUX_PANGO_SRCS}

include ../buildsys.mk

CPPFLAGS += ${LIB_CPPFLAGS} ${AUX_CFLAGS} -I..
