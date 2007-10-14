lib_LTLIBRARIES = libaosd.la

INCLUDES = $(LIBAOSD_CFLAGS)
#libaosd_la_LDFLAGS = -version-info @VERSION_INFO@
libaosd_la_LIBADD = $(LIBAOSD_LIBS)
libaosd_la_includedir = $(includedir)/aosd

headers = libaosd.h
# libaosd-internal.h is in SOURCES not HEADERS because we don't
# want to install it into /usr/include/.../
sources = libaosd-internal.h libaosd.c libaosd-main.c

#pango_headers = libaosd-text.h
#pango_sources = libaosd-text.c

#if PANGO
#libaosd_la_SOURCES = $(sources) $(pango_sources)
#libaosd_la_include_HEADERS = $(headers) $(pango_headers)
#else
libaosd_la_SOURCES = $(sources)
libaosd_la_include_HEADERS = $(headers)
#endif
