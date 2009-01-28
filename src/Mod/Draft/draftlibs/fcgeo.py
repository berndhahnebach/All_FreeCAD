
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


"this file contains generic geometry functions for manipulating Part shapes"

import FreeCAD, Part, fcvec, math, cmath
from FreeCAD import Vector

NORM = Vector(0,0,1) # provisory normal direction for all geometry ops.
PREC = 4 # Precision of comparisions (decimal places/digits).

def vec(edge):
	"vec(edge) or vec(line) -- returns a vector from an edge or a Part.line"
	if isinstance(edge,Part.Shape):
		return fcvec.new(edge.Vertexes[0].Point,edge.Vertexes[-1].Point)
	elif isinstance(edge,Part.Line):
		return fcvec.new(edge.StartPoint,edge.EndPoint)
	else:
		return None

def findIntersection(edge1,edge2,infinite1=False,infinite2=False):
	'''
	findIntersection(edge,edge,[infinite1],[infinite2])
	finds intersection point between 2 edges, if any. Currently only works in 2D
	if infinite1 is True, edge1 will be considered infinite.
	if infinite2 is True, edge2 will be considered infinite.
	algorithm from http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
	'''

	if (isinstance(edge1.Curve,Part.Line)) and (isinstance(edge2.Curve,Part.Line)):
		# intersection of two lines
		print "debug: 2 lines"
		p1 = edge1.Vertexes[0].Point
		p2 = edge1.Vertexes[1].Point
		p3 = edge2.Vertexes[0].Point
		p4 = edge2.Vertexes[1].Point
		int = fcvec.intersect(p1, p2, p3, p4, infinite1, infinite2)
		if int:
			return [int]
		else:
			return None

	elif (isinstance(edge1.Curve,Part.Circle)) and (isinstance(edge2.Curve,Part.Circle)):
		# intersection of two curves
		print "debug: 2 curves"
		c1 = edge1.Curve.Center
		c2 = edge2.Curve.Center
		r1 = edge1.Curve.Radius
		r2 = edge2.Curve.Radius
		d = fcvec.new(c1, c2)
		dist = d.Length
		if (round(dist, PREC) > round(r1 + r2, PREC)):
			print "debug: no intersection"
			#print "debug: distance: " + str(dist) + " r1: " + str(r1) + " r2: " + str(r2) + "\n"
			return None # No intersection
		elif (round(dist, PREC) == 0):
			print "debug: circles in the same place"
			return None # Circles are in the same place.
		elif (round(dist, PREC) < round(abs(r1 - r2), PREC)):
			print "debug: one circle inside the other"
			return None # one circle is inside the other
		elif (round(dist, PREC) == round(r1 + r2, PREC)):
			print "debug: one int point"
			return [fcvec.scale(fcvec.normalized(d),r1)]
		else:
			print "debug: 2 int points"
			a = (r1**2 - r2**2 + dist**2) / (2.0 * dist)
			p2 = Vector(c1.x + (d.x * a / dist), c1.y + (d.y * a / dist), c1.z)
			h = math.sqrt(r1**2 - a**2)
			rx = -d.y * (h / dist)
			ry = d.x* (h / dist)
			intpnts = []
			intpnts.append(Vector(p2.x+rx, p2.y+ry, c1.z))
			intpnts.append(Vector(p2.x-rx, p2.y-ry, c1.z))
			return intpnts

	elif ((isinstance(edge1.Curve,Part.Line) and isinstance(edge2.Curve,Part.Circle))
	or (isinstance(edge1.Curve,Part.Circle) and isinstance(edge2.Curve,Part.Line))):
		# intersection of one curve and one line
		print "debug: 1 line & 1 curve"
		if (isinstance(edge1.Curve,Part.Circle)):
			c = edge1.Curve.Center
			r = edge1.Curve.Radius
			p1 = edge2.Vertexes[0].Point
			p2 = edge2.Vertexes[1].Point
		else:
			c = edge2.Curve.Center
			r = edge2.Curve.Radius
			p1 = edge1.Vertexes[0].Point
			p2 = edge1.Vertexes[1].Point

		intpnts = []
		num = (c.x - p1.x) * (p2.x - p1.x) + (c.y - p1.y) * (p2.y - p1.y)
		denom = (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y)
		if denom == 0:
			return

		u = num / denom
		xp = p1.x + u * (p2.x - p1.x)
		yp = p1.y + u * (p2.y - p1.y)
		a = (p2.x - p1.x)**2 + (p2.y - p1.y)**2
		b = 2 * ((p2.x - p1.x) * (p1.x - c.x) + (p2.y - p1.y) * (p1.y - c.y))
		c = c.x**2 + c.y**2 + p1.x**2 + p1.y**2 - 2 * (c.x * p1.x + c.y * p1.y) - r**2
		q = b**2 - 4 * a * c
		if q == 0:
			intpnts.append(Vector(xp,yp,p1.z))
		elif q:
			u1 = (-b + math.sqrt(abs(q))) / (2 * a)
			u2 = (-b - math.sqrt(abs(q))) / (2 * a)
			intpnts.append(Vector((p1.x + u1 * (p2.x - p1.x)), (p1.y + u1 * (p2.y - p1.y)), p1.z))
			intpnts.append(Vector((p1.x + u2 * (p2.x - p1.x)), (p1.y + u2 * (p2.y - p1.y)), p1.z))
		return intpnts

	else:
		print "debug: findIntersection bad parameters!\n"
		return None


def findReflection (edge, point):
	'''
	Returns the reflection (reflexion / mirrored point) of a given point.
	The given edge is used as the axis of reflection.
	See also: http://en.wikipedia.org/wiki/Reflection_(mathematics)

	@todo Support different reflection types (e.g. reflect through a point -> http://en.wikipedia.org/wiki/Point_reflection)
	'''

	if isinstance(edge.Curve, Part.Line) and isinstance(point, FreeCAD.Vector):
		# Project the given point onto the edge:
		normPoint = findProjection(point, edge, False)

		if normPoint:
			normPoint_point = Vector.sub(point, normPoint)

			# Calculate the vector from the projected normal point to the new reflected point
			# i.e. the inverted vector from the projected normal point to the original point.
			normPoint_refl = fcvec.neg(normPoint_point)
			refl = Vector.add(normPoint, normPoint_refl)
			return refl
		else:
			return None
	else:
		print "debug: findReflection bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: findReflection bad parameters!\n")
		return None


def findClosest(basepoint,pointslist):
	'''
	findClosest(vector,list)
	in a list of 3d points, finds the closest point to the base point.
	an index from the list is returned.
	'''
	if not pointslist: return None
	smallest = 100000
	for n in range(len(pointslist)):
		new = fcvec.new(basepoint,pointslist[n]).Length
		if new < smallest:
			smallest = new
			npoint = n
	return npoint

def concatenate(shape):
	"concatenate(shape) -- turns several faces into one"
	edges = getBoundary(shape)
	edges = sortEdges(edges)
	try:
		wire=Part.Wire(edges)
		face=Part.Face(wire)
	except:
		print "error: Couldn't join faces into one"
		return(shape)
	else:
		if not wire.isClosed():	return(wire)
		else: return(face)

