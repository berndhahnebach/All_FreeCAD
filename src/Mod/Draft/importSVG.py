
#***************************************************************************
#*   FreeCAD Draft Workbench -                                             *
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


#    This script imports SVG files in FreeCAD. Currently only path (M, L, C and A commands)
#    and rect types are read, and any curved segment gets flattened...
#    All this will be fixed and enhanced later on...

# the following line is needed for registering this script in freecad io system
filetype="SVG file as geometry (*.svg)"

import xml.sax, string, FreeCAD, FreeCADGui, os

plines=[]
pythonopen = open
class svgHandler(xml.sax.ContentHandler):

	def startElement(self, name, attrs):
	
#		processing paths
	
		if name == "path":
			for (k,v) in attrs.items():
				if k == "d":
					organized=v.split()
					currentpline=[]
					currentpoint=[]
					nextline=""
					for i in organized:
						if i == "M" or i == "m":
							if currentpline != []:
								plines.append(currentpline)
								currentpline=[]
							nextline="normalpoint"
						elif i == "":
							pass
						elif i == "L" or i == "l":
							nextline="normalpoint"
						elif i == "C" or i == "c":
							pass
						elif i == "A" or i == "a":
							nextline="arcfirstline"
						elif i == "Z" or i == "z":
							currentpline.append(currentpline[0])
						elif nextline=="arcfirstline":
							if i.find(',')==-1:
								nextline="arcsecondline"
							else:
								nextline="arcthirdline"
						elif nextline=="arcsecondline":
							nextline="arcthirdline"
						elif nextline=="arcthirdline":
							nextline="arcfourthline"
						elif nextline=="arcfourthline":
							nextline="arcfifthline"
						elif nextline=="arcfifthline":
							nextline="normalpoint"
						elif nextline=="normalpoint":
							if i.find(',')==-1:
								nextline="secondcoord"
								currentpoint=[]
								currentpoint.append(i)
							else: 
								currentpoint=(i.split(','))
								currentpline.append(currentpoint)
						elif nextline=="secondcoord":
								currentpoint.append(i)
								currentpline.append(currentpoint)
								nextline=="normalpoint"
					if currentpline !=[]:
						plines.append(currentpline)
						currentpline=[]
					
#		processing paths
	
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
			plines.append(currentpline)
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
			plines.append(currentpline)
			currentpline=[]


# main course
			
def open(filename):
	global plines
	plines=[]		
	parser = xml.sax.make_parser()
	parser.setContentHandler(svgHandler())
	parser.parse(pythonopen(filename))

# integrity test against BAD formated svg!!! Another time I'll do a routine to fix it before processing

	for i in plines:
		for j in i:
			for k in j:
				try:
					k = float(k)
				except:
					print "found bad formatting in point", j
					print "operation aborted. Please fix the svg file manually"
					raise
	
# drawing FreeCAD objects

	docname=os.path.split(filename)[1]
	doc=FreeCAD.newDocument(docname)
	for polyline in plines:
		if (len(polyline.points) > 1):
			newob = doc.addObject("Part::Feature","Path")
			points = []
			for p in polyline:
				v=FreeCAD.Vector(float(p[0]),-float(p[1]),0.0)
				points.append(v)
			if (len(points)>1):
				sh = Part.makeLine((points[0].x,points[0].y,points[0].z),(points[1].x,points[1].y,points[1].z))
				for i in range(1,len(points)-1):
					newseg = Part.makeLine((points[i].x,points[i].y,points[i].z),(points[i+1].x,points[i+1].y,points[i+1].z))
					sh = sh.fuse(newseg)
			if (polyline.closed == 1):
				l=len(points)-1
				newseg = Part.makeLine((points[l].x,points[l].y,points[l].z),(points[0].x,points[0].y,points[0].z))
				sh = sh.fuse(newseg)
			newob.Shape = sh
	doc.recompute()

