# Menu test module  
# (c) 2005 Werner Mayer
#

#***************************************************************************
#*   (c) Werner Mayer <werner.wm.mayer@gmx.de> 2005                        *   
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
#*   Werner Mayer 2005                                                     *
#***************************************************************************/

import FreeCAD, os, unittest, FreeCADGui


#---------------------------------------------------------------------------
# define the functions to test the FreeCAD base code
#---------------------------------------------------------------------------

#def suite():
#    suite = unittest.TestSuite()
#    suite.addTest(DocTestCase("DocumentProperties"))
#    suite.addTest(DocTestCase("DocumentLabels"))
#    suite.addTest(DocTestCase("DocumentSaveAndRestore"))
#    return suite


class MenuCreateCases(unittest.TestCase):

    def setUp(self):
        FreeCAD.PrintLog ('   Set up Test menus...\n')
        list = ["Std_CommandLine","Std_DlgCustomize","Std_DlgMacroStop","Std_DlgMacroExecute"]
        FreeCADGui.MenuAppendItems("TestMenu",list,0)

    def testSaveAndRestore(self):
        # saving and restoring
        FreeCAD.PrintMessage ('Switch to <none> and back to Test framework\n')
        FreeCADGui.WorkbenchActivate("<none>")
        FreeCADGui.WorkbenchActivate("Test framework")

    def tearDown(self):
        FreeCAD.PrintMessage ('Press Test_TestDeleteMenu to remove menu...\n')


class MenuDeleteCases(unittest.TestCase):

    def setUp(self):
        FreeCAD.PrintLog ('   Delete menu:\n')

    def testLabels(self):
        FreeCAD.PrintLog ('   Test menus...\n')

    def tearDown(self):
        FreeCAD.PrintMessage ('Remove Test menus...\n')
        FreeCADGui.MenuDelete("TestMenu")

