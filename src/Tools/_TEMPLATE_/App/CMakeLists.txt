if(WIN32)
endif(WIN32)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(_TEMPLATE__LIBS
		 		 FreeCADApp)
else(WIN32)
		 set(_TEMPLATE__LIBS
		 		 FreeCADApp)
endif(WIN32)


SET(_TEMPLATE__SRCS
		 App_TEMPLATE_.cpp
		 App_TEMPLATE_Py.cpp
		 PreCompiled.cpp
		 PreCompiled.h
)

add_library(_TEMPLATE_ SHARED ${_TEMPLATE__SRCS})

target_link_libraries(_TEMPLATE_ ${_TEMPLATE__LIBS})

if(WIN32)
		 set_target_properties(_TEMPLATE_ PROPERTIES SUFFIX ".pyd")
		 set_target_properties(_TEMPLATE_ PROPERTIES DEBUG_OUTPUT_NAME "_TEMPLATE__d")
else(WIN32)
		 set_target_properties(_TEMPLATE_ PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS _TEMPLATE_ DESTINATION lib)
