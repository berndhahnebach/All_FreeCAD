
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

__title__="FreeCAD Draft Workbench - SVG importer/exporter"
__author__ = "Yorik van Havre <yorik@gmx.fr>"
__url__ = ["http://yorik.orgfree.com","http://free-cad.sourceforge.net"]

'''
This script imports SVG files in FreeCAD. Currently only reads the following entities:
paths, lines, arcs and rects.
Bezier curves are skipped.
'''

import xml.sax, string, FreeCAD, os, Part, math, re
from draftlibs import fcvec, fcgeo
from FreeCAD import Vector

try: import FreeCADGui
except: gui = False
else: gui = True
try: draftui = FreeCADGui.activeWorkbench().draftToolBar.ui
except: draftui = None

pythonopen = open

svgcolors = {
	  'Pink': [255, 192, 203], 
	  'Blue': [0, 0, 255], 
          'Honeydew': [240, 255, 240],
          'Purple': [128, 0, 128],
          'Fuchsia': [255, 0, 255],
          'LawnGreen': [124, 252, 0],
          'Amethyst': [153, 102, 204],
          'Crimson': [220, 20, 60],
          'White': [255, 255, 255],
          'NavajoWhite': [255, 222, 173],
          'Cornsilk': [255, 248, 220],
          'Bisque': [255, 228, 196],
          'PaleGreen': [152, 251, 152],
          'Brown': [165, 42, 42],
          'DarkTurquoise': [0, 206, 209],
          'DarkGreen': [0, 100, 0],
          'MediumOrchid': [186, 85, 211],
          'Chocolate': [210, 105, 30],
          'PapayaWhip': [255, 239, 213],
          'Olive': [128, 128, 0],
          'Silver': [192, 192, 192],
          'PeachPuff': [255, 218, 185],
          'Plum': [221, 160, 221],
          'DarkGoldenrod': [184, 134, 11],
          'SlateGrey': [112, 128, 144],
          'MintCream': [245, 255, 250],
          'CornflowerBlue': [100, 149, 237],
          'Gold': [255, 215, 0],
          'HotPink': [255, 105, 180],
          'DarkBlue': [0, 0, 139],
          'LimeGreen': [50, 205, 50],
          'DeepSkyBlue': [0, 191, 255],
          'DarkKhaki': [189, 183, 107],
          'LightGrey': [211, 211, 211],
          'Yellow': [255, 255, 0],
          'Gainsboro': [220, 220, 220],
          'MistyRose': [255, 228, 225],
          'SandyBrown': [244, 164, 96],
          'DeepPink': [255, 20, 147],
          'Magenta': [255, 0, 255],
          'AliceBlue': [240, 248, 255],
          'DarkCyan': [0, 139, 139],
          'DarkSlateGrey': [47, 79, 79],
          'GreenYellow': [173, 255, 47],
          'DarkOrchid': [153, 50, 204],
          'OliveDrab': [107, 142, 35],
          'Chartreuse': [127, 255, 0],
          'Peru': [205, 133, 63],
          'Orange': [255, 165, 0],
          'Red': [255, 0, 0],
          'Wheat': [245, 222, 179],
          'LightCyan': [224, 255, 255],
          'LightSeaGreen': [32, 178, 170],
          'BlueViolet': [138, 43, 226],
          'LightSlateGrey': [119, 136, 153],
          'Cyan': [0, 255, 255],
          'MediumPurple': [147, 112, 219],
          'MidnightBlue': [25, 25, 112],
          'FireBrick': [178, 34, 34],
          'PaleTurquoise': [175, 238, 238],
          'PaleGoldenrod': [238, 232, 170],
          'Gray': [128, 128, 128],
          'MediumSeaGreen': [60, 179, 113],
          'Moccasin': [255, 228, 181],
          'Ivory': [255, 255, 240],
          'DarkSlateBlue': [72, 61, 139],
          'Beige': [245, 245, 220],
          'Green': [0, 128, 0],
          'SlateBlue': [106, 90, 205],
          'Teal': [0, 128, 128],
          'Azure': [240, 255, 255],
          'LightSteelBlue': [176, 196, 222],
          'DimGrey': [105, 105, 105],
          'Tan': [210, 180, 140],
          'AntiqueWhite': [250, 235, 215],
          'SkyBlue': [135, 206, 235],
          'GhostWhite': [248, 248, 255],
          'MediumTurquoise': [72, 209, 204],
          'FloralWhite': [255, 250, 240],
          'LavenderBlush': [255, 240, 245],
          'SeaGreen': [46, 139, 87],
          'Lavender': [230, 230, 250],
          'BlanchedAlmond': [255, 235, 205],
          'DarkOliveGreen': [85, 107, 47],
          'DarkSeaGreen': [143, 188, 143],
          'SpringGreen': [0, 255, 127],
          'Navy': [0, 0, 128],
          'Orchid': [218, 112, 214],
          'SaddleBrown': [139, 69, 19],
          'IndianRed': [205, 92, 92],
          'Snow': [255, 250, 250],
          'SteelBlue': [70, 130, 180],
          'MediumSlateBlue': [123, 104, 238],
          'Black': [0, 0, 0],
          'LightBlue': [173, 216, 230],
          'Turquoise': [64, 224, 208],
          'MediumVioletRed': [199, 21, 133],
          'DarkViolet': [148, 0, 211],
          'DarkGray': [169, 169, 169],
          'Salmon': [250, 128, 114],
          'DarkMagenta': [139, 0, 139],
          'Tomato': [255, 99, 71],
          'WhiteSmoke': [245, 245, 245],
          'Goldenrod': [218, 165, 32],
          'MediumSpringGreen': [0, 250, 154],
          'DodgerBlue': [30, 144, 255],
          'Aqua': [0, 255, 255],
          'ForestGreen': [34, 139, 34],
          'LemonChiffon': [255, 250, 205],
          'LightSlateGray': [119, 136, 153],
          'SlateGray': [112, 128, 144],
          'LightGray': [211, 211, 211],
          'Indigo': [75, 0, 130],
          'CadetBlue': [95, 158, 160],
          'LightYellow': [255, 255, 224],
          'DarkOrange': [255, 140, 0],
          'PowderBlue': [176, 224, 230],
          'RoyalBlue': [65, 105, 225],
          'Sienna': [160, 82, 45],
          'Thistle': [216, 191, 216],
          'Lime': [0, 255, 0],
          'Seashell': [255, 245, 238],
          'DarkRed': [139, 0, 0],
          'LightSkyBlue': [135, 206, 250],
          'YellowGreen': [154, 205, 50],
          'Aquamarine': [127, 255, 212],
          'LightCoral': [240, 128, 128],
          'DarkSlateGray': [47, 79, 79],
          'Khaki': [240, 230, 140],
          'DarkGrey': [169, 169, 169],
          'BurlyWood': [222, 184, 135],
          'LightGoldenrodYellow': [250, 250, 210],
          'MediumBlue': [0, 0, 205],
          'DarkSalmon': [233, 150, 122],
          'RosyBrown': [188, 143, 143],
          'LightSalmon': [255, 160, 122],
          'PaleVioletRed': [219, 112, 147],
          'Coral': [255, 127, 80],
          'Violet': [238, 130, 238],
          'Grey': [128, 128, 128],
          'LightGreen': [144, 238, 144],
          'Linen': [250, 240, 230],
          'OrangeRed': [255, 69, 0],
          'DimGray': [105, 105, 105],
          'Maroon': [128, 0, 0],
          'LightPink': [255, 182, 193],
          'MediumAquamarine': [102, 205, 170],
          'OldLace': [253, 245, 230]
          }

