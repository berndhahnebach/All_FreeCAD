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
		Log ('   Set up toolbar...\n')
		Gui.ToolbarAddTo("TestTools","Std_MDIToplevel")
		Gui.ToolbarAddTo("TestTools","Std_MDITabed")
		Gui.ToolbarAddTo("TestTools","Std_MDINormal")
		Gui.ToolbarAddSeperator("TestTools")
		Gui.ToolbarAddTo("TestTools","Std_TilePragmatic")
		Gui.ToolbarAddTo("TestTools","Std_TileVertical")
		Gui.ToolbarAddTo("TestTools","Std_TileHoricontal")
		Gui.ToolbarAddSeperator("TestTools")
		Gui.ToolbarAddTo("TestTools","Std_Test1")
		Gui.ToolbarAddTo("TestTools","Std_Test2")
		Gui.ToolbarAddSeperator("TestTools")
		Gui.ToolbarAddTo("TestTools","Std_DlgParameter")
		Gui.ToolbarAddTo("TestTools","Std_DlgPreferences")
		Gui.ToolbarAddTo("TestTools","Std_DlgMacroRecord")
		Gui.ToolbarAddTo("TestTools","Std_DlgMacroExecute")
		Gui.ToolbarAddTo("TestTools","Std_DlgCustomize")
		Gui.ToolbarAddSeperator("TestTools")
		Gui.ToolbarAddTo("TestTools","Std_CommandLine")

		# test tool bar
		Gui.ToolbarAddTo("Standard views","Std_ViewFitAll")
		Gui.ToolbarAddTo("Standard views","Std_ViewAxo")
		Gui.ToolbarAddSeperator("Standard views")
		Gui.ToolbarAddTo("Standard views","Std_ViewFront")
		Gui.ToolbarAddTo("Standard views","Std_ViewRight")
		Gui.ToolbarAddTo("Standard views","Std_ViewTop")
		Gui.ToolbarAddSeperator("Standard views")
		Gui.ToolbarAddTo("Standard views","Std_ViewRear")
		Gui.ToolbarAddTo("Standard views","Std_ViewLeft")
		Gui.ToolbarAddTo("Standard views","Std_ViewBottom")

		Log ('   Set up commandbar...\n')
		Gui.CommandbarAddTo("TestTools","Std_Test1")
		Gui.CommandbarAddTo("TestTools","Std_Test2")

		Gui.CommandbarAddTo("Standard views","Std_ViewFitAll")
		Gui.CommandbarAddTo("Standard views","Std_ViewAxo")
		Gui.CommandbarAddSeperator("Standard views")
		Gui.CommandbarAddTo("Standard views","Std_ViewFront")
		Gui.CommandbarAddTo("Standard views","Std_ViewRight")
		Gui.CommandbarAddTo("Standard views","Std_ViewTop")
		Gui.CommandbarAddSeperator("Standard views")
		Gui.CommandbarAddTo("Standard views","Std_ViewRear")
		Gui.CommandbarAddTo("Standard views","Std_ViewLeft")
		Gui.CommandbarAddTo("Standard views","Std_ViewBottom")

		Gui.CommandbarAddTo("Special Ops","Std_DlgParameter")
		Gui.CommandbarAddTo("Special Ops","Std_DlgPreferences")
		Gui.CommandbarAddTo("Special Ops","Std_DlgMacroRecord")
		Gui.CommandbarAddTo("Special Ops","Std_DlgMacroExecute")
		Gui.CommandbarAddTo("Special Ops","Std_DlgCustomize")
		Gui.CommandbarAddTo("Special Ops","Std_CommandLine")

		Log ('   Set up menues...\n')
	def Stop(self):
		Log ('   Stoping workbench...\n')
		Gui.CommandbarDelete("Special Ops")
		Gui.CommandbarDelete("TestTools")
		Gui.CommandbarDelete("Standard views")
		Gui.ToolbarDelete("Standard views")
		Gui.ToolbarDelete("TestTools")
		
