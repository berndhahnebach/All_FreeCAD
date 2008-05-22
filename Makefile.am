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
	package=`$(PYTHON) -c "import sys; \
			package = sys.argv[1]; \
			package = package.lower(); \
			print package" $(PACKAGE)` ; \
	svnver=`$(PYTHON) -c "import os,sys,string,re; \
			srcdir = sys.argv[1]; \
			rev = os.popen('svnversion ' + srcdir + ' -n').read(); \
			rev = rev[rev.find(':')+1:]; \
			rx = re.match('^\\d+',rev); \
			svnver = rev[:rx.end()]; \
			print svnver" $(srcdir)` ; \
	version=`$(PYTHON) -c "import sys; \
			version = '0.7.' + sys.argv[1]; \
			print version" $$svnver` ; \
	$(MAKE) dist ; \
	$(MKDIR_P) debian-build && cd debian-build && \
	$(RM) -r $(PACKAGE)-$(VERSION) && \
	tar zxvf ../$(PACKAGE)-$(VERSION).tar.gz && \
	mv $(PACKAGE)-$(VERSION) $$package-$$version && \
	tar czf $$package"_"$$version.orig.tar.gz $$package-$$version && \
	cd $$package-$$version && \
	upstream=`dpkg-parsechangelog | grep '^Version:' | cut -d' ' -f2 | cut -d'-' -f1` && \
	dch --newversion $$version-1 "New upstream version" && \
	debuild
