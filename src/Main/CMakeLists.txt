#add_defintions(-D_FC_GUI_ENABLED_)
#add_defintions(-DFREECADMAINPY)

include_directories(
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${SOQT_INCLUDE_DIR}
		 ${QT_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/src/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/src/lib)

######################## FreeCADMain ########################

SET(FreeCAD_SRCS
		 freecad.rc
		 icon.ico
		 MainGui.cpp
)

if(MSVC)
	SET(FreeCAD_LIBS
		FreeCADGui
		 debug qtmaind.lib
		 optimized qtmain.lib
	)
else(MSVC)
	SET(FreeCAD_LIBS
		FreeCADGui
	)
endif(MSVC)

add_executable(FreeCADMain WIN32 ${FreeCAD_SRCS})

target_link_libraries(FreeCADMain ${FreeCAD_LIBS})
set_target_properties(FreeCADMain PROPERTIES OUTPUT_NAME "FreeCAD")
if(MSVC)
set_target_properties(FreeCADMain PROPERTIES DEBUG_OUTPUT_NAME "FreeCADD")
endif(MSVC)

if(WIN32)
	INSTALL(TARGETS FreeCADMain
	  RUNTIME DESTINATION bin
	  LIBRARY DESTINATION lib
	)
else(WIN32)
	INSTALL(TARGETS FreeCADMain 
	  RUNTIME DESTINATION bin
	)
endif(WIN32)

######################## FreeCADMainCmd ########################

SET(FreeCADMainCmd_SRCS
		 MainCmd.cpp
)
add_executable(FreeCADMainCmd ${FreeCADMainCmd_SRCS})

target_link_libraries(FreeCADMainCmd FreeCADApp)
set_target_properties(FreeCADMainCmd PROPERTIES OUTPUT_NAME "FreeCADCmd")
if(MSVC)
set_target_properties(FreeCADMainCmd PROPERTIES DEBUG_OUTPUT_NAME "FreeCADCmdD")
endif(MSVC)

if(WIN32)
	INSTALL(TARGETS FreeCADMainCmd
	  RUNTIME DESTINATION bin
	  LIBRARY DESTINATION lib
	)
else(WIN32)
	INSTALL(TARGETS FreeCADMainCmd 
	  RUNTIME DESTINATION bin
	)
endif(WIN32)

######################## FreeCADMainPy ########################

SET(FreeCADMainPy_SRCS
		 MainPy.cpp
)

add_library(FreeCADMainPy SHARED ${FreeCADMainPy_SRCS})

target_link_libraries(FreeCADMainPy FreeCADApp)

if(WIN32)
		 set_target_properties(FreeCADMainPy PROPERTIES OUTPUT_NAME "FreeCAD")
		 set_target_properties(FreeCADMainPy PROPERTIES DEBUG_OUTPUT_NAME "FreeCAD_d")
		 set_target_properties(FreeCADMainPy PROPERTIES SUFFIX ".pyd")
else(WIN32)
		 set_target_properties(FreeCADMainPy PROPERTIES OUTPUT_NAME "FreeCAD")
		 set_target_properties(FreeCADMainPy PROPERTIES PREFIX "")
endif(WIN32)

if(WIN32)
	INSTALL(TARGETS FreeCADMainPy
	  RUNTIME DESTINATION bin
	  LIBRARY DESTINATION lib
	)
else(WIN32)
	INSTALL(TARGETS FreeCADMainPy
	  LIBRARY DESTINATION lib
	)
endif(WIN32)