def getBoundary(shape):
	"getBoundary(shape) -- this function returns the boundary edges of a group of faces"
        # make a lookup-table where we get the number of occurrences
	# to each edge in the fused face
	lut={}
	for f in shape.Faces:
		for e in f.Edges:
			hc= e.hashCode()
			if lut.has_key(hc): lut[hc]=lut[hc]+1
			else: lut[hc]=1
	# filter out the edges shared by more than one sub-face
	bound=[]
	for e in shape.Edges:
		if lut[e.hashCode()] == 1: bound.append(e)
	return bound

def sortEdges(edgeslist):
	"this function sorts edges so they are all in following order and in the right direction"
	edgeslist = Part.__sortEdges__(edgeslist)
	if (len(edgeslist) < 2): return edgeslist
	edges = []
	for e in range(len(edgeslist)-1):
		edge = edgeslist[e]
		next = edgeslist[e+1]
		first = edge.Vertexes[0].Point
		last = edge.Vertexes[-1].Point
		if (fcvec.equals(last,next.Vertexes[0].Point)) or (fcvec.equals(last,next.Vertexes[-1].Point)):
			edges.append(edge)
		else:
			if (isinstance(edge.Curve,Part.Line)):
				edges.append(Part.Line(last,first).toShape())
			else:
				edges.append(Part.Arc(last,findMidpoint(edge),first).toShape())
	edge = edgeslist[-1]
	first = edge.Vertexes[0].Point
	last = edge.Vertexes[-1].Point
	if (fcvec.equals(first,edgeslist[-2].Vertexes[-1].Point)):
		edges.append(edge)
	else:
		if (isinstance(edge.Curve,Part.Line)):
			edges.append(Part.Line(last,first).toShape())
		else:
			edges.append(Part.Arc(last,findMidpoint(edge),first).toShape())
	return edges

def findMidpoint(edge):
	"calculates the midpoint of an edge"
	first = edge.Vertexes[0].Point
	last = edge.Vertexes[-1].Point
	if isinstance(edge.Curve,Part.Circle):
		center = edge.Curve.Center
		radius = edge.Curve.Radius
		chord = fcvec.new(first,last)
		perp = fcvec.crossproduct(chord)
		ray = fcvec.new(center,first)
		apothem = fcvec.project(ray,perp)
		sagitta = radius - apothem.Length
		startpoint = Vector.add(first, fcvec.scale(chord,0.5))
		endpoint = fcvec.scale(fcvec.normalized(perp),sagitta)
		return Vector.add(startpoint,endpoint)

	elif isinstance(edge.Curve,Part.Line):
		halfedge = fcvec.scale(fcvec.new(first,last),.5)
		return Vector.add(first,halfedge)

	else:
		return None

def complexity(obj):
	'''
	tests given object for shape complexity:
	1: line
	2: arc
	3: circle
	4: open wire with no arc
	5: closed wire
	6: wire with arcs
	7: faces
	8: faces with arcs
	'''
	shape = obj.Shape
	if shape.Faces:
		for e in shape.Edges:
			if (isinstance(e.Curve,Part.Circle)): return 8
		return 7
	if shape.Wires:
		for e in shape.Edges:
			if (isinstance(e.Curve,Part.Circle)): return 6
		for w in shape.Wires:
			if w.isClosed(): return 5
		return 4
	if (isinstance(shape.Edges[0].Curve,Part.Circle)):
		if len(shape.Vertexes) == 1:
			return 3
		return 2
	return 1

def findPerpendicular(point,edgeslist,force=None):
	'''
	findPerpendicular(vector,wire,[force]):
	finds the shortest perpendicular distance between a point and an edgeslist.
	If force is specified, only the edge[force] will be considered, and it will be
	considered infinite.
	The function will return a list	[vector_from_point_to_closest_edge,edge_index]
	or None if no perpendicular vector could be found.
	'''
	if (force == None):
		valid = None
		for edge in edgeslist:
			dist = findDistance(point,edge,strict=True)
			if dist:
				if not valid: valid = [dist,edgeslist.index(edge)]
				else:
					if (dist.Length < valid[0].Length):
						valid = [dist,edgeslist.index(edge)]

		return valid
	else:
		edge = edgeslist[force]
		dist = findDistance(point,edge)
		if dist: return [dist,force]
		else: return None

def offset(edge,vector):
	'''
	offset(edge,vector)
	returns a copy of the edge at a certain (vector) distance
	if the edge is an arc, the vector will be added at its first point
	and a complete circle will be returned
	'''
	if isinstance(edge.Curve,Part.Line):
		v1 = Vector.add(edge.Vertexes[0].Point, vector)
		v2 = Vector.add(edge.Vertexes[-1].Point, vector)
		return Part.Line(v1,v2).toShape()
	else:
		rad = fcvec.new(edge.Curve.Center,edge.Vertexes[0].Point)
		newrad = Vector.add(rad,vector).Length
		return Part.Circle(edge.Curve.Center,NORM,newrad).toShape()
		

def determinant (mat,n):
	'''
	determinant(matrix,int)
	Determinat function. Calculates the determinant of a n-matrix.
	It recursively expands the minors.
	@todo Use existing library function instead
	'''
	matTemp = [[0.0,0.0,0.0],[0.0,0.0,0.0],[0.0,0.0,0.0]]
	if (n > 1):
		if n == 2:
			d = mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]
		else:
			d = 0.0
			for j1 in range(n):
				# Create minor
				for i in range(1, n):
					j2 = 0
					for j in range(n):
						if j == j1:
							continue
						matTemp[i-1][j2] = mat[i][j]
						j2 += 1
				d += (-1.0)**(1.0 + j1 + 1.0) * mat[0][j1] * determinant(matTemp, n-1)
		return d
	else:
		return 0

def findProjection(point, edge, strict=False):
	'''
	findProjection(vector, edge, [strict])
	Returns a point that is the nearest point on the edge (i.e. intersection of the edge and the perpendicular vector from the point).
	If strict is True, the point will be returned only if its endpoint lies on the edge.

	@sa findDistance
	'''
	if isinstance(point,FreeCAD.Vector):
		if isinstance(edge.Curve,Part.Line):
			segment = vec(edge)
			perp = fcvec.crossproduct(segment)
			chord = fcvec.new(point,edge.Vertexes[0].Point)
			dist = fcvec.project(chord,perp)
			newpoint = Vector.add(point, dist)

			if strict:
				s1 = fcvec.new(edge.Vertexes[0].Point, newpoint)
				s2 = fcvec.new(edge.Vertexes[-1].Point, newpoint)
				if (s1.Length <= segment.Length) and (s2.Length <= segment.Length):
					return newpoint
				else:
					return None
			else:
				return newpoint

		elif isinstance(edge.Curve,Part.Circle):
			ve1 = edge.Vertexes[0].Point
			if len(edge.Vertexes) > 1:
				ve2 = edge.Vertexes[-1].Point
			else:
				ve2 = None
			center = edge.Curve.Center
			segment = fcvec.new(point,center)
			ratio = (segment.Length - edge.Curve.Radius) / segment.Length
			dist = fcvec.scale(segment, ratio)
			newpoint = Vector.add(point, dist)

			if strict and ve2:
				ang1 = fcvec.angle(fcvec.new(center, ve1))
				ang2 = fcvec.angle(fcvec.new(center, ve2))
				angpt = fcvec.angle(fcvec.new(center, newpoint))
				if ((angpt <= ang2 and angpt >= ang1) or (angpt <= ang1 and angpt >= ang2)):
					return newpoint
				else:
					return None
			else:
				return newpoint
		else:
			# instance not supported
			print "debug: findProjection bad parameters!\n"
			FreeCAD.Console.PrintMessage("debug: findProjection bad parameters!\n")
			return None
	else:
		print "debug: findProjection bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: findProjection bad parameters!\n")
		return None


