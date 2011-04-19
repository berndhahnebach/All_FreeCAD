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

import FreeCAD, collada, Mesh, os

__title__="FreeCAD Collada importer"
__author__ = "Yorik van Havre"
__url__ = "http://free-cad.sourceforge.net"

DEBUG = True

def open(filename):
    "called when freecad wants to open a file"
    docname = os.path.splitext(os.path.basename(filename))[0]
    doc = FreeCAD.newDocument(docname)
    doc.Label = decode(docname)
    FreeCAD.ActiveDocument = doc
    read(filename)
    return doc

def decode(name):
    "decodes encoded strings"
    try:
        decodedName = (name.decode("utf8"))
    except UnicodeDecodeError:
        try:
            decodedName = (name.decode("latin1"))
        except UnicodeDecodeError:
            print "ifc: error: couldn't determine character encoding"
            decodedName = name
    return decodedName

def read(filename):
    col = collada.Collada(filename, ignore=[collada.DaeUnsupportedError])
    for geom in col.scene.objects('geometry'):
        for prim in geom.primitives():
            print prim
            meshdata = []
            for tri in prim.triangles():
                face = []
                for v in tri.vertices:
                    face.append([v[0],v[1],v[2]])
                meshdata.append(face)
            print meshdata
            newmesh = Mesh.Mesh(meshdata)
            print newmesh
            obj = FreeCAD.ActiveDocument.addObject("Mesh::Feature","Mesh")
            obj.Mesh = newmesh
