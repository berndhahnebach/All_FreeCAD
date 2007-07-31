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
	def Activate(self):
		try:
			Log ('Loading GUI of Test module...\n')
			import TestGui
			w=0
			if Gui.hasWorkbench('Test framework') == False:
				w = Gui.createWorkbench('Test framework')

				list = ["Test_Test","Test_TestAll","Test_TestDoc","Test_TestBase"]
				w.appendToolbar("TestTools",list)

				menu = ["Test &Commands","TestToolsGui"]
				list = ["Test_Test","Test_TestAll","Test_TestDoc","Test_TestBase"]
				w.appendCommandbar("TestToolsGui",list)
				w.appendMenu(menu,list)

				menu = ["Test &Commands","TestToolsText"]
				list = ["Test_TestAllText","Test_TestDocText","Test_TestBaseText"]
				w.appendCommandbar("TestToolsText",list)
				w.appendMenu(menu,list)

				menu = ["Test &Commands","TestToolsMenu"]
				list = ["Test_TestCreateMenu", "Test_TestDeleteMenu"]
				w.appendCommandbar("TestToolsMenu",list)
				w.appendMenu(menu,list)
				
				menu = ["Test &Commands","TestFeatureMenu"]
				list = ["Test_InsertFeature"]
				w.appendCommandbar("TestFeature",list)
				w.appendMenu(menu,list)

				menu = ["Test &Commands","Progress bar"]
				list = ["Std_TestProgress1", "Std_TestProgress2", "Std_TestProgress3"]
				w.appendMenu(menu,list)

				list = ["Std_ViewExample1", "Std_ViewExample2", "Std_ViewExample3", 
				"Separator", "Std_ViewIvStereoRedGreen", "Std_ViewIvStereoQuadBuff", "Std_ViewIvStereoInterleavedRows", 
				"Std_ViewIvStereoInterleavedColumns", "Std_ViewIvStereoOff", "Std_ViewIvIssueCamPos"]
				w.appendMenu("Inventor View",list)

				Log ('Setup Test menues...\n')
			Log ('Loading GUI of Test module... done\n')
		except:
			Err('Activating of workbench "Test framework" failed\n')
			raise
	def GetClassName(self):
		return "Gui::PythonWorkbench"
	def GetIcon(self):
		# returns an icon for the workbench
		return ["/* XPM */\n"
			"static const char *test_icon[]={\n"
			"\"16 16 2 1\",\n"
			"\"a c #000000\",\n"
			"\". c None\",\n"
			"\"................\",\n"
			"\"................\",\n"
			"\"..############..\",\n"
			"\"..############..\",\n"
			"\"..############..\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"......####......\",\n"
			"\"................\",\n"
			"\"................\"};\n"]

Gui.addWorkbenchHandler("Test framework",TestWorkbench())
