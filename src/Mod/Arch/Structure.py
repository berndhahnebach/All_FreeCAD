#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2011                                                    *  
#*   Yorik van Havre <yorik@uncreated.net>                                 *  
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with this program; if not, write to the Free Software   *
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#***************************************************************************

import FreeCAD,FreeCADGui,Part,Draft,Component
from draftlibs import fcgeo,fcvec
from FreeCAD import Vector
from PyQt4 import QtCore

__title__="FreeCAD Structure"
__author__ = "Yorik van Havre"
__url__ = "http://free-cad.sourceforge.net"

def makeStructure(baseobj,length=None,name="Structure"):
    '''makeStructure(obj,[length]): creates a structure element
    based on the given object and the given length'''
    obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython",name)
    Structure(obj)
    ViewProviderStructure(obj.ViewObject)
    obj.Base = baseobj
    if length: obj.Length = length
    if obj.Base: obj.Base.ViewObject.hide()
    return obj

class CommandStructure:
    "the Arch Structure command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_Structure',
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Arch_Structure","Structure"),
                'Accel': "S, T",
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Arch_Structure","Creates a structure object from scratch or from a selected object (sketch, wire, face or solid)")}
        
    def Activated(self):
        sel = FreeCADGui.Selection.getSelection()
        if sel:
            for obj in sel:
                makeStructure(obj)
        else:
            FreeCADGui.runCommand("Draft_Wire")
       
class Structure(Component.Component):
    "The Structure object"
    def __init__(self,obj):
        Component.Component.__init__(self,obj)
        obj.addProperty("App::PropertyLength","Length","Base",
                        "The length of this element. Keep 0 for automatic")
        obj.addProperty("App::PropertyVector","Normal","Base",
                        "The normal extrusion direction of this wall (keep (0,0,0) for automatic normal)")
        self.Type = "Structure"
        
    def execute(self,obj):
        self.createGeometry(obj)
        
    def onChanged(self,obj,prop):
        if prop in ["Base","Length","Normal","Additions","Subtractions"]:
            self.createGeometry(obj)

    def createGeometry(self,obj):
        # getting default values
        length = normal = None
        if obj.Length:
            length = obj.Length
        else:
            for p in obj.InList:
                if Draft.getType(p) == "Floor":
                    length = p.Height
        if not length: length = 1
        if obj.Normal == Vector(0,0,0):
            normal = Vector(0,0,1)
        else:
            normal = Vector(obj.Normal)

        # creating shape
        if obj.Base:
            if obj.Base.isDerivedFrom("Part::Feature"):
                pl = obj.Placement
                norm = normal.multiply(length)
                base = obj.Base.Shape.copy()
                base = base.extrude(normal)
                obj.Shape = base
                obj.Placement = pl

class ViewProviderStructure(Component.ViewProviderComponent):
    "A View Provider for the Structure object"

    def __init__(self,vobj):
        Component.ViewProviderComponent.__init__(self,vobj)

    def getIcon(self):          
        return """
                /* XPM */
                static char * Arch_Structure_xpm[] = {
                "16 16 9 1",
                " 	c None",
                ".	c #2A303B",
                "+	c #484F58",
                "@	c #506573",
                "#	c #617887",
                "$	c #7A98A6",
                "%	c #A5A4A8",
                "&	c #C1C5C5",
                "*	c #ECEEEB",
                "......++++@##%%@",
                "@$@************%",
                "@$@************%",
                "@$@********&%#+ ",
                "@$@****%%@+.%%  ",
                ".@+%#+.+ @$@*&  ",
                "   #$+*% @$@*&  ",
                "   #$@*% @$@*&  ",
                "   #$@*% @$@*&  ",
                "   #$@*% @$@*&  ",
                "   #$@*% @$@*&  ",
                "   #$@*% @$@*&  ",
                "   #$@*% .@#%+  ",
                "   #$@*%   .    ",
                "   +#@*@        ",
                "    .++         "};
		"""

FreeCADGui.addCommand('Arch_Structure',CommandStructure())
