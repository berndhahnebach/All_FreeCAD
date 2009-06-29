#add_defintions(-D_FC_GUI_ENABLED_)
#add_defintions(-DFREECADMAINPY)

include_directories(
		 ${CMAKE_CURRENT_SOURCE_DIR}/..
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

######################## FreeCADMain ########################

SET(FreeCAD_SRCS
		 freecad.rc
		 icon.ico
		 MainGui.cpp
)

add_executable(FreeCAD ${FreeCAD_SRCS})

target_link_libraries(FreeCAD FreeCADGui)

install(TARGETS FreeCAD DESTINATION bin)

######################## FreeCADMainCmd ########################

SET(FreeCADMainCmd_SRCS
		 MainCmd.cpp
)
add_executable(FreeCADCmd ${FreeCADMainCmd_SRCS})

target_link_libraries(FreeCADCmd FreeCADApp)

install(TARGETS FreeCADCmd DESTINATION bin)

######################## FreeCADMainPy ########################

SET(FreeCADMainPy_SRCS
		 MainPy.cpp
)

add_library(FreeCADPy SHARED ${FreeCADMainPy_SRCS})

target_link_libraries(FreeCADPy FreeCADApp)

if(WIN32)
		 set_target_properties(FreeCADPy PROPERTIES OUTPUT_NAME "FreeCAD")
		 set_target_properties(FreeCADPy PROPERTIES SUFFIX ".pyd")
else(WIN32)
		 set_target_properties(FreeCADPy PROPERTIES OUTPUT_NAME "FreeCAD")
		 set_target_properties(FreeCADPy PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS FreeCADPy DESTINATION lib)
