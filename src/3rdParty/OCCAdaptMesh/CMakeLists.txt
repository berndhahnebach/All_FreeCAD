project(OCCAdaptMesh)


set(OCC_Includes
c:/FreeCAD/FreeCADLibs4/include/OpenCascade62
)

set(OCCAdaptMesh_Include
${CMAKE_CURRENT_SOURCE_DIR}/Include
)

include_directories(${OCCAdaptMesh_Include} ${OCC_Includes})

set(FreeCAD_LibPack_Libs
c:/FreeCAD/FreeCADLibs4/lib
)

add_definitions(-DWNT -DNO_TIMER -D_CRT_SECURE_NO_WARNINGS)





set(OCC_Libs
${FreeCAD_LibPack_Libs}/TKBRep.lib
${FreeCAD_LibPack_Libs}/TKernel.lib
${FreeCAD_LibPack_Libs}/TKG2d.lib
${FreeCAD_LibPack_Libs}/TKG3d.lib
${FreeCAD_LibPack_Libs}/TKGeomAlgo.lib
${FreeCAD_LibPack_Libs}/TKGeomBase.lib
${FreeCAD_LibPack_Libs}/TKMath.lib
${FreeCAD_LibPack_Libs}/TKMesh.lib
${FreeCAD_LibPack_Libs}/TKTopAlgo.lib

)



set(OCCAdaptMesh_BRepMeshAdapt_SRCS
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_HArray1OfVertexOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_VertexHasher_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/meshtriangleAdapt.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_Triangle.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_Array1OfBiPoint_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_SurfaceGrid.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_Array1OfVertexOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_BiPoint.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ShapeTool.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_SelectorOfDataStructureOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/meshchronoAdapt.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_NodeHasherOfDataStructureOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_Classifier.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ListOfXY_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ListOfVertex_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ListOfSurfaceGrid_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ListNodeOfListOfXY_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ListNodeOfListOfVertex_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ComparatorOfIndexedVertexOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ListNodeOfListOfSurfaceGrid_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ComparatorOfVertexOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ListIteratorOfListOfXY_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapIteratorOfDataMapOfCouplePnt_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ListIteratorOfListOfVertex_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapIteratorOfDataMapOfIntegerFace_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ListIteratorOfListOfSurfaceGrid_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapIteratorOfDataMapOfIntegerListOfXY_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_LinkHasherOfDataStructureOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapIteratorOfDataMapOfMeshVertexInteger_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_IndexedMapOfVertex_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapIteratorOfDataMapOfShapeEdgeDiscret_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_IndexedMapNodeOfIndexedMapOfVertex_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapIteratorOfDataMapOfShapeReal_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_IndexedMapNodeOfIMapOfElementOfDataStructureOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapIteratorOfDataMapOfShapeSurfaceGrid_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapIteratorOfDataMapOfVertexInteger_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfLinkOfDataStructureOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapNodeOfDataMapOfCouplePnt_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_IncrementalMesh.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_IMapOfElementOfDataStructureOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerListOfXY_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_IDMapOfNodeOfDataStructureOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapNodeOfDataMapOfMeshVertexInteger_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_IDMapOfLinkOfDataStructureOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapNodeOfDataMapOfShapeEdgeDiscret_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_HeapSortVertexOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapNodeOfDataMapOfShapeReal_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_HeapSortIndexedVertexOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapNodeOfDataMapOfShapeSurfaceGrid_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapNodeOfDataMapOfVertexInteger_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_GeomTool_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapOfCouplePnt_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_FastDiscret.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapOfIntegerFace_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_ElemHasherOfDataStructureOfDelaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapOfIntegerListOfXY_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_EdgeDiscret.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapOfMeshVertexInteger_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_Vertex.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapOfShapeEdgeDiscret_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_Edge.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapOfShapeReal_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_Discret.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapOfShapeSurfaceGrid_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_Delaun_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataMapOfVertexInteger_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepMeshAdapt/BRepMeshAdapt_DataStructureOfDelaun_0.cxx
)

set(OCCAdaptMesh_MeshAlgoAdapt_SRCS
${CMAKE_CURRENT_SOURCE_DIR}/src/MeshAlgoAdapt/MeshAlgoAdapt.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/MeshAlgoAdapt/MeshAlgoAdapt_Vertex.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/MeshAlgoAdapt/MeshAlgoAdapt_Triangle.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/MeshAlgoAdapt/MeshAlgoAdapt_Edge.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/MeshAlgoAdapt/MeshAlgoAdapt_Circ.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/MeshAlgoAdapt/MeshAlgoAdapt_DataMapOfIntegerCirc_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/MeshAlgoAdapt/MeshAlgoAdapt_DataMapIteratorOfDataMapOfIntegerCirc_0.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/MeshAlgoAdapt/MeshAlgoAdapt_CircleTool.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/MeshAlgoAdapt/MeshAlgoAdapt_DataMapNodeOfDataMapOfIntegerCirc_0.cxx
)

set(OCCAdaptMesh_TriangleAdapt_SRCS
${CMAKE_CURRENT_SOURCE_DIR}/src/TriangleAdapt/triangleAdapt.c
${CMAKE_CURRENT_SOURCE_DIR}/src/TriangleAdapt/occcallbackAdapt.cxx
)

set(OCCAdaptMesh_BREPUTILS_SRCS
${CMAKE_CURRENT_SOURCE_DIR}/src/BRepUtils.cpp
)

set(OCCAdaptMesh_BREPUTILS_HDRS
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepUtils.h
)

