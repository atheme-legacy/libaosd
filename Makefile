SUBDIRS = libaosd \
	  examples
DISTCLEAN = autom4te.cache aclocal.m4 buildsys.mk config.h config.log config.status extra.mk libaosd.pc
EXTRA = libaosd.pc

include buildsys.mk

install-extra:
	for i in ${EXTRA}; do \
	    ${INSTALL_STATUS}; \
	    if ${MKDIR_P} ${DESTDIR}${libdir}/pkgconfig && ${INSTALL} -m 644 $$i ${DESTDIR}${libdir}/pkgconfig/$$i; then \
	    	${INSTALL_OK}; \
	    else \
	    	${INSTALL_FAILED}; \
	    fi \
	done

uninstall-extra:
	for i in ${EXTRA}; do \
	    if [ -f ${DESTDIR}${libdir}/pkgconfig/$$i ]; then \
		    if rm -f ${DESTDIR}${libdir}/pkgconfig/$$i; then \
			    ${DELETE_OK}; \
		    else \
			    ${DELETE_FAILED}; \
		    fi \
	    fi \
	done
