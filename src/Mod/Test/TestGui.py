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
class Test1Cmd:
    "Test1 commando object"
    def Activated(self):
        FreeCAD.PrintLog("TEst1 in python activated")
        TestApp.TestUnit()

    def GetResources(self):
        return {'MenuText': 'Test 1', 'ToolTip': 'First very important Test!!!'}

class Test2Cmd:
    "Test2 commando object"
    def Activated(self):
        FreeCAD.PrintLog("TEst2 in python activated")

    def GetResources(self):
        return {'MenuText': 'Test 1', 'ToolTip': 'First very important Test!!!'}

class TestAllCmd:
    "Test all commando object"
    def Activated(self):
        TestApp.TestAll()

    def GetResources(self):
        return {'MenuText': 'Test all', 'ToolTip': 'Runs all tests at once (can take very long!)'}

class TestDocCmd:
    "Document test commando object"
    def Activated(self):
        TestApp.TestDoc()

    def GetResources(self):
        return {'MenuText': 'Test Document',
                'ToolTip' : 'Test the document (creation, save, load and destruction)'}

class TestParameterHeavy:
    "Document test commando object"
    def Activated(self):
        TestApp.TestParameterHeavy(100)

    def GetResources(self):
        return {'Heavy parameter test': 'Test parameter heavy',
                'Test parameter very long' : 'Test parameter very long'}

class TestBaseCmd:
    "Base test commando object"
    def Activated(self):
        TestApp.TestBase()

    def GetResources(self):
        return {'MenuText': 'Test base',
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
        return {'MenuText': 'Test workbench',
                'ToolTip' : 'Test the switching of workbenches in FreeCAD'}

#---------------------------------------------------------------------------
# Adds the commands to the FreeCAD command manager
#---------------------------------------------------------------------------
FreeCADGui.CommandAdd('Test_Test1'   ,Test1Cmd())
FreeCADGui.CommandAdd('Test_Test2'   ,Test2Cmd())
FreeCADGui.CommandAdd('Test_TestAll' ,TestAllCmd())
FreeCADGui.CommandAdd('Test_TestDoc' ,TestDocCmd())
FreeCADGui.CommandAdd('Test_TestBase',TestBaseCmd())
FreeCADGui.CommandAdd('Test_TestWork',TestWorkbenchCmd())
FreeCADGui.CommandAdd('Test_TestParameter',TestParameterHeavy())

