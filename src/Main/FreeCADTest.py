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
s = Doc.Name

Main = Doc.Main #getting the main label of the document 



SavePath = TempPath + os.sep + "Test.std"
Log("   Save and Open the document to: " + SavePath + "\n")
Doc.SaveAs(SavePath)





FreeCAD.PrintLog ("FreeCAD test done\n")


