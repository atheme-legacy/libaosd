SUBDIRS = src

DISTCLEAN = buildsys.mk config.h config.log config.status extra.mk libaosd.pc

include buildsys.mk
