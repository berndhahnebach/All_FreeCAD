
lib_LTLIBRARIES=libRaytracing.la

BUILT_SOURCES=\
		FreeCADpov.h

libRaytracing_la_SOURCES=\
		AppRaytracing.cpp \
		AppRaytracingPy.cpp \
		FeatureRayExportPov.cpp \
		FeatureRayExportPov.h \
		PovTools.cpp \
		PovTools.h \
		PreCompiled.cpp \
		PreCompiled.h \
		RayFeature.cpp \
		RayFeature.h


FreeCADpov.h: FreeCADpov
	$(PYTHON) $(top_srcdir)/../../Tools/PythonToCPP.py $(<F) $@


# the library search path.
libRaytracing_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

libRaytracing_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp \
		-lPart

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

# We need this softlink for Python to load
install-data-local:
		cd $(prefix) && \
		rm -f Raytracing.so && \
		$(LN_S) lib/libRaytracing.so Raytracing.so
