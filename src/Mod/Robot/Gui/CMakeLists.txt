include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${QT_INCLUDE_DIR}
		 ${Eigen_INCLUDE_DIR}
		 ${SOQT_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(RobotGui_LIBS
		 		 Robot
		 		 FreeCADGui)
else(WIN32)
		 set(RobotGui_LIBS
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
		 ViewProviderRobotObject.cpp
		 ViewProviderRobotObject.h
)

add_library(RobotGui SHARED ${RobotGui_SRCS})

target_link_libraries(RobotGui ${RobotGui_LIBS})

if(WIN32)
		 set_target_properties(RobotGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(RobotGui PROPERTIES DEBUG_OUTPUT_NAME "RobotGui_d")
		 set_target_properties(RobotGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Robot)
		 set_target_properties(RobotGui PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(RobotGui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Robot)
		 set_target_properties(RobotGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS RobotGui DESTINATION lib)
