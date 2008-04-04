SUBDIRS=src debian

EXTRA_DIST = \
		installer/BuildInstaller.bat \
		installer/BuildRelease.bat \
		installer/CopyRelease.bat.in \
		installer/FreeCAD.wxs \
		installer/FreeCADBase.wxs \
		installer/FreeCADDoc.wxs \
		installer/FreeCADModules.wxs \
		installer/LibPackDll.wxi \
		installer/LibPack.wxs \
		installer/License.rtf \
		installer/ModImage.wxi \
		installer/ModMesh.wxi \
		installer/ModPart.wxi \
		installer/ModPoints.wxi \
		installer/ModRaytracing.wxi \
		installer/Version.wxi.in \
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