def InitToolbars():
	Log ('   Set up toolbars...\n')
	# test tool bar
	Gui.ToolbarAddTo("TestTools","Std_MDIToplevel")
	Gui.ToolbarAddTo("TestTools","Std_MDITabed")
	Gui.ToolbarAddTo("TestTools","Std_MDINormal")
	Gui.ToolbarAddSeperator("TestTools")
	Gui.ToolbarAddTo("TestTools","Std_TilePragmatic")
	Gui.ToolbarAddTo("TestTools","Std_TileVertical")
	Gui.ToolbarAddTo("TestTools","Std_TileHoricontal")
	Gui.ToolbarAddSeperator("TestTools")
	Gui.ToolbarAddTo("TestTools","Std_Test1")
	Gui.ToolbarAddTo("TestTools","Std_Test2")
	Gui.ToolbarAddSeperator("TestTools")
	Gui.ToolbarAddTo("TestTools","Std_DlgParameter")
	Gui.ToolbarAddTo("TestTools","Std_DlgPreferences")
	Gui.ToolbarAddTo("TestTools","Std_DlgMacroRecord")
	Gui.ToolbarAddTo("TestTools","Std_DlgMacroExecute")
	Gui.ToolbarAddTo("TestTools","Std_DlgCustomize")
	Gui.ToolbarAddSeperator("TestTools")
	Gui.ToolbarAddTo("TestTools","Std_CommandLine")

	# test tool bar
	Gui.ToolbarAddTo("Standard views","Std_ViewFitAll")
	Gui.ToolbarAddTo("Standard views","Std_ViewAxo")
	Gui.ToolbarAddSeperator("Standard views")
	Gui.ToolbarAddTo("Standard views","Std_ViewFront")
	Gui.ToolbarAddTo("Standard views","Std_ViewRight")
	Gui.ToolbarAddTo("Standard views","Std_ViewTop")
	Gui.ToolbarAddSeperator("Standard views")
	Gui.ToolbarAddTo("Standard views","Std_ViewRear")
	Gui.ToolbarAddTo("Standard views","Std_ViewLeft")
	Gui.ToolbarAddTo("Standard views","Std_ViewBottom")
	

def InitMenues():
	Log ('   Set up menues...\n')

def InitCmdBar():
	Log ('   Set up command bar...\n')
	Gui.CommandbarAddTo("TestTools","Std_Test1")
	Gui.CommandbarAddTo("TestTools","Std_Test2")

	Gui.CommandbarAddTo("Standard views","Std_ViewFitAll")
	Gui.CommandbarAddTo("Standard views","Std_ViewAxo")
	Gui.CommandbarAddSeperator("Standard views")
	Gui.CommandbarAddTo("Standard views","Std_ViewFront")
	Gui.CommandbarAddTo("Standard views","Std_ViewRight")
	Gui.CommandbarAddTo("Standard views","Std_ViewTop")
	Gui.CommandbarAddSeperator("Standard views")
	Gui.CommandbarAddTo("Standard views","Std_ViewRear")
	Gui.CommandbarAddTo("Standard views","Std_ViewLeft")
	Gui.CommandbarAddTo("Standard views","Std_ViewBottom")

	Gui.CommandbarAddTo("Special Ops","Std_DlgParameter")
	Gui.CommandbarAddTo("Special Ops","Std_DlgPreferences")
	Gui.CommandbarAddTo("Special Ops","Std_DlgMacroRecord")
	Gui.CommandbarAddTo("Special Ops","Std_DlgMacroExecute")
	Gui.CommandbarAddTo("Special Ops","Std_DlgCustomize")
	Gui.CommandbarAddTo("Special Ops","Std_CommandLine")




def InitApplications():
	# Checking on FreeCAD Module path ++++++++++++++++++++++++++++++++++++++++++
	ModDir = '../src/Mod'
	# Searching modules dirs +++++++++++++++++++++++++++++++++++++++++++++++++++
	ModDirs = dircache.listdir(ModDir)
	#print ModDirs
	Log('   Searching modules...\n')
	ModPar = App.ParamGet("Modules")
	for Dir in ModDirs:
		if ( (Dir != 'CVS') & (Dir != '__init__.py')):
			Log('      Init: ' + Dir + '... ')
			InstallFile = os.path.join(os.path.join(ModDir,Dir),"InitGui.py")
			if ( os.path.exists(InstallFile) ):
				execfile(InstallFile)
				Log('done\n')
			else:
				Wrn("InitGui.py not found! "+Dir+" not initialized!\n")


Log ('FreeCAD gui init running....\n')

# init the gui


#StdWrkb.Start()

Temp2 = StandardWorkbench()

Gui.WorkbenchAdd("<none>",Temp2)
#Gui.WorkbenchAdd("<none>",StandardWorkbench())

# init modules
InitApplications()

# set standard workbench
Gui.WorkbenchActivate("<none>")

#InitMenues()
#InitToolbars()
#InitCmdBar()

Log ('\nFreeCAD gui init done\n')
   
    


