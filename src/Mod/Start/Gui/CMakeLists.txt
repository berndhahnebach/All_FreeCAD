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
		 set(StartGui_LIBS
		 		 Start
		 		 FreeCADGui)
else(WIN32)
		 set(StartGui_LIBS
		 		 Start
		 		 FreeCADGui)
endif(WIN32)

qt4_add_resources(Start_QRC_SRCS Resources/Start.qrc)

SET(StartGui_SRCS
		 ${Start_QRC_SRCS}
		 AppStartGui.cpp
		 AppStartGuiPy.cpp
		 Command.cpp
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(StartGui SHARED ${StartGui_SRCS})

target_link_libraries(StartGui ${StartGui_LIBS})

if(WIN32)
		 set_target_properties(StartGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(StartGui PROPERTIES DEBUG_OUTPUT_NAME "StartGui_d")
		 set_target_properties(StartGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Start)
		 set_target_properties(StartGui PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(StartGui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Start)
		 set_target_properties(StartGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS StartGui DESTINATION lib)
