# FreeCAD gui init module
# (c) 2003 Jürgen Riegel
#
# Gathering all the information to start FreeCAD
# This is the second one of three init scripts, the third one
# runs when the gui is up

#***************************************************************************
#*   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
#*                                                                         *
#*   This file is part of the FreeCAD CAx development system.              *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   FreeCAD is distributed in the hope that it will be useful,            *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with FreeCAD; if not, write to the Free Software        * 
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#*   Juergen Riegel 2002                                                   *
#***************************************************************************/


# imports the one and only
import FreeCAD, FreeCADGui

# shortcuts
Gui = FreeCADGui

# Important definitions
class Workbench:
	def Start(self):
		Err("Workbench start() not implemented!")
	def Stop(self):
		Err("Workbench start() not implemented!")


class StandardWorkbench ( Workbench ):
	"Standard workbench object"
	def Start(self):
		Log ('Init: Setup <none> workbench\n   Set up toolbar...\n')

#		list = ["Std_MDIToplevel","Std_MDITabed","Std_MDINormal","Separator",
#		"Std_TilePragmatic","Std_TileVertical","Std_TileHoricontal","Separator",
#		"Std_Test1","Std_Test2","Separator","Std_DlgParameter","Std_DlgPreferences",
#		"Std_DlgMacroRecord","Std_DlgMacroExecute","Std_DlgCustomize","Separator",
#		"Std_CommandLine"]
#		Gui.ToolbarAppendItems("TestTools", list, 0)

		Log ('   Set up commandbar...\n')
		list = ["Std_Test1", "Std_Test2", "Std_Test3", "Std_Test4", "Std_Test5", "Std_Test6", "Std_Test7", "Std_Test8"]
		Gui.CommandbarAppendItems("TestTools", list, 0)

		list = ["Std_ViewFitAll", "Std_ViewAxo", "Separator", "Std_ViewFront",
		"Std_ViewRight","Std_ViewTop","Separator","Std_ViewRear","Std_ViewLeft","Std_ViewBottom"]
		Gui.CommandbarAppendItems("Standard views", list, 0)

		list = ["Std_DlgParameter","Std_DlgPreferences","Std_DlgMacroRecord",
		"Std_DlgMacroExecute","Std_DlgCustomize","Std_CommandLine"]
		Gui.CommandbarAppendItems("Special Ops", list, 0)

		Log ('   Set up menues...\n')

	def Stop(self):
		Log ('Init: Stopping <none> workbench...\n')
		Gui.CommandbarDelete("Special Ops")
		Gui.CommandbarDelete("TestTools")
		Gui.CommandbarDelete("Standard views")
		Gui.ToolbarDelete("Standard views")
#		Gui.ToolbarDelete("TestTools")
		
def InitToolbars():
	Log ('   Set up toolbars...\n')

	# standard file tool bar
	list = ["Std_New", "Std_Open", "Std_Save", "Std_Print", "Separator", "Std_Cut","Std_Copy",
	"Std_Paste", "Separator", "Std_Undo", "Std_Redo", "Separator", "Std_Workbench","Std_WhatsThis"]
	Gui.ToolbarAppendItems("file operations", list, 0, 0)

	# macro tool bar
	list = ["Std_DlgMacroRecord", "Std_DlgMacroStop", "Std_DlgMacroExecute"]
	Gui.ToolbarAppendItems("Macro recording", list, 0, 0)

	# view tool bar
	list = ["Std_ViewFitAll", "Std_ViewAxo", "Separator",
	"Std_ViewFront","Std_ViewRight","Std_ViewTop", 
	"Separator", "Std_ViewRear", "Std_ViewLeft","Std_ViewBottom"]
	Gui.ToolbarAppendItems("Standard views",list, 0, 0)

#	# test tool bar
#	list = ["Std_MDIToplevel","TestTools","Std_MDITabed","Std_MDINormal",
#	"Separator","Std_TilePragmatic","Std_TileVertical","Std_TileHoricontal",
#	"Separator","Std_Test1","Std_Test2","Separator","Std_DlgParameter",
#	"Std_DlgPreferences","Std_DlgMacroRecord","Std_DlgMacroExecute","Std_DlgCustomize",
#	"Separator","Std_CommandLine"]
#	Gui.ToolbarAppendItems("TestTools", list, 0)
#
#	# test tool bar
#	list = ["Std_ViewFitAll","Std_ViewAxo","Separator","Std_ViewFront",
#	"Std_ViewRight","Std_ViewTop","Std_ViewRear","Std_ViewLeft","Std_ViewBottom"]
#	Gui.ToolbarAppendItems("Standard views", list, 0)
	

