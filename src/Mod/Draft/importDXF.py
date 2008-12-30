
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
__url__ = ["http://yorik.orgfree.com","http://free-cad.sourceforge.net"]

'''
This script uses a DXF-parsing module created by Kitsu and Migius for Blender
DF object types read by this script + implementation in FreeCAD:
line: read and converted to FreeCAD Line
polyline: read and converted to FreeCAD Wire
lwpolyline: read and converted to FreeCAD Wire
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

import FreeCAD, os, Part, math, re
from draftlibs.dxfReader import readDXF
from draftlibs import fcvec, dxfColorMap

try: import FreeCADGui
except ValueError: gui = False
else: gui = True

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

def locateLayer(wantedLayer,doc,layers):
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
	
		if gui:
			ui = FreeCADGui.activeWorkbench().draftToolBar.ui
			r = float(ui.color.red()/255.0)
			g = float(ui.color.green()/255.0)
			b = float(ui.color.blue()/255.0)
			self.lw = float(ui.widthButton.value())
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
		try: f = pythonopen(tablefile)
		except ValueError:
			print "error: ",tablefile, " not found"
			return None
		table = {}
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
		return table
		
	def formatObject(self,obj,dxfobj):
		if self.paramstyle == 1:
			obj.ViewObject.LineColor = self.col
			obj.ViewObject.LineWidth = self.lw
		elif self.paramstyle == 2:
			if dxfobj.color_index == 256: cm = self.getLayerColor(dxfobj)
			elif (dxfobj.color_index == 7) and self.brightbg: cm = [0.0,0.0,0.0]
			else: cm = dxfColorMap.color_map[dxfobj.color_index]
			obj.ViewObject.LineColor = (cm[0],cm[1],cm[2],0.0)
			obj.ViewObject.LineWidth = self.lw
		elif self.paramstyle == 3:
			if dxfobj.color_index == 256:
				cm = self.table[self.getLayerColor(dxfobj,index=True)][0]
				wm = self.table[self.getLayerColor(dxfobj,index=True)][1]
			else:
				cm = self.table[dxfobj.color_index][0]
				wm = self.table[dxfobj.color_index][1]
			if cm == "object": cm = self.getLayerColor(dxfobj)
			else: obj.ViewObject.LineColor = (cm[0],cm[1],cm[2],0.0)
			if wm == "object": wm = self.lw
			else: obj.ViewObject.LineWidth = wm

	def getLayerColor(self,dxfobj,index=False):
		name = dxfobj.layer
		for table in self.dxf.tables.get_type("table"):
			if table.name == "layer":
				for l in table.get_type("layer"):
					if l.name == name:
						if index: return l.color
						else:
							if (l.color == 7) and self.brightbg: return [0.0,0.0,0.0]
							else: return dxfColorMap.color_map[l.color]

def processdxf(doc,filename):

	global drawing # for debugging - so drawing is still accessible to python after the script
	drawing = readDXF(filename)
	print "dxf: parsing file ",filename
	layers=[]

#    getting config parameters

	fmt = fcformat(drawing)

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
				if gui: fmt.formatObject(newob,line)
						
#    drawing polylines - at the moment arc segments get straightened...

	polylines = drawing.entities.get_type("lwpolyline")
	polylines.extend(drawing.entities.get_type("polyline"))
	if (len(polylines) > 0): FreeCAD.Console.PrintMessage("drawing "+str(len(polylines))+" polylines...\n")
	for polyline in polylines:
		lay=locateLayer(polyline.layer,doc,layers)
		if (len(polyline.points) > 1):
			if len(polyline.points) == 2: print "debug: 2-point polyline!"
			newob = doc.addObject("Part::Feature","Polyline")
			lay.addObject(newob)
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

			sh = Part.Wire(edges)
			newob.Shape = sh
			if gui:
				fmt.formatObject(newob,polyline)

				
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
		if gui: fmt.formatObject (newob,arc)

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
		if gui: fmt.formatObject (newob,circle)

#    drawing texts

	if fmt.paramtext:
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
				if gui:
					newob.ViewObject.FontSize=int(text.height)*12
					if paramstyle == 0: newob.ViewObject.TextColor = (r,g,b)
					elif paramstyle == 1: newob.ViewObject.TextColor = (0.0,0.0,0.0)
					elif paramstyle == 2:
						if text.color_index > 255: cm = [0.0,0.0,0.0]
						else: cm = dxfColorMap.color_map[text.color_index]
						newob.ViewObject.TextColor = (cm[0],cm[1],cm[2])
					elif paramstyle == 3:
						cm = table[text.color_index][0]
						wm = table[text.color_index][1]
						newob.ViewObject.TextColor = (cm[0],cm[1],cm[2])
					
	else: FreeCAD.Console.PrintMessage("skipping texts...\n")

	doc.recompute()
	del fmt
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
