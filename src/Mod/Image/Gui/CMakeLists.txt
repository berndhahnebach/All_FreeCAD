include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${OPENCV_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(ImageGui_LIBS
		 		 Image
		 		 FreeCADGui)
else(WIN32)
		 set(ImageGui_LIBS
#libImageGui_la_LIBADD   = \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lxerces-c \
#		 		 -lcv \
#		 		 -lcxcore \
#		 		 -lhighgui \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lImage
)
endif(WIN32)

set(ImageGui_MOC_HDRS
		 ImageView.h
		 GLImageBox.h
)
fc_wrap_cpp(ImageGui_MOC_SRCS ${ImageGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${ImageGui_MOC_SRCS})

SET(ImageGui_RES_SRCS
		 Image.qrc
)
fc_add_resources(ImageGui_QRC_SRCS ${ImageGui_RES_SRCS})
#QT4_ADD_RESOURCES(ImageGui_QRC_SRCS ${ImageGui_RES_SRCS})

SET(ImageGui_SRCS
#		 ${ImageGui_MOC_SRCS}
#		 ${ImageGui_QRC_SRCS}
		 AppImageGui.cpp
		 AppImageGuiPy.cpp
		 Command.cpp
		 GLImageBox.cpp
		 GLImageBox.h
		 Image.qrc
		 ImageView.cpp
		 ImageView.h
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
		 XpmImages.h
)

add_library(ImageGui SHARED ${ImageGui_SRCS})

target_link_libraries(ImageGui ${ImageGui_LIBS})

set_target_properties(ImageGui PROPERTIES SUFFIX ".pyd")

install(TARGETS ImageGui DESTINATION lib)
