include extra.mk

SUBDIRS = libaosd ${TEXT_DIR} examples
DISTCLEAN = buildsys.mk extra.mk \
	    config.h config.log config.status \
	    libaosd.pc ${TEXT_PKGCONF}
EXTRA = libaosd.pc ${TEXT_PKGCONF}

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
