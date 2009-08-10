if(WIN32)
		 add_definitions(-DFCAppPart -DFC_DEBUG -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(WIN32)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(WIN32)

include_directories(
		 ${CMAKE_BINARY_DIR}/src
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR})


if(WIN32)
		 set(Part_LIBS
				 TKFillet
				 TKMesh
		 		 TKernel
		 		 TKG2d
		 		 TKG3d
		 		 TKMath
		 		 TKSTEP
		 		 TKIGES
		 		 TKSTL
		 		 TKShHealing
		 		 TKXSBase
		 		 TKBool
		 		 TKBO
		 		 TKBRep
		 		 TKTopAlgo
		 		 TKGeomAlgo
		 		 TKGeomBase
		 		 TKOffset
		 		 TKPrim
		 		 FreeCADApp)
else(WIN32)
		 set(Part_LIBS
				 TKFillet
				 TKMesh
		 		 TKernel
		 		 TKG2d
		 		 TKG3d
		 		 TKMath
		 		 TKSTEP
		 		 TKIGES
		 		 TKSTL
		 		 TKShHealing
		 		 TKXSBase
		 		 TKBool
		 		 TKBO
		 		 TKBRep
		 		 TKTopAlgo
		 		 TKGeomAlgo
		 		 TKGeomBase
		 		 TKOffset
		 		 TKPrim
		 		 FreeCADApp)
endif(WIN32)

generate_from_xml(ArcPy)
generate_from_xml(CirclePy)
generate_from_xml(EllipsePy)
generate_from_xml(HyperbolaPy)
generate_from_xml(ParabolaPy)
generate_from_xml(OffsetCurvePy)
generate_from_xml(GeometryPy)
generate_from_xml(LinePy)
generate_from_xml(BezierCurvePy)
generate_from_xml(BSplineCurvePy)
generate_from_xml(PlanePy)
generate_from_xml(ConePy)
generate_from_xml(CylinderPy)
generate_from_xml(SpherePy)
generate_from_xml(ToroidPy)
generate_from_xml(BezierSurfacePy)
generate_from_xml(BSplineSurfacePy)
generate_from_xml(OffsetSurfacePy)
generate_from_xml(SurfaceOfExtrusionPy)
generate_from_xml(SurfaceOfRevolutionPy)
generate_from_xml(PartFeaturePy)
generate_from_xml(Part2DObjectPy)
generate_from_xml(OffsetSurfacePy)
generate_from_xml(OffsetSurfacePy)
generate_from_xml(TopoShapePy)
generate_from_xml(TopoShapeCompoundPy)
generate_from_xml(TopoShapeCompSolidPy)
generate_from_xml(TopoShapeEdgePy)
generate_from_xml(TopoShapeFacePy)
generate_from_xml(TopoShapeShellPy)
generate_from_xml(TopoShapeSolidPy)
generate_from_xml(TopoShapeVertexPy)
generate_from_xml(TopoShapeWirePy)

SET(Features_SRCS
		 FeaturePartBoolean.cpp
		 FeaturePartBoolean.h
		 FeaturePartBox.cpp
		 FeaturePartBox.h
		 FeaturePartCircle.cpp
		 FeaturePartCircle.h
		 FeaturePartCommon.cpp
		 FeaturePartCommon.h
		 FeaturePartCurveNet.cpp
		 FeaturePartCurveNet.h
		 FeaturePartCut.cpp
		 FeaturePartCut.h
		 FeaturePartFuse.cpp
		 FeaturePartFuse.h
		 FeaturePartImportBrep.cpp
		 FeaturePartImportBrep.h
		 FeaturePartImportIges.cpp
		 FeaturePartImportIges.h
		 FeaturePartImportStep.cpp
		 FeaturePartImportStep.h
		 FeaturePartPolygon.cpp
		 FeaturePartPolygon.h
		 FeaturePartSection.cpp
		 FeaturePartSection.h
		 FeatureExtrusion.cpp
		 FeatureExtrusion.h
		 FeatureFillet.cpp
		 FeatureFillet.h
		 PartFeature.cpp
		 PartFeature.h
		 Part2DObject.cpp
		 Part2DObject.h
		 PrimitiveFeature.cpp
		 PrimitiveFeature.h
)
SOURCE_GROUP("Features" FILES ${Features_SRCS})

