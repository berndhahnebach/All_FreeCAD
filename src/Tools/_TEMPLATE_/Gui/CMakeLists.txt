include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(_TEMPLATE_Gui_LIBS
		 		 _TEMPLATE_
		 		 FreeCADGui)
else(WIN32)
		 set(_TEMPLATE_Gui_LIBS
		 		 _TEMPLATE_
		 		 FreeCADGui)
endif(WIN32)

qt4_add_resources(_TEMPLATE__QRC_SRCS Resources/_TEMPLATE_.qrc)

SET(_TEMPLATE_Gui_SRCS
		 ${_TEMPLATE__QRC_SRCS}
		 App_TEMPLATE_Gui.cpp
		 App_TEMPLATE_GuiPy.cpp
		 Command.cpp
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(_TEMPLATE_Gui SHARED ${_TEMPLATE_Gui_SRCS})

target_link_libraries(_TEMPLATE_Gui ${_TEMPLATE_Gui_LIBS})

if(WIN32)
		 set_target_properties(_TEMPLATE_Gui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(_TEMPLATE_Gui PROPERTIES DEBUG_OUTPUT_NAME "_TEMPLATE_Gui_d")
else(WIN32)
		 set_target_properties(_TEMPLATE_Gui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS _TEMPLATE_Gui DESTINATION lib)
