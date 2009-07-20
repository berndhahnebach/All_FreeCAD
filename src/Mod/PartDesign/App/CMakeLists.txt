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
		 set(PartDesign_LIBS
		 		 Part
		 		 TKHLR
		 		 FreeCADApp)
else(WIN32)
		 set(PartDesign_LIBS
		 		 Part
		 		 TKHLR
		 		 FreeCADApp)
endif(WIN32)

SET(Features_SRCS
		 FeatureFillet.cpp
		 FeatureFillet.h
		 FeaturePad.cpp
		 FeaturePad.h
)
SOURCE_GROUP("Features" FILES ${Features_SRCS})

SET(PartDesign_SRCS
		 ${Features_SRCS}
		 AppPartDesign.cpp
		 AppPartDesignPy.cpp
		 PreCompiled.cpp
		 PreCompiled.h
)

add_library(PartDesign SHARED ${PartDesign_SRCS})

target_link_libraries(PartDesign ${PartDesign_LIBS})

if(WIN32)
		 set_target_properties(PartDesign PROPERTIES SUFFIX ".pyd")
		 set_target_properties(PartDesign PROPERTIES DEBUG_OUTPUT_NAME "PartDesign_d")
		 set_target_properties(PartDesign PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(PartDesign PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS PartDesign DESTINATION lib)
