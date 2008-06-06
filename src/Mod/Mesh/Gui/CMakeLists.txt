include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_BINARY_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${SO_QT_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(MeshGui_LIBS
		 		 Mesh
		 		 FreeCADGui)
else(WIN32)
		 set(MeshGui_LIBS
#libMeshGui_la_LIBADD		 = \
#		 		 -lboost_signals \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lMesh
)
endif(WIN32)

set(Mesh_MOC_HDRS
		 DlgEvaluateMeshImp.h
		 DlgRegularSolidImp.h
)
fc_wrap_cpp(Mesh_MOC_SRCS ${Mesh_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${Mesh_MOC_SRCS})

set(Dialogs_UIC_SRCS
		 DlgEvaluateMesh.ui
		 DlgRegularSolid.ui
		 ../../../Gui/DlgTreeWidget.ui
)
qt4_wrap_ui(Dialogs_UIC_HDRS ${Dialogs_UIC_SRCS})
SET(Dialogs_SRCS
		 ${Dialogs_UIC_HDRS}
		 DlgEvaluateMesh.ui
		 DlgEvaluateMeshImp.cpp
		 DlgEvaluateMeshImp.h
		 DlgRegularSolid.ui
		 DlgRegularSolidImp.cpp
		 DlgRegularSolidImp.h
)
SOURCE_GROUP("Dialogs" FILES ${Dialogs_SRCS})

SET(Inventor_SRCS
		 SoFCMeshFaceSet.cpp
		 SoFCMeshFaceSet.h
		 SoFCMeshNode.cpp
		 SoFCMeshNode.h
		 SoFCMeshVertex.cpp
		 SoFCMeshVertex.h
)
SOURCE_GROUP("Inventor" FILES ${Inventor_SRCS})

fc_add_resources(resource_SRCS Mesh.qrc)
SET(resource_SRCS
#		 ${resource_SRCS}
		 Mesh.qrc
)
SOURCE_GROUP("resource" FILES ${resource_SRCS})

SET(ViewProvider_SRCS
		 ViewProvider.cpp
		 ViewProvider.h
		 ViewProviderCurvature.cpp
		 ViewProviderCurvature.h
		 ViewProviderDefects.cpp
		 ViewProviderDefects.h
		 ViewProviderMeshFaceSet.cpp
		 ViewProviderMeshFaceSet.h
		 ViewProviderMeshNode.cpp
		 ViewProviderMeshNode.h
		 ViewProviderTransform.cpp
		 ViewProviderTransform.h
		 ViewProviderTransformDemolding.cpp
		 ViewProviderTransformDemolding.h
)
SOURCE_GROUP("ViewProvider" FILES ${ViewProvider_SRCS})

SET(MeshGui_SRCS
		 ${Dialogs_SRCS}
		 ${Inventor_SRCS}
		 ${resource_SRCS}
		 ${ViewProvider_SRCS}
		 AppMeshGui.cpp
		 Command.cpp
		 Doxygen.cpp
		 PreCompiled.cpp
		 PreCompiled.h
		 PropertyEditorMesh.cpp
		 PropertyEditorMesh.h
		 Workbench.cpp
		 Workbench.h
)

add_library(MeshGui SHARED ${MeshGui_SRCS})

target_link_libraries(MeshGui ${MeshGui_LIBS})

set_target_properties(MeshGui PROPERTIES SUFFIX ".pyd")

install(TARGETS MeshGui DESTINATION lib)
