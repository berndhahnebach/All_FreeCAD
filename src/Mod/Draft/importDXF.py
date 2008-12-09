
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

__title__="FreeCAD DXF importer"
__author__ = "Yorik van Havre"
__url__ = "http://yorik.orgfree.com"
__version__ = "0.1.4"

'''
This script uses a DXF-parsing module created by Kitsu and Migius for Blender
DF object types read by this script + implementation in FreeCAD:
line: read and converted to FreeCAD Line
polyline: read and converted to FreeCAD Wire w/ arcs straigthened
lwpolyline: read and converted to FreeCAD Wire w/ arcs straigthened
text: read and converted to FreeCAD Annotation
mtext: read and converted to FreeCAD Annotation
circle: read and converted to FreeCAD Circle
arc: read and converted to FreeCAD Circle
layer: read and layers containing objects are conveted to FreeCAD Group
block_record: read but no implementation in FreeCAD
block: read but no implementation in FreeCAD
insert: read but no implementation in FreeCAD
ellipse: read but no implementation in FreeCAD
3dface: read but not implemented yet
'''

import FreeCAD, FreeCADGui, os, Part, math, re
from draftlibs.dxfReader import readDXF
from draftlibs import fcvec

def decodeName(name): # decodes encoded strings
	try:
		decodedName = (name.decode("utf8"))
	except UnicodeDecodeError:
		try:
			decodedName = (name.decode("latin1"))
		except UnicodeDecodeError:
			print "dxf: error: couldn't determine character encoding"
			decodedName = name
	return decodedName

def locateLayer(wantedLayer,doc,layers): # returns layer group and creates it if needed
	wantedLayerName = decodeName(wantedLayer)
	for l in layers:
		if wantedLayerName==l.Label:
			return l
	newLayer = doc.addObject("App::DocumentObjectGroup",wantedLayer)
	newLayer.Label = wantedLayerName
	layers.append(newLayer)
	return newLayer

def processdxf(doc,filename):

	global drawing # for debugging - so drawing is still accessible to python after the script
	drawing = readDXF(filename)
	print "dxf: parsing file ",filename
	layers=[]

#    getting config parameters

	params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
	paramtext = params.GetBool("dxftext")
	paramlayouts = params.GetBool("dxflayouts")
	paramstyle = params.GetInt("dxfstyle")
	parammappingfile = params.GetString("dxfmappingfile")
	ui = FreeCADGui.activeWorkbench().draftToolBar.ui
	r = float(ui.color.red()/255.0)
	g = float(ui.color.green()/255.0)
	b = float(ui.color.blue()/255.0)
	lw = float(ui.widthButton.value())
	col = (r,g,b)

#    drawing lines

	lines = drawing.entities.get_type("line")
	if (len(lines) > 0): FreeCAD.Console.PrintMessage("drawing "+str(len(lines))+" lines...\n")
	for line in lines:
		if (len(line.points) > 1):
			v1=FreeCAD.Vector(line.points[0][0],line.points[0][1],line.points[0][2])
			v2=FreeCAD.Vector(line.points[1][0],line.points[1][1],line.points[1][2])
			lay=locateLayer(line.layer,doc,layers)
			newob=doc.addObject("Part::Feature","Line")
			lay.addObject(newob)
			if not fcvec.equals(v1,v2):
				newseg = Part.Line(v1,v2)
				newob.Shape = newseg.toShape()