def getcolor(color):
	"checks if the given string is a RGB value, or if it is a named color. returns 1-based RGBA tuple."
	if (color[:1] == "#"):
		r = float(int(color[1:3],16)/255.0)
		g = float(int(color[3:5],16)/255.0)
		b = float(int(color[5:],16)/255.0)
		return (r,g,b,0.0)
	else:
		for k,v in svgcolors.iteritems():
			if (k.lower() == color.lower()):
				r = float(v[0]/255.0)
				g = float(v[1]/255.0)
				b = float(v[2]/255.0)
				return (r,g,b,0.0)

def getsize(width):
	"extracts a number from the given string (removes suffixes)"
	if width[-1] == "%":
		return float(width[:-1])
	elif len(width) > 1:
		for s in ['pt','pc','mm','cm','in','px']:
			if width[-2:] == s:
				return float(width[:-2])
	try:
		s = float(width)
		return s
	except ValueError:
		return width

def getrgb(color):
	"returns a rgb value #000000 from a freecad color"
	r = str(hex(int(color[0]*255)))[2:].zfill(2)
	g = str(hex(int(color[1]*255)))[2:].zfill(2)
	b = str(hex(int(color[2]*255)))[2:].zfill(2)
	return "#"+r+g+b

class svgHandler(xml.sax.ContentHandler):
	"this handler parses the svg files and creates freecad objects"

	def __init__(self):
		"retrieving Draft parameters"
		params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
		self.style = params.GetInt("svgstyle")
	
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

	def format(self,obj):
		"applies styles to passed object"
		if self.style and gui:
			v = obj.ViewObject
			if self.color: v.LineColor = self.color
			if self.width: v.LineWidth = self.width
			if self.fill: v.ShapeColor = self.fill
	
	def startElement(self, name, attrs):

		# reorganizing data into a nice clean dictionary
		
		data = {}
		for (keyword,content) in attrs.items():
			content = content.replace(',',' ')
			content = content.split()
			data[keyword]=content

		if 'style' in data:
			content = data['style'][0].replace(' ','')
			content = content.split(';')
			for i in content:
				pair = i.split(':')
				if len(pair)>1:	data[pair[0]]=pair[1]

		for k in ['x','y','x1','y1','x2','y2','width','height']:
			if k in data:
				data[k] = getsize(data[k][0])

		for k in ['fill','stroke','stroke-width','font-size']:
			if k in data:
				if isinstance(data[k],list):
					data[k]=data[k][0]

		# extracting style info
			
		self.fill = None
		self.color = None
		self.width = None
		self.text = None
		self.translate = None
		
		if 'fill' in data:
			if data['fill'][0] != 'none':
				self.fill = getcolor(data['fill'])
		if 'stroke' in data:
			if data['stroke'][0] != 'none':
				self.color = getcolor(data['stroke'])
		if 'stroke-width' in data:
			if data['stroke-width'] != 'none':
				self.width = getsize(data['stroke-width'])
		if 'transform' in data:
			t = data['transform'][0]
			t = t.split('(')
			t.append(data['transform'][1].replace(')',''))
			if t[0] == "translate":
				self.translate = Vector(float(t[1]),-float(t[2]),0)

		if (self.style == 1):
			self.color = self.col
			self.width = self.lw

		# processing paths

		if name == "path":

			path = []
			point = []
			lastvec = Vector(0,0,0)
			command = None
			relative = False
			firstvec = None

			pathdata = []
			for d in data['d']:
				if (len(d) == 1) and (d in ['m','M','l','L','h','H','v','V','a','A','c','C']):
					pathdata.append(d)
				else:
					try:
						f = float(d)
						pathdata.append(f)
					except ValueError:
						if d[0].isdigit():
							pathdata.append(d[:-1])
							pathdata.append(d[-1])
						else:
							pathdata.append(d[0])
							pathdata.append(d[1:])

			for d in pathdata:
				if (d == "M"):
					command = "move"
					relative = False
					point = []
				elif (d == "m"):
					command = "move"
					relative = True
					point = []
				elif (d == "L"):
					command = "line"
					relative = False
					point = []
				elif (d == "l"):
					command = "line"
					relative = True
					point = []
				elif (d == "H"):
					command = "horizontal"
					relative = False
					point = []
				elif (d == "h"):
					command = "horizontal"
					relative = True
					point = []
				elif (d == "V"):
					command = "vertical"
					relative = False
					point = []
				elif (d == "v"):
					command = "vertical"
					relative = True
					point = []
				elif (d == "A"):
					command = "arc"
					relative = False
					point = []
				elif (d == "a"):
					command = "arc"
					relative = True
					point = []
				elif (d == "Z") or (d == "z"):
					command = "close"
					point = []
				elif (d == "C"):
					command = "curve"
					relative = False
					point = []
				elif (d == "c"):
					command = "curve"
					relative = True
					point = []
				else:
					try:
						point.append(float(d))
					except ValueError:
						pass

				print "command: ",command, ' point: ',point

				if (len(point)==2) and (command=="move"):
					if path:
						sh = Part.Wire(path)
						if self.fill: sh = Part.Face(sh)
						if self.translate: sh.translate(self.translate)
						obj = self.doc.addObject("Part::Feature","Path")
						obj.Shape = sh
						self.format(obj)
						path = []
					if relative:
						lastvec = lastvec.add(Vector(point[0],-point[1],0))
						command="line"
					else:
						lastvec = Vector(point[0],-point[1],0)
					firstvec = lastvec
					print "move ",lastvec
					command = "line"
					point = []
				elif (len(point)==2) and (command=="line"):
					if relative:
						currentvec = lastvec.add(Vector(point[0],-point[1],0))
					else:
						currentvec = Vector(point[0],-point[1],0)
					seg = Part.Line(lastvec,currentvec).toShape()
					print "line ",lastvec,currentvec
					lastvec = currentvec
					path.append(seg)
					point = []
				elif (len(point)==1) and (command=="horizontal"):
					if relative:
						currentvec = lastvec.add(Vector(point[0],0,0))
					else:
						lasty = path[-1].y
						currentvec = Vector(point[0],lasty,0)
					seg = Part.Line(lastvec,currentvec).toShape()
					lastvec = currentvec
					path.append(seg)
					point = []
				elif (len(point)==1) and (command=="vertical"):
					if relative:
						currentvec = lastvec.add(Vector(0,-point[0],0))
					else:
						lastx = path[-1].x
						currentvec = Vector(lastx,-point[0],0)
					seg = Part.Line(lastvec,currentvec).toShape()
					lastvec = currentvec
					path.append(seg)
					point = []
				elif (len(point)==7) and (command=="arc"):
					if relative:
						currentvec = lastvec.add(Vector(point[-2],-point[-1],0))
					else:
						currentvec = Vector(point[-2],-point[-1],0)
					chord = fcvec.new(lastvec,currentvec)
					perp = fcvec.crossproduct(chord)
					chord = fcvec.scale(chord,.5)
					if chord.Length > point[0]: a = 0
					else: a = math.sqrt(point[0]**2-chord.Length**2)
					s = point[0] - a
					perp = fcvec.scale(perp,s/perp.Length)
					midpoint = fcvec.add(lastvec,fcvec.add(chord,perp))
					seg = Part.Arc(lastvec,midpoint,currentvec).toShape()
					lastvec = currentvec
					path.append(seg)
					point = []
					command = None
				elif (command == "close"):
					if not fcvec.equals(lastvec,firstvec):
						seg = Part.Line(lastvec,firstvec).toShape()
						path.append(seg)
					if path:
						sh = Part.Wire(path)
						if self.fill: sh = Part.Face(sh)
						if self.translate: sh.translate(self.translate)
						obj = self.doc.addObject("Part::Feature","Path")
						obj.Shape = sh
						self.format(obj)
						path = []
					point = []
					command = None
				elif (len(point)==6) and (command=="curve"):
					if relative:
						currentvec = lastvec.add(Vector(point[4],-point[5],0))
					else:
						currentvec = Vector(point[4],-point[5],0)
					if not fcvec.equals(currentvec,lastvec):
						seg = Part.Line(lastvec,currentvec).toShape()
						print "connect ",lastvec,currentvec
						lastvec = currentvec
						path.append(seg)
					point = []

			if path:
				sh = Part.Wire(path)
				if self.fill: sh = Part.Face(sh)
				if self.translate: sh.translate(self.translate)
				obj = self.doc.addObject("Part::Feature","Path")
				obj.Shape = sh
				self.format(obj)

		# processing rects

		if name == "rect":
			p1 = Vector(data['x'],-data['y'],0)
			p2 = Vector(data['x']+data['width'],-data['y'],0)
			p3 = Vector(data['x']+data['width'],-data['y']-data['height'],0)
			p4 = Vector(data['x'],-data['y']-data['height'],0)
			edges = []
			edges.append(Part.Line(p1,p2).toShape())
			edges.append(Part.Line(p2,p3).toShape())
			edges.append(Part.Line(p3,p4).toShape())
			edges.append(Part.Line(p4,p1).toShape())
			sh = Part.Wire(edges)
			if self.fill: sh = Part.Face(sh)
			if self.translate: sh.translate(self.translate)
			obj = self.doc.addObject("Part::Feature","Rectangle")
			obj.Shape = sh
			self.format(obj)
				     
