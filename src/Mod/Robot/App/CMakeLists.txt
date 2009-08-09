
include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 #${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
 		 ${ZLIB_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(Robot_LIBS
				 #ode
		 		 FreeCADApp)
else(WIN32)
		 set(Robot_LIBS
				 #ode
		 		 FreeCADApp)
endif(WIN32)

SET(Robot_SRCS
		 AppRobot.cpp
		 AppRobotPy.cpp
		 PreCompiled.cpp
		 PreCompiled.h
		 RobotObject.cpp
		 RobotObject.h
)

add_library(Robot SHARED ${Robot_SRCS})

target_link_libraries(Robot ${Robot_LIBS})

if(WIN32)
		 set_target_properties(Robot PROPERTIES SUFFIX ".pyd")
		 set_target_properties(Robot PROPERTIES DEBUG_OUTPUT_NAME "Robot_d")
		 set_target_properties(Robot PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Robot)
		 set_target_properties(Robot PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(Robot PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Robot)
		 set_target_properties(Robot PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS Robot DESTINATION lib)
