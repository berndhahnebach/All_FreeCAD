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
        self.Active = FreeCADGui.ActiveWorkbench()
        FreeCAD.PrintLog(FreeCADGui.ActiveWorkbench().Name())
        
    def testActivate(self):
        list=FreeCADGui.ListWorkbenchHandlers()
        for i in list:
            FreeCADGui.ActivateWorkbench(i)
            FreeCADGui.UpdateGui()
            FreeCAD.PrintLog("Currently: "+i+ " Should be: "+FreeCADGui.ActiveWorkbench().Name())
            self.failUnless(FreeCADGui.ActiveWorkbench().Name()==i, "Test on activating workbench failed")
        
# Need a method to delete a workbench
#    def testWorkbench(self):
#        self.failUnless(FreeCADGui.HasWorkbench("TestWb")==False, "TestWb was not expected to be in the list")
#        self.failUnless(FreeCADGui.CreateWorkbench("TestWb")!=None, "Creation of TestWb failed")
#        self.failUnless(FreeCADGui.HasWorkbench("TestWb")==True, "TestWb was not expected to be in the list")
#        self.failUnless(FreeCADGui.GetWorkbench("TestWb")!=None, "Retrieval of TestWb failed")
#        list=FreeCADGui.ListWorkbenches()
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
                    if FreeCADGui.HasWorkbench('Unittest') == False:
                        w = FreeCADGui.CreateWorkbench('Unittest')
                        list = ["Test_Test"]
                        w.AppendToolbar("My Unittest",list)
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

        FreeCADGui.AddWorkbenchHandler("Unittest",UnitWorkbench())
        list=FreeCADGui.ListWorkbenchHandlers()
        self.failUnless(list.has_key("Unittest")==True, "Test on adding workbench handler failed")
        FreeCADGui.ActivateWorkbench("Unittest")
        FreeCADGui.UpdateGui()
        self.failUnless(FreeCADGui.ActiveWorkbench().Name()=="Unittest", "Test on loading workbench 'Unittest' failed")
        FreeCADGui.RemoveWorkbenchHandler("Unittest")
        list=FreeCADGui.ListWorkbenchHandlers()
        self.failUnless(list.has_key("Unittest")==False, "Test on removing workbench handler failed")

    def tearDown(self):
        FreeCADGui.ActivateWorkbench(self.Active.Name())
        FreeCAD.PrintLog(self.Active.Name())
