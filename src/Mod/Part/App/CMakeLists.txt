add_definitions(-DFCAppPart -DFC_DEBUG)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Part_LIBS
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
#libPart_la_LIBADD		 = \
#		 		 -lxerces-c \
#		 		 -lboost_filesystem \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lTKernel \
#		 		 -lTKG2d \
#		 		 -lTKG3d \
#		 		 -lTKMath \
#		 		 -lTKSTEP \
#		 		 -lTKIGES \
#		 		 -lTKSTL \
#		 		 -lTKShHealing \
#		 		 -lTKXSBase \
#		 		 -lTKBool \
#		 		 -lTKBO \
#		 		 -lTKBRep \
#		 		 -lTKTopAlgo \
#		 		 -lTKGeomAlgo \
#		 		 -lTKGeomBase \
#		 		 -lTKOffset \
#		 		 -lTKPrim
endif(WIN32)

generate_from_xml(PartFeaturePy)
generate_from_xml(TopoShapeFacePy)
generate_from_xml(TopoShapePy)

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
		 FeaturePartLine.cpp
		 FeaturePartLine.h
		 FeaturePartLineSet.cpp
		 FeaturePartLineSet.h
		 FeaturePartPolygon.cpp
		 FeaturePartPolygon.h
		 FeaturePartSection.cpp
		 FeaturePartSection.h
		 PartFeature.cpp
		 PartFeature.h
		 PrimitiveFeature.cpp
		 PrimitiveFeature.h
)
SOURCE_GROUP("Features" FILES ${Features_SRCS})

SET(Properties_SRCS
		 PropertyCircle.cpp
		 PropertyCircle.h
		 PropertyLine.cpp
		 PropertyLine.h
)
SOURCE_GROUP("Properties" FILES ${Properties_SRCS})

SET(Python_SRCS
		 CirclePy.cpp
		 CirclePy.h
		 LinePy.cpp
		 LinePy.h
		 PartFeaturePy.xml
		 PartFeaturePyImp.cpp
		 TopologyPy.cpp
		 TopologyPy.h
)
SOURCE_GROUP("Python" FILES ${Python_SRCS})

SET(Part_SRCS
		 ${Features_SRCS}
		 ${Properties_SRCS}
		 ${Python_SRCS}
		 AppPart.cpp
		 AppPartPy.cpp
		 ImportStep.cpp
		 ImportStep.h
		 PreCompiled.cpp
		 PreCompiled.h
		 TopoShape.cpp
		 TopoShape.h
		 TopoShapeFacePy.xml
		 TopoShapeFacePyImp.cpp
		 TopoShapePy.xml
		 TopoShapePyImp.cpp
)

add_library(Part SHARED ${Part_SRCS})

target_link_libraries(Part ${Part_LIBS})

set_target_properties(Part PROPERTIES SUFFIX ".pyd")

install(TARGETS Part DESTINATION lib)
