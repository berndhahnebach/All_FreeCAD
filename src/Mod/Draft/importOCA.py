
#***************************************************************************
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

'''
This script imports OCA/gcad files into FreeCAD.
'''

import FreeCAD, os, Part, math
from draftlibs import fcvec, fcgeo
from FreeCAD import Vector

try: import FreeCADGui
except ValueError: gui = False
else: gui = True

pythonopen = open

def getpoint(data):
    "turns an OCA point definition into a FreeCAD Vector"
    if (data[0:2] == "P("):
        n = data.replace(")","")
        n = n[2:].split()
        return Vector(float(n[0]),float(n[1]),float(n[2]))
    elif (data[0] == "P") and (data[1] != "("):
        return objects[data]
              
def getarea(data):
    "turns an OCA area definition into a FreeCAD Part Face"
    if (data[0:2] == "S("):
        n = data.replace(")","")
        n = n[2:].split()
        if (n[0] == "POL"):
            pts = n[1:]
            verts = []
            for p in pts:
                if (p[0] == "P"):
                    verts.append(getpoint(p))
            verts.append(objects[pts[0]])
            w = Part.makePolygon(verts)
            return Part.Face(w)

def getarc(data):
    "turns an OCA arc definition into a FreeCAD Part Arc"
    n = data.split()
    if (n[0] == "ARC"):
        pts = n[1:]
        verts = []
        for p in pts:
            if (p[0] == "P"):
                verts.append(getpoint(p))
        w = Part.Arc(verts[0],verts[1],verts[2])
        print w
        return w.toShape()
        
def parse(filename,doc):
    "inports an opened OCA file into the given doc"
    filebuffer = pythonopen(filename)
    global objects
    objects = {}
    global color
    color = (0,0,0)
    for l in filebuffer:
        rline = l.replace(","," ").upper()
        print rline
        if ("=" in rline):

            # entity definitions
            pair = rline.split("=")
            id = pair[0]
            if id[0] == "P":
                data = getpoint(pair[1])
                objects[id] = data
            elif id[0] == "A":
                data = getarea(pair[1])
                objects[id] = data
                ob = doc.addObject("Part::Feature",id)
                ob.Shape = data
                if gui: ob.ViewObject.ShapeColor = color
            elif id[0] == "C":
                data = getarc(pair[1])
                objects[id] = data
                ob = doc.addObject("Part::Feature",id)
                ob.Shape = data
                if gui: ob.ViewObject.ShapeColor = color

        elif (rline[0:6] == "DEFCOL"):
            c = rline.split()
            color = (float(c[1])/255,float(c[2])/255,float(c[3])/255)

    del color

def decodeName(name):
	"decodes encoded strings"
	try:
		decodedName = (name.decode("utf8"))
	except UnicodeDecodeError:
		try:
			decodedName = (name.decode("latin1"))
		except UnicodeDecodeError:
			print "oca: error: couldn't determine character encoding"
			decodedName = name
	return decodedName
    
def open(filename):
    docname=os.path.split(filename)[1]
    doc=FreeCAD.newDocument(docname)
    doc.Label = decodeName(docname[:-4])    
    parse(filename,doc)
    doc.recompute()

def insert(filename):
    doc = FreeCAD.ActiveDocument
    parse(filename,doc)
    doc.recompute()
            
            
                
                
            
        

        
    
