TEMPLATE          = lib
CONFIG		+= qt warn_on release
LANGUAGE	= C++
win32:DEFINES	+= FCGui
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
INTERFACES	= ../DlgActions.ui \
	../DlgCmdbars.ui \
	../DlgCommands.ui \
	../DlgCustomize.ui \
	../DlgDocTemplates.ui \
	../DlgEditor.ui \
	../DlgGeneral.ui \
	../DlgMacroExecute.ui \
	../DlgMacroRecord.ui \
	../DlgMacros.ui \
	../DlgOnlineHelp.ui \
	../DlgParameter.ui \
	../DlgPreferences.ui \
	../DlgSettings3DView.ui \
	../DlgSettings.ui \
	../DlgSettingsMacro.ui \
	../DlgToolbars.ui

HEADERS		=../Application.h \
		../ButtonGroup.h \
		../Command.h \
		../CommandLine.h \
		../DlgActions.h \
		../DlgActionsImp.h \
		../DlgCmdbars.h \
		../DlgCmdbarsImp.h \
		../DlgCommands.h \
		../DlgCommandsImp.h \
		../DlgCustomizeImp.h \
		../DlgDocTemplates.h \
		../DlgDocTemplatesImp.h \
		../DlgEditor.h \
		../DlgEditorImp.h \
		../DlgGeneral.h \
		../DlgGeneralImp.h \
		../DlgMacroExecute.h \
		../DlgMacroExecuteImp.h \
		../DlgMacroRecord.h \
		../DlgMacroRecordImp.h \
		../DlgOnlineHelp.h \
		../DlgOnlineHelpImp.h \
		../DlgParameter.h \
		../DlgParameterImp.h \
		../DlgPreferences.h \
		../DlgPreferencesImp.h \
		../DlgSettings3DView.h \
		../DlgSettings3DViewImp.h \
		../DlgSettings.h \
		../DlgSettingsImp.h \
		../DlgSettingsMacro.h \
		../DlgSettingsMacroImp.h \
		../DlgToolbars.h \
		../DlgToolbarsImp.h \
		../DlgUndoRedo.h \
		../Document.h \
		../GuiConsole.h \
		../HtmlView.h \
		../Macro.h \
		../MouseModel.h \
		../PrefWidgets.h \
		../Process.h \
		../ProcessQt.h \
		../PropertyPage.h \
		../PropertyView.h \
		../ReportView.h \
		../ScintillaQt.h \
		../Splashscreen.h \
		../Tools.h \
		../Tree.h \
		../View3D.h \
		../View3DInventor.h \
		../View.h \
		../WidgetFactory.h \
		../Widgets.h \
		../Window.h \
		../qextmdi/dummykmainwindow.h \
		../qextmdi/dummykpartsdockmainwindow.h \
		../qextmdi/dummyktoolbar.h \
		../qextmdi/exportdockclass.h \
		../qextmdi/kdocktabctl.h \
		../qextmdi/kdockwidget.h \
		../qextmdi/kdockwidget_private.h \
		../qextmdi/qextmdichildarea.h \
		../qextmdi/qextmdichildfrm.h \
		../qextmdi/qextmdichildfrmcaption.h \
		../qextmdi/qextmdichildview.h \
		../qextmdi/qextmdidefines.h \
		../qextmdi/qextmdiiterator.h \
		../qextmdi/qextmdimainfrm.h \
		../qextmdi/qextmdinulliterator.h \
		../qextmdi/qextmditaskbar.h \
		LanguageFactory.h

SOURCES		=../Application.cpp \
		../ButtonGroup.cpp \
		../Command.cpp \
		../CommandLine.cpp \
		../CommandStd.cpp \
		../CommandTest.cpp \
		../CommandView.cpp \
		../DlgActions.cpp \
		../DlgActionsImp.cpp \
		../DlgCmdbars.cpp \
		../DlgCmdbarsImp.cpp \
		../DlgCommands.cpp \
		../DlgCommandsImp.cpp \
		../DlgCustomizeImp.cpp \
		../DlgDocTemplates.cpp \
		../DlgDocTemplatesImp.cpp \
		../DlgEditor.cpp \
		../DlgEditorImp.cpp \
		../DlgGeneral.cpp \
		../DlgGeneralImp.cpp \
		../DlgMacroExecute.cpp \
		../DlgMacroExecuteImp.cpp \
		../DlgMacroRecord.cpp \
		../DlgMacroRecordImp.cpp \
		../DlgOnlineHelp.cpp \
		../DlgOnlineHelpImp.cpp \
		../DlgParameter.cpp \
		../DlgParameterImp.cpp \
		../DlgPreferences.cpp \
		../DlgPreferencesImp.cpp \
		../DlgSettings3DView.cpp \
		../DlgSettings3DViewImp.cpp \
		../DlgSettings.cpp \
		../DlgSettingsImp.cpp \
		../DlgSettingsMacro.cpp \
		../DlgSettingsMacroImp.cpp \
		../DlgToolbars.cpp \
		../DlgToolbarsImp.cpp \
		../DlgUndoRedo.cpp \
		../Document.cpp \
		../GuiConsole.cpp \
		../HtmlView.cpp \
		../Macro.cpp \
		../MouseModel.cpp \
		../PrefWidgets.cpp \
		../Process.cpp \
		../PropertyPage.cpp \
		../PropertyView.cpp \
		../ReportView.cpp \
		../ScintillaQt.cpp \
		../Splashscreen.cpp \
		../Tools.cpp \
		../Tree.cpp \
		../View3D.cpp \
		../View3DInventor.cpp \
		../View.cpp \
		../WidgetFactory.cpp \
		../Widgets.cpp \
		../Window.cpp \
		../qextmdi/kdocktabctl.cpp \
		../qextmdi/kdockwidget.cpp \
		../qextmdi/kdockwidget_private.cpp \
		../qextmdi/qextmdichildarea.cpp \
		../qextmdi/qextmdichildfrm.cpp \
		../qextmdi/qextmdichildfrmcaption.cpp \
		../qextmdi/qextmdichildview.cpp \
		../qextmdi/qextmdimainfrm.cpp \
		../qextmdi/qextmditaskbar.cpp \
		LanguageFactory.cpp \
		FreeCAD.po

TRANSLATIONS	= FreeCAD_fr.ts \
		  FreeCAD_de.ts

TARGET		= FreeCAD
DESTDIR = ../../../lib
DLLDESTDIR= ../../../bin
INCLUDEPATH	+= . scintilla qextmdi Inventor