def findDistance(point,edge,strict=False):
	'''
	findDistance(vector,edge,[strict])
	Returns a vector from the point to its closest point on the edge.
	If strict is True, the vector will be returned only if its endpoint lies on the edge.
	@sa findProjection
	'''
	if isinstance(point, FreeCAD.Vector):
		if isinstance(edge.Curve, Part.Line):
			segment = vec(edge)
			perp = fcvec.crossproduct(segment)
			chord = fcvec.new(point,edge.Vertexes[0].Point)
			dist = fcvec.project(chord,perp)
			newpoint = Vector.add(point, dist)
			if (dist.Length == 0):
				return None
			if strict:
				s1 = fcvec.new(edge.Vertexes[0].Point,newpoint)
				s2 = fcvec.new(edge.Vertexes[-1].Point,newpoint)
				if (s1.Length <= segment.Length) and (s2.Length <= segment.Length):
					return dist
				else:
					return None
			else: return dist
		elif isinstance(edge.Curve, Part.Circle):
			ve1 = edge.Vertexes[0].Point
			if (len(edge.Vertexes) > 1):
				ve2 = edge.Vertexes[-1].Point
			else:
				ve2 = None
			center = edge.Curve.Center
			segment = fcvec.new(point, center)
			ratio = (segment.Length - edge.Curve.Radius) / segment.Length
			dist = fcvec.scale(segment,ratio)
			newpoint = Vector.add(point, dist)
			if (dist.Length == 0):
				return None
			if strict and ve2:
				ang1 = fcvec.angle(fcvec.new(center, ve1))
				ang2 = fcvec.angle(fcvec.new(center, ve2))
				angpt = fcvec.angle(fcvec.new(center, newpoint))
				if ((angpt <= ang2 and angpt >= ang1) or (angpt <= ang1 and angpt >= ang2)):
					return dist
				else:
					return None
			else:
				return dist
		else:
			# instance not supported
			print "debug: findDistance bad parameters!\n"
			return None
	else:
		print "debug: findDistance bad parameters!\n"
		return None


def angleBisection(edge1, edge2):
	'''
	angleBisection(edge,edge)
	Returns a Part.Line that bisects the angle between the two given edges. Also called bisector.
	If the 2 lines are parallel a thrid parallel line in the middle will be returned.
	The two lines are always considered to be infinite.
	'''

	if isinstance(edge1, Part.Line) and isinstance(edge2, Part.Line):
		p1 = edge1.Vertexes[0].Point
		p2 = edge1.Vertexes[-1].Point
		p3 = edge2.Vertexes[0].Point
		p4 = edge2.Vertexes[-1].Point
		int = fcvec.intersect(p1, p2, p3, p4, True, True)
		if int == None:
			# Parallel lines - Return a third parallel line that is exactly in the middle.

			# Calculate median point.
			diff = Vector.sub(p3, p1)	# vector between origin points
			origin = Vector.add(p1, fcvec.scale(diff, 0.5))
			
			# Copy direction vector from first line (and normalize it).
			dir = fcvec.normalized(Vector.sub(p2, p1))
			return Part.Line(origin,Vector.add(origin+dir))

		# Calculate the bisector angle.
		line1Dir = Vector.sub(p2, p1)
		angleDiff = fcvec.angle(line1Dir, Vector.sub(p4, p3))
		ang = angleDiff * 0.5
		# FreeCAD.Console.PrintMessage("debug: diff:"+str(angleDiff*180/math.pi)+"\n")

		# Use intersection point as origin for the bisector.
		origin = int
		# Copy direction vector from first line (and normalize it).
		dir = fcvec.normalized(line1Dir)
		dir = fcvec.rotate(dir, ang)
		return Part.Line(origin,Vector.add(origin,dir))
	else:
		print "debug: angleBisection bad parameters!\n"
		return None


def findHomotheticCenterOfCircles(circle1, circle2):
	'''
	findHomotheticCenterOfCircles(circle1, circle2)
	Calculates the homothetic center(s) of two circles.

	http://en.wikipedia.org/wiki/Homothetic_center
	http://mathworld.wolfram.com/HomotheticCenter.html
	'''

	if isinstance(circle1.Curve, Part.Circle) and isinstance(circle2.Curve, Part.Circle):
		if fcvec.equals(circle1.Curve.Center, circle2.Curve.Center):
			return None

		cen1_cen2 = Part.Line(circle1.Curve.Center, circle2.Curve.Center).toShape()
		cenDir = fcvec.normalized(vec(cen1_cen2))

		# Get the perpedicular vector.
		perpCenDir = fcvec.normalized(fcvec.crossproduct(cenDir))

		# Get point on first circle
		p1 = Vector.add(circle1.Curve.Center, fcvec.scale(perpCenDir, circle1.Curve.Radius))

		centers = []
		# Calculate inner homothetic center
		# Get point on second circle
		p2_inner = Vector.add(circle1.Curve.Center, fcvec.scale(perpCenDir, -circle1.Curve.Radius))
		hCenterInner = fcvec.intersect(circle1.Curve.Center, circle2.Curve.Center, p1, p2_inner, True, True)
		if hCenterInner:
			centers.append(hCenterInner)

		# Calculate outer homothetic center (only exists of the circles have different radii)
		if circle1.Curve.Radius != circle2.Curve.Radius:
			# Get point on second circle
			p2_outer = Vector.add(circle1.Curve.Center, fcvec.scale(perpCenDir, circle1.Curve.Radius))
			hCenterOuter = fcvec.intersect(circle1.Curve.Center, circle2.Curve.Center, p1, p2_outer, True, True)
			if hCenterOuter:
				centers.append(hCenterOuter)

		if len(centers):
			return centers
		else:
			return None

	else:
		print "debug: findHomotheticCenterOfCircles bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: findHomotheticCenterOfCirclescleFrom3tan bad parameters!\n")
		return None


def findRadicalAxis(circle1, circle2):
	'''
	Calculates the radical axis of two circles.
	On the radical axis (also called power line) of two circles any
	tangents drawn from a point on the axis to both circles have the same length.

	http://en.wikipedia.org/wiki/Radical_axis
	http://mathworld.wolfram.com/RadicalLine.html

	@sa findRadicalCenter
	'''

	if isinstance(circle1.Curve, Part.Circle) and isinstance(circle2.Curve, Part.Circle):
		if fcvec.equals(circle1.Curve.Center, circle2.Curve.Center):
			return None
		r1 = circle1.Curve.Radius
		r2 = circle1.Curve.Radius
		cen1 = circle1.Curve.Center
		# dist .. the distance from cen1 to cen2.
		dist = fcvec.dist(cen1, circle2.Curve.Center)
		cenDir = fcvec.normalized(Vector.sub(cen1, circle2.Curve.Center))

		# Get the perpedicular vector.
		perpCenDir = fcvec.normalized(fcvec.crossproduct(cenDir))

		# J ... The radical center.
		# K ... The point where the cadical axis crosses the line of cen1->cen2.
		# k1 ... Distance from cen1 to K.
		# k2 ... Distance from cen2 to K.
		# dist = k1 + k2

		k1 = (dist + (r1^2 - r2^2) / dist) / 2.0
		#k2 = dist - k1

		K = Vector.add(cen1, fcvec.scale(cenDir, k1))

		# K_ .. A point somewhere between K and J (actually with a distance of 1 unit from K).
		K_ = Vector,add(K, perpCenDir)

		radicalAxis = Part.Line(K, Vector.add(origin, dir))

		if radicalAxis:
			return radicalAxis
		else:
			return None
	else:
		print "debug: findRadicalAxis bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: findRadicalAxis bad parameters!\n")
		return None


