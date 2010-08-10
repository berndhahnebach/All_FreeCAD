if(MSVC)
		add_definitions(-DHAVE_ACOSH -DHAVE_ATANH -DHAVE_ASINH)
else(MSVC)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_BINARY_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
 		 ${ZLIB_INCLUDE_DIR}
		 ${SOQT_INCLUDE_DIR}
		 ${QT_INCLUDE_DIR}
		 ${XERCESC_INCLUDE_DIR})

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
		 TaskDlgEditSketch.h
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
		 CommandCreateGeo.cpp
		 CommandConstraints.cpp
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
		 ViewProviderSketchSF.cpp
		 ViewProviderSketchSF.h
		 ViewProviderSketch.cpp
		 ViewProviderSketch.h
		 DrawSketchHandler.cpp
		 DrawSketchHandler.h
		 Workbench.cpp
		 Workbench.h
		 TaskDlgEditSketch.cpp
		 TaskDlgEditSketch.h
		 ViewProviderPython.cpp
		 ViewProviderPython.h
)

add_library(SketcherGui SHARED ${SketcherGui_SRCS})

target_link_libraries(SketcherGui ${SketcherGui_LIBS})

if(WIN32)
		 set_target_properties(SketcherGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(SketcherGui PROPERTIES DEBUG_OUTPUT_NAME "SketcherGui_d")
		 set_target_properties(SketcherGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Sketcher)
		 set_target_properties(SketcherGui PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(SketcherGui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Sketcher)
		 set_target_properties(SketcherGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS SketcherGui DESTINATION lib)
