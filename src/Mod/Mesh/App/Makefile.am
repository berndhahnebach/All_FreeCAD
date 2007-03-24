if OCC_LIBRARY_FILES
if WM3_LIBRARY_FILES
if GTS_LIBRARY_FILES

lib_LTLIBRARIES=libMesh.la Mesh.la

libMesh_la_SOURCES=\
		Core/Algorithm.cpp \
		Core/Algorithm.h \
		Core/Approximation.cpp \
		Core/Approximation.h \
		Core/Builder.cpp \
		Core/Builder.h \
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
		Core/SetOperations.cpp \
		Core/SetOperations.h \
		Core/triangle.cpp \
		Core/triangle.h \
		Core/tritritest.h \
		Core/Tools.cpp \
		Core/Tools.h \
		Core/TopoAlgorithm.cpp \
		Core/TopoAlgorithm.h \
		Core/Visitor.cpp \
		Core/Visitor.h \
		AppMeshPy.cpp \
		CurveProjector.cpp \
		CurveProjector.h \
		Doxygen.cpp \
		FeatureMeshCurvature.cpp \
		FeatureMeshCurvature.h \
		FeatureMeshExport.cpp \
		FeatureMeshExport.h \
		FeatureMeshDefects.cpp \
		FeatureMeshDefects.h \
		FeatureMeshImport.cpp \
		FeatureMeshImport.h \
		FeatureMeshSegmentByMesh.cpp \
		FeatureMeshSegmentByMesh.h \
		FeatureMeshSetOperations.cpp \
		FeatureMeshSetOperations.h \
		FeatureMeshSolid.cpp \
		FeatureMeshSolid.h \
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
libMesh_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -L$(OCC_LIB) $(WM3_LDFLAGS) $(GTS_LIBS) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libMesh_la_CPPFLAGS = -DAppMeshExport= -DWM3_VERSION=$(WM3_VERSION)

libMesh_la_LIBADD   = \
		-lxerces-c \
		-l@PYTHON_LIB@ \
		-lFreeCADBase \
		-lFreeCADApp \
		$(WM3_LIBS) \
		-lTKernel \
		-lTKG2d \
		-lTKG3d \
		-lTKMath \
		-lTKService \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKBRep \
		-lTKTopAlgo

#--------------------------------------------------------------------------------------
# Loader of libMesh

Mesh_la_SOURCES=\
		AppMesh.cpp

# the library search path.
Mesh_la_LDFLAGS = $(libMesh_la_LDFLAGS) -module -avoid-version
Mesh_la_CPPFLAGS = $(libMesh_la_CPPFLAGS)

Mesh_la_LIBADD   = \
		$(libMesh_la_LIBADD) \
		-lMesh

#--------------------------------------------------------------------------------------

# set the include path found by configure
INCLUDES= $(all_includes) -I$(OCC_INC) $(WM3_CFLAGS) $(GTS_CFLAGS) -I$(top_srcdir)/src

endif
endif
endif

libdir = $(prefix)/Mod/Mesh

EXTRA_DIST = \
		AppMesh.vcproj