set(OCCAdaptMesh_TriangleAdapt_HDRS
${CMAKE_CURRENT_SOURCE_DIR}/src/TriangleAdapt/TriangleAdapt_CMPLRS.edl
${CMAKE_CURRENT_SOURCE_DIR}/Include/TriangleAdapt_Parameters.h
${CMAKE_CURRENT_SOURCE_DIR}/Include/triangleAdapt.h
${CMAKE_CURRENT_SOURCE_DIR}/Include/TriangleAdapt.hxx
)

set(OCCAdaptMesh_MeshAlgoAdapt_HDRS
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Delaunay.gxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_PntComparator.gxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_IndexedPntComparator.gxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Triangle.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_DataMapIteratorOfDataMapOfIntegerCirc.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_CircleTool.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_DataMapNodeOfDataMapOfIntegerCirc.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_MeshAlgoAdapt_DataMapNodeOfDataMapOfIntegerCirc.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_DataMapOfIntegerCirc.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Circ.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Vertex.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Edge.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Triangle.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Edge.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_CircleTool.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Vertex.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Circ.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Triangle.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Edge.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Circ.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_CircleTool.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Vertex.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Circ.lxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/MeshAlgoAdapt_Delaunay.lxx
)



set(OCCAdaptMesh_BRepMeshAdapt_HDRS
${CMAKE_CURRENT_SOURCE_DIR}/Include/meshtriangleAdapt.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/meshchronoAdapt.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Delaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapOfVertexInteger.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapOfShapeSurfaceGrid.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Discret.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_IndexedMapNodeOfIndexedMapOfVertex.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_IndexedMapNodeOfIMapOfElementOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapOfShapeReal.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Edge.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapOfShapeEdgeDiscret.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_HArray1OfVertexOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapOfMeshVertexInteger.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_EdgeDiscret.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_FastDiscret.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_EdgeDiscret.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ElemHasherOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapOfIntegerListOfXY.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapOfIntegerFace.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_FastDiscret.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_Discret.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_DataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfVertexInteger.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_GeomTool.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapOfCouplePnt.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapNodeOfDataMapOfVertexInteger.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_HeapSortIndexedVertexOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapNodeOfDataMapOfShapeSurfaceGrid.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_HeapSortVertexOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapNodeOfDataMapOfShapeReal.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IDMapOfLinkOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapNodeOfDataMapOfShapeEdgeDiscret.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IDMapOfNodeOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapNodeOfDataMapOfMeshVertexInteger.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IMapOfElementOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerListOfXY.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IncrementalMesh.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfShapeSurfaceGrid.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfShapeReal.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfLinkOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapNodeOfDataMapOfCouplePnt.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapIteratorOfDataMapOfVertexInteger.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IndexedMapNodeOfIMapOfElementOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapIteratorOfDataMapOfShapeSurfaceGrid.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IndexedMapNodeOfIndexedMapOfVertex.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapIteratorOfDataMapOfShapeReal.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IndexedMapOfVertex.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapIteratorOfDataMapOfShapeEdgeDiscret.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_LinkHasherOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapIteratorOfDataMapOfMeshVertexInteger.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ListIteratorOfListOfSurfaceGrid.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapIteratorOfDataMapOfIntegerListOfXY.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ListIteratorOfListOfVertex.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapIteratorOfDataMapOfIntegerFace.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ListIteratorOfListOfXY.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_DataMapIteratorOfDataMapOfCouplePnt.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ListNodeOfListOfSurfaceGrid.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ComparatorOfVertexOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ListNodeOfListOfVertex.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ComparatorOfIndexedVertexOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ListNodeOfListOfXY.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_ListNodeOfListOfSurfaceGrid.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ListOfSurfaceGrid.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_ListNodeOfListOfVertex.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ListOfVertex.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_ListNodeOfListOfXY.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ListOfXY.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Classifier.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_NodeHasherOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_SurfaceGrid.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_SelectorOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_BiPoint.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ShapeTool.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfShapeEdgeDiscret.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfMeshVertexInteger.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerListOfXY.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Status.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Array1OfVertexOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_SurfaceGrid.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfCouplePnt.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Array1OfBiPoint.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Triangle.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_VertexHasher.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Vertex.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/Handle_BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfLinkOfDataStructureOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_HArray1OfVertexOfDelaun.hxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Vertex.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Discret.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Edge.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Triangle.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_EdgeDiscret.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_SurfaceGrid.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_FastDiscret.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_BiPoint.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ShapeTool.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IncrementalMesh.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Classifier.ixx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ShapeTool.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_FastDiscret.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_EdgeDiscret.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_SurfaceGrid.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_BiPoint.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Edge.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Triangle.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Discret.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Vertex.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_IncrementalMesh.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Classifier.jxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_BiPoint.lxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Edge.lxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_FastDiscret.lxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_ShapeTool.lxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Vertex.lxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Triangle.lxx
${CMAKE_CURRENT_SOURCE_DIR}/Include/BRepMeshAdapt_Classifier.lxx
)

set(OCCAdaptMesh_SRCS
${OCCAdaptMesh_TriangleAdapt_SRCS}
${OCCAdaptMesh_BRepMeshAdapt_SRCS}
${OCCAdaptMesh_MeshAlgoAdapt_SRCS}
${OCCAdaptMesh_BREPUTILS_SRCS}
)

set(OCCAdaptMesh_HDRS
${OCCAdaptMesh_TriangleAdapt_HDRS}
${OCCAdaptMesh_BRepMeshAdapt_HDRS}
${OCCAdaptMesh_MeshAlgoAdapt_HDRS}
${OCCAdaptMesh_BREPUTILS_HDRS}
)




ADD_LIBRARY(OCCAdaptMesh SHARED ${OCCAdaptMesh_SRCS} ${OCCAdaptMesh_HDRS})

target_link_libraries(OCCAdaptMesh ${OCC_Libs})
