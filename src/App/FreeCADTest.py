# FreeCAD test module  
# (c) 2002 Jürgen Riegel
#
# Testing the function of the base system and run 
# (if existing) the test function of the modules
#

#***************************************************************************
#*   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
#*                                                                         *
#*   This file is part of the FreeCAD CAx development system.              *
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
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


Log ("FreeCAD test running...\n\n")

TempPath = os.getenv('TEMP')
if not TempPath: TempPath='.'
Log ('Using temp path: ' + TempPath + '\n')

# Basics +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FreeCAD.PrintLog ("Testing console...\n")

FreeCAD.PrintMessage("   Printing message\n")
FreeCAD.PrintError("   Printing error\n")
FreeCAD.PrintWarning("   Printing warning\n")
FreeCAD.PrintLog("   Printing Log\n")

# Application and Document +++++++++++++++++++++++++++++++++++++++++++++++++
Log ("Testing Application and Document...\n")

Log("   Creating new Part document and filing up\n")
Doc = App.New("Part")
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

SavePath = TempPath + os.sep + "Test1.FCStd"
Log("   Save and Open the document to: " + SavePath + "\n")
Doc.SaveAs(SavePath)
Log("provocate exceptio by try loading a already open document\n")
try:
    Doc2 = App.DocOpen(SavePath)
except:
    Log("exception thrown, OK\n")
else:
    Log("no exception thrown, ERROR\n")
    raise IOError
# closing doc
Doc = 0

#Doc2 = App.DocOpen(SavePath)

#Log(Doc2.Main.GetLabel(1).Name)



FreeCAD.PrintLog ("FreeCAD test done\n")


