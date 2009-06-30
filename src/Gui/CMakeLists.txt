#add_subdirectory(Icons)

add_definitions(-DFCGui)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")

include_directories(
		 ${CMAKE_CURRENT_SOURCE_DIR}
		 ${CMAKE_CURRENT_BINARY_DIR}
		 ${CMAKE_CURRENT_SOURCE_DIR}/..
		 ${CMAKE_CURRENT_BINARY_DIR}/..
		 ${CMAKE_CURRENT_BINARY_DIR}/Language
		 ${CMAKE_CURRENT_BINARY_DIR}/propertyeditor
		 ${CMAKE_CURRENT_BINARY_DIR}/TaskView
		 ${Boost_INCLUDE_DIRS}
		 ${COIN_INCLUDE_DIR}
		 ${PYTHON_INCLUDE_PATH}
		 ${SOQT_INCLUDE_DIR}
		 ${XERCES_INCLUDE_DIR})

if(WIN32)
		 set(FreeCADGui_LIBS
		 		 FreeCADApp
		 		 debug ${COIN3D_LIBRARY_DEBUG}
		 		 optimized ${COIN3D_LIBRARY_RELEASE}
		 		 debug ${PYTHON_DEBUG_LIBRARY}
		 		 optimized ${PYTHON_LIBRARY}
		 		 debug ${SOQT_LIBRARY_DEBUG}
		 		 optimized ${SOQT_LIBRARY_RELEASE}
		 		 debug ${QT_DEBUG_LIBRARIES}
		 		 optimized ${QT_LIBRARIES}
		 		 ${XERCES_LIBRARIES})
else(WIN32)
		 set(FreeCADGui_LIBS
		 		 FreeCADBase
		 		 FreeCADApp
		 		 ${COIN3D_LIBRARY}
		 		 ${PYTHON_LIBRARY}
		 		 ${SOQT_LIBRARIES}
		 		 ${QT_LIBRARIES}
		 		 ${XERCESC_LIBRARIES}
		 		 ${Boost_LIBRARIES}
#libFreeCADGui_la_LIBADD		 =\
#		 		 Icons/libIcons.la \
#		 		 Language/libLanguage.la \
#		 		 propertyeditor/libpropertyeditor.la \
#		 		 -l@PYTHON_LIB@ \
#		 		 -lQtUiTools \
#		 		 -lxerces-c \
#		 		 -lboost_filesystem \
#		 		 -lboost_signals \
#		 		 -lFreeCADBase \
#		 		 -lFreeCADApp
)
endif(WIN32)

generate_from_xml(DocumentPy)
generate_from_xml(PythonWorkbenchPy)
generate_from_xml(ViewProviderPy)
generate_from_xml(ViewProviderDocumentObjectPy)
generate_from_xml(ViewProviderPythonFeaturePy)
generate_from_xml(WorkbenchPy)

generate_from_py(FreeCADGuiInit GuiInitScript.h)

