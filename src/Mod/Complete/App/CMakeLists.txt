if(WIN32)
endif(WIN32)

include_directories(
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

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
)

add_library(Complete SHARED ${Complete_SRCS})

target_link_libraries(Complete ${Complete_LIBS})

if(WIN32)
		 set_target_properties(Complete PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Complete PROPERTIES DEBUG_OUTPUT_NAME "Complete_d")
else(WIN32)
		 set_target_properties(Complete PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Complete DESTINATION lib)
