include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_BINARY_DIR}/src
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${SO_QT_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(CamGui_LIBS
		 		 Cam
		 		 PartGui
		 		 FreeCADGui)
else(WIN32)
		 set(CamGui_LIBS
#libCamGui_la_LIBADD   = \
#		 		 -lxerces-c \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lTKernel \
#		 		 -lCam
)
endif(WIN32)

set(CamGui_UIC_SRCS
		 Cutting.ui
)
qt4_wrap_ui(CamGui_UIC_HDRS ${CamGui_UIC_SRCS})

SET(CamGui_SRCS
		 ${CamGui_UIC_HDRS}
		 AppCamGui.cpp
		 Command.cpp
		 Cutting.cpp
		 Cutting.h
		 Cutting.ui
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(CamGui SHARED ${CamGui_SRCS})

target_link_libraries(CamGui ${CamGui_LIBS})

set_target_properties(CamGui PROPERTIES SUFFIX ".pyd")

install(TARGETS CamGui DESTINATION lib)
