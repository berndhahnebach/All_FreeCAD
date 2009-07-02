SUBDIRS=src

EXTRA_DIST = \
		cmake/ConfigureChecks.cmake \
		cmake/FindCoin3D.cmake \
		cmake/FindOpenCasCade.cmake \
		cmake/FindOpenCV.cmake \
		cmake/FindSoQt.cmake \
		cmake/FindXercesC.cmake \
		config.h.cmake \
		CMakeLists.txt \
		autogen.sh \
		build.sh \
		ChangeLog.txt \
		copying.lib \
		package.sh \
		README.Linux \
		README.Win32

debian-package:
	$(MAKE) dist
	$(MKDIR_P) debian-build && cd debian-build && \
	$(RM) -r $(PACKAGE)-$(VERSION) && \
	tar zxvf ../$(PACKAGE)-$(VERSION).tar.gz && \
	cd $(PACKAGE)-$(VERSION) && \
	package=`$(PYTHON) -c "import sys; \
			package = sys.argv[1]; \
			package = package.lower(); \
			print package" $(PACKAGE)` ; \
	version=`dpkg-parsechangelog | grep '^Version:' | cut -d' ' -f2 | cut -d'-' -f1` && \
	cd .. && $(RM) -r $$package-$$version && \
	mv $(PACKAGE)-$(VERSION) $$package-$$version && \
	cd $$package-$$version && \
	debuild
