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

    def tearDown(self):
        FreeCADGui.ActivateWorkbench(self.Active.Name())
        FreeCAD.PrintLog(self.Active.Name())
