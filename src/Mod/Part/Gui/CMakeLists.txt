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
		 ${COIN_INCLUDE_DIR}
		 ${OCC_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${SOQT_INCLUDE_DIR}
		 ${QT_INCLUDE_DIR}
		 ${XERCESC_INCLUDE_DIR}
		 ${ZLIB_INCLUDE_DIR})

if(WIN32)
		 set(PartGui_LIBS
		 		 Part
		 		 TKernel
		 		 TKG2d
		 		 TKG3d
		 		 TKMath
		 		 TKSTEP
		 		 TKIGES
		 		 TKShHealing
		 		 TKXSBase
		 		 TKBool
		 		 TKBO
		 		 TKBRep
		 		 TKTopAlgo
		 		 TKGeomAlgo
		 		 TKGeomBase
		 		 TKPrim
		 		 TKMesh
		 		 FreeCADGui)
else(WIN32)
		 set(PartGui_LIBS
		 		 Part
		 		 TKernel
		 		 TKG2d
		 		 TKG3d
		 		 TKMath
		 		 TKSTEP
		 		 TKIGES
		 		 TKShHealing
		 		 TKXSBase
		 		 TKBool
		 		 TKBO
		 		 TKBRep
		 		 TKTopAlgo
		 		 TKGeomAlgo
		 		 TKGeomBase
		 		 TKPrim
		 		 TKMesh
		 		 FreeCADGui)
endif(WIN32)

set(PartGui_MOC_HDRS
		 DlgBooleanOperation.h
		 DlgExtrusion.h
		 DlgFilletEdges.h
		 DlgPartBoxImp.h
		 DlgPartCylinderImp.h
		 DlgPartImportIgesImp.h
		 DlgPartImportStepImp.h
		 DlgPrimitives.h
		 DlgRevolution.h
		 DlgSettings3DViewPartImp.h
		 DlgSettingsGeneral.h
)
fc_wrap_cpp(PartGui_MOC_SRCS ${PartGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${PartGui_MOC_SRCS})

fc_add_resources(PartGui_QRC_SRCS Resources/Part.qrc)

set(PartGui_UIC_SRCS
		 DlgBooleanOperation.ui
		 DlgExtrusion.ui
		 DlgFilletEdges.ui
		 DlgPartBox.ui
		 DlgPartCylinder.ui
		 DlgPartImportIges.ui
		 DlgPartImportStep.ui
		 DlgPrimitives.ui
		 DlgRevolution.ui
		 DlgSettings3DViewPart.ui
		 DlgSettingsGeneral.ui
)
qt4_wrap_ui(PartGui_UIC_HDRS ${PartGui_UIC_SRCS})

SET(PartGui_SRCS
#		 ${PartGui_QRC_SRCS}
		 ${PartGui_UIC_HDRS}
		 AppPartGui.cpp
		 Command.cpp
		 CommandSimple.cpp
		 CommandParametric.cpp
		 DlgBooleanOperation.cpp
		 DlgBooleanOperation.h
		 DlgBooleanOperation.ui
		 DlgExtrusion.cpp
		 DlgExtrusion.h
		 DlgExtrusion.ui
		 DlgFilletEdges.cpp
		 DlgFilletEdges.h
		 DlgFilletEdges.ui
		 DlgPartBox.ui
		 DlgPartBoxImp.cpp
		 DlgPartBoxImp.h
		 DlgPartCylinderImp.cpp
		 DlgPartCylinderImp.h
		 DlgPartCylinder.ui
		 DlgPartImportIges.ui
		 DlgPartImportIgesImp.cpp
		 DlgPartImportIgesImp.h
		 DlgPartImportStep.ui
		 DlgPartImportStepImp.cpp
		 DlgPartImportStepImp.h
		 DlgPrimitives.cpp
		 DlgPrimitives.h
		 DlgPrimitives.ui
		 DlgRevolution.cpp
		 DlgRevolution.h
		 DlgRevolution.ui
		 DlgSettings3DViewPart.ui
		 DlgSettings3DViewPartImp.cpp
		 DlgSettings3DViewPartImp.h
		 DlgSettingsGeneral.cpp
		 DlgSettingsGeneral.h
		 DlgSettingsGeneral.ui
		 Resources/Part.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 SoFCShapeObject.cpp
		 SoFCShapeObject.h
		 ViewProvider.cpp
		 ViewProvider.h
		 ViewProviderBox.cpp
		 ViewProviderBox.h
		 ViewProviderCurveNet.cpp
		 ViewProviderCurveNet.h
		 ViewProviderImport.cpp
		 ViewProviderImport.h
		 ViewProviderExtrusion.cpp
		 ViewProviderExtrusion.h
		 ViewProvider2DObject.cpp
		 ViewProvider2DObject.h
		 ViewProviderPython.cpp
		 ViewProviderPython.h
		 Workbench.cpp
		 Workbench.h
)

add_library(PartGui SHARED ${PartGui_SRCS})

target_link_libraries(PartGui ${PartGui_LIBS})

fc_copy_script("Mod/Part" "PartGui" InitGui.py)

if(WIN32)
		 set_target_properties(PartGui PROPERTIES SUFFIX ".pyd")
		 set_target_properties(PartGui PROPERTIES DEBUG_OUTPUT_NAME "PartGui_d")
		 set_target_properties(PartGui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Part)
		 set_target_properties(PartGui PROPERTIES PREFIX "../")
else(WIN32)
		 set_target_properties(PartGui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Part)
		 set_target_properties(PartGui PROPERTIES PREFIX "")
endif(WIN32)

install(TARGETS PartGui DESTINATION lib)
