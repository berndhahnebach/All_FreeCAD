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

import FreeCAD,FreeCADGui,Part,Draft,MeshPart
from draftlibs import fcgeo,fcvec
from FreeCAD import Vector
from PyQt4 import QtCore

__title__="FreeCAD Arch Commands"
__author__ = "Yorik van Havre"
__url__ = "http://free-cad.sourceforge.net"

# module functions ###############################################

def addComponents(objectsList,host):
    '''addComponents(objectsList,hostObject): adds the given object or the objects
    from the given list as components to the given host Object. Use this for
    example to add windows to a wall, or to add walls to a cell or floor.'''
    if not isinstance(objectsList,list):
        objectsList = [objectsList]
    tp = Draft.getType(host)
    if tp in ["Cell","Floor","Building","Site"]:
        c = host.Components
        for o in objectsList:
            if not o in c:
                c.append(o)
        host.Components = c
    elif tp in ["Wall","Structure"]:
        a = host.Additions
        for o in objectsList:
            if not o in a:
                if hasattr(o,"Shape"):
                    a.append(o)
        host.Additions = a

def removeComponents(objectsList,host=None):
    '''removeComponents(objectsList,[hostObject]): removes the given component or
    the components from the given list from their parents. If a host object is
    specified, this function will try adding the components as holes to the host
    object instead.'''
    if not isinstance(objectsList,list):
        objectsList = [objectsList]
    if host:
        if Draft.getType(host) in ["Wall","Structure"]:
            s = host.Subtractions
            for o in objectsList:
                if not o in s:
                    s.append(o)
            host.Subtractions = s
    else:
        for o in objectsList:
            if o.InList:
               h = o.InList[0]
               tp = Draft.getType(h)
               if tp in ["Cell","Floor","Building","Site"]:
                   c = h.Components
                   if o in c:
                       c.remove(o)
                       h.Components = c
                       o.ViewObject.show()
               elif tp in ["Wall","Structure"]:
                   a = h.Additions
                   s = h.Subtractions
                   if o in a:
                       a.remove(o)
                       h.Additions = a
                       o.ViewObject.show()
                   elif o in s:
                       s.remove(o)
                       h.Subtractions = s
                       o.ViewObject.show()

def splitMesh(obj,mark=True):
    '''splitMesh(object,[mark]): splits the given mesh object into separated components.
    If mark is False, nothing else is done. If True (default), non-manifold components
    will be painted in red.'''
    if not obj.isDerivedFrom("Mesh::Feature"): return
    basemesh = obj.Mesh
    comps = basemesh.getSeparateComponents()
    nlist = []
    if comps:
        basename = obj.Name
        FreeCAD.ActiveDocument.removeObject(basename)
        for c in comps:
            newobj = FreeCAD.ActiveDocument.addObject("Mesh::Feature",basename)
            newobj.Mesh = c
            if mark and (not c.isSolid()) and c.hasNonManifolds():
                newobj.ViewObject.ShapeColor = (1.0,0.0,0.0,1.0)
            nlist.append(newobj)
        return nlist
    return [obj]

def meshToShape(obj):
    '''meshToShape(object): turns a mesh into a shape, joining coplanar facets'''
    if "Mesh" in obj.PropertiesList:
        faces = []	
        mesh = obj.Mesh
        segments = mesh.getPlanes(0.01) # use rather strict tolerance here

        for i in segments:
            if len(i) > 0:
                # a segment can have inner holes
                wires = MeshPart.wireFromSegment(mesh, i)
                # we assume that the exterior boundary is that one with
                # the biggest bounding box
                if len(wires) > 0:
                    ext = None
                    max_length = 0
                    for i in wires:		
                        if i.BoundBox.DiagonalLength > max_length:
                            max_length = i.BoundBox.DiagonalLength
                            ext = i
                    wires.remove(ext)
                    # all interior wires mark a hole and must reverse
                    # their orientation, otherwise Part.Face fails
                    for i in wires:
                        i.reverse()
                    # make sure that the exterior wires comes as first in the lsit
                    wires.insert(0, ext)
                    faces.append(Part.Face(wires))

        shell=Part.Compound(faces)
        solid = Part.Solid(Part.Shell(faces))
        name = obj.Name
        FreeCAD.ActiveDocument.removeObject(name)
        newobj = FreeCAD.ActiveDocument.addObject("Part::Feature",name)
        newobj.Shape = solid
        return newobj

# command definitions ###############################################
                       
class CommandAdd:
    "the Arch Add command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_Add',
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Arch_Add","Add component"),
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Arch_Add","Adds the selected components to the active object")}

    def IsActive(self):
        if len(FreeCADGui.Selection.getSelection()) > 1:
            return True
        else:
            return False
        
    def Activated(self):
        sel = FreeCADGui.Selection.getSelection()
        host = sel.pop()
        addComponents(sel,host)

class CommandRemove:
    "the Arch Add command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_Remove',
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Arch_Remove","Remove component"),
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Arch_Remove","Remove the selected components from their parents, or create a hole in a component")}

    def IsActive(self):
        if FreeCADGui.Selection.getSelection():
            return True
        else:
            return False
        
    def Activated(self):
        sel = FreeCADGui.Selection.getSelection()
        if Draft.getType(sel[-1]) in ["Wall","Structure"]:
            host = sel.pop()
            removeComponents(sel,host)
        else:
            removeComponents(sel)


class CommandSplitMesh:
    "the Arch SplitMesh command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_SplitMesh',
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Arch_SplitMesh","Split Mesh"),
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Arch_SplitMesh","Splits selected meshes into independent components")}

    def IsActive(self):
        if len(FreeCADGui.Selection.getSelection()):
            return True
        else:
            return False
        
    def Activated(self):
        if FreeCADGui.Selection.getSelection():
            for obj in FreeCADGui.Selection.getSelection():
                nobjs = splitMesh(obj)

class CommandMeshToShape:
    "the Arch MeshToShape command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_MeshToShape',
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Arch_MeshToShape","Mesh to Shape"),
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Arch_MeshToPart","Turns selected meshes into Part Shape objects")}

    def IsActive(self):
        if len(FreeCADGui.Selection.getSelection()):
            return True
        else:
            return False
        
    def Activated(self):
        if FreeCADGui.Selection.getSelection():
            for obj in FreeCADGui.Selection.getSelection():
                nobjs = meshToShape(obj)
            
FreeCADGui.addCommand('Arch_Add',CommandAdd())
FreeCADGui.addCommand('Arch_Remove',CommandRemove())
FreeCADGui.addCommand('Arch_SplitMesh',CommandSplitMesh())
FreeCADGui.addCommand('Arch_MeshToShape',CommandMeshToShape())
