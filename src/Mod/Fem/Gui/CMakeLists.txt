if(MSVC)
		 add_definitions(-DFCGuiFem -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)



include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
 		 ${OCC_INCLUDE_DIR}
		 ${QT_INCLUDE_DIR}
		 ${EIGEN2_INCLUDE_DIR}
		 ${SOQT_INCLUDE_DIR}
 		 ${SMESH_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCESC_INCLUDE_DIR})

if(WIN32)
		 set(FemGui_LIBS
		 		 Fem
				 ${SMESH_LIBRARIES}
		 		 FreeCADGui)
else(WIN32)
		 set(FemGui_LIBS
		 		 Fem
				 ${SMESH_LIBRARIES}
		 		 FreeCADGui)
endif(WIN32)

qt4_add_resources(FemResource_SRCS Resources/Fem.qrc)

SOURCE_GROUP("Resources" FILES ${FemResource_SRCS})

#set(FemGui_UIC_SRCS
#		 TaskTrajectory.ui
#		 
#)

#qt4_wrap_ui(FemGui_UIC_HDRS ${FemGui_UIC_SRCS})

#set(FemGui_MOC_HDRS
 #		 TaskTrajectory.h
#		 TaskFem6Axis.h
#		 TrajectorySimulate.h
#)
#fc_wrap_cpp(FemGui_MOC_SRCS ${FemGui_MOC_HDRS})
#SOURCE_GROUP("Qt" FILES ${PartDesignGui_MOC_SRCS})



SET(FemGui_SRCS
		 #${FemGui_UIC_HDRS}
	     ${FemResource_SRCS}
		 AppFemGui.cpp
		 AppFemGuiPy.cpp
		 Command.cpp
		 Resources/Fem.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
		 ViewProviderFemMesh.cpp
		 ViewProviderFemMesh.h
)

add_library(FemGui SHARED ${FemGui_SRCS})

target_link_libraries(FemGui ${FemGui_LIBS})

if(WIN32)
		 set_target_properties(FemGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(FemGui PROPERTIES DEBUG_OUTPUT_NAME "FemGui_d")
		 set_target_properties(FemGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Fem)
		 set_target_properties(FemGui PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(FemGui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Fem)
		 set_target_properties(FemGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS FemGui DESTINATION lib)
