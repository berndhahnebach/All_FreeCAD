# Part gui init module  
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



class PartWorkbench ( Workbench ):
	"Part workbench object"
	def Start(self):
		# load the module
		try:
			Log ('Loading PartGui module')
			import PartGui
		except:
			Err('   can not load PartGui')
			raise
		else:	
			Log ('   Set up part toolbar...\n')
			Gui.ToolbarAddTo("PartTools","Part_Test1")
			Gui.ToolbarAddTo("PartTools","Part_Test2")
			Gui.ToolbarAddTo("PartTools","Part_Box")
			Gui.ToolbarAddTo("PartTools","Part_Cut")
			Gui.ToolbarLoadSettings("PartTools")

			Log ('   Set up part commandbar...\n')
			Gui.CommandbarAddTo("PartTools","Part_Test1")
			Gui.CommandbarAddTo("PartTools","Part_Test2")
			Gui.CommandbarAddTo("PartTools","Part_Box")
			Gui.CommandbarAddTo("PartTools","Part_Cut")
			Gui.CommandbarLoadSettings("PartTools")

			Log ('   Set up part menues...\n')

	def Stop(self):
		Log ('   Stoping part workbench...\n')
		Gui.CommandbarDelete("PartTools")
		Gui.ToolbarDelete("PartTools")


#Gui.WorkbenchAdd("Part design",PartWorkbench())
Gui.WorkbenchAdd("Part design",PartWorkbench())