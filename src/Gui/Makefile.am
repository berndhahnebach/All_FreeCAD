noinst_LIBRARIES=libFreeCADGui.a

BUILT_SOURCES=\
moc_Application.cpp \
moc_ButtonGroup.cpp \
moc_ButtonGroup.cpp \
moc_Command.cpp \
moc_CommandLine.cpp \
moc_DlgActions.cpp \
moc_DlgCmdbars.cpp \
moc_DlgCommands.cpp \
moc_DlgCustomize.cpp \
moc_DlgCustomizeImp.cpp \
moc_DlgDocTemplates.cpp\
moc_DlgDocTemplatesImp.cpp \
moc_DlgMacroExecute.cpp \
moc_DlgMacroExecuteImp.cpp \
moc_DlgMacroRecord.cpp \
moc_DlgMacroRecordImp.cpp \
moc_DlgMacros.cpp \
moc_DlgOnlineHelp.cpp \
moc_DlgParameter.cpp \
moc_DlgParameterImp.cpp \
moc_DlgPreferences.cpp \
moc_DlgPreferencesImp.cpp \
moc_DlgSettings.cpp \
moc_DlgSettingsImp.cpp \
moc_DlgToolbars.cpp \
moc_DlgUndoRedo.cpp \
moc_Document.cpp \
moc_HtmlView.cpp \
moc_PrefWidgets.cpp \
moc_Splashscreen.cpp \
moc_Tree.cpp \
moc_View.cpp\
moc_View3D.cpp \
moc_WidgetFactory.cpp \
moc_Widgets.cpp \
moc_Window.cpp \
DlgActions.h \
DlgCmdbars.h \
DlgCommands.h \
DlgCustomize.h\
DlgDocTemplates.h \
DlgMacroExecute.h\
DlgMacroRecord.h \
DlgMacros.h \
DlgOnlineHelp.h \
DlgParameter.h \
DlgPreferences.h \
DlgSettings.h \
DlgToolbars.h \
DlgSettings3DView.h \
DlgSettingsMacro.h \
DlgEditor.h \
DlgGeneral.h \
moc_DlgSettings3DView.cpp \
moc_DlgSettingsMacro.cpp \
moc_DlgEditor.cpp \
moc_DlgGeneral.cpp


#DlgCustomize.cpp\
#DlgDocTemplates.cpp \
#DlgMacroExecute.cpp \
#DlgMacroRecord.cpp \
#DlgMacros.cpp \
#DlgParameter.cpp \
#DlgPreferences.cpp \
#DlgSettings.cpp


libFreeCADGui_a_SOURCES=\
DlgCustomize.cpp\
DlgDocTemplates.cpp \
DlgMacroExecute.cpp \
DlgMacroRecord.cpp \
DlgMacros.cpp \
DlgParameter.cpp \
DlgPreferences.cpp \
DlgSettings.cpp \
\
Application.cpp \
ButtonGroup.cpp \
CommandLine.cpp \
Command.cpp \
CommandStd.cpp  \
CommandView.cpp \
DlgCustomizeImp.cpp \
DlgDocTemplatesImp.cpp \
DlgMacroExecuteImp.cpp \
DlgMacroRecordImp.cpp \
DlgMacros.cpp \
DlgParameterImp.cpp \
DlgPreferencesImp.cpp \
DlgSettingsImp.cpp \
DlgUndoRedo.cpp \
Document.cpp \
GuiConsole.cpp \
HtmlView.cpp \
MouseModel.cpp \
PrefWidgets.cpp \
Splashscreen.cpp \
Themes.cpp \
Tree.cpp \
View.cpp \
View3D.cpp \
Window.cpp \
WidgetFactory.cpp \
Widgets.cpp 

#moc_DlgMacros.cpp


# rule for Qt MetaObject Compiler:
moc_%.cpp: %.h
	cd $(@D) && $(MOC) $(<F) -o $(@F)

# rules for Qt User Interface Compiler:
# (it adds path to #include automatically, so cd before calling it)
%.h: %.ui
	cd $(@D) && $(UIC) $(<F) -o $(@F)
%.cpp: %.h %.ui 
	cd $(@D) && $(UIC) -o $(@F) -impl $(*F).h $(*F).ui

