if(WIN32)
endif(WIN32)

include_directories(
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCESC_INCLUDE_DIR})

if(WIN32)
		 set(Complete_LIBS
		 		 FreeCADApp)
else(WIN32)
		 set(Complete_LIBS
		 		 FreeCADApp)
endif(WIN32)


SET(Complete_SRCS
		 AppComplete.cpp
		 AppCompletePy.cpp
		 PreCompiled.cpp
		 PreCompiled.h
		 CompleteConfiguration.h
)

add_library(Complete SHARED ${Complete_SRCS})

target_link_libraries(Complete ${Complete_LIBS})

if(WIN32)
		 set_target_properties(Complete PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Complete PROPERTIES DEBUG_OUTPUT_NAME "Complete_d")
		 set_target_properties(Complete PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Complete)
		 set_target_properties(Complete PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(Complete PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Complete)
		 set_target_properties(Complete PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Complete DESTINATION lib)
