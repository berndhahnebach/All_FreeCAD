if(WIN32)
		 add_definitions(-DFCApp)
		 add_definitions(-DBOOST_DYN_LINK)
endif(WIN32)

include_directories(
		 ${CMAKE_CURRENT_SOURCE_DIR}/..
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${PYTHON_INCLUDE_PATH}
	     ${XERCESC_INCLUDE_DIR}
	     ${ZLIB_INCLUDE_DIR})

if(WIN32)
		 set(FreeCADApp_LIBS
		 		FreeCADBase
		 		${Boost_DEBUG_LIBRARIES}
		 		${Boost_LIBRARIES})
else(WIN32)
		 set(FreeCADApp_LIBS
		 		 FreeCADBase
		 		 ${Boost_LIBRARIES})
endif(WIN32)

generate_from_xml(DocumentPy)
generate_from_xml(DocumentObjectPy)
generate_from_xml(DocumentObjectGroupPy)
generate_from_xml(FeaturePy)
generate_from_xml(FeaturePythonPy)
generate_from_xml(ComplexGeoDataPy)
generate_from_xml(PropertyContainerPy)

generate_from_py(FreeCADInit InitScript.h)
generate_from_py(FreeCADTest TestScript.h)

SET(Document_SRCS
		 Annotation.cpp
		 Annotation.h
		 Document.cpp
		 Document.h
		 DocumentObject.cpp
		 DocumentObject.h
		 DocumentObjectFileIncluded.cpp
		 DocumentObjectFileIncluded.h
		 DocumentObjectGroup.cpp
		 DocumentObjectGroup.h
		 DocumentObjectGroupPy.xml
		 DocumentObjectGroupPyImp.cpp
		 DocumentObjectPy.xml
		 DocumentObjectPyImp.cpp
		 DocumentObserver.cpp
		 DocumentObserver.h
		 DocumentPy.xml
		 DocumentPyImp.cpp
		 Feature.cpp
		 Feature.h
		 FeaturePy.xml
		 FeaturePyImp.cpp
		 FeaturePython.cpp
		 FeaturePython.h
		 FeaturePythonPy.xml
		 FeaturePythonPyImp.cpp
		 FeatureTest.cpp
		 FeatureTest.h
		 GeoFeature.cpp
		 GeoFeature.h
		 InventorObject.cpp
		 InventorObject.h
		 MeasureDistance.cpp
		 MeasureDistance.h
		 Placement.cpp
		 Placement.h
		 Transactions.cpp
		 Transactions.h
		 VRMLObject.cpp
		 VRMLObject.h
)
SOURCE_GROUP("Document" FILES ${Document_SRCS})

SET(Properties_SRCS
		 DynamicProperty.cpp
		 DynamicProperty.h
		 Property.cpp
		 Property.h
		 PropertyContainer.cpp
		 PropertyContainer.h
		 PropertyContainerPy.xml
		 PropertyContainerPyImp.cpp
		 PropertyFile.cpp
		 PropertyFile.h
		 PropertyGeo.cpp
		 PropertyGeo.h
		 PropertyLinks.cpp
		 PropertyLinks.h
		 PropertyPythonObject.cpp
		 PropertyPythonObject.h
		 PropertyStandard.cpp
		 PropertyStandard.h
)
SOURCE_GROUP("Properties" FILES ${Properties_SRCS})

SET(FreeCADApp_SRCS
		 ${Document_SRCS}
		 ${Properties_SRCS}
		 Application.cpp
		 Application.h
		 ApplicationPy.cpp
		 ColorModel.cpp
		 ColorModel.h
		 ComplexGeoData.cpp
		 ComplexGeoData.h
		 ComplexGeoDataPy.xml
		 ComplexGeoDataPyImp.cpp
		 Doxygen.cpp
		 FreeCADInit.py
		 FreeCADTest.py
		 Material.cpp
		 Material.h
		 MaterialPy.cpp
		 MaterialPy.h
		 PreCompiled.cpp
		 PreCompiled.h
)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../Main)

add_library(FreeCADApp SHARED ${FreeCADApp_SRCS})

target_link_libraries(FreeCADApp ${FreeCADApp_LIBS})

if(MSVC)
		set_target_properties(FreeCADApp PROPERTIES DEBUG_OUTPUT_NAME "FreeCADAppD")
endif(MSVC)
# This doesn't work!? 
#IF(MSVC)
#	SET_TARGET_PROPERTIES(FreeCADApp PROPERTIES LINK_FLAGS "/LARGEADDRESSAWARE")
#ENDIF(MSVC)

install(TARGETS FreeCADApp DESTINATION lib)
