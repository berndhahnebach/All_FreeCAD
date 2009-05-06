
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


# Generic functions *********************************************************


def vec(edge):
	"vec(edge) or vec(line) -- returns a vector from an edge or a Part.line"
	# if edge is not straight, you'll get strange results!
	if isinstance(edge,Part.Shape):
		return edge.Vertexes[-1].Point.sub(edge.Vertexes[0].Point)
	elif isinstance(edge,Part.Line):
		return edge.EndPoint.sub(edge.StartPoint)
	else:
		return None

def edg(p1,p2):
	"edge(Vector,Vector) -- returns an edge from 2 vectors"
	if isinstance(p1,FreeCAD.Vector) and isinstance(p2,FreeCAD.Vector):
		if fcvec.equals(p1,p2): return None
		else: return Part.Line(p1,p2).toShape()

def v1(edge):
	"v1(edge) -- returns the first point of an edge"
	return edge.Vertexes[0].Point

def isPtOnEdge(pt,edge) :
	''' Tests if a point is on an edge'''
	if isinstance(edge.Curve,Part.Line) :
		orig = edge.Vertexes[0].Point
		if fcvec.isNull(pt.sub(orig).cross(vec(edge))) :
			return pt.sub(orig).Length <= vec(edge).Length and pt.sub(orig).dot(vec(edge)) >= 0
		else : 
			return False
		
	elif isinstance(edge.Curve,Part.Circle) :
		center = edge.Curve.Center
		axis   = edge.Curve.Axis ; axis.normalize()
		radius = edge.Curve.Radius
		if  round(pt.sub(center).dot(axis),PREC) == 0 \
		and round(pt.sub(center).Length - radius,PREC) == 0 :
			if len(edge.Vertexes) == 1 :
				return True # edge is a complete circle
			else :
				begin = edge.Vertexes[0].Point
				end   = edge.Vertexes[-1].Point
				if fcvec.isNull(pt.sub(begin)) or fcvec.isNull(pt.sub(end)) :
					return True
				else :
					newArc = Part.Arc(begin,pt,end)
					return fcvec.isNull(newArc.Center.sub(center)) and fcvec.isNull(newArc.Axis-axis) \
							and round(newArc.Radius-radius,PREC) == 0
		else :
			return False

	
# edge functions *****************************************************************
	

def findIntersection(edge1,edge2,infinite1=False,infinite2=False) :
	'''findIntersection(edge1,edge2,infinite1=False,infinite2=False):
	returns a list containing the intersection point(s) of 2 edges'''
	
	if isinstance(edge1.Curve,Part.Line) and isinstance(edge2.Curve,Part.Line) :
		
		# deals with 2 lines
		
		pt1, pt2, pt3, pt4 = [edge1.Vertexes[0].Point,edge1.Vertexes[1].Point,
							  edge2.Vertexes[0].Point,edge2.Vertexes[1].Point]
							  
		if fcvec.isNull(pt2.sub(pt1).cross(pt3.sub(pt1)).cross(pt2.sub(pt4).cross(pt3.sub(pt4)))) :
			vec1 = vec(edge1) ; vec2 = vec(edge2)
			vec1.normalize()  ; vec2.normalize()
			cross = vec1.cross(vec2)
			if not fcvec.isNull(cross) :
				k = ((pt3.z-pt1.z)*(vec2.x-vec2.y)+(pt3.y-pt1.y)*(vec2.z-vec2.x)+ \
					 (pt3.x-pt1.x)*(vec2.y-vec2.z))/(cross.x+cross.y+cross.z)
				vec1.scale(k,k,k)
				int = pt1.add(vec1)
				
				if  infinite1 == False and not isPtOnEdge(int,edge1) :
					return []
					
				if  infinite2 == False and not isPtOnEdge(int,edge2) :
					return []
					
				return [int]
			else :
				return [] # Lines have same direction
		else :
			return [] # Lines aren't on same plane
			
	elif isinstance(edge1.Curve,Part.Circle) and isinstance(edge2.Curve,Part.Line) \
	  or isinstance(edge1.Curve,Part.Line)   and isinstance(edge2.Curve,Part.Circle) :
	  	
	  	# deals with an arc or circle and a line
	  	
		edges = [edge1,edge2]
		for edge in edges :
			if isinstance(edge.Curve,Part.Line) :
				line = edge
			else :
				arc  = edge
				
		dirVec = vec(line) ; dirVec.normalize()
		pt1    = line.Vertexes[0].Point
		pt2    = line.Vertexes[1].Point
		center = arc.Curve.Center
		
		if fcvec.isNull(pt1.sub(center).cross(pt2.sub(center)).cross(arc.Curve.Axis)) :
			# Line and Arc are on same plane
			
			dOnLine = center.sub(pt1).dot(dirVec)
			onLine  = Vector(dirVec) ; onLine.scale(dOnLine,dOnLine,dOnLine)
			toLine  = pt1.sub(center).add(onLine)
			
			if toLine.Length < arc.Curve.Radius :
				dOnLine = (arc.Curve.Radius**2 - toLine.Length**2)**(0.5)
				onLine  = Vector(dirVec) ; onLine.scale(dOnLine,dOnLine,dOnLine)
				int = [center.add(toLine).add(onLine)]
				onLine  = Vector(dirVec) ; onLine.scale(-dOnLine,-dOnLine,-dOnLine)
				int += [center.add(toLine).add(onLine)]
			elif round(toLine.Length-arc.Curve.Radius,PREC) == 0 :
				int = [center.add(toLine)]
			else :
				return []
				
		else : # Line isn't on Arc's plane
			if dirVec.dot(arc.Curve.Axis) != 0 :
				toPlane  = Vector(arc.Curve.Axis) ; toPlane.normalize()
				d = vec1.dot(toPlane)
				dToPlane = center.sub(pt1).dot(toPlane)
				toPlane = Vector(vec1)
				toPlane.scale(dToPlane/d,dToPlane/d,dToPlane/d)
				ptOnPlane = toPlane.add(pt1)
				if round(ptOnPlane.sub(center).Length - arc.Curve.Radius,PREC) == 0 :
					int = [ptOnPlane]
				else :
					return []
			else :
				return []
				
		if infinite1 == False :
			for i in range(len(int)-1,-1,-1) :
				if not isPtOnEdge(int[i],edge1) :
					del int[i]
		if infinite2 == False :
			for i in range(len(int)-1,-1,-1) :
				if not isPtOnEdge(int[i],edge2) :
					del int[i]
		return int
		
	elif isinstance(edge1.Curve,Part.Circle) and isinstance(edge2.Curve,Part.Circle) :
		
		# deals with 2 arcs or circles

		cent1, cent2 = edge1.Curve.Center, edge2.Curve.Center
		rad1 , rad2  = edge1.Curve.Radius, edge2.Curve.Radius
		axis1, axis2 = edge1.Curve.Axis  , edge2.Curve.Axis
		c2c          = cent2.sub(cent1)
		
		if fcvec.isNull(axis1.cross(axis2)) :
			if round(c2c.dot(axis1),PREC) == 0 :
				# circles are on same plane
				dc2c = c2c.Length ;
				if not fcvec.isNull(c2c): c2c.normalize()
				if round(rad1+rad2-dc2c,PREC) < 0 \
				or round(rad1-dc2c-rad2,PREC) > 0 or round(rad2-dc2c-rad1,PREC) > 0 :
					return []
				else :
					norm = c2c.cross(axis1)
					if not fcvec.isNull(norm): norm.normalize()
					if fcvec.isNull(norm): x = 0
					else: x = (dc2c**2 + rad1**2 - rad2**2)/(2*dc2c)
					y = abs(rad1**2 - x**2)**(0.5)
					c2c.scale(x,x,x)
					if round(y,PREC) != 0 :
						norm.scale(y,y,y)
						int =  [cent1.add(c2c).add(norm)]
						int += [cent1.add(c2c).sub(norm)]
					else :
						int = [cent1.add(c2c)]
			else :
				return [] # circles are on parallel planes
		else :
			# circles aren't on same plane
			axis1.normalize() ; axis2.normalize()
			U = axis1.cross(axis2)
			V = axis1.cross(U)
			dToPlane = c2c.dot(axis2)
			d        = V.add(cent1).dot(axis2)
			V.scale(dToPlane/d,dToPlane/d,dToPlane/d)
			PtOn2Planes = V.add(cent1)
			planeIntersectionVector = U.add(PtOn2Planes)
			intTemp = findIntersection(planeIntersectionVector,edge1,True,True)
			int = []
			for pt in intTemp :
				if round(pt.sub(cent2).Length-rad2,PREC) == 0 :
					int += [pt]
					
		if infinite1 == False :
			for i in range(len(int)-1,-1,-1) :
				if not isPtOnEdge(int[i],edge1) :
					del int[i]
		if infinite2 == False :
			for i in range(len(int)-1,-1,-1) :
				if not isPtOnEdge(int[i],edge2) :
					del int[i]
		
		return int
	else :
		print "fcgeo: Unsupported curve type"


