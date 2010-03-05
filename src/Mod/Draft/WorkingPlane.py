#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2009, 2010                                              *
#*   Ken Cline <cline@frii.com>                                            *
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


import FreeCAD, FreeCADGui, Part, math
from FreeCAD import Vector
from draftlibs import fcvec

__title__="FreeCAD Working Plane utility"
__author__ = "Ken Cline"
__url__ = "http://free-cad.sourceforge.net"

'''
This module provides a class called plane to assist in selecting and maintaining a working plane.
'''


class plane:
	'''class for keping track of working plane.'''

	def __init__(self):
		# keep track of active document.  Reset view when doc changes.
		self.doc = None
		# u, v axes and position define plane, perpendicular axis is handy, though redundant.
		self.u = None
		self.v = None
		self.axis = None
		self.position = None

	def offsetToPoint(self, p, direction=None):
		'''
		Return the signed distance from p to the plane, such
		that p + offsetToPoint(p)*direction lies on the plane.
		direction defaults to -plane.axis
		'''

		'''
		A picture will help explain the computation:

                                                            p
                                                          //|
                                                        / / |
                              			      /  /  |
						    /  	/   |
						  /    /    |
		-------------------- plane -----c-----x-----a--------

                Here p is the specified point,
                     c is a point (in this case plane.position) on the plane
                     x is the intercept on the plane from p in the specified direction, and
                     a is the perpendicular intercept on the plane (i.e. along plane.axis)

	        Using vertival bars to denote the length operator,
                     |ap| = |cp| * cos(apc) = |xp| * cos(apx)
                so
                     |xp| = |cp| * cos(apc) / cos(apx)
                          = (cp . axis) / (direction . axis)
		'''
		if direction == None: direction = self.axis
		return self.axis.dot(self.position.sub(p))/self.axis.dot(direction)

	def projectPoint(self, p, direction=None):
		'''project point onto plane, default direction is orthogonal'''
		if not direction: direction = self.axis
		t = Vector(direction)
		t.multiply(self.offsetToPoint(p, direction))
		return p.add(t)

	def alignToPointAndAxis(self, point, axis, offset):
		self.doc = FreeCAD.ActiveDocument
		self.axis = axis;
		self.axis.normalize()
		if fcvec.equals(axis, Vector(1,0,0)):
			self.u = Vector(0,1,0)
			self.v = Vector(0,0,1)
		else:
			self.v = axis.cross(Vector(1,0,0))
			self.v.normalize()
			self.u = fcvec.rotate(self.v, -math.pi/2, self.axis)
		offsetVector = Vector(axis); offsetVector.multiply(offset)
		self.position = point.add(offsetVector)
		FreeCAD.Console.PrintMessage("(position = " + str(self.position) + ")\n")
		FreeCAD.Console.PrintMessage("Plane selected.\n")

	def alignToCurve(self, shape, offset):
		if shape.ShapeType == 'Edge':
			#??? TODO: process curve here.  look at shape.edges[0].Curve
			return False
		elif shape.ShapeType == 'Wire':
			#??? TODO: determine if edges define a plane
			return False
		else:
			return False

	def alignToFace(self, shape, offset):
		# Set face to the unique selected face, if found
		if shape.ShapeType == 'Face':
			#we should really use face.tangentAt to get u and v here, and implement alignToUVPoint
			self.alignToPointAndAxis(shape.Faces[0].CenterOfMass, shape.Faces[0].normalAt(0,0), offset)
			return True
		else:
			return False

	def alignToSelection(self, offset):
		'''If selection uniquely defines a plane, align working plane to it.  Return success (bool)'''
		sex = FreeCADGui.Selection.getSelectionEx(FreeCAD.ActiveDocument.Name)
		if len(sex) == 0:
			return False
		elif len(sex) == 1:
			return self.alignToCurve(sex[0].Object.Shape, offset) \
				or self.alignToFace(sex[0].Object.Shape, offset) \
				or (len(sex[0].SubObjects) == 1 and self.alignToFace(sex[0].SubObjects[0], offset))
		else:
			# len(sex) > 2, look for point and line, three points, etc.
			return False

	def setup(self, direction, point):
		'''If working plane is undefined, define it!'''
		if self.doc != FreeCAD.ActiveDocument:
			self.alignToPointAndAxis(point, direction)

	def reset():
		self.doc = None
		self.u, self.v, self.axis, self.position = \
			None, None, None, None
