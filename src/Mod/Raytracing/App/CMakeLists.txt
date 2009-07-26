if(WIN32)
		 add_definitions(-DFCAppRaytracing -DFC_DEBUG)
else(WIN32)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(WIN32)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR})


if(WIN32)
		 set(Raytracing_LIBS
		 		 Part
		 		 TKernel
		 		 TKG2d
		 		 TKG3d
		 		 TKMath
		 		 TKBool
		 		 TKBRep
		 		 TKGeomAlgo
		 		 TKGeomBase
		 		 TKMesh
		 		 FreeCADApp)
else(WIN32)
		 set(Raytracing_LIBS
		 		 Part
		 		 TKernel
		 		 TKG2d
		 		 TKG3d
		 		 TKMath
		 		 TKBool
		 		 TKBRep
		 		 TKGeomAlgo
		 		 TKGeomBase
		 		 TKMesh
		 		 FreeCADApp)
endif(WIN32)

macro(generate_from_py2 BASE_NAME OUTPUT_FILE)
		 file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR}/src/Tools/PythonToCPP.py TOOL_PATH)
		 file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${BASE_NAME} SOURCE_PATH)
		 add_custom_command(
		 		 OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILE}
		 		 COMMAND ${PYTHON_EXECUTABLE} ${TOOL_PATH} ${SOURCE_PATH} ${OUTPUT_FILE}
		 		 MAIN_DEPENDENCY ${BASE_NAME}
		 		 WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		 		 COMMENT Building files out of ${BASE_NAME}.py)
endmacro(generate_from_py2)

generate_from_py2(FreeCADpov FreeCADpov.h)

SET(Raytracing_SRCS
		 AppRaytracing.cpp
		 AppRaytracingPy.cpp
		 FreeCADpov
		 PovTools.cpp
		 PovTools.h
		 PreCompiled.cpp
		 PreCompiled.h
		 RayFeature.cpp
		 RayFeature.h
		 RayProject.cpp
		 RayProject.h
)

add_library(Raytracing SHARED ${Raytracing_SRCS})

target_link_libraries(Raytracing ${Raytracing_LIBS})

if(WIN32)
		 set_target_properties(Raytracing PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Raytracing PROPERTIES DEBUG_OUTPUT_NAME "Raytracing_d")
		 set_target_properties(Raytracing PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Raytracing)
		 set_target_properties(Raytracing PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(Raytracing PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Raytracing)
		 set_target_properties(Raytracing PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Raytracing DESTINATION lib)
