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

add_executable(FreeCADMain ${FreeCAD_SRCS})

target_link_libraries(FreeCADMain FreeCADGui)
#FreeCAD_LDADD=\
#		 -l@PYTHON_LIB@ \
#		 -lFreeCADBase \
#		 -lFreeCADApp \
#		 -lFreeCADGui

install(TARGETS FreeCADMain DESTINATION bin)

######################## FreeCADMainCmd ########################

SET(FreeCADMainCmd_SRCS
		 MainCmd.cpp
)
add_executable(FreeCADMainCmd ${FreeCADMainCmd_SRCS})

target_link_libraries(FreeCADMainCmd FreeCADApp)
#FreeCADCmd_LDADD=\
#		 -l@PYTHON_LIB@ \
#		 -lFreeCADBase \
#		 -lFreeCADApp

install(TARGETS FreeCADMainCmd DESTINATION bin)

######################## FreeCADMainPy ########################

SET(FreeCADMainPy_SRCS
		 MainPy.cpp
)

add_library(FreeCADMainPy SHARED ${FreeCADMainPy_SRCS})

target_link_libraries(FreeCADMainPy FreeCADApp)
#FreeCAD_la_LIBADD=\
#		 -ldl \
#		 -l@PYTHON_LIB@ \
#		 -lFreeCADBase \
#		 -lFreeCADApp

set_target_properties(FreeCADMainPy PROPERTIES SUFFIX ".pyd")

install(TARGETS FreeCADMainPy DESTINATION lib)
