# Part gui init module  
# (c) 2003 Juergen Riegel
#

#***************************************************************************
#*   (c) Juergen Riegel (juergen.riegel@web.de) 2002                       *   
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
			list = ["Test_Test","Test_TestAll","Test_TestDoc","Test_TestBase"]
			Gui.ToolbarAppendItems("TestTools",list,0)

			Log ('   Set up Test commandbar...\n')
			list = ["Test_Test","Test_TestAll","Test_TestDoc","Test_TestBase"]
			Gui.CommandbarAppendItems("TestToolsGui",list,0)
			list = ["Test_TestAllText","Test_TestDocText","Test_TestBaseText"]
			Gui.CommandbarAppendItems("TestToolsText",list,0)
			list = ["Test_TestCreateMenu", "Test_TestDeleteMenu"]
			Gui.CommandbarAppendItems("TestToolsMenu",list,0)
                            
			Log ('   Set up Test menues...\n')

	def Stop(self):
		Log ('   Stoping Test workbench...\n')
		Gui.CommandbarDelete("TestTools")
		Gui.ToolbarDelete("TestTools")


#Gui.WorkbenchAdd("Part design",PartWorkbench())
Gui.WorkbenchAdd("Test framework",TestWorkbench())