def findRadicalCenter(circle1, circle2, circle3):
	'''
	findRadicalCenter(circle1, circle2, circle3):
	Calculates the radical center (also called the power center) of three circles.
	It is the intersection point of the three radical axes of the pairs of circles.

	http://en.wikipedia.org/wiki/Power_center_(geometry)
	http://mathworld.wolfram.com/RadicalCenter.html

	@sa findRadicalAxis
	'''

	if isinstance(circle1.Curve, Part.Circle) and isinstance(circle2.Curve, Part.Circle) and isinstance(circle2.Curve, Part.Circle):
		radicalAxis12 = findRadicalAxis(circle1, circle2)
		radicalAxis23 = findRadicalAxis(circle1, circle2)

		if not radicalAxis12 or not radicalAxis23:
			# No radical center could be calculated.
			return None

		int = findIntersection(radicalAxis12, radicalAxis23, True, True)

		if int:
			return int
		else:
			# No radical center could be calculated.
			return None
	else:
		print "debug: findRadicalCenter bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: findRadicalCenter bad parameters!\n")
		return None

#------------------------------------------------------------
# INVERSION FUNCTIONS

def pointInversion(circle, point):
	'''
	pointInversion(Circle, Vector)

	Circle inversion of a point.
	Will calculate the inversed point an return it.
	If the given point is equal to the center of the circle "None" will be returned.

	See also:
	http://en.wikipedia.org/wiki/Inversive_geometry
	'''

	if isinstance(circle.Curve, Part.Circle) and isinstance(point, FreeCAD.Vector):
		cen = circle.Curve.Center
		rad = circle.Curve.Radius

		if fcvec.equals(cen, point):
			return None

		# Inverse the distance of the point
		# dist(cen -> P) = r^2 / dist(cen -> invP)

		dist = fcvec.dist(point, cen)
		invDist = rad**2 / d

		invPoint = Vector(0, 0, point.z)
		invPoint.x = cen.x + (point.x - cen.x) * invDist / dist;
		invPoint.y = cen.y + (point.y - cen.y) * invDist / dist;

		return invPoint

	else:
		print "debug: pointInversion bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: pointInversion bad parameters!\n")
		return None

def polarInversion(circle, edge):
	'''
	polarInversion(circle, edge):
	Returns the inversion pole of a line.
	edge ... The polar.
	i.e. The nearest point on the line is inversed.

	http://mathworld.wolfram.com/InversionPole.html
	'''

	if isinstance(circle.Curve, Part.Circle) and isinstance(edge.Curve, Part.Line):
		nearest = findProjection(circle.Curve.Center, edge, False)
		if nearest:
			inversionPole = pointInversion(circle, nearest)
			if inversionPole:
				return inversionPole

	else:
		print "debug: circleInversionPole bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: circleInversionPole bad parameters!\n")
		return None

def circleInversion(circle, circle2):
	'''
	pointInversion(Circle, Circle)

	Circle inversion of a circle.
	'''
	if isinstance(circle.Curve, Part.Circle) and isinstance(circle2.Curve, Part.Circle):
		cen1 = circle.Curve.Center
		rad1 = circle.Curve.Radius

		if fcvec.equals(cen1, point):
			return None

		invCen2 = Inversion(circle, circle2.Curve.Center)

		pointOnCircle2 = Vector.add(circle2.Curve.Center, Vector(circle2.Curve.Radius, 0, 0))
		invPointOnCircle2 = Inversion(circle, pointOnCircle2)

		return Part.Circle(invCen2, norm, fcvec.dist(invCen2, invPointOnCircle2))

	else:
		print "debug: circleInversion bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: circleInversion bad parameters!\n")
		return None

# END OF INVERSION FUNCTIONS
#------------------------------------------------------------

#------------------------------------------------------------
# CIRCLE CREATION FUNCTIONS

def circleFrom3Points (p1, p2, p3):
	'''
	Please use the built-in function Part.Circle(Point1,Point2,Point3).
	This function only exists for completeness' sake.
	Calculate a circle defined by 3 points on its circumference. There is exactly
	one solution. A Part.Circle is returned.
	'''
	if isinstance(p1,Vector) and isinstance(p2,Vector) and isinstance(p3,Vector):
		mat =	[[p1.x, p1.y, 1],
			 [p2.x, p2.y, 1],
			 [p3.x, p3.y, 1]]
		m11 = determinant(mat, 3)

		if m11 == 0:
			# The points are not on a circle (more likely on a line)
			return None

		# Store re-used values.
		p1x_p1y = p1.x**2 + p1.y**2
		p2x_p2y = p2.x**2 + p2.y**2
		p3x_p3y = p3.x**2 + p3.y**2

		mat =	[[p1x_p1y, p1.y, 1],
			 [p2x_p2y, p2.y, 1],
			 [p3x_p3y, p3.y, 1]]
		m12 = determinant(mat, 3)

		mat =	[[p1x_p1y, p1.x, 1],
			 [p2x_p2y, p2.x, 1],
			 [p3x_p3y, p3.x, 1]]
		m13 = determinant(mat, 3)

		mat =	[[p1x_p1y, p1.x, p1.y],
			 [p2x_p2y, p2.x, p2.y],
			 [p3x_p3y, p3.x, p3.y]]
		m14 = determinant(mat, 3)

		center = Vector(0.5 * m12 / m11, -0.5 * m13 / m11, p1.z)
	
		rad = math.sqrt(center.x**2 + center.y**2 + m14 / m11)
		# FreeCAD.Console.PrintMessage("rad=" +str(r) +"\n")

		return Part.Circle(center, NORM, rad)
	else:
		print "debug: circleFrom3Points bad parameters!\n"
		return None

def circleFrom2PointsRadius(p1, p2, radius):
	'''
	circleFrom2PointsRadiust(Vector, Vector, radius)
	Calculate a Part.Circle from 2 points and a radius.
	Calculates all possible locations (centers) of a circle defined by
	2 points on its circumference and a radius.
	If multiple locations are possible (max. 4) they are all returned.
	'''

	if isinstance(p1, FreeCAD.Vector) and isinstance(p2, FreeCAD.Vector):
		if fcvec.equals(p1, p2):
			print "debug: circleFrom2PointsRadius The two points are the same."
			return None

		p1_p2 = Part.Line(p1, p2).toShape()
		dist_p1p2 = fcvec.dist(p1, p1)

		# Get the midpoint between p1 and p2
		mid = findMidpoint(p1_p2)

		if dist_p1p2 == 2*radius:
			# The circle can only lie in the exact middle between the two points.
			circle = Part.Circle(mid, norm, radius)
			if circle:
				return [circle]
			else:
				return None

		# Get the normalized vector of the edge ...
		dir = fcvec.normalized(vec(p1_p2))
		# ... and the perpedicular vector.
		perpDir = fcvec.normalized(fcvec.crossproduct(dir))

		# Calculate the distance of the circle-centers from the line p1->p2.
		dist = math.sqrt(radius**2 - (dist_p1p2 / 2.0)**2)

		# Calculate the circle enters
		cen1 = Vector.add(mid, fcvec.scale(perpDir, dist))
		cen2 = Vector.add(mid, fcvec.scale(perpDir, -dist))

		circles = []
		if cen1:
			circles.append(Part.Circle(cen1, norm, radius))
		if cen2:
			circles.append(Part.Circle(cen2, norm, radius))

		if circles:
			return circles
		else:
			return None

	else:
		print "debug: circleFrom2PointsRadius bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: circleFrom2PointsRadius bad parameters!\n")
		return None

	
