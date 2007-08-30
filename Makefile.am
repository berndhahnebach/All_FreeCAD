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
		installer/LibPack4.wxs \
		installer/License.rtf \
		installer/ModImage.wxi \
		installer/ModMesh.wxi \
		installer/ModPart.wxi \
		installer/ModPoints.wxi \
		installer/ModRaytracing.wxi \
		installer/Version.wxi.in \
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
