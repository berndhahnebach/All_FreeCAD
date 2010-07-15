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
		cMake/UseLibPackCustom.cmake \
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
	$(top_srcdir)/package/makedebian.sh

