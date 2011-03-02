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

import FreeCAD,FreeCADGui,Part,Draft
from draftlibs import fcgeo,fcvec
from FreeCAD import Vector

__title__="FreeCAD Floor"
__author__ = "Yorik van Havre"
__url__ = "http://free-cad.sourceforge.net"

def makeFloor(objectslist):
    '''makeFloor(objectslist): creates a floor including the
    objects from the givne list'''
    obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython","Floor")
    Floor(obj)
    ViewProviderFloor(obj.ViewObject)
    obj.Components = objectslist
    for comp in obj.Components:
        comp.ViewObject.hide()
    return obj

class CommandFloor:
    "the Arch Floor command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_Floor',
                'MenuText': "Floor",
                'ToolTip': "Creates a floor object including selected objects"}
        
    def Activated(self):
        sel = FreeCADGui.Selection.getSelection()
        makeFloor(sel)

class Floor:
    "The Floor object"
    def __init__(self,obj):
        obj.addProperty("App::PropertyLinkList","Components","Base",
                        "The objects that make part of this floor")
        obj.addProperty("App::PropertyLength","PlanHeight","Base",
                        "The height of the section plane when making a plan view of this floor")
        obj.addProperty("App::PropertyBool","DebugMode","Base",
                        "if debug mode, no face cleaning!")
        obj.PlanHeight = 1.8
        obj.Proxy = self
        self.Type = "Floor"
        
    def execute(self,obj):
        self.createGeometry(obj)
        
    def onChanged(self,obj,prop):
        if prop in ["Components"]:
            self.createGeometry(obj)

    def createGeometry(self,obj):
        pl = obj.Placement
        if obj.Components:
            components = obj.Components[:]
            f = components.pop(0)
            baseShape = f.Shape
            for comp in obj.Components:
                if Draft.getType(comp) in ["Wall","Floor","Shape"]:
                    baseShape = baseShape.fuse(comp.Shape)
            if not obj.DebugMode: baseShape = fcgeo.cleanFaces(baseShape)
            obj.Shape = baseShape
            obj.Placement = pl

class ViewProviderFloor:
    "A View Provider for the Floor object"
    def __init__(self,vobj):
        vobj.Proxy = self
        self.Object = vobj.Object

    def getIcon(self):          
        return """
                /* XPM */
                static char * Arch_Floor_xpm[] = {
                "16 16 9 1",
                " 	c None",
                ".	c #171817",
                "+	c #2E2876",
                "@	c #545653",
                "#	c #605C98",
                "$	c #959794",
                "%	c #9694BC",
                "&	c #C8C9CC",
                "*	c #F9FBFA",
                "                ",
                "                ",
                "............... ",
                ".&&%&*&%%%%%&*. ",
                ".&++#*#+++++#*. ",
                ".&+%****&+*+#*. ",
                ".&+#%%&*&+*+#*. ",
                ".&++++&**+*+#*. ",
                ".&+%***%%**+.$. ",
                ".&++###*#+#.$@. ",
                ".&++++#*+++.&.. ",
                ".&********&..   ",
                ".$$$$$$$$$@.    ",
                " ..........     ",
                "                ",
                "                "};
                """
        
    def updateData(self,obj,prop):
        return

    def onChanged(self,vobj,prop):
        return

    def claimChildren(self):
        return self.Object.Components

    def attach(self,obj):
        return

    def getDisplayModes(self,obj):
        modes=[]
        return modes

    def setDisplayMode(self,mode):
        return mode

    def __getstate__(self):
        return None

    def __setstate__(self,state):
        return None

