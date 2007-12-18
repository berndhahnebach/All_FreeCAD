# FreeCAD TemplatePyMod module  
# (c) 2007 Juergen Riegel LGPL

#


# import FreeCAD modules
import FreeCAD, FreeCADGui,inspect

# helper -------------------------------------------------------------------

def addCommand(name,cmdObject):
	(list,num) = inspect.getsourcelines(cmdObject.Activated)
	pos = 0
	# check for indentation
	while(list[1][pos] == ' ' or list[1][pos] == '\t'):
		pos += 1
	source = ""
	for i in range(len(list)-1):
		source += list[i+1][pos:]
	FreeCADGui.addCommand(name,cmdObject,source)
	

#---------------------------------------------------------------------------
# The command classes
#---------------------------------------------------------------------------

class TemplatePyMod_Cmd1:
    "Example command class"
    def Activated(self):
        print "TemplatePyMod_Cmd1 activated ;-) "

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool1', 'MenuText': 'Example command', 'ToolTip': 'Very unimportand example command'}


class TemplatePyMod_Cmd2:
    "Example command class"
    def Activated(self):
        d = FreeCAD.ActiveDocument
        v = FreeCADGui.ActiveDocument.ActiveView
        class PolygonCreator:
            def __init__(self, doc, view, max):
                self.doc = doc
                self.view = view
                self.call = view.addEventCallback("SoMouseButtonEvent",self.addVertex)
                self.max = max
                self.node=[]
                self.count=0
                self.poly=None

            def addVertex(self, d):
                if (d["State"] == "DOWN"):
                    pos = d["Position"]
                    self.node.append(self.view.getPoint(pos[0],pos[1]))
                    self.count = self.count+1
                    if (self.count == 1):
                        import Part,PartGui
                        self.poly=self.doc.addObject("Part::Polygon","Polygon")
                        self.poly.Nodes = self.node
                        self.poly.Close=True
                    else:
                        self.poly.Nodes = self.node
                        self.doc.recompute()
                    if (self.count == self.max):
                        self.node=[]
                        self.view.removeEventCallback("SoMouseButtonEvent",self.call)

        p=PolygonCreator(d,v,10)

    def IsActive(self):
        if FreeCAD.ActiveDocument == None:
            return False
        else:
            return True

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool2', 'MenuText': 'Create polygon...', 'ToolTip': 'Create a polygon by clicking inside the viewer'}


class TemplatePyMod_Cmd3:
    "Import PyQt4"
    def Activated(self):
        import PythonQt
        from PyQt4 import QtGui
        mw=QtGui.qApp.activeWindow()
        QtGui.QMessageBox.information(mw,"PyQt","""PyQt was loaded successfully.\n
        Load the PyQt sandbox now...""")
        FreeCADGui.activateWorkbench("PythonQtWorkbench")

    def GetResources(self):
        return {'Pixmap'  : 'python', 'MenuText': 'Import PyQt4', 'ToolTip': 'Add a workbench for PyQt4 samples'}

#---------------------------------------------------------------------------
# Adds the commands to the FreeCAD command manager
#---------------------------------------------------------------------------
addCommand('TemplatePyMod_Cmd1',TemplatePyMod_Cmd1())
addCommand('TemplatePyMod_Cmd2',TemplatePyMod_Cmd2())
addCommand('TemplatePyMod_Cmd3',TemplatePyMod_Cmd3())

