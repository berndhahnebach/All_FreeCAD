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

__title__="FreeCAD Arch Component"
__author__ = "Yorik van Havre"
__url__ = "http://free-cad.sourceforge.net"

def addToComponent(compobject,addobject):
    '''addToComponent(compobject,addobject): adds addobject
    to the given component'''
    if "Additions" in compobject.PropertiesList:
        compobject.Additions.append(addobject)

def removeFromComponent(compobject,subobject):
    '''removeFromComponent(compobject,subobject): subtracts subobject
    from the given component'''
    if "Subtractions" in compobject.PropertiesList:
        compobject.Subtractions.append(subobject)
        
class Component:
    "The Component object"
    def __init__(self,obj):
        obj.addProperty("App::PropertyLink","Base","Base",
                        "The base object this component is built upon")
        obj.addProperty("App::PropertyLinkList","Additions","Base",
                        "Other shapes that are appended to this wall")
        obj.addProperty("App::PropertyLinkList","Subtractions","Base",
                        "Other shapes that are subtracted from this wall")
        obj.addProperty("App::PropertyVector","Normal","Base",
                        "The normal extrusion direction of this wall (keep (0,0,0) for automatic normal)")
        obj.Proxy = self
        self.Type = "Component"
        
class ViewProviderComponent:
    "A View Provider for Component objects"
    def __init__(self,vobj):
        vobj.Proxy = self
        self.Object = vobj.Object
        
    def updateData(self,obj,prop):
        return

    def onChanged(self,vobj,prop):
        return

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

    def claimChildren(self):
        return [self.Object.Base]+self.Object.Additions+self.Object.Subtractions


