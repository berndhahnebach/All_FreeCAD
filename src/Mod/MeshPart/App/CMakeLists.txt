if(MSVC)
		 add_definitions(-DFCAppMeshPart -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${ZLIB_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(MeshPart_LIBS
		 		 Part
		 		 Mesh)
else(WIN32)
		 set(MeshPart_LIBS
		 		 Part
		 		 Mesh)
endif(WIN32)

SET(MeshPart_SRCS
		 AppMeshPart.cpp
		 AppMeshPartPy.cpp
		 CurveProjector.cpp
		 CurveProjector.h
		 MeshAlgos.cpp
		 MeshAlgos.h
		 PreCompiled.cpp
		 PreCompiled.h
)

add_library(MeshPart SHARED ${MeshPart_SRCS})

target_link_libraries(MeshPart ${MeshPart_LIBS})

if(WIN32)
		 set_target_properties(MeshPart PROPERTIES SUFFIX ".pyd")
		 set_target_properties(MeshPart PROPERTIES DEBUG_OUTPUT_NAME "MeshPart_d")
		 set_target_properties(MeshPart PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/MeshPart)
		 set_target_properties(MeshPart PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(MeshPart PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/MeshPart)
		 set_target_properties(MeshPart PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS MeshPart DESTINATION lib)