SET(Properties_SRCS
		 PropertyTopoShape.cpp
		 PropertyTopoShape.h
)
SOURCE_GROUP("Properties" FILES ${Properties_SRCS})

SET(Python_SRCS
		 ArcPy.xml
		 ArcPyImp.cpp
		 CirclePy.xml
		 CirclePyImp.cpp
		 EllipsePy.xml
		 EllipsePyImp.cpp
		 HyperbolaPy.xml
		 HyperbolaPyImp.cpp
		 ParabolaPy.xml
		 ParabolaPyImp.cpp
		 OffsetCurvePy.xml
		 OffsetCurvePyImp.cpp
		 GeometryPy.xml
		 GeometryPyImp.cpp
		 LinePy.xml
		 LinePyImp.cpp
		 BezierCurvePy.xml
		 BezierCurvePyImp.cpp
		 BSplineCurvePy.xml
		 BSplineCurvePyImp.cpp
		 PlanePy.xml
		 PlanePyImp.cpp
		 ConePy.xml
		 ConePyImp.cpp
		 CylinderPy.xml
		 CylinderPyImp.cpp
		 SpherePy.xml
		 SpherePyImp.cpp
		 ToroidPy.xml
		 ToroidPyImp.cpp
		 BezierSurfacePy.xml
		 BezierSurfacePyImp.cpp
		 BSplineSurfacePy.xml
		 BSplineSurfacePyImp.cpp
		 OffsetSurfacePy.xml
		 OffsetSurfacePyImp.cpp
		 SurfaceOfExtrusionPy.xml
		 SurfaceOfExtrusionPyImp.cpp
		 SurfaceOfRevolutionPy.xml
		 SurfaceOfRevolutionPyImp.cpp
		 PartFeaturePy.xml
		 PartFeaturePyImp.cpp
		 Part2DObjectPy.xml
		 Part2DObjectPyImp.cpp
		 TopoShapePy.xml
		 TopoShapePyImp.cpp
		 TopoShapeCompSolidPy.xml
		 TopoShapeCompSolidPyImp.cpp
		 TopoShapeCompoundPy.xml
		 TopoShapeCompoundPyImp.cpp
		 TopoShapeEdgePy.xml
		 TopoShapeEdgePyImp.cpp
		 TopoShapeFacePy.xml
		 TopoShapeFacePyImp.cpp
		 TopoShapeShellPy.xml
		 TopoShapeShellPyImp.cpp
		 TopoShapeSolidPy.xml
		 TopoShapeSolidPyImp.cpp
		 TopoShapeVertexPy.xml
		 TopoShapeVertexPyImp.cpp
		 TopoShapeWirePy.xml
		 TopoShapeWirePyImp.cpp
)
SOURCE_GROUP("Python" FILES ${Python_SRCS})

SET(Part_SRCS
		 ${Features_SRCS}
		 ${Properties_SRCS}
		 ${Python_SRCS}
		 AppPart.cpp
		 AppPartPy.cpp
		 Geometry.cpp
		 Geometry.h
		 ImportStep.cpp
		 ImportStep.h
		 PreCompiled.cpp
		 PreCompiled.h
		 ProgressIndicator.cpp
		 ProgressIndicator.h
		 TopoShape.cpp
		 TopoShape.h
)

add_library(Part SHARED ${Part_SRCS})

target_link_libraries(Part ${Part_LIBS})

if(WIN32)
		 set_target_properties(Part PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Part PROPERTIES DEBUG_OUTPUT_NAME "Part_d")
		 set_target_properties(Part PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Part)
		 set_target_properties(Part PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(Part PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Part)
		 set_target_properties(Part PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Part DESTINATION lib)
