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
from PyQt4 import QtCore

__title__="FreeCAD Cell"
__author__ = "Yorik van Havre"
__url__ = "http://free-cad.sourceforge.net"

def makeCell(objectslist):
    '''makeCell(objectslist): creates a cell including the
    objects from the given list'''
    obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython","Cell")
    Cell(obj)
    ViewProviderCell(obj.ViewObject)
    obj.Components = objectslist
    for comp in obj.Components:
        comp.ViewObject.hide()
    return obj

class CommandCell:
    "the Arch Cell command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_Cell',
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Arch_Cell","Cell"),
                'Accel': "C, E",
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Arch_Cell","Creates a cell object including selected objects")}
        
    def Activated(self):
        sel = FreeCADGui.Selection.getSelection()
        makeCell(sel)

class Cell:
    "The Cell object"
    def __init__(self,obj):
        obj.addProperty("App::PropertyLinkList","Components","Base",
                        "The objects that make part of this cell")
        obj.addProperty("App::PropertyLength","PlanHeight","Base",
                        "The height of the section plane when making a plan view of this cell")
        obj.addProperty("App::PropertyBool","DebugMode","Base",
                        "if debug mode, no face cleaning!")
        obj.addProperty("App::PropertyEnumeration","CellType","Base",
                        "This cell can define a Cell, a Floor or a Building")
        obj.PlanHeight = 1.8
        obj.CellType = ["Cell","Floor","Building"]
        obj.CellType = "Cell"
        obj.Proxy = self
        self.Type = "Cell"
        
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
            for comp in components:
                if Draft.getType(comp) in ["Wall","Cell","Shape"]:
                    baseShape = baseShape.fuse(comp.Shape)
            if components:
                if not obj.DebugMode:
                    baseShape = fcgeo.cleanFaces(baseShape)
            obj.Shape = baseShape
            obj.Placement = pl

class ViewProviderCell:
    "A View Provider for the Cell object"
    def __init__(self,vobj):
        vobj.Proxy = self
        self.Object = vobj.Object

    def getIcon(self):
        if self.Object.CellType == "Cell":
            return """
                /* XPM */
                static char * Arch_Cell_xpm[] = {
                "16 16 9 1",
                " 	c None",
                ".	c #0C0A04",
                "+	c #413F37",
                "@	c #636057",
                "#	c #7E7D75",
                "$	c #9C9B95",
                "%	c #B7B8B3",
                "&	c #D2D4D1",
                "*	c #FCFEFB",
                "        +++     ",
                "    ++#++@$+    ",
                " +@$%**@%#@++++ ",
                " #&**%*@%%%$@+##",
                " #&%@@*@%$@+@&*%",
                " #&% @*@$@@%***%",
                " #&% @*@+@#****%",
                " #&%.@*@+@#****%",
                " #&%$&*@+@#****%",
                ".@&****@+@#****%",
                " @&***&@+@#****%",
                " @&**$  .@#****%",
                " @&%#    @#****#",
                " +@      @#**&  ",
                "         @#*$   ",
                "         @@#    "};
                """
        elif self.Object.CellType == "Floor":
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
        else:
            return """
                /* XPM */
                static char * Arch_Building_xpm[] = {
                "16 16 9 1",
                " 	c None",
                ".	c #160E0A",
                "+	c #C10007",
                "@	c #FF0006",
                "#	c #8F3F00",
                "$	c #5E5F5D",
                "%	c #7F817E",
                "&	c #A0A29F",
                "*	c #F4F6F3",
                "                ",
                "     ........   ",
                "    ..#@@@@@.   ",
                "   .&&.+@@@@@.  ",
                "  .&**%.@@@@@+. ",
                " .&****..@@@+...",
                ".%******.##..$$.",
                ".&******&.$&**%.",
                ".%*...$**.****% ",
                ".%*..#.**.****% ",
                " %*..#.**.****$ ",
                " $*..#.**.***$. ",
                " $*..#$**.**..  ",
                " .$...$**.&.    ",
                "   .  .$%..     ",
                "        ..      "};
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

