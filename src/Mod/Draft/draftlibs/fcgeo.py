
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

import FreeCAD, Part, fcvec, math

norm = FreeCAD.Vector(0,0,1) # provisory normal direction for all geometry ops.

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
	if infinite1 is True, edge1 will be considered infinite
	if infinite2 is True, edge2 will be considered infinite
	algorithm from http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
	'''
	if (isinstance(edge1.Curve,Part.Line)) and (isinstance(edge2.Curve,Part.Line)):
		# two lines
		print "debug: 2 lines"
		p1 = edge1.Vertexes[0].Point
		p2 = edge1.Vertexes[1].Point
		p3 = edge2.Vertexes[0].Point
		p4 = edge2.Vertexes[1].Point
		int = fcvec.intersect(p1,p2,p3,p4)
		if int:
			return [int]
		else:
			return None

	elif (isinstance(edge1.Curve,Part.Circle)) and (isinstance(edge2.Curve,Part.Circle)):
		# two curves
		print "debug: 2 curves"
		c1 = edge1.Curve.Center
		c2 = edge2.Curve.Center
		r1 = edge1.Curve.Radius
		r2 = edge2.Curve.Radius
		d = fcvec.new(c1,c2)
		dist = d.Length
		if (dist < (r1+r2)):
			print "debug: no intersection"
			return None # no intersection
		elif (d == 0):
			print "debug: circles are equals"
			return None # circles are equals
		elif (d < abs(r1-r2)):
			print "debug: one circle inside the other"
			return None # one circle is inside the other
		elif (dist == (r1+r2)):
			print "debug: one int point"
			return [fcvec.scale(fcvec.normalized(d),r1)]
		else:
			print "debug: 2 int points"
			a = ((r1*r1)-(r2*r2)+(dist*dist))/(2.0*dist)
			p2 = FreeCAD.Vector(c1.x+(d.x*a/dist),c1.y+(d.y*a/dist),c1.z)
			h = math.sqrt((r1*r1)-(a*a))
			rx = -d.y*(h/dist)
			ry = d.x*(h/dist)
			intpnts = []
			intpnts.append(FreeCAD.Vector(p2.x+rx,p2.y+ry,c1.z))
			intpnts.append(FreeCAD.Vector(p2.x-rx,p2.y-ry,c1.z))
			return intpnts
			       
	elif ((isinstance(edge1.Curve,Part.Line) and isinstance(edge2.Curve,Part.Circle))
	or (isinstance(edge1.Curve,Part.Circle) and isinstance(edge2.Curve,Part.Line))):
		# one curve and one line
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
		num = (c.x-p1.x)*(p2.x-p1.x)+(c.y-p1.y)*(p2.y-p1.y)
		denom = (p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y)
		if denom == 0:
			return
		u = num / denom
		xp = p1.x + u*(p2.x-p1.x)
		yp = p1.y + u*(p2.y-p1.y)
		a = (p2.x - p1.x)**2 + (p2.y - p1.y)**2
		b = 2*((p2.x-p1.x)*(p1.x-c.x) + (p2.y-p1.y)*(p1.y-c.y))
		c = c.x**2+c.y**2+p1.x**2+p1.y**2-2*(c.x*p1.x+c.y*p1.y)-r**2
		q = b**2 - 4*a*c
		if q == 0:
			intpnts.append(FreeCAD.Vector(xp,yp,p1.z))
		elif q:
			u1 = (-b+math.sqrt(abs(q)))/(2*a)
			u2 = (-b-math.sqrt(abs(q)))/(2*a)
			intpnts.append(FreeCAD.Vector((p1.x + u1*(p2.x-p1.x)),(p1.y + u1*(p2.y-p1.y)),p1.z))
			intpnts.append(FreeCAD.Vector((p1.x + u2*(p2.x-p1.x)), (p1.y + u2*(p2.y-p1.y)),p1.z))
		return intpnts

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
		startpoint = fcvec.add(first,fcvec.scale(chord,0.5))
		endpoint = fcvec.scale(fcvec.normalized(perp),sagitta)
		return fcvec.add(startpoint,endpoint)
	else:
		halfedge = fcvec.scale(fcvec.new(first,last),.5)
		return fcvec.add(first,halfedge)

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
		v1 = fcvec.add(edge.Vertexes[0].Point,vector)
		v2 = fcvec.add(edge.Vertexes[-1].Point,vector)
		return Part.Line(v1,v2).toShape()
	else:
		rad = fcvec.new(edge.Curve.Center,edge.Vertexes[0].Point)
		newrad = fcvec.add(rad,vector).Length
		return Part.Circle(edge.Curve.Center,norm,newrad).toShape()
		

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

def findDistance(point,edge,strict=False):
	'''
	findDistance(vector,edge,[strict])
	findDistance(Vector, Vector, Vector): Returns a vector from the point to its closest
	point on the edge. If strict is True, the vector will be returned only if its endpoint
	lies on the edge.
	'''
	if not isinstance(point,FreeCAD.Vector): return None
	if isinstance(edge.Curve,Part.Line):
		segment = vec(edge)
		perp = fcvec.crossproduct(segment)
		chord = fcvec.new(point,edge.Vertexes[0].Point)
		dist = fcvec.project(chord,perp)
		newpoint = fcvec.add(point,dist)
		if (dist.Length == 0): return None
		if strict:
			s1 = fcvec.new(edge.Vertexes[0].Point,newpoint)
			s2 = fcvec.new(edge.Vertexes[-1].Point,newpoint)
			if (s1.Length <= segment.Length) and (s2.Length <= segment.Length):
				return dist
			else: return None
		else: return dist
	elif isinstance(edge.Curve,Part.Circle):
		ve1 = edge.Vertexes[0].Point
		if (len(edge.Vertexes) > 1): ve2 = edge.Vertexes[-1].Point
		else: ve2 = None
		center=edge.Curve.Center
		segment = fcvec.new(point,center)
		ratio=(segment.Length-edge.Curve.Radius)/segment.Length
		dist = fcvec.scale(segment,ratio)
		newpoint = fcvec.add(point,dist)
		if (dist.Length == 0): return None
		if strict and ve2:
			ang1=fcvec.angle(fcvec.new(center,ve1))
			ang2=fcvec.angle(fcvec.new(center,ve2))
			angpt=fcvec.angle(fcvec.new(center,newpoint))
			if (((angpt <= ang2) and (angpt >= ang1)) or ((angpt <= ang1) and (angpt >= ang2))):
				return dist
			else: return None
		else: return dist

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
			diff = fcvec.sub(p3, p1)	# vector between origin points
			origin = fcvec.add(p1, fcvec.scale(diff, 0.5))
			
			# Copy direction vector from first line (and normalize it).
			dir = fcvec.normalized(fcvec.sub(p2, p1))
			return Part.Line(origin,fcvec.add(origin+dir))

		# Calculate the bisector angle.
		line1Dir = fcvec.sub(p2, p1)
		angleDiff = fcvec.angle(line1Dir, fcvec.sub(p4, p3))
		ang = angleDiff * 0.5
		FreeCAD.Console.PrintMessage("diff:"+str(angleDiff*180/math.pi)+"\n") #debug

		# Use intersection point as origin for the bisector.
		origin = int
		# Copy direction vector from first line (and normalize it).
		dir = fcvec.normalized(line1Dir)
		dir = fcvec.rotate(dir, ang)
		return Part.Line(origin,fcvec.add(origin,dir))
	else:
		return None
		
def circleFrom3Points (p1, p2, p3):
	'''
	Calculate a circle defined by 3 points on its circumference. There is exactly
	one solution. A Part.Circle is returned.
	'''
	if isinstance(p1,FreeCAD.Vector) and isinstance(p2,FreeCAD.Vector) and isinstance(p3,FreeCAD.Vector):
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

		center = FreeCAD.Vector(0.5 * m12 / m11, -0.5 * m13 / m11, p1.z)
	
		rad = math.sqrt(center.x**2 + center.y**2 + m14 / m11)
		# FreeCAD.Console.PrintMessage("rad=" +str(r) +"\n")

		return Part.Circle(center,norm,rad)
	else:
		print "debug: circleFrom3Points bad parameters!\n"
		# FreeCAD.Console.PrintMessage("debug: circleFrom3Points bad parameters!\n")
		return None

def circleFrom2LinesRadius (edge1, edge2, radius):
	'''
	circleFrom2LinesRadius(edge,edge,radius)
	Calculate a Part.Circle from 2 line tangents and a radius.
	Calculates all possible locations (centers) of a circle defined by
	2 line tangents and a radius. If multiple locations are possible (max. 4)
	they are all returned.	
	'''
	if not isinstance(edge1.Curve,Part.Line): return None
	if not isinstance(edge2.Curve,Part.Line): return None
	int = findIntersection(edge1,edge2, True, True)[0]
	
	if not int: return None # Lines are parallel
		
	# Calculate bisectors
	bis12 = angleBisection(edge1,edge2)
	bis21 = Part.Line(bis12.StartPoint,fcvec.rotate(vec(bis12), math.pi/2.0))
	
	#print "bis: " + str(bis12) + " " +  str(bis21) + "\n"
	
	ang12 = abs(fcvec.angle(vec(edge1),vec(edge2)))
	ang21 = math.pi - ang12
	
	# print "ang: " + str(ang12 * 180.0/math.pi) + " " + str(ang21* 180.0/math.pi) + "\n"
	
	# Calculate distances from center of the circle(s) to the intersection point of the lines
	# sin(angle / 2.0) = radius / dist
	dist12 = radius / math.sin(ang12 * 0.5)
	dist21 = radius / math.sin(ang21 * 0.5)
	
	#print "dist: " + str(dist12) + " " + str(dist21) + "\n"
	
	# Extend a vector from the center (the intersection point)
	# along all four bisector directions (depends on the radius)
	circles = []
	cen = fcvec.add(int, fcvec.scale(vec(bis12), dist12))
	circles.append(Part.Circle(cen,norm,radius))
	cen = fcvec.add(int, fcvec.scale(vec(bis12), -dist12))
	circles.append(Part.Circle(cen,norm,radius))
	cen = fcvec.add(int, fcvec.scale(vec(bis21), dist21))
	circles.append(Part.Circle(cen,norm,radius))
	cen = fcvec.add(int, fcvec.scale(vec(bis21), -dist21))
	circles.append(Part.Circle(cen,norm,radius))
	return circles

def circleFrom3LineTangents (edge1, edge2, edge3):
	'''
	circleFrom3LineTangents(edge,edge,edge)
	Calculate a Part.Circle from 3 line tangents.
	Calculates all possible locations of a circle defined by 3 line tangents.
	If multiple locations are possible (max. 4) they are all returned.
	'''

	# Calculate new lines by bisecting the angles and use the intersection point of the new lines as center.
	# Calculate all 3 angle bisection lines. (check for parallels?)
	# We actually need 6 bisection lines, bht the other are simply perpendicular to the calculated ones.
	bis12 = angleBisection(edge1,edge2)
	bis23 = angleBisection(edge2,edge3)
	bis31 = angleBisection(edge3,edge1)

	# Calculate the 4 intersection points (there are some duplicate intersections in some cases)
	# @todo Store radius and/or relationship to tangents
	intersections = []
	int = fcvec.intersect(bis12.StartPoint, fcvec.add(bis12.StartPoint, vec(bis12)), bis23.StartPoint, fcvec.add(bis23.StartPoint, vec(bis23)), True, True)

	if int != None:
		radius = findDistance(int,edge1)
		intersections.append(Part.Circle(int,norm,radius))
	int = fcvec.intersect(bis12.StartPoint, fcvec.add(bis12.StartPoint, vec(bis12)), bis23.StartPoint, fcvec.add(bis23.StartPoint, vec(bis23)), True, True)
	if int != None:
		radius = findDistance(int,edge1)
		intersections.append(Part.Circle(int,norm,radius))
	int = fcvec.intersect(bis23.StartPoint, fcvec.add(bis23.StartPoint, vec(bis23)), bis31.StartPoint, fcvec.add(bis31.StartPoint, vec(bis31)), True, True)
	if int != None:
		radius = findDistance(int,edge1)
		intersections.append(Part.Circle(int,norm,radius))
	int = fcvec.intersect(bis31.StartPoint, fcvec.add(bis31.StartPoint, vec(bis31)), bis12.StartPoint, fcvec.add(bis12.StartPoint, vec(bis12)), True, True)
	if int != None:
		radius = findDistance(int,edge1)
		intersections.append(Part.Circle(int,norm,radius))
	int = fcvec.intersect(bis12.StartPoint, fcvec.add(bis12.StartPoint, fcvec.rotate(vec(bis12), math.pi/2.0)), bis23.StartPoint, fcvec.add(bis23.StartPoint, fcvec.rotate(vec(bis23),math.pi/2.0)), True, True)
	if int != None:
		radius = findDistance(int,edge1)
		intersections.append(Part.Circle(int,norm,radius))
	int = fcvec.intersect(bis23.StartPoint, fcvec.add(bis23.StartPoint, fcvec.rotate(vec(bis23), math.pi/2.0)), bis31.StartPoint, fcvec.add(bis31.StartPoint, fcvec.rotate(vec(bis31),math.pi/2.0)), True, True)
	if int != None:
		radius = findDistance(int,edge1)
		intersections.append(Part.Circle(int,norm,radius))
	int = fcvec.intersect(bis31.StartPoint, fcvec.add(bis31.StartPoint, fcvec.rotate(vec(bis31), math.pi/2.0)), bis12.StartPoint, fcvec.add(bis12.StartPoint, fcvec.rotate(vec(bis12),math.pi/2.0)), True, True)
	if int != None:
		radius = findDistance(int,edge1)
		intersections.append(Part.Circle(int,norm,radius))

	# Check for duplicates
	centers = []
	for int in intersections:
		exists = False
		for cen in centers:
			if fcvec.equals(cen.Center, int.Center):
				exists = True
				break
		if not exists:
			centers.append(int)

	if centers: return centers
	else: return None

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
	if isinstance(p1, FreeCAD.Vector) and isinstance(p2, FreeCAD.Vector):
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

