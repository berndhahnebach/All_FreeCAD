SUBDIRS=src debian

EXTRA_DIST = \
		FreeCAD.sln \
		autogen.sh \
		ChangeLog.txt \
		copying.lib \
		README.Cygwin \
		README.Linux \
		README.Win32

debian-package: dist
	-mkdir debian-build
	cd debian-build && \
	$(RM) -r $(PACKAGE)-$(VERSION) && \
	tar zxvf ../$(PACKAGE)-$(VERSION).tar.gz && \
	cd $(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot
