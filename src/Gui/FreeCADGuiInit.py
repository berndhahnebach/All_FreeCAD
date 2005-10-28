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
	def Activate(self):
		Wrn("Init: "+ str(self) + ": Workbench.Activate() not implemented!")
	def GetIcon(self):
		Log("Init: "+ str(self) + ": Workbench.GetIcon() not implemented!")


class StandardWorkbench ( Workbench ):
	"Standard workbench object"
	def Activate(self):
		# load the module
		Log ('Mod : Loading FreeCADGui')
	def GetIcon(self):
		Log("Workbench.GetIcon() not implemented!")

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
	Log('Init:   Searching modules...\n')
	ModPar = App.ParamGet("System parameter:Modules")
	for Dir in ModDirs:
		if ( (Dir != 'CVS') & (Dir != '__init__.py')):
			Log('Init:      Initializing: ' + Dir + '...')
			InstallFile = os.path.join(os.path.join(ModDir,Dir),"InitGui.py")
			if ( os.path.exists(InstallFile) ):
				try:
					execfile(InstallFile)
				except:
					Err(" Unexpected error in " + InstallFile + " not initialized!\n")
				else:
					Log('Init:        done\n')
			else:
				Wrn("InitGui.py not found! "+Dir+" not initialized!\n")
 

Log ('Init: starting Gui::FreeCADGuiInit.py\n')

# init the gui

# signal that the gui is up
App.GuiUp = 1
App.Gui = FreeCADGui

Gui.AddWorkbenchHandler("<none>",StandardWorkbench())

# init modules
InitApplications()

# set standard workbench (needed as fallback)
Gui.ActivateWorkbench("<none>")

# Register .py, .FCScript and .FCMacro
FreeCAD.EndingAdd("py","FreeCADGui")
FreeCAD.EndingAdd("FCMacro","FreeCADGui")
FreeCAD.EndingAdd("FCScript","FreeCADGui")


Log ('Init: Gui::FreeCADGuiInit.py done\n')
