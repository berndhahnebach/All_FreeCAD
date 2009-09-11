
#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2009 Yorik van Havre <yorik@gmx.fr>                     *
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

__title__="FreeCAD Draft Workbench - DXF importer/exporter"
__author__ = "Yorik van Havre <yorik@gmx.fr>"
__url__ = ["http://yorik.orgfree.com","http://free-cad.sourceforge.net"]

'''
This script uses a DXF-parsing library created by Stani,
Kitsu and Migius for Blender

imports:
line, polylines, lwpolylines, arcs, circles, texts,
mtexts, layers (as groups), colors

exports:
lines, polylines, lwpolylines, circles, arcs,
texts, colors,layers (from groups)
'''

import FreeCAD, os, Part, math, re, string
from draftlibs import fcvec, dxfColorMap, dxfLibrary, fcgeo
from draftlibs.dxfReader import readDXF
from PyQt4 import QtGui
from Draft import Dimension, DimensionViewProvider

try: import FreeCADGui
except: gui = False
else: gui = True
try: draftui = FreeCADGui.activeWorkbench().draftToolBar.ui
except: draftui = None

pythonopen = open # to distinguish python built-in open function from the one declared here

def decodeName(name):
	"decodes encoded strings"
	try:
		decodedName = (name.decode("utf8"))
	except UnicodeDecodeError:
		try:
			decodedName = (name.decode("latin1"))
		except UnicodeDecodeError:
			print "dxf: error: couldn't determine character encoding"
			decodedName = name
	return decodedName

def locateLayer(wantedLayer):
	"returns layer group and creates it if needed"
	wantedLayerName = decodeName(wantedLayer)
	for l in layers:
		if wantedLayerName==l.Label:
			return l
	newLayer = doc.addObject("App::DocumentObjectGroup",wantedLayer)
	newLayer.Label = wantedLayerName
	layers.append(newLayer)
	return newLayer

def calcBulge(v1,bulge,v2):
	'''
	calculates intermediary vertex for curved segments.
	algorithm from http://www.afralisp.net/lisp/Bulges1.htm
	'''
	chord = fcvec.new(v1,v2)
	sagitta = (bulge * chord.Length)/2
	startpoint = fcvec.add(v1,fcvec.scale(chord,0.5))
	perp = fcvec.crossproduct(chord)
	endpoint = fcvec.scale(fcvec.normalized(perp),sagitta)
	return fcvec.add(startpoint,endpoint)

def getGroup(ob,exportList):
	"checks if the object is part of a group"
	for i in exportList:
		if (i.Type == "App::DocumentObjectGroup"):
			for j in i.Group:
				if (j == ob):
					return i.Label
	return "0"

def getACI(ob,text=False):
	"gets the ACI color closest to the objects color"
	if not gui: return 0
	else:
		if text:
			col=ob.ViewObject.TextColor
		else:
			col=ob.ViewObject.LineColor
		aci=[0,442]
		for i in range (255,-1,-1):
			ref=dxfColorMap.color_map[i]
			dist=((ref[0]-col[0])**2 + (ref[1]-col[1])**2 + (ref[2]-col[2])**2)
			if (dist <= aci[1]): aci=[i,dist]
		return aci[0]

def rawValue(entity,code):
	"returns the  value of a DXF code in an entity section"
	value = None
	for pair in entity.data:
		if pair[0] == code:
			value = pair[1]
	return value

class fcformat:
	"this contains everything related to color/lineweight formatting"
	def __init__(self,drawing):
		self.dxf = drawing
		params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
		self.paramtext = params.GetBool("dxftext")
		self.paramlayouts = params.GetBool("dxflayouts")
		self.paramstyle = params.GetInt("dxfstyle")
		bparams = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/View")

		if self.paramstyle > 1:
			# checking if FreeCAD background is dark or bright
			if bparams.GetBool("Gradient"):
				c1 = bparams.GetUnsigned("BackgroundColor2")
				c2 = bparams.GetUnsigned("BackgroundColor3")
				r1 = float((c1>>24)&0xFF)
				g1 = float((c1>>16)&0xFF)
				b1 = float((c1>>8)&0xFF)
				r2 = float((c2>>24)&0xFF)
				g2 = float((c2>>16)&0xFF)
				b2 = float((c2>>8)&0xFF)
				v1 = FreeCAD.Vector(r1,g1,b1)
				v2 = FreeCAD.Vector(r2,g2,b2)
				v = fcvec.new(v1,v2)
				v = fcvec.scale(v,0.5)
				cv = fcvec.add(v1,v)
			else:
				c1 = bparams.GetUnsigned("BackgroundColor")
				r1 = float((c1>>24)&0xFF)
				g1 = float((c1>>16)&0xFF)
				b1 = float((c1>>8)&0xFF)
				cv = FreeCAD.Vector(r1,g1,b1)
			value = cv.x*.3 + cv.y*.59 + cv.z*.11
			if value < 128: self.brightbg = False
			else:
				self.brightbg = True
	
		if gui and draftui:
			r = float(draftui.color.red()/255.0)
			g = float(draftui.color.green()/255.0)
			b = float(draftui.color.blue()/255.0)
			self.lw = float(draftui.widthButton.value())
		else:
			self.lw = float(params.GetInt("linewidth"))
			c = params.GetUnsigned("color")
			r = float(((c>>24)&0xFF)/255)
			g = float(((c>>16)&0xFF)/255)
			b = float(((c>>8)&0xFF)/255)
		self.col = (r,g,b,0.0)

		if self.paramstyle == 3:
			parammappingfile = params.GetString("dxfmappingfile")
			self.table = self.buildTable(parammappingfile)
		
	def buildTable(self,tablefile):
		"builds a table for converting colors into linewidths"
		try: f = pythonopen(tablefile)
		except ValueError:
			print "error: ",tablefile, " not found"
			return None
		table = {}
		header = len(f.readline().split("\t"))
		if header == 15:
			for l in f:
				s = l.split("\t")
				if "Color_" in s[0]:
					index = int(s[0].split("_")[1])
					if s[1] == "(Object)": color = "object"
					else:
						c = s[2].split(",")
						color = [float(c[0])/255,float(c[1])/255,float(c[2])/255]
						if (color == [0.0,0.0,0.0]) and (not self.brightbg):
							color = [1.0,1.0,1.0]
					if s[2] == "(Object)": width = "object"
					else: width = float(s[10])*10
					table[index]=[color,width]
		elif header == 3:
			for l in f:
				s = l.split("\t")
				index = int(s[0])
				c = string.replace(s[1],'"','')
				c = c.split(",")
				color = [float(c[0])/255,float(c[1])/255,float(c[2])/255]
				width = float(s[2])
				table[index]=[color,width]
			for i in range(256):
				if not i in table.keys():
					table[i]=["object","object"]
		else:
			print "error building mapping table: file format not recognized"
			table = None
		print table
		return table
		
	def formatObject(self,obj,dxfobj):
		"applies color and linetype to objects"
		if self.paramstyle == 1:
			obj.ViewObject.LineColor = self.col
			obj.ViewObject.LineWidth = self.lw
		elif self.paramstyle == 2:
			if dxfobj.color_index == 256: cm = self.getGroupColor(dxfobj)
			elif (dxfobj.color_index == 7) and self.brightbg: cm = [0.0,0.0,0.0]
			else: cm = dxfColorMap.color_map[dxfobj.color_index]
			obj.ViewObject.LineColor = (cm[0],cm[1],cm[2],0.0)
			obj.ViewObject.LineWidth = self.lw
		elif self.paramstyle == 3:
			if dxfobj.color_index == 256:
				cm = self.table[self.getGroupColor(dxfobj,index=True)][0]
				wm = self.table[self.getGroupColor(dxfobj,index=True)][1]
			else:
				cm = self.table[dxfobj.color_index][0]
				wm = self.table[dxfobj.color_index][1]
			if cm == "object": cm = self.getGroupColor(dxfobj)
			else: obj.ViewObject.LineColor = (cm[0],cm[1],cm[2],0.0)
			if wm == "object": wm = self.lw
			else: obj.ViewObject.LineWidth = wm

	def getGroupColor(self,dxfobj,index=False):
		"get color of bylayer stuff"
		name = dxfobj.layer
		for table in self.dxf.tables.get_type("table"):
			if table.name == "layer":
				for l in table.get_type("layer"):
					if l.name == name:
						if index: return l.color
						else:
							if (l.color == 7) and self.brightbg: return [0.0,0.0,0.0]
							else: return dxfColorMap.color_map[l.color]

def drawLine(line):
	"returns a Part shape from a dxf line"
	if (len(line.points) > 1):
		v1=FreeCAD.Vector(line.points[0][0],line.points[0][1],line.points[0][2])
		v2=FreeCAD.Vector(line.points[1][0],line.points[1][1],line.points[1][2])
		if not fcvec.equals(v1,v2):
			return Part.Line(v1,v2).toShape()
	return None

