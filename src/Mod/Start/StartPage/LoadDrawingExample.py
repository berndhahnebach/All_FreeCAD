import FreeCAD,FreeCADGui
FreeCAD.open(FreeCAD.getResourceDir()+"data/examples/DrawingExample.FCStd")
FreeCADGui.SendMsgToActiveView("ViewFit")
FreeCADGui.activeDocument().activeView().viewAxometric()
