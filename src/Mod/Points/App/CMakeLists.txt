if(WIN32)
		 add_definitions(-DFCAppPoints -DFC_DEBUG)
endif(WIN32)

include_directories(
		 ${CMAKE_CURRENT_SOURCE_DIR}
		 ${CMAKE_SOURCE_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Points_LIBS
		 		 FreeCADApp)
else(WIN32)
		 set(Points_LIBS
		 		 FreeCADApp)
endif(WIN32)

generate_from_xml(PointsPy)

SET(Points_SRCS
		 AppPoints.cpp
		 AppPointsPy.cpp
		 FeaturePointsImportAscii.cpp
		 FeaturePointsImportAscii.h
		 Points.cpp
		 Points.h
		 PointsPy.xml
		 PointsPyImp.cpp
		 PointsAlgos.cpp
		 PointsAlgos.h
		 PointsFeature.cpp
		 PointsFeature.h
		 PointsGrid.cpp
		 PointsGrid.h
		 PreCompiled.cpp
		 PreCompiled.h
		 Properties.cpp
		 Properties.h
		 PropertyPointKernel.cpp
		 PropertyPointKernel.h
)

add_library(Points SHARED ${Points_SRCS})

target_link_libraries(Points ${Points_LIBS})

if(WIN32)
		 set_target_properties(Points PROPERTIES SUFFIX ".pyd")
else(WIN32)
		 set_target_properties(Points PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Points DESTINATION lib)