def drawPolyline(polyline):
	"returns a Part shape from a dxf polyline"
	if (len(polyline.points) > 1):
		if len(polyline.points) == 2: print "debug: 2-point polyline!"
		edges = []
		for p in range(len(polyline.points)-1):
			p1 = polyline.points[p]
			p2 = polyline.points[p+1]
			v1 = FreeCAD.Vector(p1[0],p1[1],p2[2])
			v2 = FreeCAD.Vector(p2[0],p2[1],p2[2])
			if not fcvec.equals(v1,v2):
				if polyline.points[p].bulge:
					cv = calcBulge(v1,polyline.points[p].bulge,v2)
					edges.append(Part.Arc(v1,cv,v2).toShape())
				else:
					edges.append(Part.Line(v1,v2).toShape())
		if polyline.closed:
			p1 = polyline.points[len(polyline.points)-1]
			p2 = polyline.points[0]
			v1 = FreeCAD.Vector(p1[0],p1[1],p2[2])
			v2 = FreeCAD.Vector(p2[0],p2[1],p2[2])
			if not fcvec.equals(v1,v2):
				edges.append(Part.Line(v1,v2).toShape())
		if edges: return Part.Wire(edges)
	return None

def drawArc(arc):
	"returns a Part shape from a dxf arc"
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
	return Part.Arc(v1,v2,v3).toShape()

def drawCircle(circle):
	"returns a Part shape from a dxf circle"
	v = FreeCAD.Vector(circle.loc[0],circle.loc[1],circle.loc[2])
	curve = Part.Circle()
	curve.Radius = circle.radius
	curve.Center = v
	return curve.toShape()

def addObject(shape,name,layer):
	"adds a new object to the document with passed arguments"
	newob=doc.addObject("Part::Feature",name)
	lay=locateLayer(layer)
	lay.addObject(newob)
	newob.Shape = shape
	return newob

