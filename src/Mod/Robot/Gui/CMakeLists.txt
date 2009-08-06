include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${QT_INCLUDE_DIR}
		 ${SOQT_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(RobotGui_LIBS
				 #ode
		 		 Robot
		 		 FreeCADGui)
else(WIN32)
		 set(RobotGui_LIBS
				 #ode
		 		 Robot
		 		 FreeCADGui)
endif(WIN32)

qt4_add_resources(RobotGui_SRCS Resources/Robot.qrc)

SET(RobotGui_SRCS
		 ${RobotGui_SRCS}
		 AppRobotGui.cpp
		 AppRobotGuiPy.cpp
		 Command.cpp
		 Resources/Robot.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(RobotGui SHARED ${RobotGui_SRCS})

target_link_libraries(RobotGui ${RobotGui_LIBS})

if(WIN32)
		 set_target_properties(Part PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Part PROPERTIES DEBUG_OUTPUT_NAME "RobotGui_d")
		 set_target_properties(Part PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Robot)
		 set_target_properties(Part PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(Part PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Robot)
		 set_target_properties(Part PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS RobotGui DESTINATION lib)
