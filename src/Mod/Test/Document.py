#***************************************************************************
#*   (c) Juergen Riegel (juergen.riegel@web.de) 2003                       *   
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
#*   Juergen Riegel 2003                                                   *
#***************************************************************************/

import FreeCAD, os, unittest


#---------------------------------------------------------------------------
# define the functions to test the FreeCAD Document code
#---------------------------------------------------------------------------


# class DocumentBasicCases(unittest.TestCase):
    # def setUp(self):
        # self.Doc = FreeCAD.newDocument("CreateTest")

    # def testCreateDestroy(self):
        # self.failUnless(FreeCAD.getDocument("CreateTest")!= None,"Creating Document failed")

    # def testObjects(self):
        # L1 = self.Doc.addObject("App::FeatureTest","Label_1")
        # self.Doc.recompute()
        # self.failUnless(L1.Integer == 4711,    "Different value to '4711'")
        # FreeCAD.PrintLog("Integer: "+str(L1.Integer))
        # self.failUnless(L1.Float-47.11<0.001,   "Different value to '47.11'")
        # FreeCAD.PrintLog("Float: "+str(L1.Float))
        # self.failUnless(L1.Bool    == True,    "Different value to 'True'")
        # FreeCAD.PrintLog("Boolean: "+str(L1.Bool))
        # self.failUnless(L1.String  == "empty",  "Different value to '4711'")
        # FreeCAD.PrintLog("String: "+L1.String)
        
        # self.failUnless(L1.name== "Label_1","Invalid object name")
        # L1.name="Label_2"
        # self.Doc.recompute()
        # self.failUnless(L1.name== "Label_2","Invalid object name")
        # self.Doc.removeObject("Label_1")

#    def testSaveAndRestore(self):
#        # saving and restoring
#        TempPath = os.getenv('TEMP')
#        FreeCAD.PrintLog ('  Using temp path: ' + TempPath + '\n')
#
#        SavePath = TempPath + os.sep + "Test1.FCStd"
#        FreeCAD.PrintLog("   Save and Open the document to: " + SavePath + "\n")
#        self.Doc.SaveAs(SavePath)
#        FreeCAD.PrintLog("   provocate exceptio by try loading a already open document\n")
#        try:
#            Doc2 = App.DocOpen(SavePath)
#        except:
#            FreeCAD.PrintLog("   exception thrown, OK\n")
#        else:
#            FreeCAD.PrintLog("   no exception thrown, ERROR\n")
#            raise IOError

    # def tearDown(self):
        #closing doc
        # FreeCAD.closeDocument("CreateTest")

class UndoRedoCases(unittest.TestCase):
    def setUp(self):
        self.Doc = FreeCAD.newDocument("UndoTest")
        self.Doc.addObject("App::FeatureTest","Base")
        self.Doc.setUndoMode(1)

    def testUndo(self):
        self.Doc.openCommand()
        self.Doc.addObject("App::FeatureTest","test1")
        self.Doc.getObject("test1").Integer  = 1
        self.Doc.openCommand()
        self.Doc.getObject("test1").Integer  = 2
        self.Doc.abortCommand()
        self.assertEqual(self.Doc.getObject("test1").Integer, 1)
        self.Doc.openCommand()
        self.Doc.getObject("test1").Integer  = 2		
        self.Doc.undo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 1)
        self.Doc.undo()
        self.failUnless(self.Doc.getObject("test1") == None)
        self.Doc.redo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 1)
        self.Doc.redo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 2)
        self.Doc.undo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 1)
        self.Doc.openCommand()
        self.Doc.getObject("test1").Integer  = 8
        self.assertEqual(self.Doc.getObject("test1").Integer, 8)
        self.Doc.undo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 1)
        self.Doc.redo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 8)
        self.Doc.undo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 1)
        self.Doc.undo()
        self.failUnless(self.Doc.getObject("test1") == None)

    def tearDown(self):
        # closing doc
        FreeCAD.closeDocument("UndoTest")


        
