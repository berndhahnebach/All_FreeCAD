import FreeCAD,FreeCADGui,sys
rf=FreeCAD.ParamGet("User parameter:BaseApp/Preferences/RecentFiles")
FreeCAD.open(rf.GetString("MRU0"))
