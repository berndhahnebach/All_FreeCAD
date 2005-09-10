# FreeCAD Part module  
# (c) 2001 Juergen Riegel
#
# Part design module

#***************************************************************************
#*   (c) Juergen Riegel (juergen.riegel@web.de) 2002                       *
#*                                                                         *
#*   This file is part of the FreeCAD CAx development system.              *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU Lesser General Public License (LGPL)    *
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

# import FreeCAD modules
import FreeCAD,FreeCADGui
# import the App Test module
import TestApp               #Test as Module name not possible

#---------------------------------------------------------------------------
# define the Commands of the Test Application module
#---------------------------------------------------------------------------
class TestCmd:
    "Test1 commando object"
    def Activated(self):
        TestApp.TestGui("TestApp.All")

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1', 'MenuText': 'Test 1', 'ToolTip': 'First very important Test!!!'}

class TestAllCmd:
    "Test all commando object"
    def Activated(self):
        TestApp.TestGui("TestApp.All")

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1', 'MenuText': 'Test all', 'ToolTip': 'Runs all tests at once (can take very long!)'}

class TestDocCmd:
    "Document test commando object"
    def Activated(self):
        TestApp.TestGui("Document")

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1',
        		'MenuText': 'Test Document',
                'ToolTip' : 'Test the document (creation, save, load and destruction)'}

class TestBaseCmd:
    "Base test commando object"
    def Activated(self):
        TestApp.TestGui("Base")

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1',
        		'MenuText': 'Test base',
                'ToolTip' : 'Test the basic functions of FreeCAD'}

class TestAllTextCmd:
    "Test all commando object"
    def Activated(self):
        TestApp.TestText("TestApp.All")

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1', 'MenuText': 'Test all', 'ToolTip': 'Runs all tests at once (can take very long!)'}

class TestDocTextCmd:
    "Document test commando object"
    def Activated(self):
        TestApp.TestText("Document")

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1',
        		'MenuText': 'Test Document',
                'ToolTip' : 'Test the document (creation, save, load and destruction)'}

class TestBaseTextCmd:
    "Base test commando object"
    def Activated(self):
        TestApp.TestText("Base")

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1',
        		'MenuText': 'Test base',
                'ToolTip' : 'Test the basic functions of FreeCAD'}





class TestWorkbenchCmd:
    "Workbench test"
    def Activated(self):
    	i=0
    	while (i<20):
	        FreeCADGui.WorkbenchActivate("Import")
	        FreeCADGui.UpdateGui()
	        FreeCADGui.WorkbenchActivate("<none>")
	        FreeCADGui.UpdateGui()
	        FreeCADGui.WorkbenchActivate("Part design")
	        FreeCADGui.UpdateGui()
	        FreeCADGui.WorkbenchActivate("Sketcher")
	        FreeCADGui.UpdateGui()
	        print i
	        i=i+1

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1',
        		'MenuText': 'Test workbench',
                'ToolTip' : 'Test the switching of workbenches in FreeCAD'}

class TestCreateMenuCmd:
    "Base test commando object"
    def Activated(self):
        TestApp.TestText("Menu.MenuCreateCases")

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1',
        		'MenuText': 'Test menu',
                'ToolTip' : 'Test the menu stuff of FreeCAD'}

class TestDeleteMenuCmd:
    "Base test commando object"
    def Activated(self):
        TestApp.TestText("Menu.MenuDeleteCases")

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1',
        		'MenuText': 'Test menu',
                'ToolTip' : 'Test the menu stuff of FreeCAD'}

#---------------------------------------------------------------------------
# Adds the commands to the FreeCAD command manager
#---------------------------------------------------------------------------
FreeCADGui.CommandAdd('Test_Test'        ,TestCmd())
FreeCADGui.CommandAdd('Test_TestAllText' ,TestAllTextCmd())
FreeCADGui.CommandAdd('Test_TestDocText' ,TestDocTextCmd())
FreeCADGui.CommandAdd('Test_TestBaseText',TestBaseTextCmd())
FreeCADGui.CommandAdd('Test_TestAll'     ,TestAllCmd())
FreeCADGui.CommandAdd('Test_TestDoc'     ,TestDocCmd())
FreeCADGui.CommandAdd('Test_TestBase'    ,TestBaseCmd())
FreeCADGui.CommandAdd('Test_TestWork'    ,TestWorkbenchCmd())
FreeCADGui.CommandAdd('Test_TestCreateMenu'    ,TestCreateMenuCmd())
FreeCADGui.CommandAdd('Test_TestDeleteMenu'    ,TestDeleteMenuCmd())