def processdxf(document,filename):
	"this does the translation of the dxf contents into FreeCAD Part objects"

	global drawing # for debugging - so drawing is still accessible to python after the script
	drawing = readDXF(filename)
	global layers
	layers = []
	global doc
	doc = document
	print "dxf: parsing file ",filename

	# getting config parameters

	fmt = fcformat(drawing)

	# drawing lines

	lines = drawing.entities.get_type("line")
	if lines: FreeCAD.Console.PrintMessage("drawing "+str(len(lines))+" lines...\n")
	for line in lines:
		shape = drawLine(line)
		if shape:
			newob = addObject(shape,"Line",line.layer)
			if gui: fmt.formatObject(newob,line)
						
	# drawing polylines - at the moment arc segments get straightened...

	polylines = drawing.entities.get_type("lwpolyline")
	polylines.extend(drawing.entities.get_type("polyline"))
	if polylines: FreeCAD.Console.PrintMessage("drawing "+str(len(polylines))+" polylines...\n")
	for polyline in polylines:
		shape = drawPolyline(polyline)
		if shape:
			newob = addObject(shape,"Polyline",polyline.layer)
			if gui: fmt.formatObject(newob,polyline)
				
	# drawing arcs

	arcs = drawing.entities.get_type("arc")
	if arcs: FreeCAD.Console.PrintMessage("drawing "+str(len(arcs))+" arcs...\n")
	for arc in arcs:
		shape = drawArc(arc)
		if shape:
			newob = addObject(shape,"Arc",arc.layer)
			if gui: fmt.formatObject(newob,arc)

	# drawing circles

	circles = drawing.entities.get_type("circle")
	if circles: FreeCAD.Console.PrintMessage("drawing "+str(len(circles))+" circles...\n")
	for circle in circles:
		shape = drawCircle(circle)
		if shape:
			newob = addObject(shape,"Circle",circle.layer)
			if gui: fmt.formatObject(newob,circle)

	# drawing texts

	if fmt.paramtext:
		texts = drawing.entities.get_type("mtext")
		texts.extend(drawing.entities.get_type("text"))
		if texts: FreeCAD.Console.PrintMessage("drawing "+str(len(texts))+" texts...\n")
		for text in texts:
				lay=locateLayer(text.layer)
				newob=doc.addObject("App::Annotation","Text")
				lay.addObject(newob)
				newob.LabelText = re.sub('{([^!}]([^}]|\n)*)}', '', text.value) 
				newob.Position=FreeCAD.Vector(text.loc[0],text.loc[1],text.loc[2])
				if gui:
					newob.ViewObject.FontSize=float(text.height)
					newob.ViewObject.DisplayMode = "World"
					if fmt.paramstyle == 0: newob.ViewObject.TextColor = (r,g,b)
					elif fmt.paramstyle == 1: newob.ViewObject.TextColor = (0.0,0.0,0.0)
					elif fmt.paramstyle == 2:
						if text.color_index > 255: cm = [0.0,0.0,0.0]
						else: cm = dxfColorMap.color_map[text.color_index]
						newob.ViewObject.TextColor = (cm[0],cm[1],cm[2])
					elif fmt.paramstyle == 3:
						cm = table[text.color_index][0]
						wm = table[text.color_index][1]
						newob.ViewObject.TextColor = (cm[0],cm[1],cm[2])
					
	else: FreeCAD.Console.PrintMessage("skipping texts...\n")

	# drawing dims

	dims = drawing.entities.get_type("dimension")
	if dims and fmt.paramtext:
		FreeCAD.Console.PrintMessage("drawing "+str(len(dims))+" dimensions...\n")
		for dim in dims:
			try:
				layer = rawValue(dim,8)
				x1 = float(rawValue(dim,10))
				y1 = float(rawValue(dim,20))
				z1 = float(rawValue(dim,30))
				x2 = float(rawValue(dim,13))
				y2 = float(rawValue(dim,23))
				z2 = float(rawValue(dim,33))
				x3 = float(rawValue(dim,14))
				y3 = float(rawValue(dim,24))
				z3 = float(rawValue(dim,34))
				d = rawValue(dim,70)
				if d: align = int(d)
				else: align = 0
				d = rawValue(dim,50)
				if d: angle = float(d)
				else: angle = 0
			except:
				print "debug: dim data not processed: ",dim.data
			else:
				lay=locateLayer(layer)
				pt = FreeCAD.Vector(x1,y1,z1)
				p1 = FreeCAD.Vector(x2,y2,z2)
				p2 = FreeCAD.Vector(x3,y3,z3)
				if align == 0:
					if angle == 0:
						p2 = FreeCAD.Vector(x3,y2,z2)
					else:
						p2 = FreeCAD.Vector(x2,y3,z2)

				newob = doc.addObject("App::FeaturePython","Dimension")
				lay.addObject(newob)
				Dimension(newob)
				DimensionViewProvider(newob.ViewObject)
				newob.Start = p1
				newob.End = p2
				newob.Dimline = pt
				if gui: fmt.formatObject (newob,dim)
						
	else: FreeCAD.Console.PrintMessage("skipping dimensions...\n")

	# drawing blocks

	inserts = drawing.entities.get_type("insert")
	if inserts:
		FreeCAD.Console.PrintMessage("drawing "+str(len(dims))+" blocks...\n")
		blockrefs = drawing.blocks.data
		blockshapes = {}
		for ref in blockrefs:
			shapes = []
			for line in ref.entities.get_type('line'):
				s = drawLine(line)
				if s: shapes.append(s)
			for polyline in ref.entities.get_type('polyline'):
				s = drawPolyline(polyline)
				if s: shapes.append(s)
			for polyline in ref.entities.get_type('lwpolyline'):
				s = drawPolyline(polyline)
				if s: shapes.append(s)
			for arc in ref.entities.get_type('arc'):
				s = drawArc(arc)
				if s: shapes.append(s)
			for circle in ref.entities.get_type('circle'):
				s = drawCircle(circle)
				if s: shapes.append(s)
			blockshapes[ref.name]=Part.makeCompound(shapes)
		for insert in inserts:
			shape = blockshapes[insert.block]
			if shape:
				pos = FreeCAD.Vector(insert.loc[0],insert.loc[1],insert.loc[2])
				rot = insert.rotation
				shape.translate(pos)
				newob = addObject(shape,"Block",insert.layer)
				if gui: fmt.formatObject(newob,insert)
			

	doc.recompute()
	del fmt
	FreeCAD.Console.PrintMessage("successfully imported "+filename+"\n")

def open(filename):
	"called when freecad opens a file"
	docname = os.path.splitext(os.path.basename(filename))[0]
	doc = FreeCAD.newDocument(docname)
	doc.Label = decodeName(docname[:-4])
	processdxf(doc,filename)

