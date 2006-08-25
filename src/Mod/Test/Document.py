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

    def testUndo(self):
        # switch on the Undo
        self.Doc.setUndoMode(1)
        self.assertEqual(self.Doc.getAvailableUndoNames(),[])
        self.assertEqual(self.Doc.getAvailableUndos(),0)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        
        # first transaction 
        self.Doc.openTransaction("Transaction1")
        self.Doc.addObject("App::FeatureTest","test1")
        self.Doc.getObject("test1").Integer  = 1
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),1)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        
        # second transaction 
        self.Doc.openTransaction("Transaction2")
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction2','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),2)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        self.Doc.getObject("test1").Integer  = 2
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction2','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),2)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        
        # abort second transaction 
        self.Doc.abortTransaction()
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),1)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        self.assertEqual(self.Doc.getObject("test1").Integer, 1)
        
        # again second transaction 
        self.Doc.openTransaction("Transaction2")
        self.Doc.getObject("test1").Integer  = 2	
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction2','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),2)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)

        # third transaction
        self.Doc.openTransaction("Transaction3")
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction3','Transaction2','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),3)
        self.Doc.getObject("test1").Integer  = 3		
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        
        # fourth transaction
        self.Doc.openTransaction("Transaction4")
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction4','Transaction3','Transaction2','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),4)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        self.Doc.getObject("test1").Integer  = 4

        # undo the fourth transaction 
        self.Doc.undo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 3)
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction3','Transaction2','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),3)
        self.assertEqual(self.Doc.getAvailableRedoNames(),['Transaction4'])
        self.assertEqual(self.Doc.getAvailableRedos(),1)

        # undo the third transaction 
        self.Doc.undo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 2)
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction2','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),2)
        self.assertEqual(self.Doc.getAvailableRedoNames(),['Transaction3','Transaction4'])
        self.assertEqual(self.Doc.getAvailableRedos(),2)

        # undo the second transaction 
        self.Doc.undo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 1)
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),1)
        self.assertEqual(self.Doc.getAvailableRedoNames(),['Transaction2','Transaction3','Transaction4'])
        self.assertEqual(self.Doc.getAvailableRedos(),3)

        # undo the first transaction 
        self.Doc.undo()
        self.failUnless(self.Doc.getObject("test1") == None)
        self.assertEqual(self.Doc.getAvailableUndoNames(),[])
        self.assertEqual(self.Doc.getAvailableUndos(),0)
        self.assertEqual(self.Doc.getAvailableRedoNames(),['Transaction1','Transaction2','Transaction3','Transaction4'])
        self.assertEqual(self.Doc.getAvailableRedos(),4)

        # redo the first transaction 
        self.Doc.redo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 1)
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),1)
        self.assertEqual(self.Doc.getAvailableRedoNames(),['Transaction2','Transaction3','Transaction4'])
        self.assertEqual(self.Doc.getAvailableRedos(),3)

        # redo the second transaction 
        self.Doc.redo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 2)
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction2','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),2)
        self.assertEqual(self.Doc.getAvailableRedoNames(),['Transaction3','Transaction4'])
        self.assertEqual(self.Doc.getAvailableRedos(),2)

        # undo the second transaction 
        self.Doc.undo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 1)
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),1)
        self.assertEqual(self.Doc.getAvailableRedoNames(),['Transaction2','Transaction3','Transaction4'])
        self.assertEqual(self.Doc.getAvailableRedos(),3)

        # new transaction eight
        self.Doc.openTransaction("Transaction8")
        self.Doc.getObject("test1").Integer  = 8
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction8','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),2)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        self.Doc.abortTransaction()
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),1)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)

        # again new transaction eight
        self.Doc.openTransaction("Transaction8")
        self.Doc.getObject("test1").Integer  = 8
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction8','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),2)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        
        # again new transaction nine
        self.Doc.openTransaction("Transaction9")
        self.Doc.getObject("test1").Integer  = 9
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction9','Transaction8','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),3)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        self.Doc.commitTransaction()
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction9','Transaction8','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),3)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)
        self.assertEqual(self.Doc.getObject("test1").Integer, 9)

        # undo the ninth transaction
        self.Doc.undo()
        self.assertEqual(self.Doc.getObject("test1").Integer, 8)
        self.assertEqual(self.Doc.getAvailableUndoNames(),['Transaction8','Transaction1'])
        self.assertEqual(self.Doc.getAvailableUndos(),2)
        self.assertEqual(self.Doc.getAvailableRedoNames(),['Transaction9'])
        self.assertEqual(self.Doc.getAvailableRedos(),1)

        # switch on the Undo OFF
        self.Doc.setUndoMode(0)
        self.assertEqual(self.Doc.getAvailableUndoNames(),[])
        self.assertEqual(self.Doc.getAvailableUndos(),0)
        self.assertEqual(self.Doc.getAvailableRedoNames(),[])
        self.assertEqual(self.Doc.getAvailableRedos(),0)


    def tearDown(self):
        # closing doc
        FreeCAD.closeDocument("UndoTest")


        
