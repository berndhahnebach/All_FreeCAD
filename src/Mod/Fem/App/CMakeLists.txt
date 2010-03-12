if(MSVC)
		 add_definitions(-DFCAppFem -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)


include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_BINARY_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
 		 ${ZLIB_INCLUDE_DIR}
 		 ${SMESH_INCLUDE_DIR}
		 ${XERCESC_INCLUDE_DIR})

if(WIN32)
		 set(Fem_LIBS
				 Part
				 ${SMESH_LIBRARIES}
		 		 FreeCADApp)
else(WIN32)
		 set(Fem_LIBS
				 Part
				 ${SMESH_LIBRARIES}
		 		 FreeCADApp)
endif(WIN32)

generate_from_xml(FemMeshPy)


SET(Python_SRCS
		 FemMeshPy.xml
		 FemMeshPyImp.cpp
)

SET(Mod_SRCS
		 AppFem.cpp
		 AppFemPy.cpp
		 PreCompiled.cpp
		 PreCompiled.h
)

SET(Fem_SRCS
		 FemMeshObject.cpp
		 FemMeshObject.h
		 FemMesh.cpp
		 FemMesh.h
		 FemMeshProperty.cpp
		 FemMeshProperty.h
		 ${KDL_SRCS}
		 ${KDL_HPPS}
		 ${UTIL_SRCS}
		 ${UTIL_HPPS}
		 ${Mod_SRCS}
		 ${Python_SRCS}
)


SOURCE_GROUP("KDL" FILES ${KDL_SRCS} ${KDL_HPPS} ${UTIL_SRCS} ${UTIL_HPPS} )

SOURCE_GROUP("Python" FILES ${Python_SRCS})
SOURCE_GROUP("Module" FILES ${Mod_SRCS})

add_library(Fem SHARED ${Fem_SRCS})

target_link_libraries(Fem ${Fem_LIBS})

if(WIN32)
		 set_target_properties(Fem PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Fem PROPERTIES DEBUG_OUTPUT_NAME "Fem_d")
		 set_target_properties(Fem PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Fem)
		 set_target_properties(Fem PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(Fem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Fem)
		 set_target_properties(Fem PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Fem DESTINATION lib)
