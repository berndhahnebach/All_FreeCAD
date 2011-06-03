import FreeCAD, FreeCADGui
workbench = FreeCAD.ConfigGet("StartWorkbench")
FreeCADGui.activateWorkbench(workbench)
App.newDocument()
