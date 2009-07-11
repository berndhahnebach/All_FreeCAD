SUBDIRS=src

EXTRA_DIST = \
		cMakeFiles/ConfigureChecks.cmake \
		cMakeFiles/FindCoin3D.cmake \
		cMakeFiles/FindOpenCasCade.cmake \
		cMakeFiles/FindOpenCV.cmake \
		cMakeFiles/FindSoQt.cmake \
		cMakeFiles/FindXercesC.cmake \
		config.h.cmake \
		CMakeLists.txt \
		autogen.sh \
		build.sh \
		ChangeLog.txt \
		copying.lib \
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
