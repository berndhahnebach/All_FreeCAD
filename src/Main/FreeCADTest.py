# FreeCAD test module  
# (c) 2002 Jürgen Riegel
#
# Testing the function of the base system and run 
# (if existing) the test function of the modules
#

Log = FreeCAD.PrintLog

FreeCAD.PrintLog ("FreeCAD test running...\n\n")

TempPath = os.getenv('TEMP')
FreeCAD.PrintLog ('Using temp path: ' + TempPath + '\n')

# Basics +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FreeCAD.PrintLog ("Testing console...\n")

FreeCAD.PrintMessage("   Printing message\n")
FreeCAD.PrintError("   Printing error\n")
FreeCAD.PrintLog("   Printing Log\n")

# Application and Document +++++++++++++++++++++++++++++++++++++++++++++++++
FreeCAD.PrintLog ("Testing Application and Document...\n")

Log("   Creating new Part document and filing up\n")
Doc = App.DocNew("Part")
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


SavePath = TempPath + os.sep + "Test.std"
Log("   Save and Open the document to: " + SavePath + "\n")
Doc.SaveAs(SavePath)
Log( Doc.Path + Doc.Name)





FreeCAD.PrintLog ("FreeCAD test done\n")


