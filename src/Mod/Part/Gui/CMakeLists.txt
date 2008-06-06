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
#if OCC_VERSION6
#libPartGui_la_LIBADD		 = \
#		 		 -lxerces-c \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lTKernel \
#		 		 -lTKG2d \
#		 		 -lTKG3d \
#		 		 -lTKMath \
#		 		 -lTKSTEP \
#		 		 -lTKIGES \
#		 		 -lTKShHealing \
#		 		 -lTKXSBase \
#		 		 -lTKBool \
#		 		 -lTKBO \
#		 		 -lTKBRep \
#		 		 -lTKTopAlgo \
#		 		 -lTKGeomAlgo \
#		 		 -lTKGeomBase \
#		 		 -lTKPrim \
#		 		 -lTKMesh \
#		 		 -lPart 
#else
#libPartGui_la_LIBADD		 = \
#		 		 -lxerces-c \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp \
#		 		 -lFreeCADGui \
#		 		 -lTKernel \
#		 		 -lTKG2d \
#		 		 -lTKG3d \
#		 		 -lTKMath \
#		 		 -lTKSTEP \
#		 		 -lTKIGES \
#		 		 -lTKShHealing \
#		 		 -lTKXSBase \
#		 		 -lTKBool \
#		 		 -lTKBO \
#		 		 -lTKBRep \
#		 		 -lTKTopAlgo \
#		 		 -lTKGeomAlgo \
#		 		 -lTKGeomBase \
#		 		 -lTKPrim \
#		 		 -lPart 
#endif
)
endif(WIN32)

set(PartGui_MOC_HDRS
		 DlgBooleanOperation.h
		 DlgPartBoxImp.h
		 DlgPartImportIgesImp.h
		 DlgPartImportStepImp.h
		 DlgSettings3DViewPartImp.h
)
fc_wrap_cpp(PartGui_MOC_SRCS ${PartGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${PartGui_MOC_SRCS})

fc_add_resources(PartGui_QRC_SRCS Part.qrc)

set(PartGui_UIC_SRCS
		 DlgBooleanOperation.ui
		 DlgPartBox.ui
		 DlgPartImportIges.ui
		 DlgPartImportStep.ui
		 DlgPrimitives.ui
		 DlgSettings3DViewPart.ui
)
qt4_wrap_ui(PartGui_UIC_HDRS ${PartGui_UIC_SRCS})

SET(PartGui_SRCS
#		 ${PartGui_QRC_SRCS}
		 ${PartGui_UIC_HDRS}
		 AppPartGui.cpp
		 Command.cpp
		 DlgBooleanOperation.cpp
		 DlgBooleanOperation.h
		 DlgBooleanOperation.ui
		 DlgPartBox.ui
		 DlgPartBoxImp.cpp
		 DlgPartBoxImp.h
		 DlgPartImportIges.ui
		 DlgPartImportIgesImp.cpp
		 DlgPartImportIgesImp.h
		 DlgPartImportStep.ui
		 DlgPartImportStepImp.cpp
		 DlgPartImportStepImp.h
		 DlgPrimitives.cpp
		 DlgPrimitives.h
		 DlgPrimitives.ui
		 DlgSettings3DViewPart.ui
		 DlgSettings3DViewPartImp.cpp
		 DlgSettings3DViewPartImp.h
		 Part.qrc
		 PreCompiled.cpp
		 PreCompiled.h
		 ViewProvider.cpp
		 ViewProvider.h
		 ViewProviderBox.cpp
		 ViewProviderBox.h
		 ViewProviderCurveNet.cpp
		 ViewProviderCurveNet.h
		 ViewProviderImport.cpp
		 ViewProviderImport.h
		 Workbench.cpp
		 Workbench.h
)

add_library(PartGui SHARED ${PartGui_SRCS})

target_link_libraries(PartGui ${PartGui_LIBS})

set_target_properties(PartGui PROPERTIES SUFFIX ".pyd")

install(TARGETS PartGui DESTINATION lib)
