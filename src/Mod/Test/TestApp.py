# FreeCAD Part module  
# (c) 2001 Jürgen Riegel
#
# Part design module

#***************************************************************************
#*   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
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

import FreeCAD, os


#---------------------------------------------------------------------------
# define the functions to test the FreeCAD base code
#---------------------------------------------------------------------------


def TestAll():
    FreeCAD.PrintMessage("Checking on basic stuff\n")
    TestBase()
    FreeCAD.PrintMessage("Checking on Parameters\n")
    TestParameter()
    FreeCAD.PrintMessage("Checking on document\n")
    TestDoc()

def TestDoc():
    FreeCAD.PrintLog("Creating new Part document and filing up\n")
    Doc = FreeCAD.DocNew()
    FreeCAD.PrintLog("   Reading properties\n")
    i = Doc.UndoLimit
    i = Doc.AvailableUndos
    i = Doc.AvailableRedos

    Main = Doc.Main #getting the main label of the document 
    L1 = Main.GetLabel(1)
    L1 = Main.GetLabel(1)
    L1 = Main.GetLabel(1)
    L1 = Main.GetLabel(1)
    L1 = Main.GetLabel(1)
    L1 = Main.GetLabel(1)
    L2 = Main.GetLabel(2)
    L3 = Main.GetLabel(3)
    L4 = Main.GetLabel(4)
    L5 = Main.GetLabel(5)
    L6 = Main.GetLabel(6)
    L7 = Main.GetLabel(7)
    L1.Int = 1
    L1.Real = 1.0
    L1.Name = "Hallo"

    # saving and restoring
    TempPath = os.getenv('TEMP')
    FreeCAD.PrintLog ('  Using temp path: ' + TempPath + '\n')

    SavePath = TempPath + os.sep + "Test1.FCStd"
    FreeCAD.PrintLog("   Save and Open the document to: " + SavePath + "\n")
    Doc.SaveAs(SavePath)
    FreeCAD.PrintLog("   provocate exceptio by try loading a already open document\n")
    try:
        Doc2 = App.DocOpen(SavePath)
    except:
        FreeCAD.PrintLog("   exception thrown, OK\n")
    else:
        FreeCAD.PrintLog("   no exception thrown, ERROR\n")
        raise IOError
    # closing doc
    Doc = 0

def TestBase():
    FreeCAD.PrintLog ("Testing console...\n")

    FreeCAD.PrintMessage("   Printing message\n")
    FreeCAD.PrintError("   Printing error\n")
    FreeCAD.PrintWarning("   Printing warning\n")
    FreeCAD.PrintLog("   Printing Log\n")

def TestParameter():
    # Parameter testing
    TestPar = FreeCAD.ParamGet("System parameter:Test")
    FreeCAD.PrintLog("Test:")
    for i in range(50):
        FreeCAD.PrintLog(`i`+",")
        TestPar.SetFloat(`i`,4711.4711)
        TestPar.SetInt(`i`,4711)
        TestPar.SetBool(`i`,1)
        Temp = TestPar.GetGroup(`i`)
        for l in range(50):
            Temp.SetFloat(`l`,4711.4711)
            Temp.SetInt(`l`,4711)
            Temp.SetBool(`l`,1)
    Temp = 0
    FreeCAD.PrintLog("\n")
    #check on special conditions
    TestPar = FreeCAD.ParamGet("System parameter:Test/44")
    # check on Int
    if(TestPar.GetInt("44") == 4711):
        FreeCAD.PrintLog("Int OK\n")
    else:
        FreeCAD.PrintLog("Error reading back Int")
        raise
    # check on float
    if(TestPar.GetFloat("44") == 4711.4711):
        FreeCAD.PrintLog("Float OK\n")
    else:
        FreeCAD.PrintLog("Error reading back Float")
        raise
    # check on Bool
    if(TestPar.GetBool("44") == 1):
        FreeCAD.PrintLog("Bool OK\n")
    else:
        FreeCAD.PrintLog("Error reading back Bool")
        raise
    #remove all
    TestPar = FreeCAD.ParamGet("System parameter:Test")
    TestPar.Clear()


def TestParameterHeavy(i=10):
    for l in range(i):
        TestParameter()

        

