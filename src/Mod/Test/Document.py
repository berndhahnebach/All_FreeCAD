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


class DocumentBasicCases(unittest.TestCase):
  def setUp(self):
    self.Doc = FreeCAD.newDocument("CreateTest")

  def testCreateDestroy(self):
    #FIXME: Causes somehow a ref count error but it's _not_ FreeCAD.getDocument()!!!
    #If we remove the whole method no error appears.
    self.failUnless(FreeCAD.getDocument("CreateTest")!= None,"Creating Document failed")

  def testObjects(self):
    L1 = self.Doc.addObject("App::FeatureTest","Label_1")
    #call members to check for errors in ref counting
    self.Doc.ActiveObject
    self.Doc.Objects
    self.Doc.UndoMode
    self.Doc.UndoRedoMemSize
    self.Doc.UndoCount
    self.Doc.RedoCount
    self.Doc.UndoNames
    self.Doc.RedoNames
    self.Doc.recompute()
    self.failUnless(L1.Integer == 4711)
    self.failUnless(L1.Float-47.11<0.001)
    self.failUnless(L1.Bool    == True)
    self.failUnless(L1.String  == "empty")
    #temporarily not checked because of strange behavior of boost::fielesystem JR
    #self.failUnless(L1.Path  == "c:/temp")
    self.failUnless(L1.Angle-3.0<0.001)
    self.failUnless(L1.Distance-47.11<0.001)

    # test the constraint types ( both are constraint to percent range)
    self.failUnless(L1.ConstraintInt == 5)
    self.failUnless(L1.ConstraintFloat-5.0<0.001)
    L1.ConstraintInt = 500
    L1.ConstraintFloat = 500.0
    self.failUnless(L1.ConstraintInt == 100)
    self.failUnless(L1.ConstraintFloat - 100.0 < 0.001)
    L1.ConstraintInt = -500
    L1.ConstraintFloat = -500.0
    self.failUnless(L1.ConstraintInt == 0)
    self.failUnless(L1.ConstraintFloat - 0.0 < 0.001)
    
    # test enum property
    self.failUnless(L1.Enum  == "Four")
    L1.Enum = "One"
    L1.Enum = 2
    self.failUnless(L1.Enum  == "Two",     "Different value to 'Two'")
    try:
      L1.Enum = "SurlyNotInThere!"
    except:
      FreeCAD.PrintLog("   exception thrown, OK\n")
    else:
      self.fail("no exeption thrown")
      
    #self.failUnless(L1.IntegerList  == [4711]   )
    #f = L1.FloatList 
    #self.failUnless(f -47.11<0.001    )
    #self.failUnless(L1.Matrix  == [1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0] )
    #self.failUnless(L1.Vector  == [1.0,2.0,3.0])
    
    self.failUnless(L1.name== "Label_1","Invalid object name")
    L1.name="Label_2"
    self.Doc.recompute()
    self.failUnless(L1.name== "Label_2","Invalid object name")
    self.Doc.removeObject("Label_1")

  def testMem(self):
    self.Doc.MemSize
    
  def tearDown(self):
    #closing doc
    FreeCAD.closeDocument("CreateTest")

class DocumentSaveRestoreCases(unittest.TestCase):
  def setUp(self):
    self.Doc = FreeCAD.newDocument("SaveRestoreTests")
    L1 = self.Doc.addObject("App::FeatureTest","Label_1")
    L2 = self.Doc.addObject("App::FeatureTest","Label_2")   
    self.TempPath = os.getenv('TEMP')
    if self.TempPath == None:
        self.TempPath = "/tmp";
    print '  Using temp path: ' + self.TempPath + '\n'
    
  def testSaveAndRestore(self):
    # saving and restoring
    SaveName = self.TempPath + os.sep + "Test1.FCStd"
    self.Doc.FileName = SaveName
    print "   Save and Open the document to: " + SaveName + "\n"
    self.Doc.save()
    FreeCAD.closeDocument("SaveRestoreTests")
    self.Doc = FreeCAD.openDocument(SaveName)
    self.failUnless(self.Doc.Label_1.Integer == 4711)
    self.failUnless(self.Doc.Label_2.Integer == 4711)

  def testActiveDocument(self):
    # open 2nd doc
    Second = FreeCAD.newDocument("Active")
    FreeCAD.closeDocument("Active")
    try:
        # There might be no active document anymore
        # This also checks for dangling pointers
        Active = FreeCAD.activeDocument()
        # Second is still a valid object
        self.failUnless(Second != Active)
    except:
        # Okay, no document open
        self.failUnless(True)

  def tearDown(self):
    #closing doc
    FreeCAD.closeDocument("SaveRestoreTests")
    print "TesrDown"


