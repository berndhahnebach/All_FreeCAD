ACLOCAL_AMFLAGS = -I m4

SUBDIRS=src

EXTRA_DIST = \
		cMake/ConfigureChecks.cmake \
		cMake/FindCoin3D.cmake \
		cMake/FindEigen2.cmake \
		cMake/FindODE.cmake \
		cMake/FindOpenCasCade.cmake \
		cMake/FindOpenCV.cmake \
		cMake/FindSoQt.cmake \
		cMake/FindXercesC.cmake \
		cMake/FreeCadMacros.cmake \
		cMake/UseLibPack6x.cmake \
		cMake/UseLibPack7x.cmake \
		config.h.cmake \
		CMakeLists.txt \
		autogen.sh \
		BuildAll.bat \
		BuildAllNice.bat \
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
