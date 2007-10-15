lib_LTLIBRARIES = libaosd.la

INCLUDES = ${LIBAOSD_CFLAGS}

libaosd_la_LIBADD = ${LIBAOSD_LIBS}
libaosd_la_LDFLAGS = -version-info @VERSION_INFO@
libaosd_la_includedir = ${includedir}/${PACKAGE}

sources = aosd.c aosd-main.c
headers = aosd.h

if PANGO
libaosd_la_SOURCES = ${sources} aosd-text.c
libaosd_la_include_HEADERS = ${headers} aosd-text.h
else
libaosd_la_SOURCES = ${sources}
libaosd_la_include_HEADERS = ${headers}
endif