def insert(filename,docname):
	"called when freecad imports a file"
	groupname = os.path.splitext(os.path.basename(filename))[0]
	doc=FreeCAD.getDocument(docname)
	importgroup = doc.addObject("App::DocumentObjectGroup",groupname)
	importgroup.Label = decodeName(groupname)
	processdxf(doc,filename)
	for l in layers:
		importgroup.addObject(l)

def export(exportList,filename):
	"called when freecad exports a file"
	dxf = dxfLibrary.Drawing()
	for i in exportList:
		if isinstance(i,Part.Feature):			
			for wire in i.Shape.Wires:

				# polylines
				
				points = []
				bulges = []
				flag= int(wire.isClosed())
				org_point = [0.0,0.0,0.0]
				edges = fcgeo.sortEdges(wire.Edges)
				for edge in edges:
					v1 = edge.Vertexes[0].Point
					if (isinstance(edge.Curve,Part.Circle)):
						v2 = edge.Vertexes[-1].Point
						c = edge.Curve.Center
						angle = abs(fcvec.angle(fcvec.new(c,v1),fcvec.new(c,v2)))
						if (fcvec.angle(fcvec.new(c,v2)) < fcvec.angle(fcvec.new(c,v1))):
							angle = -angle
						bul = math.tan(angle/4)
					else:
						bul = 0.0
					points.append((v1.x,v1.y,v1.z,None,None,bul))
				if not wire.isClosed():
					points.append(fcvec.tup(edges[-1].Vertexes[-1].Point))
						
				dxf.append(dxfLibrary.PolyLine(points, org_point, flag, color=getACI(i), layer=getGroup(i,exportList)))
										
			if (len(i.Shape.Wires) == 0) and (len(i.Shape.Edges) == 1):
				edge = i.Shape.Edges[0]
				if (len(i.Shape.Vertexes) == 2) and (isinstance (edge.Curve,Part.Line)):

					# lines
					
					ve1=edge.Vertexes[0].Point
					ve2=edge.Vertexes[1].Point
					dxf.append(dxfLibrary.Line([fcvec.tup(ve1),fcvec.tup(ve2)], color=getACI(i), layer=getGroup(i,exportList)))
				elif (len(i.Shape.Vertexes) == 1):

					# circles
					
					center = fcvec.tup(edge.Curve.Center)
					radius = i.Shape.Edges[0].Curve.Radius
					dxf.append(dxfLibrary.Circle(center, radius, color=getACI(i), layer=getGroup(i,exportList)))

				elif (len(i.Shape.Vertexes) == 3) or (isinstance (edge.Curve,Part.Circle)):

					# arcs
					
					ce = i.Shape.Edges[0].Curve.Center
					radius = i.Shape.Edges[0].Curve.Radius
					ve1 = edge.Vertexes[0].Point
					ve2 = edge.Vertexes[-1].Point
					ang1=-math.degrees(fcvec.angle(fcvec.new(ce,ve1)))
					ang2=-math.degrees(fcvec.angle(fcvec.new(ce,ve2)))
					dxf.append(dxfLibrary.Arc(fcvec.tup(ce), radius, ang1, ang2, color=getACI(i), layer=getGroup(i,exportList)))

		elif (i.Type == "App::Annotation"):

			# texts

			# temporary - as dxfLibrary doesn't support mtexts well, we use several single-line texts
			# well, anyway, at the moment, Draft only writes single-line texts, so...
			for text in i.LabelText:
				point = fcvec.tup(FreeCAD.Vector(i.Position.x,i.Position.y-i.LabelText.index(text),i.Position.z))
				if gui: height = float(i.ViewObject.FontSize)
				else: height = 1
				dxf.append(dxfLibrary.Text(text,point,height=height, color=getACI(i,text=True), layer=getGroup(i,exportList)))

		elif (i.Type == "App::FeaturePython"):
			if 'Dimline' in i.PropertiesList:
				p1 = fcvec.tup(i.Start)
				p2 = fcvec.tup(i.End)
				base = Part.Line(i.Start,i.End).toShape()
				proj = fcgeo.findDistance(i.Dimline,base)
				if not proj:
					pbase = fcvec.tup(i.End)
				else:
					pbase = fcvec.tup(i.End.add(fcvec.neg(proj)))
				dxf.append(dxfLibrary.Dimension(pbase,p1,p2,color=getACI(i), layer=getGroup(i,exportList)))
					
	dxf.saveas(filename)
	FreeCAD.Console.PrintMessage("successfully exported "+filename+"\r\n")






