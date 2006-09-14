# FreeCAD init module
# (c) 2001 Jürgen Riegel
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
import FreeCAD

try:
	import sys,os,dircache
except:
	FreeCAD.PrintError("\n\nSeems the python standard libs are not installed, bailing out!\n\n")
	raise

def InitDocumentation():
	# Checking on FreeCAD documentation path ++++++++++++++++++++++++++++++++++++++++++
	if os.path.isdir(FreeCAD.ConfigGet("HomePath")+'Doc'):
		if os.path.isdir(FreeCAD.ConfigGet("HomePath")+'Doc\\free-cad.sourceforge.net'):
			FreeCAD.ConfigSet("DocPath",FreeCAD.ConfigGet("HomePath")+'Doc\\free-cad.sourceforge.net')


def InitApplications():
	# Checking on FreeCAD Module path ++++++++++++++++++++++++++++++++++++++++++
	ModDir = FreeCAD.ConfigGet("HomePath")+'src/Mod'
	#print FreeCAD.ConfigGet("HomePath")
	if os.path.isdir(FreeCAD.ConfigGet("HomePath")+'Mod'):
		ModDir = FreeCAD.ConfigGet("HomePath")+'Mod'
	else:
		if os.path.isdir(FreeCAD.ConfigGet("HomePath")+'src\\Mod'):
			ModDir = FreeCAD.ConfigGet("HomePath")+'src\\Mod'
	sys.path.append( '..\\bin' )
	sys.path.append( ModDir )
	if os.path.isdir(FreeCAD.ConfigGet("HomePath")+'src\\Tools'):
		sys.path.append( FreeCAD.ConfigGet("HomePath")+'src\\Tools' )
	Log("Using "+ModDir+" as module path!\n")
	# Searching modules dirs +++++++++++++++++++++++++++++++++++++++++++++++++++
	ModDirs = dircache.listdir(ModDir)
	PathExtension = ""
	#print ModDirs
	Log('Init:   Searching modules...\n')
	ModPar = App.ParamGet("System parameter:Modules")
	for Dir in ModDirs:
		if ( (Dir != 'CVS') & (Dir != '__init__.py')):
			Log('Init:      Initializing: ' + Dir + '... ')
			ModGrp = ModPar.GetGroup(Dir)
			sys.path.append( os.path.join(ModDir,Dir) )
			PathExtension += ";" + os.path.join(ModDir,Dir)
			InstallFile = os.path.join(os.path.join(ModDir,Dir),"Init.py")
			if ( os.path.exists(InstallFile) ):
				try:
					execfile(InstallFile)
				except:
					Err("Unexpected error in : " + InstallFile + " not initialized!\n")
				else:
					Log('done\n')
			else:
				Wrn("Init.py not found! "+Dir+" not initialized!\n")
	os.environ["PATH"] += PathExtension
	#Log("PATH after addition: " + os.environ["PATH"] + "\n")
	#Log("path extension: " + PathExtension + "\n")

#			else:
#				FreeCAD.PrintMessage('new! install it...\n')
#				InstallFile = os.path.join(os.path.join(ModDir,Dir),"Install.py")
#				if ( os.path.exists(InstallFile) ):
#					execfile(InstallFile)
#				else:
#					Wrn("         Install.py not found! "+Dir+" not installed!\n")


# some often used shortcuts (for lazy people like me ;-)
App = FreeCAD
Log = FreeCAD.PrintLog
Msg = FreeCAD.PrintMessage
Err = FreeCAD.PrintError
Wrn = FreeCAD.PrintWarning

Log ('\nInit: starting App::FreeCADInit.py\n')

# init every application by importing Init.py
InitApplications()

FreeCAD.EndingAdd("FreeCAD document (*.FCStd)","FreeCAD")

# set to no gui, is overwritten by InitGui
App.GuiUp = 0

Log ('\nInit: App::FreeCADInit.py done\n')




