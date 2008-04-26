SUBDIRS=src debian

EXTRA_DIST = \
		FreeCAD.sln \
		autogen.sh \
		build.sh \
		ChangeLog.txt \
		copying.lib \
		package.sh \
		README.Cygwin \
		README.Linux \
		README.Win32

debian-package:
	if test x"$(CASROOT)" = x; then \
		echo "**** ERROR: the environment variable CASROOT is not set."; \
		echo "First set CASROOT to point to the ros subdirectory of the"; \
		echo "OpenCascade installation directory and retry make debian-package. ****"; \
		exit 1; \
	fi
	$(MAKE) dist
	-mkdir debian-build
	cd debian-build && \
	$(RM) -r $(PACKAGE)-$(VERSION) && \
	tar zxvf ../$(PACKAGE)-$(VERSION).tar.gz && \
	cd $(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot
