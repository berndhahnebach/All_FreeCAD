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

Log ('FreeCAD gui init running....\n')

# init the gui
InitMenues()
InitToolbars()
InitCmdBar()

Log ('\nFreeCAD gui init done\n')
   
    


