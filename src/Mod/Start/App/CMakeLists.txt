if(WIN32)
endif(WIN32)

include_directories(
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCESC_INCLUDE_DIR})

if(WIN32)
		 set(Start_LIBS
		 		 FreeCADApp)
else(WIN32)
		 set(Start_LIBS
		 		 FreeCADApp)
endif(WIN32)


SET(Start_SRCS
		 AppStart.cpp
		 AppStartPy.cpp
		 PreCompiled.cpp
		 PreCompiled.h
		 StartConfiguration.h
)

add_library(Start SHARED ${Start_SRCS})

target_link_libraries(Start ${Start_LIBS})

if(WIN32)
		 set_target_properties(Start PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Start PROPERTIES DEBUG_OUTPUT_NAME "Start_d")
		 set_target_properties(Start PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Start)
		 set_target_properties(Start PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(Start PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Start)
		 set_target_properties(Start PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Start DESTINATION lib)