def mirror (point, edge):
	"finds mirror point relative to an edge"
	normPoint = point.add(findDistance(point, edge, False))
	if normPoint:
		normPoint_point = Vector.sub(point, normPoint)
		normPoint_refl = fcvec.neg(normPoint_point)
		refl = Vector.add(normPoint, normPoint_refl)
		return refl
	else:
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
		print "fcgeo: Couldn't join faces into one"
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

	
def sortEdges(lEdges, aVertex=None):
	"an alternative, more accurate version of Part.__sortEdges__"
	
	def isSameVertex(V1, V2):
		''' Test if vertexes have same coordinates with precision 10E(-PREC)'''
		if round(V1.X-V2.X,1)==0 and round(V1.Y-V2.Y,1)==0 and round(V1.Z-V2.Z,1)==0 :
			return True
		else :
			return False
	
	def lookfor(aVertex, inEdges):
		''' Look for (aVertex, inEdges) returns count, the position of the instance
			the position in the instance and the instance of the Edge'''
		count = 0
		linstances = [] #lists the instances of aVertex
		for i in range(len(inEdges)) :
			for j in range(2) :
				if isSameVertex(aVertex,inEdges[i].Vertexes[j-1]):
					instance = inEdges[i]
					count += 1
					linstances += [i,j-1,instance]
		return [count]+linstances
	
	if (len(lEdges) < 2): return lEdges
	olEdges = [] # ol stands for ordered list 
	if aVertex == None:
		for i in range(len(lEdges)*2) :
			result = lookfor(lEdges[i/2].Vertexes[i%2],lEdges)
			if result[0] == 1 :  # Have we found an end ?
				olEdges = sortEdges(lEdges, result[3].Vertexes[result[2]])
				return olEdges
		# if the wire is closed there is no end so choose 1st Vertex
		return sortEdges(lEdges, lEdges[0].Vertexes[0]) 
	else :
		result = lookfor(aVertex,lEdges)
		if result[0] != 0 :
			del lEdges[result[1]]
			next = sortEdges(lEdges, result[3].Vertexes[-((-result[2])^1)])
			olEdges += [result[3]] + next
			return olEdges
		else :
			return []

def findMidpoint(edge):
	"calculates the midpoint of an edge"
	first = edge.Vertexes[0].Point
	last = edge.Vertexes[-1].Point
	if isinstance(edge.Curve,Part.Circle):
		center = edge.Curve.Center
		radius = edge.Curve.Radius
		axis = edge.Curve.Axis
		chord = fcvec.new(first,last)
		perp = fcvec.crossproduct(chord,axis)
		perp = fcvec.normalized(perp)
		ray = fcvec.new(center,first)
		apothem = fcvec.dotproduct(ray,perp)
		sagitta = radius - apothem
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

