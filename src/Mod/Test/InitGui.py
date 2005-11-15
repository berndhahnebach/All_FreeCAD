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
			Log ('Loading TestGui module\n')
			import TestGui
			w=0
			if Gui.HasWorkbench('Test framework') == False:
				w = Gui.CreateWorkbench('Test framework')
				Log ('Activating Test workbench...\n')

				list = ["Test_Test","Test_TestAll","Test_TestDoc","Test_TestBase"]
				w.AppendToolbar("TestTools",list)

				list = ["Test_Test","Test_TestAll","Test_TestDoc","Test_TestBase"]
				w.AppendCommandbar("TestToolsGui",list)

				list = ["Test_TestAllText","Test_TestDocText","Test_TestBaseText"]
				w.AppendCommandbar("TestToolsText",list)

				list = ["Test_TestCreateMenu", "Test_TestDeleteMenu"]
				w.AppendCommandbar("TestToolsMenu",list)

				list = ["Std_Test1", "Std_Test2", "Std_Test3", "Std_Test4", "Std_Test5", "Std_Test6", "Std_Test7", "Std_Test8"]
				w.AppendMenu("Test &Commands",list)

				list = ["Std_ViewExample1", "Std_ViewExample2", "Std_ViewExample3", "Std_ViewIvDecorationOn", "Std_ViewIvDecorationOff", 
				"Separator", "Std_ViewIvStereoRedGreen", "Std_ViewIvStereoQuadBuff", "Std_ViewIvStereoInterleavedRows", 
				"Std_ViewIvStereoInterleavedColumns", "Std_ViewIvStereoOff", "Std_ViewIvIssueCamPos"]
				w.AppendMenu("Inventor View",list)

				Log ('   Set up Test menues...\n')
		except:
			Err('Activating of workbench "Test framework" failed\n')
			raise
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

Gui.AddWorkbenchHandler("Test framework",TestWorkbench())
