if(WIN32)
else(WIN32)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(WIN32)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(SketcherGui_LIBS
		 		 Sketcher
				 PartGui
		 		 FreeCADGui)
else(WIN32)
		 set(SketcherGui_LIBS
		 		 Sketcher
				 PartGui
		 		 FreeCADGui)
endif(WIN32)

set(SketcherGui_MOC_HDRS
		 TaskSketcherConstrains.h
		 TaskSketcherCreateCommands.h
		 TaskSketcherGeneral.h
)
fc_wrap_cpp(SketcherGui_MOC_SRCS ${SketcherGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${SketcherGui_MOC_SRCS})

qt4_add_resources(SketcherGui_SRCS Resources/Sketcher.qrc)

set(SketcherGui_UIC_SRCS
		 TaskSketcherConstrains.ui
		 TaskSketcherGeneral.ui
)
qt4_wrap_ui(SketcherGui_UIC_HDRS ${SketcherGui_UIC_SRCS})

SET(SketcherGui_SRCS
		 ${SketcherGui_SRCS}
		 ${SketcherGui_UIC_HDRS}
		 AppSketcherGui.cpp
		 AppSketcherGuiPy.cpp
		 Command.cpp
		 Resources/Sketcher.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 TaskSketcherConstrains.ui
		 TaskSketcherConstrains.cpp
		 TaskSketcherConstrains.h
		 TaskSketcherCreateCommands.cpp
		 TaskSketcherCreateCommands.h
		 TaskSketcherGeneral.ui
		 TaskSketcherGeneral.cpp
		 TaskSketcherGeneral.h
		 ViewProviderSketch.cpp
		 ViewProviderSketch.h
		 Workbench.cpp
		 Workbench.h
)

add_library(SketcherGui SHARED ${SketcherGui_SRCS})

target_link_libraries(SketcherGui ${SketcherGui_LIBS})

if(WIN32)
		 set_target_properties(SketcherGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(SketcherGui PROPERTIES DEBUG_OUTPUT_NAME "SketcherGui_d")
else(WIN32)
		 set_target_properties(SketcherGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS SketcherGui DESTINATION lib)
