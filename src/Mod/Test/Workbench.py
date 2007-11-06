# Workbench test module  
# (c) 2006 Werner Mayer
#

#***************************************************************************
#*   (c) Werner Mayer <werner.wm.mayer@gmx.de> 2006                        *
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
#*   Werner Mayer 2006                                                     *
#***************************************************************************/

import FreeCAD, FreeCADGui, os, unittest

class WorkbenchTestCase(unittest.TestCase):
    def setUp(self):
        self.Active = FreeCADGui.activeWorkbench()
        FreeCAD.PrintLog(FreeCADGui.activeWorkbench().name())
        
    def testActivate(self):
        list=FreeCADGui.listWorkbenchHandlers()
        for i in list:
            FreeCADGui.activateWorkbenchHandler(i)
            j = list.get(i)
            try:
                name = j.WorkbenchName
            except:
                name = i
            FreeCADGui.updateGui()
            FreeCAD.PrintLog("Currently: "+name+ " Should be: "+FreeCADGui.activeWorkbench().name())
            self.failUnless(FreeCADGui.activeWorkbench().name()==name, "Test on activating workbench failed")
        
# Need a method to delete a workbench
#    def testWorkbench(self):
#        self.failUnless(FreeCADGui.hasWorkbench("TestWb")==False, "TestWb was not expected to be in the list")
#        self.failUnless(FreeCADGui.createWorkbench("TestWb")!=None, "Creation of TestWb failed")
#        self.failUnless(FreeCADGui.hasWorkbench("TestWb")==True, "TestWb was not expected to be in the list")
#        self.failUnless(FreeCADGui.getWorkbench("TestWb")!=None, "Retrieval of TestWb failed")
#        list=FreeCADGui.listWorkbenches()
#        fnd = False
#        for i in list: 
#            if (i == "TestWb"): 
#                fnd = True;
#        self.failUnless(fnd==True, "TestWb was expected to be in the list")
#
        
    def testHandler(self):
        class UnitWorkbench:
            def Activate(self):
                try:
                    if FreeCADGui.hasWorkbench('Unittest') == False:
                        w = FreeCADGui.createWorkbench('Unittest')
                        list = ["Test_Test"]
                        w.appendToolbar("My Unittest",list)
                except:
                    raise
            def GetClassName(self):
                return "Gui::PythonWorkbench"
            def GetIcon(self):
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

        FreeCADGui.addWorkbenchHandler("Unittest",UnitWorkbench())
        list=FreeCADGui.listWorkbenchHandlers()
        self.failUnless(list.has_key("Unittest")==True, "Test on adding workbench handler failed")
        FreeCADGui.activateWorkbenchHandler("Unittest")
        FreeCADGui.updateGui()
        self.failUnless(FreeCADGui.activeWorkbench().name()=="Unittest", "Test on loading workbench 'Unittest' failed")
        FreeCADGui.removeWorkbenchHandler("Unittest")
        list=FreeCADGui.listWorkbenchHandlers()
        self.failUnless(list.has_key("Unittest")==False, "Test on removing workbench handler failed")

    def tearDown(self):
        FreeCADGui.activateWorkbenchHandler(self.Active.name())
        FreeCAD.PrintLog(self.Active.name())
