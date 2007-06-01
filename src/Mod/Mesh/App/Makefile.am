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
		WildMagic4/Wm4ApprPlaneFit3.cpp \
		WildMagic4/Wm4ApprPlaneFit3.h \
		WildMagic4/Wm4ApprQuadraticFit3.cpp \
		WildMagic4/Wm4ApprQuadraticFit3.h \
		WildMagic4/Wm4BandedMatrix.h \
		WildMagic4/Wm4BandedMatrix.inl \
		WildMagic4/Wm4Box3.h \
		WildMagic4/Wm4Box3.inl \
		WildMagic4/Wm4DelPolygonEdge.cpp \
		WildMagic4/Wm4DelPolygonEdge.h \
		WildMagic4/Wm4DelPolyhedronFace.cpp \
		WildMagic4/Wm4DelPolyhedronFace.h \
		WildMagic4/Wm4DelTetrahedron.cpp \
		WildMagic4/Wm4DelTetrahedron.h \
		WildMagic4/Wm4DelTriangle.cpp \
		WildMagic4/Wm4DelTriangle.h \
		WildMagic4/Wm4Delaunay.cpp \
		WildMagic4/Wm4Delaunay.h \
		WildMagic4/Wm4Delaunay1.cpp \
		WildMagic4/Wm4Delaunay1.h \
		WildMagic4/Wm4Delaunay2.cpp \
		WildMagic4/Wm4Delaunay2.h \
		WildMagic4/Wm4Delaunay3.cpp \
		WildMagic4/Wm4Delaunay3.h \
		WildMagic4/Wm4DistLine3Segment3.cpp \
		WildMagic4/Wm4DistLine3Segment3.h \
		WildMagic4/Wm4DistLine3Triangle3.cpp \
		WildMagic4/Wm4DistLine3Triangle3.h \
		WildMagic4/Wm4DistSegment3Segment3.cpp \
		WildMagic4/Wm4DistSegment3Segment3.h \
		WildMagic4/Wm4DistSegment3Triangle3.cpp \
		WildMagic4/Wm4DistSegment3Triangle3.h \
		WildMagic4/Wm4DistVector3Plane3.cpp \
		WildMagic4/Wm4DistVector3Plane3.h \
		WildMagic4/Wm4DistVector3Segment3.cpp \
		WildMagic4/Wm4DistVector3Segment3.h \
		WildMagic4/Wm4DistVector3Triangle3.cpp \
		WildMagic4/Wm4DistVector3Triangle3.h \
		WildMagic4/Wm4Distance.cpp \
		WildMagic4/Wm4Distance.h \
		WildMagic4/Wm4ETManifoldMesh.cpp \
		WildMagic4/Wm4ETManifoldMesh.h \
		WildMagic4/Wm4ETManifoldMesh.inl \
		WildMagic4/Wm4EdgeKey.h \
		WildMagic4/Wm4EdgeKey.inl \
		WildMagic4/Wm4Eigen.cpp \
		WildMagic4/Wm4Eigen.h \
		WildMagic4/Wm4Foundation.h \
		WildMagic4/Wm4FoundationLIB.h \
		WildMagic4/Wm4FoundationPCH.cpp \
		WildMagic4/Wm4FoundationPCH.h \
		WildMagic4/Wm4GMatrix.h \
		WildMagic4/Wm4GMatrix.inl \
		WildMagic4/Wm4GVector.h \
		WildMagic4/Wm4GVector.inl \
		WildMagic4/Wm4ImplicitSurface.cpp \
		WildMagic4/Wm4ImplicitSurface.h \
		WildMagic4/Wm4Intersector.cpp \
		WildMagic4/Wm4Intersector.h \
		WildMagic4/Wm4Intersector1.cpp \
		WildMagic4/Wm4Intersector1.h \
		WildMagic4/Wm4IntrLine3Box3.cpp \
		WildMagic4/Wm4IntrLine3Box3.h \
		WildMagic4/Wm4IntrLine3Plane3.cpp \
		WildMagic4/Wm4IntrLine3Plane3.h \
		WildMagic4/Wm4IntrSegment3Box3.cpp \
		WildMagic4/Wm4IntrSegment3Box3.h \
		WildMagic4/Wm4IntrSegment3Plane3.cpp \
		WildMagic4/Wm4IntrSegment3Plane3.h \
		WildMagic4/Wm4IntrTriangle2Triangle2.cpp \
		WildMagic4/Wm4IntrTriangle2Triangle2.h \
		WildMagic4/Wm4IntrTriangle3Triangle3.cpp \
		WildMagic4/Wm4IntrTriangle3Triangle3.h \
		WildMagic4/Wm4LinComp.h \
		WildMagic4/Wm4LinComp.inl \
		WildMagic4/Wm4Line3.h \
		WildMagic4/Wm4Line3.inl \
		WildMagic4/Wm4LinearSystem.cpp \
		WildMagic4/Wm4LinearSystem.h \
		WildMagic4/Wm4Mapper2.h \
		WildMagic4/Wm4Mapper2.inl \
		WildMagic4/Wm4Mapper3.h \
		WildMagic4/Wm4Mapper3.inl \
		WildMagic4/Wm4Math.cpp \
		WildMagic4/Wm4Math.h \
		WildMagic4/Wm4Math.inl \
		WildMagic4/Wm4MathMCR.h \
		WildMagic4/Wm4Matrix2.cpp \
		WildMagic4/Wm4Matrix2.h \
		WildMagic4/Wm4Matrix2.inl \
		WildMagic4/Wm4Matrix3.cpp \
		WildMagic4/Wm4Matrix3.h \
		WildMagic4/Wm4Matrix3.inl \
		WildMagic4/Wm4Matrix4.cpp \
		WildMagic4/Wm4Matrix4.h \
		WildMagic4/Wm4Matrix4.inl \
		WildMagic4/Wm4Memory.cpp \
		WildMagic4/Wm4Memory.h \
		WildMagic4/Wm4Memory.inl \
		WildMagic4/Wm4MeshCurvature.cpp \
		WildMagic4/Wm4MeshCurvature.h \
		WildMagic4/Wm4MeshSmoother.cpp \
		WildMagic4/Wm4MeshSmoother.h \
		WildMagic4/Wm4ParametricSurface.cpp \
		WildMagic4/Wm4ParametricSurface.h \
		WildMagic4/Wm4Plane3.h \
		WildMagic4/Wm4Plane3.inl \
		WildMagic4/Wm4Platforms.h \
		WildMagic4/Wm4QuadricSurface.cpp \
		WildMagic4/Wm4QuadricSurface.h \
		WildMagic4/Wm4Query.h \
		WildMagic4/Wm4Query.inl \
		WildMagic4/Wm4Query2.h \
		WildMagic4/Wm4Query2.inl \
		WildMagic4/Wm4Query2Filtered.h \
		WildMagic4/Wm4Query2Filtered.inl \
		WildMagic4/Wm4Query2Int64.h \
		WildMagic4/Wm4Query2Int64.inl \
		WildMagic4/Wm4Query2TInteger.h \
		WildMagic4/Wm4Query2TInteger.inl \
		WildMagic4/Wm4Query2TRational.h \
		WildMagic4/Wm4Query2TRational.inl \
		WildMagic4/Wm4Query3.h \
		WildMagic4/Wm4Query3.inl \
		WildMagic4/Wm4Query3Filtered.h \
		WildMagic4/Wm4Query3Filtered.inl \
		WildMagic4/Wm4Query3Int64.h \
		WildMagic4/Wm4Query3Int64.inl \
		WildMagic4/Wm4Query3TInteger.h \
		WildMagic4/Wm4Query3TInteger.inl \
		WildMagic4/Wm4Query3TRational.h \
		WildMagic4/Wm4Query3TRational.inl \
		WildMagic4/Wm4RVector2.h \
		WildMagic4/Wm4RVector2.inl \
		WildMagic4/Wm4RVector3.h \
		WildMagic4/Wm4RVector3.inl \
		WildMagic4/Wm4Segment3.h \
		WildMagic4/Wm4Segment3.inl \
		WildMagic4/Wm4Sphere3.h \
		WildMagic4/Wm4Sphere3.inl \
		WildMagic4/Wm4Surface.cpp \
		WildMagic4/Wm4Surface.h \
		WildMagic4/Wm4System.cpp \
		WildMagic4/Wm4System.h \
		WildMagic4/Wm4System.inl \
		WildMagic4/Wm4THashSet.h \
		WildMagic4/Wm4THashSet.inl \
		WildMagic4/Wm4THashTable.h \
		WildMagic4/Wm4THashTable.inl \
		WildMagic4/Wm4TInteger.h \
		WildMagic4/Wm4TInteger.inl \
		WildMagic4/Wm4TMinHeap.h \
		WildMagic4/Wm4TMinHeap.inl \
		WildMagic4/Wm4TRVector.h \
		WildMagic4/Wm4TRVector.inl \
		WildMagic4/Wm4TRational.h \
		WildMagic4/Wm4TRational.inl \
		WildMagic4/Wm4TSmallUnorderedSet.h \
		WildMagic4/Wm4TSmallUnorderedSet.inl \
		WildMagic4/Wm4TStringHashTable.h \
		WildMagic4/Wm4TStringHashTable.inl \
		WildMagic4/Wm4TTuple.h \
		WildMagic4/Wm4TTuple.inl \
		WildMagic4/Wm4Triangle2.h \
		WildMagic4/Wm4Triangle2.inl \
		WildMagic4/Wm4Triangle3.h \
		WildMagic4/Wm4Triangle3.inl \
		WildMagic4/Wm4TriangleKey.h \
		WildMagic4/Wm4TriangleKey.inl \
		WildMagic4/Wm4TriangulateEC.cpp \
		WildMagic4/Wm4TriangulateEC.h \
		WildMagic4/Wm4UniqueVerticesTriangles.h \
		WildMagic4/Wm4UniqueVerticesTriangles.inl \
		WildMagic4/Wm4VEManifoldMesh.cpp \
		WildMagic4/Wm4VEManifoldMesh.h \
		WildMagic4/Wm4VEManifoldMesh.inl \
		WildMagic4/Wm4Vector2.cpp \
		WildMagic4/Wm4Vector2.h \
		WildMagic4/Wm4Vector2.inl \
		WildMagic4/Wm4Vector3.cpp \
		WildMagic4/Wm4Vector3.h \
		WildMagic4/Wm4Vector3.inl \
		WildMagic4/Wm4Vector4.cpp \
		WildMagic4/Wm4Vector4.h \
		WildMagic4/Wm4Vector4.inl \
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
INCLUDES= $(all_includes) -I$(OCC_INC) $(WM3_CFLAGS) $(GTS_CFLAGS) -I$(top_srcdir)/src -I$(top_builddir)/src

endif
endif
endif

libdir = $(prefix)/Mod/Mesh

EXTRA_DIST = \
		AppMesh.vcproj
