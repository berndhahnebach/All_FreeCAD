
lib_LTLIBRARIES=libMesh.la

libMesh_la_SOURCES=\
		Core/Algorithm.cpp \
		Core/Algorithm.h \
		Core/Approximation.cpp \
		Core/Approximation.h \
		Core/Definitions.cpp \
		Core/Definitions.h \
		Core/Degeneration.cpp \
		Core/Degeneration.h \
		Core/Elements.cpp \
		Core/Elements.h \
		Core/Evaluation.cpp \
		Core/Evaluation.h \
		Core/Grid.cpp \
		Core/Grid.h \
		Core/Helpers.h \
		Core/Info.cpp \
		Core/Info.h \
		Core/Iterator.h \
		Core/MeshKernel.cpp \
		Core/MeshKernel.h \
		Core/MeshIO.cpp \
		Core/MeshIO.h \
		Core/Projection.cpp \
		Core/Projection.h \
		Core/Tools.cpp \
		Core/Tools.h \
		Core/TopoAlgorithm.cpp \
		Core/TopoAlgorithm.h \
		Core/Visitor.cpp \
		Core/Visitor.h \
		AppMesh.cpp \
		AppMeshPy.cpp \
		CurveProjector.cpp \
		CurveProjector.h \
		Doxygen.cpp \
		FeatureMeshCurvature.cpp \
		FeatureMeshCurvature.h \
		FeatureMeshExport.cpp \
		FeatureMeshExport.h \
		FeatureMeshImport.cpp \
		FeatureMeshImport.h \
		FeatureMeshSegmentByMesh.cpp \
		FeatureMeshSegmentByMesh.h \
		FeatureMeshTransform.cpp \
		FeatureMeshTransform.h \
		FeatureMeshTransformDemolding.cpp \
		FeatureMeshTransformDemolding.h \
		Mesh.cpp \
		Mesh.h \
		MeshAlgos.cpp \
		MeshAlgos.h \
		MeshCurvature.cpp \
		MeshCurvature.h \
		MeshFeature.cpp \
		MeshFeature.h \
		MeshFeaturePy.cpp \
		MeshFeaturePy.h \
		MeshPy.cpp \
		MeshPy.h \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libMesh_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) $(WM3_LIBS) $(GTS_LIBS) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libMesh_la_CPPFLAGS = -DAppMeshExport= -DWM3_VERSION=$(WM3_VERSION)

libMesh_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp 

# set the include path found by configure
INCLUDES= $(all_includes) $(WM3_CFLAGS) $(GTS_CFLAGS) -I../../../

libdir = $(prefix)/Mod/Mesh

# We need this softlink for Python to load
install-data-local:
		cd $(libdir) && \
		rm -f Mesh.so && \
		$(LN_S) libMesh.so Mesh.so

EXTRA_DIST = \
		AppMesh.dsp \
		AppMesh.vcproj \
		Libs.cpp
