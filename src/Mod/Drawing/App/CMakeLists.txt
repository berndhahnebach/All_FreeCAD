if(WIN32)
else(WIN32)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(WIN32)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Drawing_LIBS
		 		 Part
		 		 TKHLR
		 		 FreeCADApp)
else(WIN32)
		 set(Drawing_LIBS
		 		 Part
		 		 TKHLR
		 		 FreeCADApp)
endif(WIN32)

SET(Features_SRCS
		 FeaturePage.cpp
		 FeaturePage.h
		 FeatureView.cpp
		 FeatureView.h
		 FeatureViewPart.cpp
		 FeatureViewPart.h
		 PageGroup.cpp
		 PageGroup.h
)
SOURCE_GROUP("Features" FILES ${Features_SRCS})

SET(Drawing_SRCS
		 ${Features_SRCS}
		 AppDrawing.cpp
		 AppDrawingPy.cpp
		 PreCompiled.cpp
		 PreCompiled.h
)

add_library(Drawing SHARED ${Drawing_SRCS})

target_link_libraries(Drawing ${Drawing_LIBS})

if(WIN32)
		 set_target_properties(Drawing PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Drawing PROPERTIES DEBUG_OUTPUT_NAME "Drawing_d")
else(WIN32)
		 set_target_properties(Drawing PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Drawing DESTINATION lib)
