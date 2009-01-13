
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
    This script imports SVG files in FreeCAD. Currently only reads the following entities:
    path containing lines, arcs, and simpe bezier curves (quadratic & smoothed not supported)
    rect
    line
'''

import xml.sax, string, FreeCAD, FreeCADGui, os, Part
from draftlibs import fcvec

vec = FreeCAD.Vector
pythonopen = open

class svgHandler(xml.sax.ContentHandler):

	def __init__(self):
		self.result = []
	
	def startElement(self, name, attrs):

		# processing paths

		if name == "path":
			for (keyword,data) in attrs.items():
				if keyword == "d":
					data = data.replace(',',' ')
					data = data.split()
					path = []
					point = []
					lastvec = None
					command = None
					for d in data:
						if (d[0] == "M"):
							command = "move"
							point = []
							if len(d)>1:
								d.pop[0]
								point.append(d)
						elif (d[0] == "L"):
							command = "line"
							point = []
							if len(d)>1:
								d.pop[0]
								point.append(d)
						elif (d[0] == "H"):
							command = "horizontal"
							point = []
							if len(d)>1:
								d.pop[0]
								point.append(d)
						elif (d[0] == "V"):
							command = "vertical"
							point = []
							if len(d)>1:
								d.pop[0]
								point.append(d)
						elif (d[0] == "A"):
							command = "arc"
							point = []
							if len(d)>1:
								d.pop[0]
								point.append(d)
						elif (d[0] == "Z"):
							command = "close"
							point = []
							if len(d)>1:
								d.pop[0]
								point.append(d)
						elif (d == ""):
							pass
						else:
							point.append(d)

						if (len(point)==2) and (command=="move"):
							if path:
								self.result.append(path)
								path = []
							lastvec = (vec(point[0],point[1],0))
							command = None
						elif (len(point)==2) and (command=="line"):
							seg = {}
							currentvec = vec(point[0],point[1],0)
							seg['type'] = 'line'
							seg['data'] = []
							seg['data'].append(lastvec)
							seg['data'].append(currentvec)
							lastvec = currentvec
							path.append(seg)
							command = None
						elif (len(point)==1) and (command=="horizontal"):
							seg = {}
							lasty = path[-1].y
							currentvec = vec(point[0],lasty,0)
							seg['type'] = 'line'
							seg['data'] = []
							seg['data'].append(lastvec)
							seg['data'].append(currentvec)
							lastvec = currentvec
							path.append(seg)
							command = None
						elif (len(point)==1) and (command=="vertical"):
							seg = {}
							lastx = path[-1].x
							currentvec = vec(lastx,point[0],0)
							seg['type'] = 'line'
							seg['data'] = []
							seg['data'].append(lastvec)
							seg['data'].append(currentvec)
							lastvec = currentvec
							path.append(seg)
							command = None
						elif (len(point)==


		# processing rects

		if name == "rect":
			for (k,v) in attrs.items():
				if k == "x":
					rectx=v
				if k == "y":
					recty=v
				if k == "width":
					rectw=v
				if k == "height":
					recth=v
			currentpline=[]
			currentpline.append([rectx,recty])
			currentpline.append([float(rectx)+float(rectw),recty])
			currentpline.append([float(rectx)+float(rectw),float(recty)+float(recth)])
			currentpline.append([rectx,float(recty)+float(recth)])
			currentpline.append([rectx,recty])
			self.result.append(currentpline)
			currentpline=[]

#		processing lines

		if name == "line":
			for (k,v) in attrs.items():
				if k == "x1":
					x1 = v
				if k == "y1":
					y1 = v
				if k == "x2":
					x2 = v
				if k == "y2":
					y2 = v
			currentpline=[]
			currentpline.append([x1,y1])
			currentpline.append([x2,y2])
			self.result.append(currentpline)
			currentpline=[]

def open(filename):

	global parser
	parser = xml.sax.make_parser()
	parser.setContentHandler(svgHandler())
	parser.parse(pythonopen(filename))

	result = parser._cont_handler.result

	# drawing FreeCAD objects

	docname=os.path.split(filename)[1]
	doc=FreeCAD.newDocument(docname)
	for polyline in result:
		edges = []
		if (len(polyline) > 1):
			newob = doc.addObject("Part::Feature","Path")
			points = []
			for p in polyline:
				v=FreeCAD.Vector(float(p[0]),-float(p[1]),0.0)
				points.append(v)
			if (len(points)>1):
				for i in range(0,len(points)-1):
					if not fcvec.equals(points[i],points[i+1]):
						edges.append(Part.Line(points[i],points[i+1]).toShape())
					else: print "debug: skipped one null segment"
			sh = Part.Wire(edges)
			newob.Shape = sh
	doc.recompute()