#    drawing polylines - at the moment arc segments get straightened...

	polylines = drawing.entities.get_type("lwpolyline")
	polylines.extend(drawing.entities.get_type("polyline"))
	if (len(polylines) > 0): FreeCAD.Console.PrintMessage("drawing "+str(len(polylines))+" polylines...\n")
	for polyline in polylines:
		lay=locateLayer(polyline.layer,doc,layers)
		if (len(polyline.points) > 1):
			newob = doc.addObject("Part::Feature","Polyline")
			lay.addObject(newob)
			p1 = polyline.points.pop(0)
			points=[FreeCAD.Vector(p1[0],p1[1],p1[2])]
			for p in polyline.points:
				v=FreeCAD.Vector(p[0],p[1],p[2])
				if not fcvec.equals(v,points[-1]): 
					points.append(v)
			if (len(points)>1):
				sh = Part.Line(points[0],points[1]).toShape()
				for i in range(1,len(points)-1):
					newseg = Part.Line(points[i],points[i+1])
					sh = sh.fuse(newseg.toShape())
				if (polyline.closed):
					if not fcvec.equals(points[-1],points[0]):
						newseg = Part.Line(points[-1],points[0])
					e = sh.Edges
					e.append(newseg.toShape())
					sh = Part.Wire(e)
				newob.Shape = sh

#    drawing arcs

	arcs = drawing.entities.get_type("arc")
	if (len(arcs) > 0): FreeCAD.Console.PrintMessage("drawing "+str(len(arcs))+" arcs...\n")
	for arc in arcs:
		lay=locateLayer(arc.layer,doc,layers)
		newob=doc.addObject("Part::Feature","Arc")
		lay.addObject(newob)
		v=FreeCAD.Vector(arc.loc[0],arc.loc[1],arc.loc[2])
		firstangle=(arc.start_angle/180)*math.pi
		lastangle=(arc.end_angle/180)*math.pi
		rayvec=FreeCAD.Vector(arc.radius,0,0)
		v1 = fcvec.add(v,fcvec.rotate(rayvec,firstangle))
		v3 = fcvec.add(v,fcvec.rotate(rayvec,lastangle))
		if lastangle > firstangle:
			v2 = fcvec.add(v,fcvec.rotate(rayvec,(lastangle-firstangle)/2+firstangle))
		else:
			v2 = fcvec.add(v,fcvec.rotate(rayvec,(lastangle-(math.pi*2-firstangle))/2))
		curve = Part.Arc(v1,v2,v3)
		newob.Shape = curve.toShape()

#    drawing circles

	circles = drawing.entities.get_type("circle")
	if (len(circles) > 0): FreeCAD.Console.PrintMessage("drawing "+str(len(circles))+" circles...\n")
	for circle in circles:
		lay = locateLayer(circle.layer,doc,layers)
		newob=doc.addObject("Part::Feature","Circle")
		lay.addObject(newob)
		v = FreeCAD.Vector(circle.loc[0],circle.loc[1],circle.loc[2])
		curve = Part.Circle()
		curve.Radius = circle.radius
		curve.Center = v
		newob.Shape = curve.toShape()

#    drawing texts

	if paramtext:
		texts = drawing.entities.get_type("mtext")
		texts.extend(drawing.entities.get_type("text"))
		if (len(texts) > 0): FreeCAD.Console.PrintMessage("drawing "+str(len(texts))+" texts...\n")
		for text in texts:
			if (len(line.points) > 1):
				lay=locateLayer(text.layer,doc,layers)
				newob=doc.addObject("App::Annotation","Text")
				lay.addObject(newob)
				newob.LabelText = re.sub('{([^!}]([^}]|\n)*)}', '', text.value) #removing tags
				newob.Position=FreeCAD.Vector(text.loc[0],text.loc[1],text.loc[2])
				vo=newob.ViewObject
				if (vo != None):
					vo.FontSize=int(text.height)*12
					vo.TextColor=(0.0,0.0,0.0)

	doc.recompute()
	print "dxf: successfully finished DXF import."

def open(filename): # called when freecad opens a file
	docname = os.path.splitext(os.path.basename(filename))[0]
	doc = FreeCAD.newDocument(docname)
	doc.Label = decodeName(docname)
	processdxf(doc,filename)

def insert(filename,docname): # called when freecad imports a file
	groupname = os.path.splitext(os.path.basename(filename))[0]
	doc=FreeCAD.getDocument(docname)
	importgroup = doc.addObject("App::DocumentObjectGroup",groupname)
	importgroup.Label = decodeName(groupname)
	processdxf(doc,filename)
	for l in layers:
		importgroup.addObject(l)
