add_definitions(-DFCApp)

include_directories(
		 ${CMAKE_CURRENT_SOURCE_DIR}/..
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(FreeCADApp_LIBS
		 		 FreeCADBase
		 		 ${XERCES_LIBRARIES})
else(WIN32)
		 set(FreeCADApp_LIBS
#libFreeCADApp_la_LIBADD		 = \
#		 		 -lFreeCADBase \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lxerces-c \
#		 		 -lboost_filesystem \
#		 		 -lboost_program_options \
#		 		 -lboost_signals
)
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
		 Document.cpp
		 Document.h
		 DocumentObject.cpp
		 DocumentObject.h
		 DocumentObjectGroup.cpp
		 DocumentObjectGroup.h
		 DocumentObjectGroupPy.xml
		 DocumentObjectGroupPyImp.cpp
		 DocumentObjectPy.xml
		 DocumentObjectPyImp.cpp
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
		 Placement.cpp
		 Placement.h
		 Transactions.cpp
		 Transactions.h
)
SOURCE_GROUP("Document" FILES ${Document_SRCS})

SET(Properties_SRCS
		 Property.cpp
		 Property.h
		 PropertyContainer.cpp
		 PropertyContainer.h
		 PropertyContainerPy.xml
		 PropertyContainerPyImp.cpp
		 PropertyGeo.cpp
		 PropertyGeo.h
		 PropertyLinks.cpp
		 PropertyLinks.h
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
		 MatrixPy.cpp
		 MatrixPy.h
		 PreCompiled.cpp
		 PreCompiled.h
)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../Main)

add_library(FreeCADApp SHARED ${FreeCADApp_SRCS})

target_link_libraries(FreeCADApp ${FreeCADApp_LIBS})

install(TARGETS FreeCADApp DESTINATION lib)
