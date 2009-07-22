if(WIN32)
else(WIN32)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(WIN32)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR})


if(WIN32)
		 set(Reen_LIBS
		 		 Part
				 Mesh
		 		 FreeCADApp)
else(WIN32)
		 set(Reen_LIBS
		 		 Part
				 Mesh
		 		 FreeCADApp)
endif(WIN32)

SET(Reen_SRCS
		 AppReverseEngineering.cpp
		 AppReverseEngineeringPy.cpp
		 ApproxSurface.cpp
		 ApproxSurface.h
		 PreCompiled.cpp
		 PreCompiled.h
)

add_library(ReverseEngineering SHARED ${Reen_SRCS})

target_link_libraries(ReverseEngineering ${Reen_LIBS})

if(WIN32)
		 set_target_properties(ReverseEngineering PROPERTIES SUFFIX ".pyd")
		 set_target_properties(ReverseEngineering PROPERTIES DEBUG_OUTPUT_NAME "ReverseEngineering_d")
		 set_target_properties(ReverseEngineering PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/ReverseEngineering)
		 set_target_properties(ReverseEngineering PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(ReverseEngineering PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS ReverseEngineering DESTINATION lib)