def InitMenues():
	Log ('   Set up menues...\n')

	list = ["Std_New", "Std_Open", "Std_Save", "Std_SaveAs", "Separator", "Std_Print",
	"Separator","Std_MRU","Separator", "Std_Quit"]
	Gui.MenuAppendItems("&File", list, 0, 0)

	list = ["Std_Cut", "Std_Copy", "Std_Paste", "Separator", "Std_Undo", "Std_Redo",
	"Separator", "Std_DlgPreferences"]
	Gui.MenuAppendItems("&Edit", list, 0, 0)

	list = ["Std_ViewCreateInventor", "Std_ViewCreateOCC","Separator","Std_ViewFullScreen","Separator"]
	Gui.MenuAppendItems("&View", list, 1, 0)

	list = ["Std_CommandLine", "Std_DlgParameter", "Std_OCAFBrowser", "Separator",
	"Std_DlgMacroRecord", "Std_DlgMacroStop", "Std_DlgMacroExecute", "Separator", "Std_DlgCustomize"]
	Gui.MenuAppendItems("&Tools", list, 1, 0)

	list = ["Std_Test1","Std_Test2", "Std_Test3", "Std_Test4", "Std_Test5", "Std_Test6", "Std_Test7", "Std_Test8"]
	Gui.MenuAppendItems("Test &Commands", list, 0, 0)

	list = ["Std_ViewExample1","Std_ViewExample2","Std_ViewExample3","Std_ViewIvDecorationOn","Std_ViewIvDecorationOff","Std_ViewIvStereoOn","Std_ViewIvStereoOff","Std_ViewIvIssueCamPos"]
	Gui.MenuAppendItems("Inventor View", list, 0, 0)

	list = ["Std_CloseActiveWindow", "Std_CloseAllWindows", "Separator", "Std_ActivateNextWindow", "Std_ActivatePrevWindow", 
	"Separator", "Std_TilePragmatic", "Std_TileHoricontal", "Std_TileVertical", "Separator", "Std_Windows"] 
	Gui.MenuAppendItems("&Windows", list, 0, 0)

	list = ["Std_OnlineHelp", "Std_TipOfTheDay", "Separator", "Std_About", "Std_AboutQt", "Separator", "Std_WhatsThis"]
	Gui.MenuAppendItems("&Help", list, 0, 0)

def InitCmdBar():
	Log ('   Set up command bar...\n')
	list = ["Std_Test1","Std_Test2", "Std_Test3", "Std_Test4", "Std_Test5", "Std_Test6", "Std_Test7", "Std_Test8"]
	Gui.CommandbarAppendItems("TestTools", list, 0)

	list = ["Std_ViewFitAll","Std_ViewAxo","Separator","Std_ViewFront","Std_ViewRight",
	"Std_ViewTop","Separator","Std_ViewRear","Std_ViewLeft","Std_ViewBottom"]
	Gui.CommandbarAppendItems("Standard views", list, 0)

	list = ["Std_DlgParameter","Std_DlgPreferences","Std_DlgMacroRecord","Std_DlgMacroExecute",
	"Std_DlgCustomize","Std_CommandLine"]
	Gui.CommandbarAppendItems("Special Ops",list,0)

	list = ["Std_ViewExample1","Std_ViewExample2","Std_ViewExample3","Std_ViewIvDecorationOn",
	"Std_ViewIvDecorationOff","Std_ViewIvStereoOn","Std_ViewIvStereoOff"]
	Gui.CommandbarAppendItems("Inventor stuff",list,0)



def InitApplications():
	# Checking on FreeCAD Module path ++++++++++++++++++++++++++++++++++++++++++
	ModDir = FreeCAD.ConfigGet("HomePath")+'src/Mod'
	#print FreeCAD.ConfigGet("HomePath")
	if os.path.isdir(FreeCAD.ConfigGet("HomePath")+'Mod'):
		ModDir = FreeCAD.ConfigGet("HomePath")+'Mod'
	else:
		if os.path.isdir(FreeCAD.ConfigGet("HomePath")+'src\\Mod'):
			ModDir = FreeCAD.ConfigGet("HomePath")+'src\\Mod'
	# Searching modules dirs +++++++++++++++++++++++++++++++++++++++++++++++++++
	ModDirs = dircache.listdir(ModDir)
	#print ModDirs
	Log('   Searching modules...\n')
	ModPar = App.ParamGet("System parameter:Modules")
	for Dir in ModDirs:
		if ( (Dir != 'CVS') & (Dir != '__init__.py')):
			Log('      Initializing: ' + Dir + '... ')
			InstallFile = os.path.join(os.path.join(ModDir,Dir),"InitGui.py")
			if ( os.path.exists(InstallFile) ):
				execfile(InstallFile)
				Log('done\n')
			else:
				Wrn("InitGui.py not found! "+Dir+" not initialized!\n")
 

Log ('Init: starting Gui::FreeCADGuiInit.py\n')

# init the gui

# signal that the gui is up
App.GuiUp = 1
App.Gui = FreeCADGui

Gui.WorkbenchAdd("<none>",StandardWorkbench())

# init modules
InitApplications()

InitMenues()
InitToolbars()
#InitCmdBar()

# set standard workbench
Gui.WorkbenchActivate("<none>")


Log ('Init: Gui::FreeCADGuiInit.py done\n')
