include_directories(
		 ${CMAKE_SOURCE_DIR}/src
		 ${CMAKE_BINARY_DIR}/src
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${SO_QT_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

set(MeshGui_LIBS
 		 ${Boost_LIBRARIES}
		 Mesh
		 FreeCADGui)

set(Mesh_MOC_HDRS
		 DlgEvaluateMeshImp.h
		 DlgRegularSolidImp.h
		 DlgSettingsMeshView.h
		 PropertyEditorMesh.h
)
fc_wrap_cpp(Mesh_MOC_SRCS ${Mesh_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${Mesh_MOC_SRCS})

set(Dialogs_UIC_SRCS
		 DlgEvaluateMesh.ui
		 DlgRegularSolid.ui
		 DlgSettingsMeshView.ui
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
		 DlgSettingsMeshView.ui
		 DlgSettingsMeshView.cpp
		 DlgSettingsMeshView.h
)
SOURCE_GROUP("Dialogs" FILES ${Dialogs_SRCS})

SET(Inventor_SRCS
		 SoFCIndexedFaceSet.cpp
		 SoFCIndexedFaceSet.h
		 SoFCMeshObject.cpp
		 SoFCMeshObject.h
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

if(WIN32)
		set_target_properties(MeshGui PROPERTIES SUFFIX ".pyd")
		set_target_properties(MeshGui PROPERTIES DEBUG_OUTPUT_NAME "MeshGui_d")
else(WIN32)
		set_target_properties(MeshGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS MeshGui DESTINATION lib)
