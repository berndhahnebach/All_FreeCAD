include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(DrawingGui_LIBS
		 		 Drawing
		 		 FreeCADGui)
else(WIN32)
		 set(DrawingGui_LIBS
#libDrawingGui_la_LIBADD   = \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lxerces-c \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lDrawing
)
endif(WIN32)

set(DrawingGui_MOC_HDRS
		 DrawingView.h
)
fc_wrap_cpp(DrawingGui_MOC_SRCS ${DrawingGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${DrawingGui_MOC_SRCS})

fc_add_resources(DrawingGui_SRCS Resources/Drawing.qrc)

SET(DrawingGui_SRCS
		 ${DrawingGui_SRCS}
		 AppDrawingGui.cpp
		 AppDrawingGuiPy.cpp
		 Command.cpp
		 Resources/Drawing.qrc
		 DrawingView.cpp
		 DrawingView.h
		 PreCompiled.cpp
		 PreCompiled.h
		 ViewProvider.cpp
		 ViewProvider.h
		 Workbench.cpp
		 Workbench.h
)

add_library(DrawingGui SHARED ${DrawingGui_SRCS})

target_link_libraries(DrawingGui ${DrawingGui_LIBS})

set_target_properties(DrawingGui PROPERTIES SUFFIX ".pyd")

install(TARGETS DrawingGui DESTINATION lib)