def circleFrom2LinesRadius (edge1, edge2, radius):
	'''
	circleFrom2LinesRadius(edge,edge,radius)
	Calculate a Part.Circle from 2 line tangents and a radius.
	Calculates all possible locations (centers) of a circle defined by
	2 line tangents and a radius.
	If multiple locations are possible (max. 4) they are all returned.	
	'''

	if isinstance(edge1.Curve, Part.Line) and isinstance(edge2.Curve, Part.Line):
		int = findIntersection(edge1, edge2, True, True)

		if not int:
			# Lines are parallel
			return None
		else:
			int = int[0]
			if not int:
				# Sanity check, this should NEVER happen.
				print "debug circleFrom2LinesRadius: array with None entry found!"
				return None

		# Calculate bisectors
		bis12 = angleBisection(edge1,edge2)
		bis21 = Part.Line(bis12.StartPoint,fcvec.rotate(vec(bis12), math.pi/2.0))

		ang12 = abs(fcvec.angle(vec(edge1),vec(edge2)))
		ang21 = math.pi - ang12

		# Calculate distances from center of the circle(s) to the intersection point of the lines
		# sin(angle / 2.0) = radius / dist
		dist12 = radius / math.sin(ang12 * 0.5)
		dist21 = radius / math.sin(ang21 * 0.5)

		# Extend a vector from the center (the intersection point)
		# along all four bisector directions (depends on the radius)
		circles = []
		cen = Vector.add(int, fcvec.scale(vec(bis12), dist12))
		circles.append(Part.Circle(cen, norm, radius))
		cen = Vector.add(int, fcvec.scale(vec(bis12), -dist12))
		circles.append(Part.Circle(cen, norm, radius))
		cen = Vector.add(int, fcvec.scale(vec(bis21), dist21))
		circles.append(Part.Circle(cen, norm, radius))
		cen = Vector.add(int, fcvec.scale(vec(bis21), -dist21))
		circles.append(Part.Circle(cen, norm, radius))
		return circles

	else:
		print "debug: circleFrom2LinesRadius bad parameters!\n"
		# FreeCAD.Console.PrintMessage("debug: circleFrom2LinesRadius bad parameters!\n")
		return None

def circlefrom1Line2Points(edge, p1, p2):
	'''
	circlefrom1Line2Points(edge, Vector, Vector):
	Calculate a Part.Circle from 1 line tangent and 2 points.
	Calculates all possible locations (centers) of a circle defined by
	one line tangent and two points on its circumference.
	If multiple locations are possible (max. 2) they are all returned.
	'''

	if isinstance(edge.Curve, Part.Line) and isinstance(p1, FreeCAD.Vector) and isinstance(p2, FreeCAD.Vector):
		p1_p2 = Part.Line(p1, p2).toShape()

		# Get the intersection point of the edge and the line p1->p2
		s = findIntersection(edge, p1_p2, True, True)
		if s:
			s = s[0]
		else:
			return None

		# Calculate the distance from s to the projected circle centers on the edge.
		# "projecte circle center" == if you project the two centers of the circle onto the edge.
		v1 = Vector.sub(p1, s)
		v2 = Vector.sub(p2, s)
		projectedDist = math.sqrt(fcvec.dotproduct(v1, v2))

		# Get the normalized vector of the edge
		edgeDir = fcvec.normalized(vec(edge))

		# Calculate the prjoected circle centers
		projectedCen1 = Vector.add(s, fcvec.scale(edgeDir, projectedDist))
		projectedCen2 = Vector.add(s, fcvec.scale(edgeDir, -projectedDist))

		# Calculate the intersection points of the two lines through projectedCen1&2 (perpenticular to the edge)
		# with the symmetry line (perpendicular to the midpoint between p1 and p2)
		# to get the original center locations.
		perpEdgeDir = fcvec.crossproduct(edgeDir)
		perpCen1 = Vector.add(projectedCen1, perpEdgeDir)
		perpCen2 = Vector.add(projectedCen2, perpEdgeDir)

		# Calc midpoint between p1 and p2
		mid = findMidpoint(p1_p2)

		# Calc point along the perp line (trough the midp.)
		perp_mid = Vector.add(mid, fcvec.normalized(fcvec.crossproduct(vec(p1_p2))))

		cen1 = fcvec.intersect(projectedCen1, perpCen1, mid, perp_mid, True, True)
		cen2 = fcvec.intersect(projectedCen2, perpCen2, mid, perp_mid, True, True)

		circles = []
		if cen1:
			radius = fcvec.dist(projectedCen1, cen1)
			circles.append(Part.Circle(cen1, norm, radius))
		if cen2:
			radius = fcvec.dist(projectedCen2, cen2)
			circles.append(Part.Circle(cen2, norm, radius))

		if circles:
			return circles
		else:
			return None

	else:
		print "debug: circlefrom1Line2Points bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: circlefrom1Line2Points bad parameters!\n")
		return None

def circlefrom2Lines1Points(edge1, edge2, point):
	'''
	circlefrom2Lines1Points(edge, edge, Vector):
	Calculate a Part.Circle from 2 line tangents and 1 point.
	Calculates all possible locations (centers) of a circle defined by
	two line tangents and a point on its circumference.
	If multiple locations are possible (max. 2) they are all returned.
	'''

	# @note The solution is the same as for LPP/circlefrom1Line2Points here.
	# We just need to calculate the mirrored (on the bisector of edge1&2) point and feed it to circlefrom1Line2Points.

	if isinstance(edge1.Curve, Part.Line) and isinstance(edge2.Curve, Part.Line) and isinstance(p1, FreeCAD.Vector):
		bis = angleBisection(edge1, edge2)
		if not bis:
			# @todo Parallel lines - check the distance
			return None

		# Mirror the given point by the bisection line.
		mirrPoint = findReflection(bis, point)
		return circlefrom1Line2Points(edge1, point, mirrPoint)

	else:
		print "debug: circlefrom2Lines1Points bad parameters!\n"
		# FreeCAD.Console.PrintMessage("debug: circlefrom2Lines1Points bad parameters!\n")
		return None


