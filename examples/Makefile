SUBDIRS = shared \
	  animation \
	  image \
	  text

SRCS = aosd_cat.c

include ../buildsys.mk
include ../extra.mk

PROG = aosd_cat

CPPFLAGS += ${LIB_CPPFLAGS} ${AUX_CFLAGS} -I../src
LDFLAGS += -L../src -laosd ${AUX_LDFLAGS} shared/example-shared.a
