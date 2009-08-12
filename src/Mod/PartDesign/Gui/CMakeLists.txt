if(MSVC)
		add_definitions(-DHAVE_ACOSH -DHAVE_ATANH -DHAVE_ASINH)
else(MSVC)
		 add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${OCC_INCLUDE_DIR}
		 ${COIN_INCLUDE_DIR}
		 ${QT_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${SOQT_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(PartDesignGui_LIBS
		 		 PartDesign
		 		 FreeCADGui)
else(WIN32)
		 set(PartDesignGui_LIBS
		 		 PartDesign
		 		 FreeCADGui)
endif(WIN32)

set(PartDesignGui_MOC_HDRS
		 TaskPadParameters.h
)
fc_wrap_cpp(PartDesignGui_MOC_SRCS ${PartDesignGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${PartDesignGui_MOC_SRCS})

qt4_add_resources(PartDesignGui_SRCS Resources/PartDesign.qrc)

set(PartDesignGui_UIC_SRCS
		 TaskPadParameters.ui
)
qt4_wrap_ui(PartDesignGui_UIC_HDRS ${PartDesignGui_UIC_SRCS})

SET(PartDesignGui_SRCS
		 ${PartDesignGui_SRCS}
		 ${PartDesignGui_UIC_HDRS}
		 AppPartDesignGui.cpp
		 AppPartDesignGuiPy.cpp
		 Command.cpp
		 Resources/PartDesign.qrc
		 TaskPadParameters.ui
		 TaskPadParameters.cpp
		 TaskPadParameters.h
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(PartDesignGui SHARED ${PartDesignGui_SRCS})

target_link_libraries(PartDesignGui ${PartDesignGui_LIBS})

if(WIN32)
		 set_target_properties(PartDesignGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(PartDesignGui PROPERTIES DEBUG_OUTPUT_NAME "PartDesignGui_d")
		 set_target_properties(PartDesignGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/PartDesign)
		 set_target_properties(PartDesignGui PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(PartDesignGui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/PartDesign)
		 set_target_properties(PartDesignGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS PartDesignGui DESTINATION lib)