#		processing lines

		if name == "line":
			p1 = Vector(data['x1'],-data['y1'],0)
			p2 = Vector(data['x2'],-data['y2'],0)
			sh = Part.Line(p1,p2).toShape()
			if self.translate: sh.translate(self.translate)
			obj = self.doc.addObject("Part::Feature","Line")
			obj.Shape = sh
			self.format(obj)

		if (name == "text") or (name == "tspan"):
			self.x = data['x']
			self.y = data['y']
			if 'font-size' in data:
				if data['font-size'] != 'none':
					self.text = getsize(data['font-size'])

	def characters(self,content):
		if self.text:
			obj=self.doc.addObject("App::Annotation","Text")
			obj.LabelText = content.encode('latin1')
			obj.Position = Vector(self.x,-self.y,0)
			if self.translate: obj.Position = obj.Position.add(self.translate)
			if gui:
				obj.ViewObject.FontSize = int(self.text)
				if self.fill: obj.ViewObject.TextColor = self.fill
				else: obj.ViewObject.TextColor = (0.0,0.0,0.0,0.0)
			self.text = None
			
			
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

def getContents(filename,tag):
        "gets the contents of all the occurences of the given tag in the given file"
        result = {}
        f = pythonopen(filename)
        contents = ''
        for line in f: contents += line
        contents = contents.replace('\n','_linebreak')
        f.close()
        searchpat = '<'+tag+'.*?</'+tag+'>'
        tags = re.findall(searchpat,contents)
        for t in tags:
                tagid = re.findall('id="(.*?)"',t)
                if tagid:
                        tagid = tagid[0]
                else:
                        tagid = 'none'
                res = t.replace('_linebreak','\n')
                result[tagid] = res
        return result

