import FreeCAD,FreeCADGui,Part,Component
from FreeCAD import Vector
from PyQt4 import QtCore

class CommandSectionPlane:
    "the Arch SectionPlane command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_SectionPlane',
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Arch_SectionPlane","Section Plane"),
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Arch_SectionPlane","Adds a section plane object to the document")}

    def Activated(self):
        FreeCAD.ActiveDocument.openTransaction("Section Plane")
        obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython","Section")
        SectionPlane(obj)
        ViewProviderSectionPlane(obj.ViewObject)
        FreeCAD.ActiveDocument.commitTransaction()
        FreeCAD.ActiveDocument.recompute()

class SectionPlane:
    "A section plane object"
    def __init__(self,obj):
        obj.Proxy = self
        self.Type = "Visual Component"
        
    def execute(self,obj):
        pl = obj.Placement
        l = obj.ViewObject.DisplaySize
        p = Part.makePlane(l,l,Vector(-l/2,-l/2,0))
        obj.Shape = p
        obj.Placement = pl

class ViewProviderSectionPlane(Component.ViewProviderComponent):
    "A View Provider for Section Planes"
    def __init__(self,vobj):
        vobj.addProperty("App::PropertyLength","DisplaySize","Base",
                        "The display size of the section plane")
        vobj.DisplaySize = 1
        vobj.Transparency = 85
        vobj.LineWidth = 1
        vobj.LineColor = (0.0,0.0,0.4,1.0)
        vobj.Proxy = self
        self.Object = vobj.Object

    def getIcon(self):
        return """
            /* XPM */
            static char * Arch_SectionPlane_xpm[] = {
            "16 16 3 1",
            " 	c None",
            ".	c #000000",
            "+	c #FFFF00",
            "     ......     ",
            "   ..+++.++..   ",
            "  .+++++..+++.  ",
            " .++++++...+++. ",
            " .++.+++....++. ",
            ".++.+.++.....++.",
            ".+.+++.+......+.",
            ".+.+++.+........",
            ".+.+++.+........",
            ".+.....+......+.",
            ".+.+++.+.....++.",
            " .++++++....++. ",
            " .++++++...+++. ",
            "  .+++++..+++.  ",
            "   ..+++.++..   ",
            "     ......     "};
            """

    def claimChildren(self):
        return []

    def onChanged(self,vobj,prop):
        if prop == "DisplaySize":
            vobj.Object.Proxy.execute(vobj.Object)

FreeCADGui.addCommand('Arch_SectionPlane',CommandSectionPlane())
