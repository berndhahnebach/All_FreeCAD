include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(MeshPartGui_LIBS
		 		 MeshPart
		 		 FreeCADGui)
else(WIN32)
		 set(MeshPartGui_LIBS
#libMeshPartGui_la_LIBADD   = \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lxerces-c \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lMeshPart
)
endif(WIN32)

set(MeshPartGui_MOC_HDRS
		 MeshPartView.h
)
fc_wrap_cpp(MeshPartGui_MOC_SRCS ${MeshPartGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${MeshPartGui_MOC_SRCS})

fc_add_resources(MeshPartGui_SRCS Resources/MeshPart.qrc)

SET(MeshPartGui_SRCS
		 ${MeshPartGui_SRCS}
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

set_target_properties(MeshPartGui PROPERTIES SUFFIX ".pyd")

install(TARGETS MeshPartGui DESTINATION lib)
