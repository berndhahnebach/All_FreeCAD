
lib_LTLIBRARIES=libRaytracing.la

BUILT_SOURCES=\
		FreeCADpov.h

libRaytracing_la_SOURCES=\
		AppRaytracing.cpp \
		AppRaytracingPy.cpp \
		PovTools.cpp \
		PovTools.h \
		PreCompiled.cpp \
		PreCompiled.h \
		RayFeature.cpp \
		RayFeature.h


FreeCADpov.h: FreeCADpov
	$(PYTHON) $(top_srcdir)/Tools/PythonToCPP.py $(<F) $@


# the library search path.
libRaytracing_la_LDFLAGS = -L../../../Base -L../../../App -L../../Part/App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libRaytracing_la_CPPFLAGS = -DAppPartExport= -DFeatureRayExportPov=

libRaytracing_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp \
		-lPart

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

libdir = $(prefix)/Mod/Raytracing

# We need this softlink for Python to load
install-data-local:
		cd $(libdir) && \
		rm -f Raytracing.so && \
		$(LN_S) libRaytracing.so Raytracing.so

EXTRA_DIST = \
		resources/FCAnimation.inc \
		resources/FCAnimation.ini \
		resources/FCAnimation.pov \
		resources/FCSimple.pov \
		AppRaytracing.dsp \
		AppRaytracing.vcproj \
		FreeCADpov \
		Libs.cpp
