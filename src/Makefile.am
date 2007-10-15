lib_LTLIBRARIES = libaosd.la

INCLUDES = ${LIBAOSD_CFLAGS}

libaosd_la_LIBADD = ${LIBAOSD_LIBS}
libaosd_la_LDFLAGS = -version-info @VERSION_INFO@
libaosd_la_includedir = ${includedir}/aosd

sources = libaosd.c libaosd-main.c
headers = libaosd.h

if PANGO
libaosd_la_SOURCES = ${sources} libaosd-text.c
libaosd_la_include_HEADERS = ${headers} libaosd-text.h
else
libaosd_la_SOURCES = ${sources}
libaosd_la_include_HEADERS = ${headers}
endif
