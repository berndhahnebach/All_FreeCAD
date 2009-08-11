
include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
 		 ${ZLIB_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Assembly_LIBS
				 ode_double
		 		 FreeCADApp)
else(WIN32)
		 set(Assembly_LIBS
				 ode_double
		 		 FreeCADApp)
endif(WIN32)

SET(Assembly_SRCS
		 AppAssembly.cpp
		 AppAssemblyPy.cpp
		 PreCompiled.cpp
		 PreCompiled.h
)

add_library(Assembly SHARED ${Assembly_SRCS})

target_link_libraries(Assembly ${Assembly_LIBS})

if(WIN32)
		 set_target_properties(Assembly PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Assembly PROPERTIES DEBUG_OUTPUT_NAME "Assembly_d")
		 set_target_properties(Assembly PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Assembly)
		 set_target_properties(Assembly PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(Assembly PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Assembly)
		 set_target_properties(Assembly PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Assembly DESTINATION lib)