def circleFrom3LineTangents (edge1, edge2, edge3):
	'''
	circleFrom3LineTangents(edge,edge,edge)
	Calculate a Part.Circle from 3 line tangents.
	Calculates all possible locations of a circle defined by 3 line tangents.
	If multiple locations are possible (max. 4) they are all returned.
	'''

	if isinstance(edge1.Curve, Part.Line) and isinstance(edge2.Curve, Part.Line) and isinstance(edge3.Curve, Part.Line):
		# Calculate new lines by bisecting the angles and use the intersection point of the new lines as center.
		# Calculate all 3 angle bisection lines. (check for parallels?)
		# We actually need 6 bisection lines, bht the other are simply perpendicular to the calculated ones.
		bis12 = angleBisection(edge1,edge2)
		bis23 = angleBisection(edge2,edge3)
		bis31 = angleBisection(edge3,edge1)
		# Calculate the 4 intersection points (there are some duplicate intersections in some cases)
		# @todo Store radius and/or relationship to tangents (if it's needed).
		intersections = []
		int = fcvec.intersect(bis12.StartPoint, Vector.add(bis12.StartPoint, vec(bis12)), bis23.StartPoint, Vector.add(bis23.StartPoint, vec(bis23)), True, True)	
		if int != None:
			radius = findDistance(int,edge1)
			intersections.append(Part.Circle(int,norm,radius))
		int = fcvec.intersect(bis12.StartPoint, Vector.add(bis12.StartPoint, vec(bis12)), bis23.StartPoint, Vector.add(bis23.StartPoint, vec(bis23)), True, True)
		if int != None:
			radius = findDistance(int,edge1)
			intersections.append(Part.Circle(int,norm,radius))
		int = fcvec.intersect(bis23.StartPoint, Vector.add(bis23.StartPoint, vec(bis23)), bis31.StartPoint, Vector.add(bis31.StartPoint, vec(bis31)), True, True)
		if int != None:
			radius = findDistance(int,edge1)
			intersections.append(Part.Circle(int,norm,radius))
		int = fcvec.intersect(bis31.StartPoint, Vector.add(bis31.StartPoint, vec(bis31)), bis12.StartPoint, Vector.add(bis12.StartPoint, vec(bis12)), True, True)
		if int != None:
			radius = findDistance(int,edge1)
			intersections.append(Part.Circle(int,norm,radius))
		int = fcvec.intersect(bis12.StartPoint, Vector.add(bis12.StartPoint, fcvec.rotate(vec(bis12), math.pi/2.0)), bis23.StartPoint, Vector.add(bis23.StartPoint, fcvec.rotate(vec(bis23),math.pi/2.0)), True, True)
		if int != None:
			radius = findDistance(int,edge1)
			intersections.append(Part.Circle(int,norm,radius))
		int = fcvec.intersect(bis23.StartPoint, Vector.add(bis23.StartPoint, fcvec.rotate(vec(bis23), math.pi/2.0)), bis31.StartPoint, Vector.add(bis31.StartPoint, fcvec.rotate(vec(bis31),math.pi/2.0)), True, True)
		if int != None:
			radius = findDistance(int,edge1)
			intersections.append(Part.Circle(int,norm,radius))
		int = fcvec.intersect(bis31.StartPoint, Vector.add(bis31.StartPoint, fcvec.rotate(vec(bis31), math.pi/2.0)), bis12.StartPoint, Vector.add(bis12.StartPoint, fcvec.rotate(vec(bis12),math.pi/2.0)), True, True)
		if int != None:
			radius = findDistance(int,edge1)
			intersections.append(Part.Circle(int,norm,radius))
 
		# Check for duplicates
		circles = []
		for int in intersections:
			exists = False
			for cir in circles:
				if fcvec.equals(cir.Center, int.Center):
					exists = True
					break
			if not exists:
				circles.append(int)

		if circles:
			return circles
		else:
			return None
	else:
		print "debug: circleFrom3LineTangents bad parameters!\n"
		# FreeCAD.Console.PrintMessage("debug: circleFrom3LineTangents bad parameters!\n")
		return None


def circleFromPointLineRadius (point, edge, radius):
 	'''
	circleFromPointLineRadius (point, edge, radius):
	Calculate a circle from a line tangent, a point on the circumference and a radius.
	If multiple locations are possible (max. 2) they are all returned.
 	'''

	# =========================
	# Example:
	# import Draft
	# import Part
	# e1 = Vector(1, 1, 0)
	# e2 = Vector(4, 5, 0)
	# edge = Part.Line(e1, e2).toShape()
	# p = Vector(5, 3, 0)
	# radius = 5.0
	# print str(Draft.fcgeo.circleFromPointLineRadius(p, edge, radius)) + "\n"
	## Results for the circle centers should be:
	## 5, -2
	## 9.8, 4,4
	# =========================

	if isinstance(point, FreeCAD.Vector) and isinstance(edge.Curve, Part.Line):
		# Get distance of point from line.
		dist = findDistance(point, edge, False)

		center1 = None
		center2 = None

		if dist.Length == 0:
			# There are TWO solutions here. One on each side of the edge.
			segment = vec(edge)
			perpVec = fcvec.normalized(fcvec.crossproduct(segment))

			normPoint_c1 = fcvec.scale(perpVec, radius)
			normPoint_c2 = fcvec.scale(perpVec, -radius)

			center1 = Vector.add(point, normPoint_c1)
			center2 = Vector.add(point, normPoint_c2)
		elif dist.Length > 2 * radius:
			return None
		elif dist.Length == 2 * radius:
			# Only one solution possible.
			# Get point exactly in the middle of the line and the point.
			normPoint = findProjection(point, edge, False)
			dummy = fcvec.scale(Vector.sub(normPoint, point), 0.5)
			cen = Vector.add(point, dummy)

			circ = Part.Circle(cen, norm, radius)
			if circ:
				return [circ]
			else:
				return None

		else:
			# Get projected point on line and the distance from the point to the line.
			normPoint = findProjection(point, edge, False)
			normDist = fcvec.dist(normPoint, point)

			# Calculate distance from given point to circle centers. (direction only along the given edge!)
			dist = math.sqrt(radius**2 - (radius - normDist)**2)

			# Calculate distance vector (perp. to the edge; length = radius) to the circle centers.
			centerNormVec = fcvec.scale(fcvec.normalized(Vector.sub(point, normPoint)), radius)

			# Calculate the two centers for the possible circles.
			edgeDir = fcvec.normalized(Vector.sub(edge.Vertexes[0].Point, normPoint))
			center1 = Vector.add(centerNormVec ,Vector.add(normPoint, fcvec.scale(edgeDir, dist)))
			center2 = Vector.add(centerNormVec ,Vector.add(normPoint, fcvec.scale(edgeDir, -dist)))

		# Return the calculated circles.
		circles = []
		if center1:
			circ = Part.Circle(center1, norm, radius)
			if circ:
				circles.append(circ)
		if center2:
			circ = Part.Circle(center2, norm, radius)
			if circ:
				circles.append(circ)

		if len(circles):
			return circles
		else:
			return None
	else:
		print "debug: circleFromPointLineRadius bad parameters!\n"
		# FreeCAD.Console.PrintMessage("debug: circleFromPointLineRadius bad parameters!\n")
		return None