set(Gui_MOC_HDRS
		 Action.h
		 CallTips.h
		 CombiView.h
		 DlgActionsImp.h
		 DlgActivateWindowImp.h
		 DlgCommandsImp.h
		 DlgCustomizeImp.h
		 DlgDisplayPropertiesImp.h
		 DlgEditorImp.h
		 DlgGeneralImp.h
		 DlgInputDialogImp.h
		 DlgKeyboardImp.h
		 DlgMacroExecuteImp.h
		 DlgMacroRecordImp.h
		 DlgMaterialPropertiesImp.h
		 DlgOnlineHelpImp.h
		 DlgParameterImp.h
		 DlgPreferencesImp.h
		 DlgReportViewImp.h
		 DlgSettings3DViewImp.h
		 DlgSettingsColorGradientImp.h
		 DlgSettingsDocumentImp.h
		 DlgSettingsImageImp.h
		 DlgSettingsMacroImp.h
		 DlgTipOfTheDayImp.h
		 DlgToolbarsImp.h
		 DlgUndoRedo.h
		 DockWindow.h
		 DockWindowManager.h
		 EditorView.h
		 FileDialog.h
		 Flag.h
		 HelpView.h
		 InputVector.h
		 MainWindow.h
		 MDIView.h
		 NetworkRetriever.h
		 OnlineDocumentation.h
		 PrefWidgets.h
		 ProgressBar.h
		 PropertyPage.h
		 PropertyView.h
		 PythonConsole.h
		 PythonEditor.h
		 ReportView.h
		 SceneInspector.h
		 SelectionView.h
		 SpinBox.h
		 Splashscreen.h
		 TaskPanelView.h
		 TextEdit.h
		 ToolBox.h
		 Tree.h
		 View3DInventor.h
		 WidgetFactory.h
		 Widgets.h
		 Language/Translator.h
		 propertyeditor/PropertyEditor.h
		 propertyeditor/PropertyItem.h
		 propertyeditor/PropertyItemDelegate.h
		 propertyeditor/PropertyModel.h
		 TaskView/TaskAppearance.h
		 TaskView/TaskEditControl.h
		 TaskView/TaskView.h
)
#qt4_wrap_cpp(Gui_MOC_SRCS ${Gui_MOC_HDRS})
fc_wrap_cpp(Gui_MOC_SRCS ${Gui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${Gui_MOC_SRCS})

set(iis_MOC_HDRS
		 iisTaskPanel/src/iisiconlabel.h
		 iisTaskPanel/src/iistaskbox.h
		 iisTaskPanel/src/iistaskgroup.h
		 iisTaskPanel/src/iistaskheader.h
)
qt4_wrap_cpp(iis_MOC_SRCS ${iis_MOC_HDRS})
#SOURCE_GROUP("Moc" FILES ${iis_MOC_SRCS})

SET(Gui_UIC_SRCS
		 AboutApplication.ui
		 DlgActions.ui
		 DlgActivateWindow.ui
		 DlgAuthorization.ui
		 DlgChooseIcon.ui
		 DlgCommands.ui
		 DlgDisplayProperties.ui
		 DlgEditor.ui
		 DlgGeneral.ui
		 DlgInputDialog.ui
		 DlgKeyboard.ui
		 DlgMacroExecute.ui
		 DlgMacroRecord.ui
		 DlgMaterialProperties.ui
		 DlgOnlineHelp.ui
		 DlgParameter.ui
		 DlgPreferences.ui
		 DlgProjectInformation.ui
		 DlgReportView.ui
		 DlgSettings3DView.ui
		 DlgSettingsColorGradient.ui
		 DlgSettingsDocument.ui
		 DlgSettingsImage.ui
		 DlgSettingsMacro.ui
		 DlgTipOfTheDay.ui
		 DlgToolbars.ui
		 DlgTreeWidget.ui
		 SceneInspector.ui
		 InputVector.ui
		 Placement.ui
		 TaskView/TaskAppearance.ui
		 TaskView/TaskEditControl.ui
)

SET(Gui_RES_SRCS
		 Icons/resource.qrc
		 Language/translation.qrc
		 iisTaskPanel/src/iisTaskPanel.qrc
)
QT4_ADD_RESOURCES(Gui_QRC_SRCS ${Gui_RES_SRCS})

QT4_WRAP_UI(Gui_UIC_HDRS ${Gui_UIC_SRCS})
SOURCE_GROUP("Uic" FILES ${Gui_UIC_HDRS})

SET(Command_SRCS
		 Action.cpp
		 Action.h
		 Command.cpp
		 Command.h
		 CommandDoc.cpp
		 CommandFeat.cpp
		 CommandStd.cpp
		 CommandWindow.cpp
		 CommandTest.cpp
		 CommandView.cpp
)
SOURCE_GROUP("Command" FILES ${Command_SRCS})

SET(Dialog_SRCS
		 AboutApplication.ui
		 DlgActivateWindow.ui
		 DlgActivateWindowImp.cpp
		 DlgActivateWindowImp.h
		 DlgAuthorization.ui
		 DlgDisplayProperties.ui
		 DlgDisplayPropertiesImp.cpp
		 DlgDisplayPropertiesImp.h
		 DlgInputDialog.ui
		 DlgInputDialogImp.cpp
		 DlgInputDialogImp.h
		 DlgMacroExecute.ui
		 DlgMacroExecuteImp.cpp
		 DlgMacroExecuteImp.h
		 DlgMacroRecord.ui
		 DlgMacroRecordImp.cpp
		 DlgMacroRecordImp.h
		 DlgMaterialProperties.ui
		 DlgMaterialPropertiesImp.cpp
		 DlgMaterialPropertiesImp.h
		 DlgParameter.ui
		 DlgParameterImp.cpp
		 DlgParameterImp.h
		 DlgProjectInformation.ui
		 DlgProjectInformationImp.cpp
		 DlgProjectInformationImp.h
		 DlgTipOfTheDay.ui
		 DlgTipOfTheDayImp.cpp
		 DlgTipOfTheDayImp.h
		 DlgTreeWidget.ui
		 DlgUndoRedo.cpp
		 DlgUndoRedo.h
		 InputVector.ui
		 InputVector.cpp
		 InputVector.h
		 Placement.ui
		 Placement.cpp
		 Placement.h
		 PropertyPage.cpp
		 PropertyPage.h
		 SceneInspector.ui
		 SceneInspector.cpp
		 SceneInspector.h
)
SOURCE_GROUP("Dialog" FILES ${Dialog_SRCS})

SET(Dialog_Customize_SRCS
		 DlgActions.ui
		 DlgActionsImp.cpp
		 DlgActionsImp.h
		 DlgChooseIcon.ui
		 DlgCommands.ui
		 DlgCommandsImp.cpp
		 DlgCommandsImp.h
		 DlgCustomizeImp.cpp
		 DlgCustomizeImp.h
		 DlgKeyboard.ui
		 DlgKeyboardImp.cpp
		 DlgKeyboardImp.h
		 DlgToolbarsImp.cpp
		 DlgToolbars.ui
		 DlgToolbarsImp.h
)
SOURCE_GROUP("Dialog\\Customize" FILES ${Dialog_Customize_SRCS})

SET(Dialog_Settings_SRCS
		 DlgEditor.ui
		 DlgEditorImp.cpp
		 DlgEditorImp.h
		 DlgGeneral.ui
		 DlgGeneralImp.cpp
		 DlgGeneralImp.h
		 DlgOnlineHelp.ui
		 DlgOnlineHelpImp.cpp
		 DlgOnlineHelpImp.h
		 DlgPreferences.ui
		 DlgPreferencesImp.cpp
		 DlgPreferencesImp.h
		 DlgReportView.ui
		 DlgReportViewImp.cpp
		 DlgReportViewImp.h
		 DlgSettings3DView.ui
		 DlgSettings3DViewImp.cpp
		 DlgSettings3DViewImp.h
		 DlgSettingsColorGradient.ui
		 DlgSettingsColorGradientImp.cpp
		 DlgSettingsColorGradientImp.h
		 DlgSettingsDocument.ui
		 DlgSettingsDocumentImp.cpp
		 DlgSettingsDocumentImp.h
		 DlgSettingsImage.ui
		 DlgSettingsImageImp.cpp
		 DlgSettingsImageImp.h
		 DlgSettingsMacro.ui
		 DlgSettingsMacroImp.cpp
		 DlgSettingsMacroImp.h
)
SOURCE_GROUP("Dialog\\Settings" FILES ${Dialog_Settings_SRCS})

SET(Dock_Windows_SRCS
		 CombiView.cpp
		 CombiView.h
		 DockWindow.cpp
		 DockWindow.h
		 HelpView.cpp
		 HelpView.h
		 PropertyView.cpp
		 PropertyView.h
		 ReportView.cpp
		 ReportView.h
		 SelectionView.cpp
		 SelectionView.h
		 TaskPanelView.cpp
		 TaskPanelView.h
		 ToolBox.cpp
		 ToolBox.h
		 Tree.cpp
		 Tree.h
)
SOURCE_GROUP("Dock Windows" FILES ${Dock_Windows_SRCS})

SET(Editor_SRCS
		 CallTips.cpp
		 CallTips.h
		 EditorView.cpp
		 EditorView.h
		 PythonConsole.cpp
		 PythonConsole.h
		 PythonConsolePy.cpp
		 PythonConsolePy.h
		 PythonEditor.cpp
		 PythonEditor.h
		 SyntaxHighlighter.cpp
		 SyntaxHighlighter.h
		 TextEdit.cpp
		 TextEdit.h
)
SOURCE_GROUP("Editor" FILES ${Editor_SRCS})

SET(Help_SRCS
		 Assistant.cpp
		 Assistant.h
		 NetworkRetriever.cpp
		 NetworkRetriever.h
		 OnlineDocumentation.cpp
		 OnlineDocumentation.h
		 WhatsThis.cpp
		 WhatsThis.h
)
SOURCE_GROUP("Help" FILES ${Help_SRCS})

SET(Inventor_SRCS
		 SoFCBackgroundGradient.cpp
		 SoFCBackgroundGradient.h
		 SoFCBoundingBox.cpp
		 SoFCBoundingBox.h
		 SoFCColorBar.cpp
		 SoFCColorBar.h
		 SoFCColorGradient.cpp
		 SoFCColorGradient.h
		 SoFCColorLegend.cpp
		 SoFCColorLegend.h
		 SoFCDB.cpp
		 SoFCDB.h
		 SoFCInteractiveElement.cpp
		 SoFCInteractiveElement.h
		 SoFCOffscreenRenderer.cpp
		 SoFCOffscreenRenderer.h
		 SoFCSelection.cpp
		 SoFCSelection.h
		 SoFCSelectionAction.cpp
		 SoFCSelectionAction.h
		 SoFCVectorizeSVGAction.cpp
		 SoFCVectorizeSVGAction.h
		 SoNavigationDragger.cpp
		 SoNavigationDragger.h
		 SoTextLabel.cpp
		 SoTextLabel.h
)
SOURCE_GROUP("Inventor" FILES ${Inventor_SRCS})

SET(Language_SRCS
		 Language/Translator.cpp
		 Language/Translator.h
)
SOURCE_GROUP("Language" FILES ${Language_SRCS})

SET(Propertyeditor_SRCS
		 propertyeditor/PropertyEditor.cpp
		 propertyeditor/PropertyEditor.h
		 propertyeditor/PropertyItem.cpp
		 propertyeditor/PropertyItem.h
 		 propertyeditor/PropertyItemDelegate.cpp
		 propertyeditor/PropertyItemDelegate.h
		 propertyeditor/PropertyModel.cpp
		 propertyeditor/PropertyModel.h

)
SOURCE_GROUP("Propertyeditor" FILES ${Propertyeditor_SRCS})

SET(Task_View_SRCS
		 TaskView/TaskAppearance.cpp
		 TaskView/TaskAppearance.h
		 TaskView/TaskAppearance.ui
		 TaskView/TaskEditControl.cpp
		 TaskView/TaskEditControl.h
		 TaskView/TaskEditControl.ui
		 TaskView/TaskView.cpp
		 TaskView/TaskView.h
)
SOURCE_GROUP("Task View" FILES ${Task_View_SRCS})

SET(iisTaskPanel_SRCS
		 iisTaskPanel/src/iisfreecadscheme.cpp
		 iisTaskPanel/src/iisfreecadscheme.h
		 iisTaskPanel/src/iisiconlabel.cpp
		 iisTaskPanel/src/iisiconlabel.h
		 iisTaskPanel/src/iistaskbox.cpp
		 iisTaskPanel/src/iistaskbox.h
		 iisTaskPanel/src/iistaskgroup.cpp
		 iisTaskPanel/src/iistaskgroup.h
		 iisTaskPanel/src/iistaskheader.cpp
		 iisTaskPanel/src/iistaskheader.h
		 iisTaskPanel/src/iistaskpanel.cpp
		 iisTaskPanel/src/iistaskpanel.h
		 iisTaskPanel/src/iistaskpanelscheme.cpp
		 iisTaskPanel/src/iistaskpanelscheme.h
		 iisTaskPanel/src/iiswinxptaskpanelscheme.cpp
		 iisTaskPanel/src/iiswinxptaskpanelscheme.h
		 iisTaskPanel/src/iistaskpanel_global.h
)
SOURCE_GROUP("iisTaskPanel" FILES ${iisTaskPanel_SRCS})

#SET(Resource_RCS
#		 Icons/resource.qrc
#		 Language/translation.qrc)
#fc_add_resources(Resource_SRCS ${Resource_RCS})
#SET(Resource_SRCS
#		 ${Resource_SRCS}
#		 Icons/resource.qrc
#		 Language/translation.qrc
#)
#SOURCE_GROUP("Resource" FILES ${Resource_SRCS})

SET(View3D_SRCS
		 Flag.cpp
		 Flag.h
		 MouseModel.cpp
		 MouseModel.h
		 NavigationStyle.cpp
		 NavigationStyle.h
		 SplitView3DInventor.cpp
		 SplitView3DInventor.h
		 View.cpp
		 View.h
		 View3DInventor.cpp
		 View3DInventor.h
		 View3DInventorExamples.cpp
		 View3DInventorExamples.h
		 View3DInventorViewer.cpp
		 View3DInventorViewer.h
		 View3DPy.cpp
		 View3DPy.h
)
SOURCE_GROUP("View3D" FILES ${View3D_SRCS})

SET(Viewprovider_SRCS
		 ViewProvider.cpp
		 ViewProvider.h
		 ViewProviderAnnotation.cpp
		 ViewProviderAnnotation.h
		 ViewProviderDocumentObject.cpp
		 ViewProviderDocumentObject.h
		 ViewProviderDocumentObjectGroup.cpp
		 ViewProviderDocumentObjectGroup.h
		 ViewProviderDocumentObjectPy.xml
		 ViewProviderDocumentObjectPyImp.cpp
		 ViewProviderPythonFeaturePy.xml
		 ViewProviderPythonFeaturePyImp.cpp
		 ViewProviderExtern.cpp
		 ViewProviderExtern.h
		 ViewProviderFeature.cpp
		 ViewProviderFeature.h
		 ViewProviderGeometryObject.cpp
		 ViewProviderGeometryObject.h
		 ViewProviderInventorObject.cpp
		 ViewProviderInventorObject.h
		 ViewProviderMeasureDistance.cpp
		 ViewProviderMeasureDistance.h
		 ViewProviderPy.xml
		 ViewProviderPyImp.cpp
		 ViewProviderPythonFeature.cpp
		 ViewProviderPythonFeature.h
		 ViewProviderVRMLObject.cpp
		 ViewProviderVRMLObject.h
)
SOURCE_GROUP("Viewprovider" FILES ${Viewprovider_SRCS})

SET(Widget_SRCS
		 FileDialog.cpp
		 FileDialog.h
		 MainWindow.cpp
		 MainWindow.h
		 MDIView.cpp
		 MDIView.h
		 PrefWidgets.cpp
		 PrefWidgets.h
		 ProgressBar.cpp
		 ProgressBar.h
		 SpinBox.cpp
		 SpinBox.h
		 Splashscreen.cpp
		 Splashscreen.h
		 WidgetFactory.cpp
		 WidgetFactory.h
		 Widgets.cpp
		 Widgets.h
		 Window.cpp
		 Window.h
)
SOURCE_GROUP("Widget" FILES ${Widget_SRCS})

SET(Workbench_SRCS
		 DockWindowManager.cpp
		 DockWindowManager.h
		 MenuManager.cpp
		 MenuManager.h
		 PythonWorkbenchPy.xml
		 PythonWorkbenchPyImp.cpp
		 ToolBarManager.cpp
		 ToolBarManager.h
		 ToolBoxManager.cpp
		 ToolBoxManager.h
		 Workbench.cpp
		 Workbench.h
		 WorkbenchFactory.cpp
		 WorkbenchFactory.h
		 WorkbenchManager.cpp
		 WorkbenchManager.h
		 WorkbenchPy.xml
		 WorkbenchPyImp.cpp
)
SOURCE_GROUP("Workbench" FILES ${Workbench_SRCS})

SET(FreeCADGui_SRCS
		 Application.cpp
		 Application.h
		 ApplicationPy.cpp
		 BitmapFactory.cpp
		 BitmapFactory.h
		 Document.cpp
		 Document.h
		 DocumentPy.xml
		 DocumentPyImp.cpp
		 Doxygen.cpp
		 FreeCADGuiInit.py
		 GuiConsole.cpp
		 GuiConsole.h
		 InventorAll.h
		 Macro.cpp
		 Macro.h
		 PreCompiled.cpp
		 PreCompiled.h
		 Qt4All.h
		 resource.cpp
		 Selection.cpp
		 Selection.h
		 Thumbnail.cpp
		 Thumbnail.h
		 WaitCursor.cpp
		 WaitCursor.h
)
#SOURCE_GROUP("" FILES ${FreeCADGui_SRCS})

SET(FreeCADGui_SRCS
		 #${Gui_MOC_SRCS}
		 ${iis_MOC_SRCS}
		 ${Gui_QRC_SRCS}
		 ${Gui_UIC_HDRS}
		 ${Command_SRCS}
		 ${Dialog_SRCS}
		 ${Dialog_Customize_SRCS}
		 ${Dialog_Settings_SRCS}
		 ${Dock_Windows_SRCS}
		 ${Editor_SRCS}
		 ${Help_SRCS}
		 ${Inventor_SRCS}
		 ${Language_SRCS}
		 ${Propertyeditor_SRCS}
		 ${Task_View_SRCS}
		 ${iisTaskPanel_SRCS}
		 ${Resource_SRCS}
		 ${View3D_SRCS}
		 ${Viewprovider_SRCS}
		 ${Widget_SRCS}
		 ${Workbench_SRCS}
		 ${FreeCADGui_SRCS}
)

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/../Main)

add_library(FreeCADGui SHARED ${FreeCADGui_SRCS})

target_link_libraries(FreeCADGui ${FreeCADGui_LIBS})

install(TARGETS FreeCADGui DESTINATION lib)
