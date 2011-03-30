"""
An example for a high-level cutsom feature object to form a full-parametric parallelepiped.

***************************************************************************
*   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
*                                                                         *
*   This file is part of the FreeCAD CAx development system.              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License (GPL)            *
*   as published by the Free Software Foundation; either version 2 of     *
*   the License, or (at your option) any later version.                   *
*   for detail see the LICENCE text file.                                 *
*                                                                         *
*   FreeCAD is distributed in the hope that it will be useful,            *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU Library General Public License for more details.                  *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with FreeCAD; if not, write to the Free Software        *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
*   USA                                                                   *
*                                                                         *
***************************************************************************
"""

__author__ = "Werner Mayer <wmayer@users.sourceforge.net>"

import FreeCAD, Part, math
from FreeCAD import Base

class Parallelepiped:
	def __init__(self, obj):
		''' Add the properties: Length, Edges, Radius, Height '''
		obj.addProperty("App::PropertyVector","A","Parallelepiped","Vector").A=Base.Vector(1,0,0)
		obj.addProperty("App::PropertyVector","B","Parallelepiped","Vector").B=Base.Vector(0,1,0)
		obj.addProperty("App::PropertyVector","C","Parallelepiped","Vector").C=Base.Vector(0,0,1)
		obj.Proxy = self

	def onChanged(self, fp, prop):
		if prop == "A" or prop == "B" or prop == "C":
			self.execute(fp)

	def execute(self, fp):
		a = fp.A
		b = fp.B
		c = fp.C

		m=Base.Matrix()
		m.A11=a.x
		m.A12=a.y
		m.A13=a.z
		m.A21=b.x
		m.A22=b.y
		m.A23=b.z
		m.A31=c.x
		m.A32=c.y
		m.A33=c.z
		box = Part.makeBox(1,1,1)
		fp.Shape = box.transformGeometry(m)

def makeParallelepiped():
	doc = FreeCAD.activeDocument()
	if doc == None:
		doc = FreeCAD.newDocument()
	obj=doc.addObject("Part::FeaturePython","Parallelepiped")
	obj.Label = "Parallelepiped"
	Parallelepiped(obj)
	obj.ViewObject.Proxy=0