def outerSoddyCircle(circle1, circle2, circle3):
	'''
	Computes the outer soddy circle for three tightly packed circles.
	'''
	if isinstance(circle1.Curve, Part.Circle) and isinstance(circle2.Curve, Part.Circle) and isinstance(circle3.Curve, Part.Circle):
		# Original Java code Copyright (rc) 2008 Werner Randelshofer
		# Converted to python by Martin Buerbaum 2009
		# http://www.randelshofer.ch/treeviz/
		# Either Creative Commons Attribution 3.0, the MIT license, or the GNU Lesser General License LGPL.

		A = circle1.Curve.Center
		B = circle2.Curve.Center
		C = circle3.Curve.Center

		ra = circle1.Curve.Radius
		rb = circle2.Curve.Radius
		rc = circle3.Curve.Radius

		# Solution using Descartes' theorem, as described here:
		# http://en.wikipedia.org/wiki/Descartes%27_theorem
		k1 = 1 / ra
		k2 = 1 / rb
		k3 = 1 / rc
		k4 = abs(k1 + k2 + k3 - 2 * math.sqrt(k1 * k2 + k2 * k3 + k3 * k1))

		q1 = (k1 + 0j) * (A.x + A.y * 1j)
		q2 = (k2 + 0j) * (B.x + B.y * 1j)
		q3 = (k3 + 0j) * (C.x + C.y * 1j)

		temp = ((q1 * q2) + (q2 * q3) + (q3 * q1))
		q4 = q1 + q2 + q3 - ((2 + 0j) * cmath.sqrt(temp) )

		z = q4 / (k4 + 0j)

		# If the formula is not solveable, we return no circle.
		if (not z or not (1 / k4)):
			return None

		X = -z.real
		Y = -z.imag
		print "Outer Soddy circle: " + str(X) + " " + str(Y) + "\n"	# Debug

		# The Radius of the outer soddy circle can also be calculated with the following formula:
		# radiusOuter = abs(r1*r2*r3 / (r1*r2 + r1*r3 + r2*r3 - 2 * math.sqrt(r1*r2*r3 * (r1+r2+r3))))
		circ = Part.Circle(Vector(X, Y, A.z), norm, 1 / k4)
		return circ

	else:
		print "debug: outerSoddyCircle bad parameters!\n"
		# FreeCAD.Console.PrintMessage("debug: outerSoddyCircle bad parameters!\n")
		return None

def innerSoddyCircle(circle1, circle2, circle3):
	'''
	Computes the inner soddy circle for three tightly packed circles.
	'''
	if isinstance(circle1.Curve, Part.Circle) and isinstance(circle2.Curve, Part.Circle) and isinstance(circle3.Curve, Part.Circle):
		# Original Java code Copyright (rc) 2008 Werner Randelshofer
		# Converted to python by Martin Buerbaum 2009
		# http://www.randelshofer.ch/treeviz/

		A = circle1.Curve.Center
		B = circle2.Curve.Center
		C = circle3.Curve.Center

		ra = circle1.Curve.Radius
		rb = circle2.Curve.Radius
		rc = circle3.Curve.Radius

		# Solution using Descartes' theorem, as described here:
		# http://en.wikipedia.org/wiki/Descartes%27_theorem
		k1 = 1 / ra
		k2 = 1 / rb
		k3 = 1 / rc
		k4 = abs(k1 + k2 + k3 + 2 * math.sqrt(k1 * k2 + k2 * k3 + k3 * k1))

		q1 = (k1 + 0j) * (A.x + A.y * 1j)
		q2 = (k2 + 0j) * (B.x + B.y * 1j)
		q3 = (k3 + 0j) * (C.x + C.y * 1j)

		temp = ((q1 * q2) + (q2 * q3) + (q3 * q1))
		q4 = q1 + q2 + q3 + ((2 + 0j) * cmath.sqrt(temp) )

		z = q4 / (k4 + 0j)

		# If the formula is not solveable, we return no circle.
		if (not z or not (1 / k4)):
			return None

		X = z.real
		Y = z.imag
		print "Outer Soddy circle: " + str(X) + " " + str(Y) + "\n"	# Debug

		# The Radius of the inner soddy circle can also be calculated with the following formula:
		# radiusInner = abs(r1*r2*r3 / (r1*r2 + r1*r3 + r2*r3 + 2 * math.sqrt(r1*r2*r3 * (r1+r2+r3))))
		circ = Part.Circle(Vector(X, Y, A.z), norm, 1 / k4)
		return circ

	else:
		print "debug: innerSoddyCircle bad parameters!\n"
		# FreeCAD.Console.PrintMessage("debug: innerSoddyCircle bad parameters!\n")
		return None
 
def circleFrom3CircleTangents(circle1, circle2, circle3):
	'''
	http://en.wikipedia.org/wiki/Problem_of_Apollonius#Inversive_methods
	http://mathworld.wolfram.com/ApolloniusCircle.html
	http://mathworld.wolfram.com/ApolloniusProblem.html
	'''

	if isinstance(circle1.Curve, Part.Circle) and isinstance(circle2.Curve, Part.Circle) and isinstance(circle3.Curve, Part.Circle):
		int12 = findIntersection(circle1, circle2, True, True)
		int23 = findIntersection(circle2, circle3, True, True)
		int31 = findIntersection(circle3, circle1, True, True)

		if int12 and int23 and int31:
			if len(int12) == 1 and len(int23) == 1 and len(int31) == 1:
				# Only one intersection with each circle.
				# => "Soddy Circle" - 2 solutions.
				# http://en.wikipedia.org/wiki/Problem_of_Apollonius#Mutually_tangent_given_circles:_Soddy.27s_circles_and_Descartes.27_theorem
				# http://mathworld.wolfram.com/SoddyCircles.html
				# http://mathworld.wolfram.com/InnerSoddyCenter.html
				# http://mathworld.wolfram.com/OuterSoddyCenter.html

				r1 = circle1.Curve.Radius
				r2 = circle2.Curve.Radius
				r3 = circle3.Curve.Radius
				outerSoddy = outerSoddyCircle(circle1, circle2, circle3)
#				print str(outerSoddy) + "\n" # Debug

				innerSoddy = innerSoddyCircle(circle1, circle2, circle3)
#				print str(innerSoddy) + "\n" # Debug

				circles = []
				if outerSoddy:
					circles.append(outerSoddy)
				if innerSoddy:
					circles.append(innerSoddy)
				return circles

			# @todo Calc all 6 homothetic centers.
			# @todo Create 3 lines from the inner and 4 from the outer h. center.
			# @todo Calc. the 4 inversion poles of these lines for each circle.
			# @todo Calc. the radical center of the 3 circles.
			# @todo Calc. the intersection points (max. 8) of 4 lines (trough each inversion pole and the radical center) with the circle.
			#       This gives us all the tangent points.
		else:
			# Some circles are inside each other or an error has occured.
			return None

	else:
		print "debug: circleFrom3CircleTangents bad parameters!\n"
		# FreeCAD.Console.PrintMessage("debug: circleFrom3CircleTangents bad parameters!\n")
		return None

# Circle meta functions ...

def circleFrom3tan(tan1, tan2, tan3):
	tan1IsLine = isinstance(tan1.Curve, Part.Line)
	tan2IsLine = isinstance(tan2.Curve, Part.Line)
	tan3IsLine = isinstance(tan3.Curve, Part.Line)
	tan1IsCircle = isinstance(tan1.Curve, Part.Circle)
	tan2IsCircle = isinstance(tan2.Curve, Part.Circle)
	tan3IsCircle = isinstance(tan3.Curve, Part.Circle)

	# LLL
	if tan1IsLine and tan2IsLine and tan3IsLine:
		return circleFrom3LineTangents(tan1, tan2, tan3)

#@todo
#	# CCC
#	elif tan1IsCircle and tan2IsCircle and tan3IsCircle:
#		return circleFrom3CircleTangents(tan1, tan2, tan3)

#@todo
#	# CLL
#	elif (tan1IsCircle and tan2IsLine and tan3IsLine):
#		return circleFrom1Circle2Lines(tan1, tan2, tan3)
#	elif (tan1IsLine and tan2IsCircle and tan3IsLine):
#		return circleFrom1Circle2Lines(tan2, tan1, tan3)
#	elif (tan1IsLine and tan2IsLine and tan3IsCircle):
#		return circleFrom1Circle2Lines(tan3, tan1, tan2)

