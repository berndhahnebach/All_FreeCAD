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
	$(MAKE) dist
	-mkdir debian-build
	cd debian-build && \
	$(RM) -r $(PACKAGE)-$(VERSION) && \
	tar zxvf ../$(PACKAGE)-$(VERSION).tar.gz && \
	cd $(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot
