
lib_LTLIBRARIES=libMesh.la

libMesh_la_SOURCES=\
		Core/Algorithm.cpp \
		Core/Algorithm.h \
		Core/BoundBox.cpp \
		Core/BoundBox.h \
		Core/Definitions.cpp \
		Core/Definitions.h \
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
		Core/Matrix.cpp \
		Core/Matrix.h \
		Core/MeshAlgorithm.cpp \
		Core/MeshAlgorithm.h \
		Core/MeshKernel.cpp \
		Core/MeshKernel.h \
		Core/MeshIO.cpp \
		Core/MeshIO.h \
		Core/Stream.cpp \
		Core/Stream.h \
		Core/Swap.cpp \
		Core/Swap.h \
		Core/Tools.cpp \
		Core/Tools.h \
		Core/TopoAlgorithm.cpp \
		Core/TopoAlgorithm.h \
		Core/Tools2D.cpp \
		Core/Tools2D.h \
		Core/Vector3D.cpp \
		Core/Vector3D.h \
		Core/Visitor.cpp \
		Core/Visitor.h \
		Core/ViewProj.h \
		AppMesh.cpp \
		AppMeshPy.cpp \
		Delaunay.cpp \
		Delaunay.h \
		Doxygen.cpp \
		FeatureMeshImportSTL.cpp \
		FeatureMeshImportSTL.h \
		Mesh.cpp \
		Mesh.h \
		MeshAlgos.cpp \
		MeshAlgos.h \
		MeshFeature.cpp \
		MeshFeature.h \
		MeshFeaturePy.cpp \
		MeshFeaturePy.h \
		MeshPy.cpp \
		MeshPy.h \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libMesh_la_LDFLAGS = -L../../../Base -L../../../App $(all_libraries) -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libMesh_la_CPPFLAGS = -DAppMeshExport=

libMesh_la_LIBADD   = \
		-lFreeCADBase \
		-lFreeCADApp \
		-lFoundation

# set the include path found by configure
INCLUDES= $(all_includes) -I../../../

# We need this softlink for Python to load
install-data-local:
		cd $(prefix) && \
		rm -f Mesh.so && \
		$(LN_S) lib/libMesh.so Mesh.so