#@todo
#	# CCL
#	elif (tan1IsLine and tan2IsCircle and tan3IsCircle):
#		return circleFrom2Circle1Lines(tan2, tan3, tan1)
#	elif (tan1IsCircle and tan2IsLine and tan3IsCircle):
#		return circleFrom2Circle1Lines(tan1, tan3, tan2)
#	elif (tan1IsCircle and tan2IsCircle and tan3IsLine):
#		return circleFrom2Circle1Lines(tan1, tan2, tan3)

	else:
		print "debug: circleFrom3tan bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: circleFrom3tan bad parameters!\n")
		return None

def circleFrom2tan1pt(tan1, tan2, point):
	if isinstance(tan1.Curve, Part.Line) and isinstance(tan2.Curve, Part.Line) and isinstance(point, FreeCAD.Vector):
		# LLP
		return circlefrom2Lines1Points(tan1, tan2, point)
#@todo
#	# CLP
#	elif isinstance(tan1.Curve, Part.Circle) and isinstance(tan2.Curve, Part.Line) and isinstance(point, FreeCAD.Vector):
#		return circlefromCircleLinePoint(tan1, tan2, point)
#	elif isinstance(tan2.Curve, Part.Circle) and isinstance(tan1.Curve, Part.Line) and isinstance(point, FreeCAD.Vector):
#		return circlefromCircleLinePoint(tan2, tan1, point)

#@todo
#	#CCP
#	elif isinstance(tan2.Curve, Part.Circle) and isinstance(tan1.Curve, Part.Circle) and isinstance(point, FreeCAD.Vector):
#		return circlefrom2Circles1Point(tan2, tan1, point)
	else:
		print "debug: circleFrom2tan1pt bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: circleFrom2tan1pt bad parameters!\n")
		return None

def circleFrom2tan1rad(tan1, tan2, rad):
	# LLR
	if isinstance(tan1.Curve, Part.Line) and isinstance(tan2.Curve, Part.Line):
		return circleFrom2LinesRadius(tan1, tan2, rad)
#@todo
#	# CLR
#	elif isinstance(tan1.Curve, Part.Circle) and isinstance(tan2.Curve, Part.Line):
#		return circleFromCircleLineRadius(tan1, tan2, rad)
#	elif isinstance(tan1.Curve, Part.Line) and isinstance(tan2.Curve, Part.Circle):
#		return circleFromCircleLineRadius(tan2, tan1, rad)

#@todo
#	# CCR
#	elif isinstance(tan1.Curve, Part.Circle) and isinstance(tan2.Curve, Part.Circle):
#		return circleFrom2CirclesRadius(tan1, tan2, rad)

	else:
		print "debug: circleFrom2tan1rad bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: circleFrom2tan1rad bad parameters!\n")
		return None

def circleFrom1tan2pt(tan1, p1, p2):
	# LPP
	if isinstance(tan1.Curve, Part.Line) and isinstance(p1, FreeCAD.Vector) and isinstance(p2, FreeCAD.Vector):
		return circlefrom1Line2Points(tan1, p1, p2)

#@todo
#	# CPP
#	if isinstance(tan1.Curve, Part.Line) and isinstance(p1, FreeCAD.Vector) and isinstance(p2, FreeCAD.Vector):
#		return circlefrom1Circle2Points(tan1, p1, p2)

	else:
		print "debug: circleFrom1tan2pt bad parameters!\n"
		FreeCAD.Console.PrintMessage("debug: circleFrom1tan2pt bad parameters!\n")
		return None

def linearFromPoints (p1, p2):
	'''
	Calculate linear equation from points.
	Calculate the slope and offset parameters of the linear equation of a line defined by two points.

	Linear equation:
	y = m * x + b
	m = dy / dx
	m ... Slope
	b ... Offset (point where the line intersects the y axis)
	dx/dy ... Delta x and y. Using both as a vector results in a non-offset direction vector.
	'''
	if isinstance(p1, Vector) and isinstance(p2, Vector):
		line = {}
		line['dx'] = (p2.x - p1.x)
		line['dy'] = (p2.y - p1.y)
		line['slope'] = line['dy'] / line['dx']
		line['offset'] = p1.y - slope * p1.x
		return line
	else:
		return None

def circleNeededParameters (parameters):
	'''
	Search for needed parameters for circle construction.
	
	Takes a list of parameters that can defined a circle and returns a list of possible parameters that are still missing.
	If the given parameters are enough for the circle construction nothing is returned.
	Most of these are special cases of Apollonius' problem:
	http://en.wikipedia.org/wiki/Problem_of_Apollonius#Ten_combinations_of_points.2C_circles.2C_and_lines
	http://en.wikipedia.org/wiki/Special_cases_of_Apollonius%27_problem
	http://whistleralley.com/tangents/tangents.htm
	http://mathworld.wolfram.com/ApolloniusProblem.html
	@todo Add callback functions here. They should know about hte subcases
	(i.e. tan3 will call the callback function and the fucntion will decide if LLL, CLL or CCL is choosen)
	'''
	
	# circleParTypes = { 'pt': True, 'cen': True, 'tan': True, 'rad': True }

	# A list of definitions that can be used to construct a circle.
	# @todo Make this a global list if needed.
	circleDefinitions = {
		# Tangent Tangent Tangent
		'tan3': { 'tan': 3 },
		# Tangent Tangent Point
#		'tan2_pt': { 'pt': 1, 'tan': 2 },
		# Tangent Tangent Radius
		'tan2_rad': { 'tan': 2, 'rad': 1 },
		# Tangent Point Point
#		'pt2_tan': { 'pt': 2, 'tan': 1 },
		# Tangent Point Radius
#		'tan_pt_rad': { 'tan': 1, 'pt': 1, 'rad': 1 },
		# Point Point Point
		'pt3': { 'pt': 3 },
		# Point Point Radius
#		'pt2_rad': { 'pt': 2, 'rad': 1 },
		# Centerpoint Point
		'cen_pt': { 'cen': 1, 'pt': 1 },
		# Centerpoint Radius
		'cen_rad': { 'cen': 1, 'rad': 1 }
		# (Centerpoint Tangent?)<-- should be covered by cen+pt+constraint
	}

	#if 'pt' in parameters:
	
	possibleParameters = {}

	# Check all circle definitions if they match the one we already have
	# and collect misisng ones for all the matching defs.
	for cDefName, cDef in circleDefinitions.iteritems():
		defMatch = True			# True if we could apply the definition if we just had a few more parameters.
		defMatchPerfectly = True	# True if the definition matches exactly.
		for parType, parNum in parameters.iteritems():
			# Check if all requested parameters match this definition (i.e. there are <= entries of each)
			if not parType in cDef:
				# Check if given parameter is even in the definition.
				defMatch = False
				defMatchPerfectly = False
				break
			elif parNum <= 0:
				# Sanity check for zero (or even negative) values.
				defMatch = False
				defMatchPerfectly = False
				break
			elif parNum > cDef[parType]:
				# Check if more parameter where given than needed by this definition.
				defMatch = False
				defMatchPerfectly = False
				break
			elif parNum != cDef[parType]:
				defMatchPerfectly = False
		
		if defMatchPerfectly:
			return None

		if defMatch:
			# Get & store remaining possible parameters.
			for parType, parNum in cDef.iteritems():
				#if not (parType in possibleParameters) and ... # use this if we want to set it only once
				if (parType not in parameters) or (parameters[parType] < parNum):
					possibleParameters[parType] = True

	if len(possibleParameters):
		return possibleParameters
	else:
		return None

