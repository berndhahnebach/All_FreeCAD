include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${QT_INCLUDE_DIR}
 		 ${ZLIB_INCLUDE_DIR}
		 ${SOQT_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(MeshPartGui_LIBS
		 		 MeshPart
		 		 FreeCADGui)
else(WIN32)
		 set(MeshPartGui_LIBS
		 		 MeshPart
		 		 FreeCADGui)
endif(WIN32)

qt4_add_resources(MeshPartGui_QRC_SRCS Resources/MeshPart.qrc)

SET(MeshPartGui_SRCS
		 ${MeshPartGui_QRC_SRCS}
		 AppMeshPartGui.cpp
		 AppMeshPartGuiPy.cpp
		 Command.cpp
		 Resources/MeshPart.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 Workbench.cpp
		 Workbench.h
)

add_library(MeshPartGui SHARED ${MeshPartGui_SRCS})

target_link_libraries(MeshPartGui ${MeshPartGui_LIBS})

if(WIN32)
		 set_target_properties(MeshPartGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(MeshPartGui PROPERTIES DEBUG_OUTPUT_NAME "MeshPartGui_d")
		 set_target_properties(MeshPartGui PROPERTIES PREFIX "../../../../Mod/MeshPart/")
else(WIN32)
		 set_target_properties(MeshPartGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS MeshPartGui DESTINATION lib)