def findDistance(point,edge,strict=False):
	'''
	findDistance(vector,edge,[strict]) - Returns a vector from the point to its
	closest point on the edge. If strict is True, the vector will be returned
	only if its endpoint lies on the edge.
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
			print "fcgeo: Couldn't project point"
			return None
	else:
		print "fcgeo: Couldn't project point"
		return None


def angleBisection(edge1, edge2):
	"angleBisection(edge,edge) - Returns an edge that bisects the angle between the 2 edges."
	if isinstance(edge1.Curve, Part.Line) and isinstance(edge2.Curve, Part.Line):
		p1 = edge1.Vertexes[0].Point
		p2 = edge1.Vertexes[-1].Point
		p3 = edge2.Vertexes[0].Point
		p4 = edge2.Vertexes[-1].Point
		int = findIntersection(edge1, edge2, True, True)
		if int:
			line1Dir = p2.sub(p1)
			angleDiff = fcvec.angle(line1Dir, p4.sub(p3))
			ang = angleDiff * 0.5
			origin = int[0]
			dir = fcvec.normalized(line1Dir)
			dir = fcvec.rotate(dir, ang)
			return Part.Line(origin,origin.add(dir)).toShape()
		else:
			diff = p3.sub(p1)
			origin = p1.add(fcvec.scale(diff, 0.5))
			dir = fcvec.normalized(p2.sub(p1))
			return Part.Line(origin,origin.add(dir)).toShape()
	else:
		return None

def findClosestCircle(point,circles):
	"findClosestCircle(Vector, list of circles) -- returns the circle with closest center"
	dist = 1000000
	closest = None
	for c in circles:
		if c.Center.sub(point).Length < dist:
			dist = c.Center.sub(point).Length
			closest = c
	return closest
	
# circle functions *********************************************************


def circleFrom2tan1pt(tan1, tan2, point):
	"circleFrom2tan1pt(edge, edge, Vector)"
	if isinstance(tan1.Curve, Part.Line) and isinstance(tan2.Curve, Part.Line) and isinstance(point, FreeCAD.Vector):
		return circlefrom2Lines1Point(tan1, tan2, point)
	elif isinstance(tan1.Curve, Part.Circle) and isinstance(tan2.Curve, Part.Line) and isinstance(point, FreeCAD.Vector):
		return circlefromCircleLinePoint(tan1, tan2, point)
	elif isinstance(tan2.Curve, Part.Circle) and isinstance(tan1.Curve, Part.Line) and isinstance(point, FreeCAD.Vector):
		return circlefromCircleLinePoint(tan2, tan1, point)
	elif isinstance(tan2.Curve, Part.Circle) and isinstance(tan1.Curve, Part.Circle) and isinstance(point, FreeCAD.Vector):
		return circlefrom2Circles1Point(tan2, tan1, point)

def circleFrom2tan1rad(tan1, tan2, rad):
	"circleFrom2tan1rad(edge, edge, float)"
	if isinstance(tan1.Curve, Part.Line) and isinstance(tan2.Curve, Part.Line):
		return circleFrom2LinesRadius(tan1, tan2, rad)
	elif isinstance(tan1.Curve, Part.Circle) and isinstance(tan2.Curve, Part.Line):
		return circleFromCircleLineRadius(tan1, tan2, rad)
	elif isinstance(tan1.Curve, Part.Line) and isinstance(tan2.Curve, Part.Circle):
		return circleFromCircleLineRadius(tan2, tan1, rad)
	elif isinstance(tan1.Curve, Part.Circle) and isinstance(tan2.Curve, Part.Circle):
		return circleFrom2CirclesRadius(tan1, tan2, rad)

def circleFrom1tan2pt(tan1, p1, p2):
	if isinstance(tan1.Curve, Part.Line) and isinstance(p1, FreeCAD.Vector) and isinstance(p2, FreeCAD.Vector):
		return circlefrom1Line2Points(tan1, p1, p2)
	if isinstance(tan1.Curve, Part.Line) and isinstance(p1, FreeCAD.Vector) and isinstance(p2, FreeCAD.Vector):
		return circlefrom1Circle2Points(tan1, p1, p2)

def circleFrom1tan1pt1rad(tan1, p1, rad):
	if isinstance(tan1.Curve, Part.Line) and isinstance(p1, FreeCAD.Vector):
		return circleFromPointLineRadius(p1, tan1, rad)
	if isinstance(tan1.Curve, Part.Circle) and isinstance(p1, FreeCAD.Vector):
		return circleFromPointCircleRadius(p1, tan1, rad)

def circleFrom3tan(tan1, tan2, tan3):
	tan1IsLine = isinstance(tan1.Curve, Part.Line)
	tan2IsLine = isinstance(tan2.Curve, Part.Line)
	tan3IsLine = isinstance(tan3.Curve, Part.Line)
	tan1IsCircle = isinstance(tan1.Curve, Part.Circle)
	tan2IsCircle = isinstance(tan2.Curve, Part.Circle)
	tan3IsCircle = isinstance(tan3.Curve, Part.Circle)
	if tan1IsLine and tan2IsLine and tan3IsLine:
		return circleFrom3LineTangents(tan1, tan2, tan3)
	elif tan1IsCircle and tan2IsCircle and tan3IsCircle:
		return circleFrom3CircleTangents(tan1, tan2, tan3)
	elif (tan1IsCircle and tan2IsLine and tan3IsLine):
		return circleFrom1Circle2Lines(tan1, tan2, tan3)
	elif (tan1IsLine and tan2IsCircle and tan3IsLine):
		return circleFrom1Circle2Lines(tan2, tan1, tan3)
	elif (tan1IsLine and tan2IsLine and tan3IsCircle):
		return circleFrom1Circle2Lines(tan3, tan1, tan2)
	elif (tan1IsLine and tan2IsCircle and tan3IsCircle):
		return circleFrom2Circle1Lines(tan2, tan3, tan1)
	elif (tan1IsCircle and tan2IsLine and tan3IsCircle):
		return circleFrom2Circle1Lines(tan1, tan3, tan2)
	elif (tan1IsCircle and tan2IsCircle and tan3IsLine):
		return circleFrom2Circle1Lines(tan1, tan2, tan3)

def circlefrom2Lines1Point(edge1, edge2, point):
	"circlefrom2Lines1Point(edge, edge, Vector)"
	bis = angleBisection(edge1, edge2)
	if not bis: return None
	mirrPoint = mirror(point, bis)
	return circlefrom1Line2Points(edge1, point, mirrPoint)

def circlefrom1Line2Points(edge, p1, p2):
	"circlefrom1Line2Points(edge, Vector, Vector)"
	p1_p2 = edg(p1, p2)
	s = findIntersection(edge, p1_p2, True, True)
	if not s: return None
	s = s[0]
	v1 = p1.sub(s)
	v2 = p2.sub(s)
	projectedDist = math.sqrt(abs(fcvec.dotproduct(v1, v2)))
	edgeDir = fcvec.normalized(vec(edge))
	projectedCen1 = Vector.add(s, fcvec.scale(edgeDir, projectedDist))
	projectedCen2 = Vector.add(s, fcvec.scale(edgeDir, -projectedDist))
	perpEdgeDir = fcvec.crossproduct(edgeDir)
	perpCen1 = Vector.add(projectedCen1, perpEdgeDir)
	perpCen2 = Vector.add(projectedCen2, perpEdgeDir)
	mid = findMidpoint(p1_p2)
	perp_mid = Vector.add(mid, fcvec.normalized(fcvec.crossproduct(vec(p1_p2))))
	cen1 = findIntersection(edg(projectedCen1, perpCen1), edg(mid, perp_mid), True, True)
	cen2 = findIntersection(edg(projectedCen2, perpCen2), edg(mid, perp_mid), True, True)
	circles = []
	if cen1:
		radius = fcvec.dist(projectedCen1, cen1[0])
		circles.append(Part.Circle(cen1[0], NORM, radius))
	if cen2:
		radius = fcvec.dist(projectedCen2, cen2[0])
		circles.append(Part.Circle(cen2[0], NORM, radius))

	if circles: return circles
	else: return None

def circleFrom2LinesRadius (edge1, edge2, radius):
	"circleFrom2LinesRadius(edge,edge,radius)"
	int = findIntersection(edge1, edge2, True, True)
	if not int: return None
	int = int[0]
	bis12 = angleBisection(edge1,edge2)
	bis21 = Part.Line(bis12.Vertexes[0].Point,fcvec.rotate(vec(bis12), math.pi/2.0))
	ang12 = abs(fcvec.angle(vec(edge1),vec(edge2)))
	ang21 = math.pi - ang12
	dist12 = radius / math.sin(ang12 * 0.5)
	dist21 = radius / math.sin(ang21 * 0.5)
	circles = []
	cen = Vector.add(int, fcvec.scale(vec(bis12), dist12))
	circles.append(Part.Circle(cen, NORM, radius))
	cen = Vector.add(int, fcvec.scale(vec(bis12), -dist12))
	circles.append(Part.Circle(cen, NORM, radius))
	cen = Vector.add(int, fcvec.scale(vec(bis21), dist21))
	circles.append(Part.Circle(cen, NORM, radius))
	cen = Vector.add(int, fcvec.scale(vec(bis21), -dist21))
	circles.append(Part.Circle(cen, NORM, radius))
	return circles

def circleFrom3LineTangents (edge1, edge2, edge3):
	"circleFrom3LineTangents(edge,edge,edge)"
	def rot(ed):
		return Part.Line(v1(ed),v1(ed).add(fcvec.rotate(vec(ed),math.pi/2))).toShape()
	bis12 = angleBisection(edge1,edge2)
	bis23 = angleBisection(edge2,edge3)
	bis31 = angleBisection(edge3,edge1)
	intersections = []
	int = findIntersection(bis12, bis23, True, True)	
	if int:
		radius = findDistance(int[0],edge1).Length
		intersections.append(Part.Circle(int[0],NORM,radius))
	int = findIntersection(bis23, bis31, True, True)
	if int:
		radius = findDistance(int[0],edge1).Length
		intersections.append(Part.Circle(int[0],NORM,radius))
	int = findIntersection(bis31, bis12, True, True)
	if int:
		radius = findDistance(int[0],edge1).Length
		intersections.append(Part.Circle(int[0],NORM,radius))
	int = findIntersection(rot(bis12), rot(bis23), True, True)
	if int:
		radius = findDistance(int[0],edge1).Length
		intersections.append(Part.Circle(int[0],NORM,radius))
	int = findIntersection(rot(bis23), rot(bis31), True, True)
	if int:
		radius = findDistance(int[0],edge1).Length
		intersections.append(Part.Circle(int[0],NORM,radius))
	int = findIntersection(rot(bis31), rot(bis12), True, True)
	if int:
		radius = findDistance(int[0],edge1).Length
		intersections.append(Part.Circle(int[0],NORM,radius))
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

def circleFromPointLineRadius (point, edge, radius):
 	"circleFromPointLineRadius (point, edge, radius)"
	dist = findDistance(point, edge, False)
	center1 = None
	center2 = None
	if dist.Length == 0:
		segment = vec(edge)
		perpVec = fcvec.normalized(fcvec.crossproduct(segment))
		normPoint_c1 = fcvec.scale(perpVec, radius)
		normPoint_c2 = fcvec.scale(perpVec, -radius)
		center1 = point.add(normPoint_c1)
		center2 = point.add(normPoint_c2)
	elif dist.Length > 2 * radius:
		return None
	elif dist.Length == 2 * radius:
		normPoint = point.add(findDistance(point, edge, False))
		dummy = fcvec.scale(normPoint.sub(point), 0.5)
		cen = point.add(dummy)
		circ = Part.Circle(cen, NORM, radius)
		if circ:
			return [circ]
		else:
			return None
	else:
		normPoint = point.add(findDistance(point, edge, False))
		normDist = fcvec.dist(normPoint, point)
		dist = math.sqrt(radius**2 - (radius - normDist)**2)
		centerNormVec = fcvec.scale(fcvec.normalized(point.sub(normPoint)), radius)
		edgeDir = fcvec.normalized(edge.Vertexes[0].Point.sub(normPoint))
		center1 = centerNormVec.add(normPoint.add(fcvec.scale(edgeDir, dist)))
		center2 = centerNormVec.add(normPoint.add(fcvec.scale(edgeDir, -dist)))
	circles = []
	if center1:
		circ = Part.Circle(center1, NORM, radius)
		if circ:
			circles.append(circ)
	if center2:
		circ = Part.Circle(center2, NORM, radius)
		if circ:
			circles.append(circ)

	if len(circles):
		return circles
	else:
		return None

def circleFrom2PointsRadius(p1, p2, radius):
	"circleFrom2PointsRadiust(Vector, Vector, radius)"
	if fcvec.equals(p1, p2): return None

	p1_p2 = Part.Line(p1, p2).toShape()
	dist_p1p2 = fcvec.dist(p1, p1)
	mid = findMidpoint(p1_p2)
	if dist_p1p2 == 2*radius:
		circle = Part.Circle(mid, norm, radius)
		if circle: return [circle]
		else: return None
	dir = fcvec.normalized(vec(p1_p2))
	perpDir = fcvec.normalized(fcvec.crossproduct(dir))
	dist = math.sqrt(radius**2 - (dist_p1p2 / 2.0)**2)
	cen1 = Vector.add(mid, fcvec.scale(perpDir, dist))
	cen2 = Vector.add(mid, fcvec.scale(perpDir, -dist))
	circles = []
	if cen1: circles.append(Part.Circle(cen1, norm, radius))
	if cen2: circles.append(Part.Circle(cen2, norm, radius))
	if circles: return circles
	else: return None



#############################33 to include







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


def determinant (mat,n):
	'''
	determinant(matrix,int) - Determinat function. Returns the determinant
	of a n-matrix. It recursively expands the minors.
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
		nearest = circle.Curve.Center.add(findDistance(circle.Curve.Center, edge, False))
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

