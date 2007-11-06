# FreeCAD TemplatePyMod module  
# (c) 2007 Juergen Riegel LGPL

#


# import FreeCAD modules
import FreeCAD, FreeCADGui

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
        d = FreeCAD.newDocument()
        v = FreeCADGui.activeDocument().activeView()
        class PolygonCreator:
            def __init__(self, doc, view, max):
                self.doc = doc
                self.view = view
                self.call = view.addEventCallback("SoMouseButtonEvent",self.addVertex)
                self.max = max
                self.down=True
                self.node=[]
                self.count=0
                self.poly=None

            def addVertex(self, x, y):
                if (self.down):
                    self.node.append(self.view.getPoint(x,y))
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
                self.down = not self.down

        p=PolygonCreator(d,v,10)

    def GetResources(self):
        return {'Pixmap'  : 'Std_Tool2', 'MenuText': 'Create polygon...', 'ToolTip': 'Create a polygon by clicking inside the viewer'}


#---------------------------------------------------------------------------
# Adds the commands to the FreeCAD command manager
#---------------------------------------------------------------------------
FreeCADGui.addCommand('TemplatePyMod_Cmd1'        ,TemplatePyMod_Cmd1())
FreeCADGui.addCommand('TemplatePyMod_Cmd2'        ,TemplatePyMod_Cmd2())

