# Part gui init module  
# (c) 2003 Jürgen Riegel
#

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



class TestWorkbench ( Workbench ):
	"Test workbench object"
	def Start(self):
		# load the module
		try:
			Log ('Loading TestGui module\n')
			import TestGui
		except:
			Err('   can not load TestGui\n')
			raise
		else:	
			Log ('   Set up Test toolbar...\n')
			Gui.ToolbarAddTo("TestTools","Test_Test1")
			Gui.ToolbarAddTo("TestTools","Test_Test2")
			Gui.ToolbarAddTo("TestTools","Test_TestAll")
			Gui.ToolbarAddTo("TestTools","Test_TestDoc")
			Gui.ToolbarAddTo("TestTools","Test_TestBase")

			Log ('   Set up Test commandbar...\n')
			Gui.CommandbarAddTo("TestTools","Test_Test1")
			Gui.CommandbarAddTo("TestTools","Test_Test2")
 			Gui.CommandbarAddTo("TestTools","Test_TestAll")
			Gui.CommandbarAddTo("TestTools","Test_TestDoc")
			Gui.CommandbarAddTo("TestTools","Test_TestBase")
                            
			Log ('   Set up Test menues...\n')

	def Stop(self):
		Log ('   Stoping Test workbench...\n')
		Gui.CommandbarDelete("TestTools")
		Gui.ToolbarDelete("TestTools")


#Gui.WorkbenchAdd("Part design",PartWorkbench())
Gui.WorkbenchAdd("Test framwork",TestWorkbench())