if(WIN32)
else(WIN32)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(WIN32)

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
		 		 Drawing
		 		 FreeCADGui)
endif(WIN32)

set(DrawingGui_MOC_HDRS
		 DrawingView.h
)
fc_wrap_cpp(DrawingGui_MOC_SRCS ${DrawingGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${DrawingGui_MOC_SRCS})

qt4_add_resources(DrawingGui_SRCS Resources/Drawing.qrc)

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
		 ViewProviderPage.cpp
		 ViewProviderPage.h
		 ViewProviderView.cpp
		 ViewProviderView.h
		 Workbench.cpp
		 Workbench.h
)

add_library(DrawingGui SHARED ${DrawingGui_SRCS})

target_link_libraries(DrawingGui ${DrawingGui_LIBS})

if(WIN32)
		 set_target_properties(DrawingGui PROPERTIES SUFFIX ".pyd")
else(WIN32)
		 set_target_properties(DrawingGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS DrawingGui DESTINATION lib)
