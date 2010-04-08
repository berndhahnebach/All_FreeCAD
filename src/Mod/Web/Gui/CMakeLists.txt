include_directories(
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${QT_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${SOQT_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCESC_INCLUDE_DIR})

if(WIN32)
		 set(WebGui_LIBS
		 		 Web
		 		 FreeCADGui)
else(WIN32)
		 set(WebGui_LIBS
		 		 Web
		 		 FreeCADGui)
endif(WIN32)

qt4_add_resources(Web_QRC_SRCS Resources/Web.qrc)

SET(WebGui_SRCS
		 ${Web_QRC_SRCS}
		 AppWebGui.cpp
		 AppWebGuiPy.cpp
		 Command.cpp
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(WebGui SHARED ${WebGui_SRCS})

target_link_libraries(WebGui ${WebGui_LIBS})

if(WIN32)
		 set_target_properties(WebGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(WebGui PROPERTIES DEBUG_OUTPUT_NAME "WebGui_d")
		 set_target_properties(WebGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Web)
		 set_target_properties(WebGui PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(WebGui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Web)
		 set_target_properties(WebGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS WebGui DESTINATION lib)
