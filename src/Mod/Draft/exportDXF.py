
#***************************************************************************
#*   FreeCAD Draft Workbench                                               *
#*   Author: Yorik van Havre - http://yorik.orgfree.com                    *
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

#    DXF object types exported by this script
#    line: converted from FreeCAD Line
#    lwpolyline: converted from FreeCAD Polygon
#    circle: converted from FreeCAD Circle
#    arc: converted from FreeCAD Circle

#    colors are mapped from objects RGB colors to autocad color index (ACI). Black will be "by layer"
#    layers are mapped from group names When groups are nested, the deepest group gives the layer name.

import FreeCAD, FreeCADGui, math, Part
from draftlibs import dxfLibrary, fcvec, dxfColorMap
from PyQt4 import QtGui

def export(exportList,filename):

	dxf = dxfLibrary.Drawing()

	for i in exportList:

		if isinstance(i,Part.Feature):
			for j in i.Shape.Wires:
				points=[]
				flag= int(j.isClosed())
				for v in j.Vertexes: points.append(fcvec.tup(v.Point))
				org_point = [0.0,0.0,0.0]
				dxf.append(dxfLibrary.PolyLine(points, org_point, flag, color=getACI(i), layer=getLayer(i,exportList)))
			if (len(i.Shape.Wires) == 0) and (len(i.Shape.Edges) == 1):
				edge = i.Shape.Edges[0]
				if (len(i.Shape.Vertexes) == 2) and (isinstance (edge.Curve,Part.Line)):
					ve1=edge.Vertexes[0].Point
					ve2=edge.Vertexes[1].Point
					dxf.append(dxfLibrary.Line([fcvec.tup(ve1),fcvec.tup(ve2)], color=getACI(i), layer=getLayer(i,exportList)))
				elif (len(i.Shape.Vertexes) == 1):
					center = fcvec.tup(edge.Curve.Center)
					radius = i.Shape.Edges[0].Curve.Radius
					dxf.append(dxfLibrary.Circle(center, radius, color=getACI(i), layer=getLayer(i,exportList)))
				elif (len(i.Shape.Vertexes) == 3) or (isinstance (edge.Curve,Part.Circle)):
					ce = i.Shape.Edges[0].Curve.Center
					radius = i.Shape.Edges[0].Curve.Radius
					ve1 = edge.Vertexes[0].Point
					ve2 = edge.Vertexes[-1].Point
					ang1=-math.degrees(fcvec.angle(fcvec.new(ce,ve1)))
					ang2=-math.degrees(fcvec.angle(fcvec.new(ce,ve2)))
					dxf.append(dxfLibrary.Arc(fcvec.tup(ce), radius, ang1, ang2, color=getACI(i), layer=getLayer(i,exportList)))

	dxf.saveas(filename)
	FreeCAD.Console.PrintMessage("successfully exported "+filename+"\n")

def getLayer(ob,exportList):
	for i in exportList:
		if (i.Type == "App::DocumentObjectGroup"):
			for j in i.Group:
				if (j == ob):
					return i.Label
	return "0"

def getACI(ob):
	col=FreeCADGui.ActiveDocument.getObject(ob.Name).LineColor
	#col=(int(objcolor[0]*255),int(objcolor[1]*255),int(objcolor[2]*255))
	aci=[0,442]
	for i in range (255,-1,-1):
		ref=dxfColorMap.color_map[i]
		dist=((ref[0]-col[0])**2 + (ref[1]-col[1])**2 + (ref[2]-col[2])**2)
		if (dist <= aci[1]): aci=[i,dist]
	return aci[0]





