include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(SketcherGui_LIBS
		 		 Sketcher
		 		 FreeCADGui)
else(WIN32)
		 set(SketcherGui_LIBS
#libSketcherGui_la_LIBADD   = \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lxerces-c \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lSketcher
)
endif(WIN32)

set(SketcherGui_MOC_HDRS
		 SketcherView.h
)
fc_wrap_cpp(SketcherGui_MOC_SRCS ${SketcherGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${SketcherGui_MOC_SRCS})

fc_add_resources(SketcherGui_SRCS Resources/Sketcher.qrc)

SET(SketcherGui_SRCS
		 ${SketcherGui_SRCS}
		 AppSketcherGui.cpp
		 AppSketcherGuiPy.cpp
		 Command.cpp
		 Resources/Sketcher.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(SketcherGui SHARED ${SketcherGui_SRCS})

target_link_libraries(SketcherGui ${SketcherGui_LIBS})

set_target_properties(SketcherGui PROPERTIES SUFFIX ".pyd")

install(TARGETS SketcherGui DESTINATION lib)
