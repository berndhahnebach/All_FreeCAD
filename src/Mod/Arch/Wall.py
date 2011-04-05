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

__title__="FreeCAD Wall"
__author__ = "Yorik van Havre"
__url__ = "http://free-cad.sourceforge.net"

def makeWall(baseobj,width=None,height=None,align="Center"):
    '''makeWall(obj,[width],[height]): creates a wall based on the
    given object'''
    obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython","Wall")
    Wall(obj)
    ViewProviderWall(obj.ViewObject)
    obj.Base = baseobj
    if width: obj.Width = width
    if height: obj.Height = height
    obj.Align = align
    if obj.Base: obj.Base.ViewObject.hide()
    return obj

class CommandWall:
    "the Arch Wall command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_Wall',
                'MenuText': "Wall",
                'Accel': "W, A",
                'ToolTip': "Creates a wall object from scratch or from a selected object (wire, face or solid)"}
        
    def Activated(self):
        sel = FreeCADGui.Selection.getSelection()
        if sel:
            for obj in sel:
                makeWall(obj)
        else:
            FreeCAD.Console.PrintWarning("Not implemented! Select an object first")
       
class Wall(Component.Component):
    "The Wall object"
    def __init__(self,obj):
        Component.Component.__init__(self,obj)
        obj.addProperty("App::PropertyLength","Width","Base",
                        "The width of this wall")
        obj.addProperty("App::PropertyLength","Height","Base",
                        "The height of this wall")
        obj.addProperty("App::PropertyVector","Normal","Base",
                        "The normal extrusion direction of this wall (keep (0,0,0) for automatic normal)")
        obj.addProperty("App::PropertyEnumeration","Align","Base",
                        "The alignment of this wall on its base object, if applicable")
        obj.addProperty("App::PropertyLinkList","Appendices","Base",
                        "Other shapes that are appended to this wall")
        obj.addProperty("App::PropertyLinkList","Holes","Base",
                        "Other shapes that are subtracted from this wall")
        obj.Align = ['Left','Right','Center']
        self.Type = "Wall"
        obj.Width = 0.1
        obj.Height = 1
        
    def execute(self,obj):
        self.createGeometry(obj)
        
    def onChanged(self,obj,prop):
        if prop in ["Base","Height","Width","Align","Appendices","Holes","CrossingWalls"]:
            self.createGeometry(obj)

    def createGeometry(self,obj):
        pl = obj.Placement
        if obj.Normal == Vector(0,0,0):
            normal = Vector(0,0,1)
        else:
            normal = Vector(obj.Normal)
        if obj.Base:
            if obj.Base.isDerivedFrom("Part::Feature"):
                base = obj.Base.Shape.copy()
                if base.Solids:
                    obj.Shape = base
                elif base.Faces:
                    if obj.Height:
                        norm = normal.multiply(obj.Height)
                        base = base.extrude(norm)
                elif base.Wires and not base.isClosed():
                    dvec = fcgeo.vec(base.Edges[0]).cross(normal)
                    dvec.normalize()
                    if obj.Align == "Left":
                        dvec = dvec.multiply(obj.Width)
                        base = Draft.offset(obj.Base,dvec,bind=True)
                    elif obj.Align == "Right":
                        dvec = dvec.multiply(obj.Width)
                        dvec = fcvec.neg(dvec)
                        base = Draft.offset(obj.Base,dvec,bind=True)
                    elif obj.Align == "Center":
                        dvec = dvec.multiply(obj.Width)
                        base = Draft.offset(obj.Base,dvec,bind=True,sym=True)
                    if obj.Height:
                        norm = normal.multiply(obj.Height)
                        base = base.extrude(norm)
                for app in obj.Appendices:
                    base = base.fuse(app.Shape)
                    app.viewObject.hide() #to be removed
                for hole in obj.Holes:
                    base = base.cut(hole.Shape)
                    hole.ViewObject.hide() # to be removed
                obj.Shape = base
                obj.Placement = pl

class ViewProviderWall(Component.ViewProviderComponent):
    "A View Provider for the Wall object"

    def __init__(self,vobj):
        Component.ViewProviderComponent.__init__(self,vobj)

    def getIcon(self):          
        return """
                /* XPM */
                static char * Arch_Wall_xpm[] = {
                "16 16 9 1",
                " 	c None",
                ".	c #543016",
                "+	c #6D2F08",
                "@	c #954109",
                "#	c #874C24",
                "$	c #AE6331",
                "%	c #C86423",
                "&	c #FD7C26",
                "*	c #F5924F",
                "                ",
                "                ",
                "       #        ",
                "      ***$#     ",
                "    .*******.   ",
                "   *##$****#+   ",
                " #**%&&##$#@@   ",
                ".$**%&&&&+@@+   ",
                "@&@#$$%&&@@+..  ",
                "@&&&%#.#$#+..#$.",
                " %&&&&+%#.$**$@+",
                "   @%&+&&&$##@@+",
                "     @.&&&&&@@@ ",
                "        @%&&@@  ",
                "           @+   ",
                "                "};
		"""
        
    def claimChildren(self):
        return [self.Object.Base]+self.Object.Appendices+self.Object.Holes
