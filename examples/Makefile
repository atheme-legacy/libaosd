PROG = aosd_cat

SUBDIRS = shared \
	  animation \
	  image \
	  text

SRCS = aosd_cat.c

include ../buildsys.mk
include ../extra.mk

CPPFLAGS += ${LIB_CPPFLAGS} ${AUX_CFLAGS} -I..
LDFLAGS += -L../libaosd -laosd ${AUX_LDFLAGS} shared/example-shared.a