class UndoRedoCases(unittest.TestCase):
  def setUp(self):
    self.Doc = FreeCAD.newDocument("UndoTest")
    self.Doc.addObject("App::FeatureTest","Base")

  def testUndoProperties(self):
    # switch on the Undo
    self.Doc.UndoMode = 1
    
    # first transaction 
    self.Doc.openTransaction("Transaction1")
    self.Doc.addObject("App::FeatureTest","test1")
    self.Doc.getObject("test1").Integer  = 1
    self.Doc.getObject("test1").String   = "test1"
    self.Doc.getObject("test1").Float    = 1.0
    self.Doc.getObject("test1").Bool  = 1

    #self.Doc.getObject("test1").IntegerList  = 1
    #self.Doc.getObject("test1").FloatList  = 1.0

    #self.Doc.getObject("test1").Matrix  = (1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0)
    #self.Doc.getObject("test1").Vector  = (1.0,1.0,1.0)
    
    # second transaction 
    self.Doc.openTransaction("Transaction2")
    self.Doc.getObject("test1").Integer  = 2
    self.Doc.getObject("test1").String   = "test2"
    self.Doc.getObject("test1").Float    = 2.0
    self.Doc.getObject("test1").Bool  = 0

    # switch on the Undo OFF
    self.Doc.UndoMode = 0

  def testUndo(self):
    # switch on the Undo
    self.Doc.UndoMode = 1
    self.assertEqual(self.Doc.UndoNames,[])
    self.assertEqual(self.Doc.UndoCount,0)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    
    # first transaction 
    self.Doc.openTransaction("Transaction1")
    self.Doc.addObject("App::FeatureTest","test1")
    self.Doc.getObject("test1").Integer  = 1
    self.assertEqual(self.Doc.UndoNames,['Transaction1'])
    self.assertEqual(self.Doc.UndoCount,1)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    
    # second transaction 
    self.Doc.openTransaction("Transaction2")
    self.assertEqual(self.Doc.UndoNames,['Transaction2','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,2)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    self.Doc.getObject("test1").Integer  = 2
    self.assertEqual(self.Doc.UndoNames,['Transaction2','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,2)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    
    # abort second transaction 
    self.Doc.abortTransaction()
    self.assertEqual(self.Doc.UndoNames,['Transaction1'])
    self.assertEqual(self.Doc.UndoCount,1)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    self.assertEqual(self.Doc.getObject("test1").Integer, 1)
    
    # again second transaction 
    self.Doc.openTransaction("Transaction2")
    self.Doc.getObject("test1").Integer  = 2	
    self.assertEqual(self.Doc.UndoNames,['Transaction2','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,2)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)

    # third transaction
    self.Doc.openTransaction("Transaction3")
    self.assertEqual(self.Doc.UndoNames,['Transaction3','Transaction2','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,3)
    self.Doc.getObject("test1").Integer  = 3		
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    
    # fourth transaction
    self.Doc.openTransaction("Transaction4")
    self.assertEqual(self.Doc.UndoNames,['Transaction4','Transaction3','Transaction2','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,4)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    self.Doc.getObject("test1").Integer  = 4

    # undo the fourth transaction 
    self.Doc.undo()
    self.assertEqual(self.Doc.getObject("test1").Integer, 3)
    self.assertEqual(self.Doc.UndoNames,['Transaction3','Transaction2','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,3)
    self.assertEqual(self.Doc.RedoNames,['Transaction4'])
    self.assertEqual(self.Doc.RedoCount,1)

    # undo the third transaction 
    self.Doc.undo()
    self.assertEqual(self.Doc.getObject("test1").Integer, 2)
    self.assertEqual(self.Doc.UndoNames,['Transaction2','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,2)
    self.assertEqual(self.Doc.RedoNames,['Transaction3','Transaction4'])
    self.assertEqual(self.Doc.RedoCount,2)

    # undo the second transaction 
    self.Doc.undo()
    self.assertEqual(self.Doc.getObject("test1").Integer, 1)
    self.assertEqual(self.Doc.UndoNames,['Transaction1'])
    self.assertEqual(self.Doc.UndoCount,1)
    self.assertEqual(self.Doc.RedoNames,['Transaction2','Transaction3','Transaction4'])
    self.assertEqual(self.Doc.RedoCount,3)

    # undo the first transaction 
    self.Doc.undo()
    self.failUnless(self.Doc.getObject("test1") == None)
    self.assertEqual(self.Doc.UndoNames,[])
    self.assertEqual(self.Doc.UndoCount,0)
    self.assertEqual(self.Doc.RedoNames,['Transaction1','Transaction2','Transaction3','Transaction4'])
    self.assertEqual(self.Doc.RedoCount,4)

    # redo the first transaction 
    self.Doc.redo()
    self.assertEqual(self.Doc.getObject("test1").Integer, 1)
    self.assertEqual(self.Doc.UndoNames,['Transaction1'])
    self.assertEqual(self.Doc.UndoCount,1)
    self.assertEqual(self.Doc.RedoNames,['Transaction2','Transaction3','Transaction4'])
    self.assertEqual(self.Doc.RedoCount,3)

    # redo the second transaction 
    self.Doc.redo()
    self.assertEqual(self.Doc.getObject("test1").Integer, 2)
    self.assertEqual(self.Doc.UndoNames,['Transaction2','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,2)
    self.assertEqual(self.Doc.RedoNames,['Transaction3','Transaction4'])
    self.assertEqual(self.Doc.RedoCount,2)

    # undo the second transaction 
    self.Doc.undo()
    self.assertEqual(self.Doc.getObject("test1").Integer, 1)
    self.assertEqual(self.Doc.UndoNames,['Transaction1'])
    self.assertEqual(self.Doc.UndoCount,1)
    self.assertEqual(self.Doc.RedoNames,['Transaction2','Transaction3','Transaction4'])
    self.assertEqual(self.Doc.RedoCount,3)

    # new transaction eight
    self.Doc.openTransaction("Transaction8")
    self.Doc.getObject("test1").Integer  = 8
    self.assertEqual(self.Doc.UndoNames,['Transaction8','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,2)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    self.Doc.abortTransaction()
    self.assertEqual(self.Doc.UndoNames,['Transaction1'])
    self.assertEqual(self.Doc.UndoCount,1)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)

    # again new transaction eight
    self.Doc.openTransaction("Transaction8")
    self.Doc.getObject("test1").Integer  = 8
    self.assertEqual(self.Doc.UndoNames,['Transaction8','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,2)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    
    # again new transaction nine
    self.Doc.openTransaction("Transaction9")
    self.Doc.getObject("test1").Integer  = 9
    self.assertEqual(self.Doc.UndoNames,['Transaction9','Transaction8','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,3)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    self.Doc.commitTransaction()
    self.assertEqual(self.Doc.UndoNames,['Transaction9','Transaction8','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,3)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)
    self.assertEqual(self.Doc.getObject("test1").Integer, 9)

    # undo the ninth transaction
    self.Doc.undo()
    self.assertEqual(self.Doc.getObject("test1").Integer, 8)
    self.assertEqual(self.Doc.UndoNames,['Transaction8','Transaction1'])
    self.assertEqual(self.Doc.UndoCount,2)
    self.assertEqual(self.Doc.RedoNames,['Transaction9'])
    self.assertEqual(self.Doc.RedoCount,1)

    # switch on the Undo OFF
    self.Doc.UndoMode = 0
    self.assertEqual(self.Doc.UndoNames,[])
    self.assertEqual(self.Doc.UndoCount,0)
    self.assertEqual(self.Doc.RedoNames,[])
    self.assertEqual(self.Doc.RedoCount,0)


  def tearDown(self):
    # closing doc
    FreeCAD.closeDocument("UndoTest")


      