def open(filename):
	docname=os.path.split(filename)[1]
	doc=FreeCAD.newDocument(docname)
	doc.Label = decodeName(docname[:-4])
	parser = xml.sax.make_parser()
	parser.setContentHandler(svgHandler())
	parser._cont_handler.doc = doc
	parser.parse(pythonopen(filename))
	doc.recompute()

def insert(filename,docname):
	doc=FreeCAD.getDocument(docname)
	parser = xml.sax.make_parser()
	parser.setContentHandler(svgHandler())
	parser._cont_handler.doc = doc
	parser.parse(pythonopen(filename))
	doc.recompute()

def export(exportList,filename):
	"called when freecad exports a file"

	# finding sheet size
	minx = 10000
	miny = 10000
	maxx = 0
	maxy = 0
	for ob in exportList:
		if isinstance(ob,Part.Feature):
			for v in ob.Shape.Vertexes:
				if v.Point.x < minx: minx = v.Point.x
				if v.Point.x > maxx: maxx = v.Point.x
				if v.Point.y < miny: miny = v.Point.y
				if v.Point.y > maxy: maxy = v.Point.y
	margin = (maxx-minx)*.01
	minx -= margin 
	maxx += margin
	miny -= margin
	maxy += margin
	sizex = maxx-minx
	sizey = maxy-miny
	miny += margin
	boty = sizey+miny

	# writing header
	svg = pythonopen(filename,'wb')	
	svg.write('<?xml version="1.0"?>\n')
	svg.write('<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"')
	svg.write(' "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">\n')
	svg.write('<svg')
	svg.write(' width="' + str(sizex) + '" height="' + str(sizey) + '"')
	svg.write(' viewBox="0 0 ' + str(sizex) + ' ' + str(sizey) + '"')
	svg.write(' xmlns="http://www.w3.org/2000/svg" version="1.1"')
	svg.write('>\n')

	# writing paths
	for ob in exportList:
		if isinstance(ob,Part.Feature):
			fill = 'none'
			stroke = 'none'
			width = 'none'
			name = ob.Name
			if ob.Shape.Faces:
				if gui: fill = getrgb(ob.ViewObject.ShapeColor)
				else: fill = '#000000'
			if gui:
				stroke = getrgb(ob.ViewObject.LineColor)
				width = ob.ViewObject.LineWidth/100
			if len(ob.Shape.Vertexes) > 1:
				svg.write('<path id="' + name + '" ')
				edges = fcgeo.sortEdges(ob.Shape.Edges)
				v = edges[0].Vertexes[0].Point
				svg.write('d="M '+ str(v.x-minx) +' '+ str(-v.y+boty) + ' ')
				for e in edges:
					if isinstance(e.Curve,Part.Line):
						v = e.Vertexes[-1].Point
						svg.write('L '+ str(v.x-minx) +' '+ str(-v.y+boty) + ' ')
					elif isinstance(e.Curve,Part.Circle):
						r = e.Curve.Radius
						v = e.Vertexes[-1].Point
						svg.write('A '+ str(r) + ' '+ str(r) +' 0 0 0 '+ str(v.x-minx) +' ')
						svg.write(str(-v.y+boty) + ' ')
				if fill != "none":
					svg.write('Z')
				svg.write('" ')
			else:
				cen = ob.Shape.Edges[0].Curve.Center
				rad = ob.Shape.Edges[0].Curve.Radius
				svg.write('<circle cx="'+str(cen.x-minx))
				svg.write('" cy="'+str(-cen.y+boty))
				svg.write('" r="'+str(rad)+'" ')
			svg.write('fill="' + fill + '" stroke="' + stroke + '" ')
			svg.write('stroke-width="' + str(width) + ' px" ')
			svg.write('style="stroke-width:'+ str(width))
			svg.write(';stroke-miterlimit:4;stroke-dasharray:none"')
			svg.write('/>\n')
					
		elif (ob.Type == "App::Annotation"):
			fill = getrgb(ob.ViewObject.TextColor)
			height = str(ob.ViewObject.FontSize)
			x = str(ob.Position.x-minx)
			y = str(-ob.Position.y+boty)
			name = ob.Name
			svg.write('<text id="' + name + '" fill="'+ fill +'" font-size="'+ height)
			svg.write('" x="' + x + '" y="' + y + '">\n')
			for line in ob.LabelText:
				svg.write(line)
			svg.write('</text>\n')

		elif (ob.Type == "App::FeaturePython"):
			if 'Dimline' in ob.PropertiesList:
				stroke = 'none'
				width = 'none'
				name = ob.Name
				height = 0.2
				dmax = 0
				if gui:
					stroke = getrgb(ob.ViewObject.LineColor)
					width = ob.ViewObject.LineWidth/100
					height = ob.ViewObject.FontSize
					dmax = ob.ViewObject.ExtLines
				p1 = ob.Start
				p4 = ob.End
				base = Part.Line(p1,p4).toShape()
				proj = fcgeo.findDistance(ob.Dimline,base)
				if not proj:
					p2 = p1
					p3 = p4
				else:
					p2 = p1.add(fcvec.neg(proj))
					p3 = p4.add(fcvec.neg(proj))
					if dmax and (proj.Length > dmax):
						p1 = p2.add(fcvec.scale(proj.normalize(),dmax))
						p4 = p3.add(fcvec.scale(proj.normalize(),dmax))
				midpoint = p2.add(fcvec.scale(p3.sub(p2),0.5))
				angle = fcvec.angle(p3.sub(p2))
				if (angle > math.pi/2) or (angle < -math.pi/2): angle = angle+math.pi
				offset = fcvec.rotate(FreeCAD.Vector(height*.2,0,0),-angle+math.pi/2)
				tbase = midpoint.add(offset)
				svg.write('<g id="'+name+'"> ')
				svg.write('<path ')
				svg.write('d="M '+str(p1.x-minx)+' '+str(-p1.y+boty)+' ')
				svg.write('L '+str(p2.x-minx)+' '+str(-p2.y+boty)+' ')
				svg.write('L '+str(p3.x-minx)+' '+str(-p3.y+boty)+' ')
				svg.write('L '+str(p4.x-minx)+' '+str(-p4.y+boty)+'" ')
				svg.write('fill="none" stroke="' + stroke + '" ')
				svg.write('stroke-width="' + str(width) + ' px" ')
				svg.write('style="stroke-width:'+ str(width))
				svg.write(';stroke-miterlimit:4;stroke-dasharray:none"')
				svg.write('/>\n')
				svg.write('<circle cx="'+str(p2.x-minx))
				svg.write('" cy="'+str(-p2.y+boty))
				svg.write('" r="'+str(height/10)+'" ')
				svg.write('fill="'+ stroke +'" stroke="none" ')
				svg.write('style="stroke-miterlimit:4;stroke-dasharray:none"/>\n')
				svg.write('<circle cx="'+str(p3.x-minx))
				svg.write('" cy="'+str(-p3.y+boty))
				svg.write('" r="'+str(height/10)+'" ')
				svg.write('fill="#000000" stroke="none" ')
				svg.write('style="stroke-miterlimit:4;stroke-dasharray:none"/>\n')
				svg.write('<text id="' + name + '" fill="'+ stroke +'" font-size="'+ str(height)+'" ')
				svg.write('style="text-anchor:middle;text-align:center" ')
				svg.write('transform="rotate('+str(math.degrees(angle)))
				svg.write(' '+str(tbase.x-minx)+' '+str(-tbase.y+boty)+')" ')
				svg.write('x="' + str(tbase.x-minx) + '" y="' + str(-tbase.y+boty) + '">\n')
				svg.write("%.2f" % p3.sub(p2).Length)
				svg.write('</text>\n')
				svg.write('</g>\n')


	# closing
	svg.write('</svg>')
	svg.close()
	FreeCAD.Console.PrintMessage("successfully exported "+filename)
