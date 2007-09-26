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
	"""The workbench base class."""
	def Activate(self):
		"""Activate this workbench."""
		Wrn("Init: "+ str(self) + ": Workbench.Activate() not implemented!")
	def GetClassName(self):
		"""Return the name of the associated C++ class."""
		Wrn("Init: "+ str(self) + ": Workbench.GetClassName() not implemented!")
	def GetIcon(self):
		"""Return a list of icons in XPM format."""
		# returns an icon for the workbench
		return ["/* XPM */\n"
			"static const char *FCIcon[]={\n"
			"\"16 16 4 1\",\n"
			"\". c None\",\n"
			"\"# c #000000\",\n"
			"\"a c #848284\",\n"
			"\"b c #ff0000\",\n"
			"\"........#.......\",\n"
			"\".......##aaaaaa.\",\n"
			"\"........#.....a.\",\n"
			"\".#######......a.\",\n"
			"\".##...........a.\",\n"
			"\".##...bbbb....a.\",\n"
			"\".##..bb..bb...a.\",\n"
			"\".###bb#...b..###\",\n"
			"\".##.bb........#.\",\n"
			"\".##.bb..........\",\n"
			"\".##.bb..........\",\n"
			"\".##.bb..........\",\n"
			"\".##.bb....b.....\",\n"
			"\".....bb..bb.....\",\n"
			"\"......bbbb......\",\n"
			"\"................\"};\n"]


class StandardWorkbench ( Workbench ):
	"""A workbench defines the tool bars, command bars, menus, 
context menu and dockable windows of the main window.
	"""
	def Activate(self):
		"""Activate this workbench."""
		# load the module
		Log ('Init: Loading FreeCAD GUI')
	def GetClassName(self):
		"""Return the name of the associated C++ class."""
		return "Gui::StdWorkbench"

class NoneWorkbench ( Workbench ):
	"""An empty workbench."""
	def Activate(self):
		"""Activate this workbench."""
		# load the module
		Log ('Init: Loading FreeCAD GUI')
	def GetClassName(self):
		"""Return the name of the associated C++ class."""
		return "Gui::NoneWorkbench"

def InitApplications():
	# Checking on FreeCAD Module path ++++++++++++++++++++++++++++++++++++++++++
	ModDir = FreeCAD.ConfigGet("AppHomePath")+'Mod'
	#print FreeCAD.ConfigGet("AppHomePath")
	if not os.path.isdir(ModDir):
		print "No modules found in " + ModDir
		return
	# Searching modules dirs +++++++++++++++++++++++++++++++++++++++++++++++++++
	ModDirs = dircache.listdir(ModDir)
	#print ModDirs
	Log('Init:   Searching modules...\n')
	ModPar = App.ParamGet("System parameter:Modules")
	for Dir in ModDirs:
		if ( (Dir != 'CVS') & (Dir != '__init__.py')):
			Log('Init:      Initializing ' + Dir + '... ')
			InstallFile = os.path.join(os.path.join(ModDir,Dir),"InitGui.py")
			if ( os.path.exists(InstallFile) ):
				try:
					execfile(InstallFile)
				except Exception, inst:
					Err("During initialization the error " + str(inst) + " occurred in " + InstallFile + "\n")
				else:
					Log('done\n')
			else:
				Wrn("InitGui.py not found! "+Dir+" not initialized!\n")
 

Log ('Init: Running FreeCADGuiInit.py start script...\n')

# init the gui

# signal that the gui is up
App.GuiUp = 1
App.Gui = FreeCADGui

Gui.addWorkbenchHandler("<none>",NoneWorkbench())

# init modules
InitApplications()

# set standard workbench (needed as fallback)
Gui.activateWorkbenchHandler("<none>")

# Register .py, .FCScript and .FCMacro
FreeCAD.EndingAdd("Inventor V2.1 (*.iv)","FreeCADGui")
FreeCAD.EndingAdd("VRML V2.0 (*.wrl *.vrml *.wrz *.wrl.gz)","FreeCADGui")
FreeCAD.EndingAdd("Python (*.py *.FCMacro *.FCScript)","FreeCADGui")


Log ('Init: Running FreeCADGuiInit.py start script... done\n')
