
include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_BINARY_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${EIGEN2_INCLUDE_DIR}
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

FILE( GLOB KDL_SRCS KDL_CP/[^.]*.cpp )
FILE( GLOB KDL_HPPS KDL_CP/[^.]*.hpp KDL_CP/[^.]*.inl)

FILE( GLOB UTIL_SRCS KDL_CP/utilities/[^.]*.cpp KDL_CP/utilities/[^.]*.cxx)
FILE( GLOB UTIL_HPPS KDL_CP/utilities/[^.]*.h KDL_CP/utilities/[^.]*.hpp)

generate_from_xml(Robot6AxisPy)
generate_from_xml(TrajectoryPy)
generate_from_xml(WaypointPy)

SET(Python_SRCS
		 Robot6AxisPy.xml
		 Robot6AxisPyImp.cpp
		 TrajectoryPy.xml
		 TrajectoryPyImp.cpp
		 WaypointPy.xml
		 WaypointPyImp.cpp
)

SET(Mod_SRCS
		 AppRobot.cpp
		 AppRobotPy.cpp
		 PreCompiled.cpp
		 PreCompiled.h
)

SET(Robot_SRCS
		 RobotObject.cpp
		 RobotObject.h
		 TrajectoryObject.cpp
		 TrajectoryObject.h
		 PropertyTrajectory.cpp
		 PropertyTrajectory.h
		 RobotAlgos.cpp
		 RobotAlgos.h
		 Robot6Axis.cpp
		 Robot6Axis.h
		 Trajectory.cpp
		 Trajectory.h
		 Waypoint.cpp
		 Waypoint.h
		 ${KDL_SRCS}
		 ${KDL_HPPS}
		 ${UTIL_SRCS}
		 ${UTIL_HPPS}
		 ${Mod_SRCS}
		 ${Python_SRCS}
)


SOURCE_GROUP("KDL" FILES ${KDL_SRCS} ${KDL_HPPS} ${UTIL_SRCS} ${UTIL_HPPS} )

SOURCE_GROUP("Python" FILES ${Python_SRCS})
SOURCE_GROUP("Module" FILES ${Mod_SRCS})

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
