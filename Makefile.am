SUBDIRS=src debian

EXTRA_DIST = \
		Installer/BuildInstaller.bat \
		Installer/FreeCAD.wxs \
		Installer/FreeCADBase.wxs \
		Installer/FreeCADDoc.wxs \
		Installer/FreeCADModules.wxs \
		Installer/LibPackDll.wxi \
		Installer/License.rtf \
		Installer/ModImage.wxi \
		Installer/ModMesh.wxi \
		Installer/ModPart.wxi \
		Installer/ModPoints.wxi \
		Installer/ModRaytracing.wxi \
		Installer/Version.wxi.in \
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
