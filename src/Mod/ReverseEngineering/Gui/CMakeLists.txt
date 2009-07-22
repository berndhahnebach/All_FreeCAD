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
		 ${QT_INCLUDE_DIR}
		 ${SOQT_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${ZLIB_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(ReenGui_LIBS
		 		 ReverseEngineering
		 		 FreeCADGui)
else(WIN32)
		 set(ReenGui_LIBS
		 		 ReverseEngineering
		 		 FreeCADGui)
endif(WIN32)

qt4_add_resources(ReenGui_QRC_SRCS Resources/ReverseEngineering.qrc)

SET(ReenGui_SRCS
		 ${ReenGui_QRC_SRCS}
		 AppReverseEngineeringGui.cpp
		 AppReverseEngineeringGuiPy.cpp
		 Command.cpp
		 Resources/ReverseEngineering.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(ReverseEngineeringGui SHARED ${ReenGui_SRCS})

target_link_libraries(ReverseEngineeringGui ${ReenGui_LIBS})

if(WIN32)
		 set_target_properties(ReverseEngineeringGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(ReverseEngineeringGui PROPERTIES DEBUG_OUTPUT_NAME "ReverseEngineeringGui_d")
		 set_target_properties(ReverseEngineeringGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/ReverseEngineering)
		 set_target_properties(ReverseEngineeringGui PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(ReverseEngineeringGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS ReverseEngineeringGui DESTINATION lib)
