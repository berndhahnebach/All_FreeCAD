
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

__title__="FreeCAD Draft Workbench"
__author__ = "Yorik van Havre, Werner Mayer, Martin Burbaum"
__url__ = "http://yorik.orgfree.com"
__version__ = "0.1.4"

'''
General description:

	The Draft module is a FreeCAD module for drawing/editing 2D entities.
	The aim is to give FreeCAD basic 2D-CAD capabilities (similar
	to Autocad and other similar software). This modules is made to be run
	inside FreeCAD and needs the PyQt4 and pivy modules availible to python.
	A complete development plan of this module can be found at
	http://yorik.orgfree.com/scripts/FreeCAD-Draft.html
	and a user manual is at
	http://juergen-riegel.net/FreeCAD/Docu/index.php?title=Draft_Module

Contents:

	- General functions: snap, constraint, format
	- Trackers, for drawing temporary stuff: snaps, lines, arcs or ghosts
	- Geometry Constructors: line, circle, rectangle, text
	- Geometry Modifiers: move, rotate, offset, upgrade, downgrade, trimex
	- FreeCAD Commands

Todo:

	- upgrade offset, dxf export and trimex to wires containing arcs
	- fix offset of rectangles
	- support unicode in texts
	- heavy bugs fix
	- dimension tool
'''

# import FreeCAD modules
import FreeCAD, FreeCADGui, Part, math
from FreeCAD import Base
from pivy import coin
from draftlibs import fcvec

snapStack=[] #storing two last snapped objects, so we can check for intersection

#---------------------------------------------------------------------------
# General common functions used by the constructors
#---------------------------------------------------------------------------

def snapPoint (target,point,cursor,ctrl=False):
	'''
	Snap function

	Currently has two modes: passive and active. Pressing CTRL while 
	clicking puts you in active mode:

	- In passive mode (an open circle appears), your point is
	  snapped to the nearest point on any underlying geometry.

	- In active mode (ctrl pressed, a filled circle appears), your point
	  can currently be snapped to the following points:
	    - Nodes and midpoints of all Part shapes
	    - Nodes and midpoints of lines/polylines
	    - Centers and quadrant points of circles
	    - Endpoints of arcs
	    - Intersection of line/polylines segments
	    - When constrained (SHIFT pressed), Intersections between
	      constraining axis and lines/polylines
	'''
	snapped=target.view.getObjectInfo((cursor[0],cursor[1]))
	if (snapped == None):
		target.snap.switch.whichChild = -1
		target.snap.isSnapping = False
		return point
	else:
		# building snap array
		ob = target.doc.getObject(snapped['Object'])

		# snapArray contains lists of 3 items: point that activates
		# snap, type, point to snap to. Type is 0=vertex point (filled
		# circle), 1=calculated point (filled square), 2=other (open
		# circle), 3=open square (intersection)
		snapArray=[] 

		if (len(snapStack)==0): snapStack.append(ob)
		elif (len(snapStack)==1) and (snapStack[0]!=ob): snapStack.append(ob)
		elif (len(snapStack)==2) and (snapStack[0]!=ob) and (snapStack[1]!=ob):
			snapStack[0] = snapStack[1]
			snapStack[1] = ob

		if (ctrl) and (ob.Type == "Part::Feature"):
			for i in ob.Shape.Vertexes:
				snapArray.append([i.Point,0,i.Point])
			for j in ob.Shape.Edges:
				if (isinstance (j.Curve,Part.Line)):
					p1 = j.Vertexes[0].Point
					p2 = j.Vertexes[1].Point
					halfedge = fcvec.scale(fcvec.sub(p2,p1),.5)
					midpoint = fcvec.add(p1,halfedge)
					snapArray.append([midpoint,1,midpoint])
					if (len(target.node) > 0):
						last = target.node[len(target.node)-1]
						if (target.constrain == 0):
							if ((last.y > p1.y) and (last.y < p2.y) or (last.y > p2.y) and (last.y < p1.y)):
								pc = (last.y-p1.y)/(p2.y-p1.y)
								constrainpoint = (FreeCAD.Vector(p1.x+pc*(p2.x-p1.x),p1.y+pc*(p2.y-p1.y),p1.z+pc*(p2.z-p1.z)))
								snapArray.append([constrainpoint,1,constrainpoint]) # constrainpoint
						if (target.constrain == 1):
							if ((last.x > p1.x) and (last.x < p2.x) or (last.x > p2.x) and (last.x < p1.x)):
								pc = (last.x-p1.x)/(p2.x-p1.x)
								constrainpoint = (FreeCAD.Vector(p1.x+pc*(p2.x-p1.x),p1.y+pc*(p2.y-p1.y),p1.z+pc*(p2.z-p1.z)))
								snapArray.append([constrainpoint,1,constrainpoint]) # constrainpoint
				if (len(snapStack) == 2):
					last = snapStack[0]
					if (last.Type == "Part::Feature"):
						for k in last.Shape.Edges:
							pt = findIntersection(j,k)
							if pt: snapArray.append([pt,3,pt])

				elif isinstance (j.Curve,Part.Circle):
					rad = j.Curve.Radius
					pos = j.Curve.Center
					for i in [0,30,45,60,90,120,135,150,180,210,225,240,270,300,315,330]:
						ang = math.radians(i)
						cur = FreeCAD.Vector(math.sin(ang)*rad+pos.x,math.cos(ang)*rad+pos.y,pos.z)
						snapArray.append([cur,1,cur])
					for i in [15,37.5,52.5,75,105,127.5,142.5,165,195,217.5,232.5,255,285,307.5,322.5,345]:
						ang = math.radians(i)
						cur = FreeCAD.Vector(math.sin(ang)*rad+pos.x,math.cos(ang)*rad+pos.y,pos.z)
						snapArray.append([cur,0,pos])

		else:
			cur = FreeCAD.Vector(snapped['x'],snapped['y'],snapped['z'])
			snapArray = [[cur,2,cur]]

		# calculating shortest distance
		shortest = 1000000000000000000
		newpoint = point
		for i in snapArray:
			sqdist = ((point.x-i[0].x)**2 + (point.y-i[0].y)**2 + (point.z-i[0].z)**2)
			if sqdist < shortest:
				shortest = sqdist
				newpoint = i
		target.snap.coords.point.setValue((newpoint[2].x,newpoint[2].y,newpoint[2].z))
		if (newpoint[1] == 1):
			target.snap.marker.markerIndex = coin.SoMarkerSet.SQUARE_FILLED_9_9
		elif (newpoint[1] == 0):
			target.snap.marker.markerIndex = coin.SoMarkerSet.CIRCLE_FILLED_9_9
		elif (newpoint[1] == 3):
			target.snap.marker.markerIndex = coin.SoMarkerSet.SQUARE_LINE_9_9
		else:
			target.snap.marker.markerIndex = coin.SoMarkerSet.CIRCLE_LINE_9_9
		if (target.snap.isSnapping == False):
			target.snap.switch.whichChild = 0
			target.snap.isSnapping = True
		return newpoint[2]

def findIntersection(edge1,edge2,infinite1=False,infinite2=False):
	'''
	finds intersection point between 2 edges, if any. Currently only works in 2D
	if infinite1 is True, edge1 will be considered infinite
	if infinite2 is True, edge2 will be considered infinite
	algorithm from http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/
	'''
	# currently doesn't work for curved edges!
	if (isinstance(edge1.Curve,Part.Circle)) or (isinstance(edge2.Curve,Part.Circle)):
		return None
	p1 = edge1.Vertexes[0].Point
	p2 = edge1.Vertexes[1].Point
	p3 = edge2.Vertexes[0].Point
	p4 = edge2.Vertexes[1].Point
	numa = (p4.x-p3.x)*(p1.y-p3.y)-(p4.y-p3.y)*(p1.x-p3.x)
	numb = (p2.x-p1.x)*(p1.y-p3.y)-(p2.y-p1.y)*(p1.x-p3.x)
	denom = (p4.y-p3.y)*(p2.x-p1.x)-(p4.x-p3.x)*(p2.y-p1.y)
	if (denom == 0): return None
	ua = numa/denom
	ub = numb/denom
	if not infinite1:
		if (ua > 1) or (ua < 0): return None
	if not infinite2:
		if (ub > 1) or (ub < 0): return None
	x = p1.x + ua*(p2.x-p1.x)
	y = p1.y + ua*(p2.y-p1.y)
	return FreeCAD.Vector(x,y,p1.z)

def findClosest(basepoint,pointslist):
	"in a list of 3d points, finds the closest point to the base point"
	if not pointslist: return None
	smallest = 100000
	for n in range(len(pointslist)):
		new = fcvec.new(basepoint,pointslist[n]).Length
		if new < smallest:
			smallest = new
			npoint = n
	return npoint

def constrainPoint (target,point):
	'''
	Constrain function

	On commands that need to enter several points (currently only line/polyline), you can constrain
	the next point to be picked to the last drawn point by pressing SHIFT. The vertical or horizontal
	constraining depends on the position of your mouse in relation to last point 
	at the moment you press SHIFT.
	'''
	if len(target.node) > 0:
		last = target.node[len(target.node)-1]
		if (target.constrain == None):
			if (abs(point.x-last.x) > abs(point.y-last.y)):
				point.y = last.y
				target.constrain = 0 #horizontal
				target.ui.yValue.setEnabled(False)
			else:
				point.x = last.x
				target.constrain = 1 #vertical
				target.ui.xValue.setEnabled(False)
		elif (target.constrain == 0):
			point.y = last.y
		else:
			point.x = last.x
	return point

def formatObject(target,origin=None):
	'''
	this function applies to the given object the current properties 
	set on the toolbar (line color and line width), or of another object if given
	'''
	obrep = FreeCADGui.ActiveDocument.getObject(target.Name)
	ui= FreeCADGui.activeWorkbench().draftToolBar.ui
	r = float(ui.color.red()/255.0)
	g = float(ui.color.green()/255.0)
	b = float(ui.color.blue()/255.0)
	lw = float(ui.widthButton.value())
	col = (r,g,b,0.0)
	if (target.Type == "App::Annotation"):
		obrep.FontSize=int(lw*9)
		obrep.TextColor=col
	else:
		if not origin:
			obrep.LineWidth = lw
			obrep.LineColor = col
		else:
			matchrep = FreeCADGui.ActiveDocument.getObject(origin.Name)
			obrep.LineWidth = matchrep.LineWidth
			obrep.LineColor = matchrep.LineColor
			obrep.ShapeColor = matchrep.ShapeColor

def displayPoint(point, last=None):
	"this sends the current point info to the Toolbar"
	ui = FreeCADGui.activeWorkbench().draftToolBar.ui
	dPoint = point
	if ui.isRelative.isChecked():
		if last != None: dPoint = fcvec.sub(point,last)
	ui.xValue.setText("%.2f" % dPoint.x)
	ui.yValue.setText("%.2f" % dPoint.y)
	if ui.zValue.isEnabled(): ui.zValue.setText("%.2f" % dPoint.z)
	if ui.xValue.isEnabled():
		ui.xValue.setFocus()
		ui.xValue.selectAll()
	else:
		ui.yValue.setFocus()
		ui.yValue.selectAll()

def select(object):
	"deselects everything and selects the passed object"
	FreeCADGui.Selection.clearSelection()
	FreeCADGui.Selection.addSelection(object)

def concatenate(shape):
	"turns several faces into one"
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
	bound = Part.__sortEdges__(bound) 
	try:
		wire=Part.Wire(bound)
		face=Part.Face(wire)
	except:
		print "draft: Couldn't join faces into one"
		return(shape)
	else:
		if not wire.isClosed():	return(wire)
		else: return(face)

def complexity(obj):
	"tests given object for shape complexity"
	shape = obj.Shape
	if shape.Faces:
		for e in shape.Edges:
			if (isinstance(e.Curve,Part.Circle)): return 8 # faces & closed wires w/ arcs
		return 7 # contains faces and closed wires
	if shape.Wires:
		for e in shape.Edges:
			if (isinstance(e.Curve,Part.Circle)): return 6 # contains wires w/ arcs
		for w in shape.Wires:
			if w.isClosed(): return 5 # contains closed wires
		return 4 # open wire with no arcs
	if (isinstance(shape.Edges[0].Curve,Part.Circle)):
		if len(shape.Vertexes) == 1:
			return 3 # contains closed circles
		return 2 # contains arcs
	return 1 # then, this is a line!
		
class snapTracker:
	"a class to create a snap marker symbol, used by the functions that support snapping"
	def __init__(self):
		snapcolor = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft").GetBool("snapcolor")
		color = coin.SoBaseColor()
		color.rgb = (0,0,0)
		self.marker = coin.SoMarkerSet() # this is the marker symbol
		self.marker.markerIndex = coin.SoMarkerSet.CIRCLE_FILLED_9_9
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValue((0,0,0))
		node = coin.SoAnnotation()
		node.addChild(self.coords)
		node.addChild(color)
		node.addChild(self.marker)
		self.switch = coin.SoSwitch() # this is the on/off switch
		self.switch.addChild(node)
		sg=FreeCADGui.ActiveDocument.ActiveView.getSceneGraph()
		sg.addChild(self.switch)
		self.switch.whichChild = -1
		self.isSnapping = False # this keeps if we are already snapping or not

	def __del__(self):
		self.switch.whichChild = -1

class lineTracker:
	"a class to create a tracking line used by the functions that need it"
	def __init__(self,dotted=False):
		color = coin.SoBaseColor()
		color.rgb = (0,0,0)
		line = coin.SoLineSet()
		line.numVertices.setValue(2)
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValues(0,2,[[0,0,0],[1,0,0]])
		node = coin.SoSeparator()
		if dotted:
			print "dotted line"
			drawstyle = coin.SoDrawStyle()
			drawstyle.style = coin.SoDrawStyle.LINES
			drawstyle.lineWeight = 3
			drawstyle.linePattern = 0xaa
			node.addChild(drawstyle)
		node.addChild(color)
		node.addChild(self.coords)
		node.addChild(line)
		self.switch = coin.SoSwitch() # this is the on/off switch
		self.switch.addChild(node)
		sg=FreeCADGui.ActiveDocument.ActiveView.getSceneGraph()
		sg.addChild(self.switch)
		self.switch.whichChild = -1

	def __del__(self):
		self.switch.whichChild = -1

	def p1(self,point):
		self.coords.point.set1Value(0,point.x,point.y,point.z)

	def p2(self,point):
		self.coords.point.set1Value(1,point.x,point.y,point.z)

	def on(self):
		self.switch.whichChild = 0

	def off(self):
		self.switch.whichChild = -1

class rectangleTracker:
	"a class to create a tracking line used by the functions that need it"
	def __init__(self):
		color = coin.SoBaseColor()
		color.rgb = (0,0,0)
		line = coin.SoLineSet()
		line.numVertices.setValue(5)
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValues(0,5,[[0,0,0],[1,0,0],[1,1,0],[0,1,0],[0,0,0]])
		node = coin.SoSeparator()
		node.addChild(color)
		node.addChild(self.coords)
		node.addChild(line)
		self.switch = coin.SoSwitch() # this is the on/off switch
		self.switch.addChild(node)
		sg=FreeCADGui.ActiveDocument.ActiveView.getSceneGraph()
		sg.addChild(self.switch)
		self.origin = FreeCAD.Vector(0,0,0)
		self.switch.whichChild = -1

	def __del__(self):
		self.switch.whichChild = -1

	def setorigin(self,point):
		self.coords.point.set1Value(0,point.x,point.y,point.z)
		self.coords.point.set1Value(4,point.x,point.y,point.z)
		self.origin = point

	def update(self,point):
		self.coords.point.set1Value(1,point.x,self.origin.y,self.origin.z)
		self.coords.point.set1Value(2,point.x,point.y,self.origin.z)
		self.coords.point.set1Value(3,self.origin.x,point.y,self.origin.z)

	def on(self):
		self.switch.whichChild = 0

	def off(self):
		self.switch.whichChild = -1

class arcTracker:
	"a class to create a tracking arc/circle, used by the functions that need it"
	def __init__(self):
		color = coin.SoBaseColor()
		color.rgb = (0,0,0)
		self.coords = coin.SoCoordinate4()
		trackpts = [[1,0,0,1],[0.707107,0.707107,0,0.707107],[0,1,0,1],
			[-0.707107,0.707107,0,0.707107],[-1,0,0,1],[-0.707107,-0.707107,0,0.707107],
			[0,-1,0,1],[0.707107,-0.707107,0,0.707107],[1,0,0,1]]
		self.coords.point.setValues(0,9,trackpts)
		self.circle = coin.SoNurbsCurve()
		self.circle.numControlPoints.setValue(9)
		self.circle.knotVector.setValues([0.00,0.0,0.0,0.25,0.25,0.5,0.5,0.75,0.75,1.0,1.0,1.00])
		self.trans = coin.SoTransform()
		self.trans.translation.setValue([0,0,0])
		self.trans.scaleFactor.setValue([0,0,0])
		self.trans.rotation.setValue(coin.SbVec3f(0,0,1),0)
		node = coin.SoSeparator()
		node.addChild(color)
		node.addChild(self.coords)
		node.addChild(self.trans)
		node.addChild(self.circle)
		self.switch = coin.SoSwitch()
		self.switch.addChild(node)
		self.switch.whichChild = -1
		sg=FreeCADGui.ActiveDocument.ActiveView.getSceneGraph()
		sg.addChild(self.switch)

	def __del__(self):
		self.switch.whichChild = -1

	def on(self):
		self.switch.whichChild = 0

	def off(self):
		self.switch.whichChild = -1

	def update(self,angle):
		'''
		this function creates a nurbs representation of an 1-unit arc of a given angle. 
		Freely adapted from the NURBS python module
		'''
		# determining how many additional arcs we will need, and sets knotvectors
		if (abs(angle) <= math.pi/2):
			knots = [0., 0., 0., 1., 1., 1.]
			narcs = 0
			control = [[1,0,0,1]]
		elif (abs(angle) <= math.pi):
			knots = [0., 0., 0., 0.5, 0.5, 1., 1., 1.]
			narcs = 1
			control = [[1,0,0,1],[0.707107,0.707107,0,0.707107],[0,1,0,1]]
		elif (abs(angle) <= 3*math.pi/2):
			knots = [0., 0., 0., 1./3., 1./3., 2./3., 2./3., 1., 1., 1.]
			narcs = 2
			control = [[1,0,0,1],[0.707107,0.707107,0,0.707107],
				[0,1,0,1],[-0.707107,0.707107,0,0.707107],[-1,0,0,1]]
		else:
			knots = [0., 0., 0., 0.25, 0.25, 0.5, 0.5, 0.75, 0.75, 1., 1., 1.]
			narcs = 3
			control = [[1,0,0,1],[0.707107,0.707107,0,0.707107],[0,1,0,1],
				[-0.707107,0.707107,0,0.707107],[-1,0,0,1],[-0.707107,-0.707107,0,0.707107],[0,-1,0,1]]

		# inverting y value if sweeping is negative (counterclockwise)
		if (angle < 0):
			for i in control:
				i[1] = -i[1]

		# drawing main arc
		sweep = abs(angle) - (narcs*math.pi/2)
		if (angle < 0):
			halfangle = angle + (sweep/2)
		else:
			halfangle = angle - (sweep/2)
		x2 = math.cos(halfangle)
		y2 = math.sin(halfangle)
		x3 = math.cos(angle)
		y3 = math.sin(angle)
		att = math.cos(sweep/2)
		control.extend([[x2,y2,0,att],[x3,y3,0,1]])
		pts = len(control)

		# updating
		self.coords.point.setValues(0,pts,control)
		self.circle.numControlPoints.setValue(pts)
		self.circle.knotVector.setValues(knots)
		self.circle.knotVector.setNum(len(knots))

class ghostTracker:
	"this class creates a copy of the coin representation of passed objects, to be used as ghost"
	def __init__(self,sel,single=None):
		sg=FreeCADGui.ActiveDocument.ActiveView.getSceneGraph()
		if single == None:
			node = coin.SoSeparator()
			self.switch = coin.SoSwitch()
			self.trans = coin.SoTransform()
			self.trans.translation.setValue([0,0,0])
			node.addChild(self.trans)
			ivsep = coin.SoSeparator()
			try:
				for i in sel:
					ivin = coin.SoInput()
					ivin.setBuffer(FreeCADGui.ActiveDocument.getObject(i.Name).toString())
					ivob = coin.SoDB.readAll(ivin)
					ivsep.addChild(ivob.getChildren()[1])
			except:
				print "draft: Couldn't create ghost"
			else:
				self.switch.addChild(ivsep)
				self.switch.whichChild = -1
				node.addChild(self.switch)
				sg.addChild(node)
		else:
			try:
				ivin = coin.SoInput()
				data = FreeCADGui.ActiveDocument.getObject(sel.Name).toString()
				# print data
				ivin.setBuffer(data)
				# print "debug: ghosttracker: valid iv node  ",bool(ivin.isValidBuffer())
				self.node = coin.SoDB.readAll(ivin)
				sg.addChild(self.node)
			except:
				print "draft: Couldn't create ghost"
			else:
				self.trans = self.node.getChild(0)
				self.switch = self.node.getChild(1)
				self.group = self.switch.getChild(0).getChild(2)
				self.group.getChildren().truncate(4) # keep only one segment
				self.coords = self.group.getChild(2)

	def __del__(self):
		self.switch.whichChild = -1

	def on(self):
		self.switch.whichChild = 0

	def off(self):
		self.switch.whichChild = -1



#---------------------------------------------------------------------------
# Geometry constructors
#---------------------------------------------------------------------------

class line:
	'''
	This class creates a line or group of lines feature. 
	Takes 1 optional argument, the max number of points.

	USAGE:

	It asks the user for points, until the max. number of points is reached. For each point, the user can: 
	-pick a point somewhere on screen with the mouse
	-pick a point on an existing object, 2d or 3d
	-double-click the last point to finish, or press F
	-on numeric input, pressing Enter passes from x to y, from y to z, and from z it adds the point (if valid)
	-press C to finish, closing the line (if more than 2 points)
	-press Z to undo last point (provisory, conflicts using CTRL+Z...)
	-press CTRL to snap
	-press SHIFT to constrain vertically or horizontally
	'''
	def __init__(self):
		self.max=2
	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			self.constrain = None
			self.featureName = "Line"
			self.view = FreeCADGui.ActiveDocument.ActiveView
			self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
			FreeCADGui.activeWorkbench().activeDraftCommand = self
			self.node = []
			self.obj = None
			self.pos = []
			self.ui.sourceCmd = self
			self.ui.lineUi()
			self.call = self.view.addEventCallback("SoEvent",self.action)
			self.snap = snapTracker()
			self.linetrack = lineTracker()
			self.constraintrack = lineTracker(dotted=True)
			FreeCAD.Console.PrintMessage("Pick first point:\n")

	def finish(self,closed=False):
		"terminates the operation and closes the poly if asked"
		if len(self.node) > 1:
			if len(self.node) > 2:
				if (closed):
					currentshape = self.obj.Shape
					first = self.node[0]
					last = self.node[len(self.node)-1]
					newseg = Part.Line(last,first).toShape()
					e=currentshape.Edges
					e.append(newseg)
					newshape=Part.Wire(e)
					self.obj.Shape = newshape
		else:
			if len(self.node) == 1: self.doc.undo()
		self.node=[]
		self.view.removeEventCallback("SoEvent",self.call)
		self.ui.offUi()
		self.ui.sourceCmd=None
		del self.linetrack
		del self.constraintrack
		del self.snap
		FreeCADGui.activeWorkbench().activeDraftCommand = None

	def createObject(self):
		"creates an object in the current doc"
		self.doc.openTransaction("Create "+self.featureName) 
		self.obj=self.doc.addObject("Part::Feature",self.featureName)
		self.doc.commitTransaction()
		formatObject(self.obj)
		select(self.obj)

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			point = snapPoint(self,point,cursor,arg["CtrlDown"])
			ctrlPoint = FreeCAD.Vector(point.x,point.y,point.z)
			if (arg["ShiftDown"]): # constraining
				point = constrainPoint(self,point)
			else:
				self.constrain = None
				self.ui.xValue.setEnabled(True)
				self.ui.yValue.setEnabled(True)
			if not self.ui.zValue.isEnabled(): point.z = float(self.ui.zValue.text())
			self.linetrack.p2(point)
			# Draw constraint tracker line.
			if (arg["ShiftDown"]):
				self.constraintrack.p1(point)
				self.constraintrack.p2(ctrlPoint)
				self.constraintrack.on()
			else: self.constraintrack.off()
			if (len(self.node)>0): displayPoint(point, self.node[-1])
			else: displayPoint(point)

		elif (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				if (arg["Position"] == self.pos):
					self.finish(False)
				else:
					self.pos = arg["Position"]
					point = self.view.getPoint(self.pos[0],self.pos[1])
					point = snapPoint(self,point,self.pos,arg["CtrlDown"])
					if (arg["ShiftDown"]): 
						point = constrainPoint(self,point)
					else:
						self.constrain = None
					if not self.ui.zValue.isEnabled(): point.z = float(self.ui.zValue.text())
					self.node.append(point)
					self.linetrack.p1(point)
					self.drawSegment(point)
					if (len(self.node) == self.max):
						self.finish(False)
					if (len(self.node) > 2):
						if fcvec.equals(point,self.node[0]):
							self.undolast()
							self.finish(True)

	def undolast(self):
		"undoes last line segment"
		if (len(self.node) > 1):
			self.node.pop()
			last = self.node[len(self.node)-1]
			self.linetrack.p1(last)
			if (len(self.node) > 2):
				edges = self.obj.Shape.Edges
				edges.pop()
				newshape = edges[0]
				for i in range(1,len(edges)): newshape = newshape.fuse(edges[i])
				self.obj.Shape = newshape

	def drawSegment(self,point):
		"draws a new segment"
		if (len(self.node) == 1):
			self.linetrack.on()
			FreeCAD.Console.PrintMessage("Pick next point:\n")
		elif (len(self.node) == 2):
			self.createObject()
			last = self.node[len(self.node)-2]
			newseg = Part.Line(last,point).toShape()
			self.obj.Shape = newseg
			FreeCAD.Console.PrintMessage("Pick next point, or (F)inish or (C)lose:\n")
		else:
			currentshape = self.obj.Shape
			last = self.node[len(self.node)-2]
			newseg = Part.Line(last,point).toShape()
			newshape=currentshape.fuse(newseg)
			self.obj.Shape = newshape
			FreeCAD.Console.PrintMessage("Pick next point, or (F)inish or (C)lose:\n")

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		point = FreeCAD.Vector(numx,numy,numz)
		self.node.append(point)
		self.linetrack.p1(point)
		self.drawSegment(point)
		if (len(self.node) == self.max):
			self.finish(False)
		if self.ui.xValue.isEnabled():
			self.ui.xValue.setFocus()
			self.ui.xValue.selectAll()
		else:
			self.ui.yValue.setFocus()
			self.ui.yValue.selectAll()

	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/line.png',
			'MenuText': 'Line',
			'ToolTip': 'Creates a 2-point line. CTRL to snap, SHIFT to constrain'}

class polyline(line):
	def __init__(self):
		self.max=128
	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/polyline.png',
			'MenuText': 'Polyline',
			'ToolTip': 'Creates a 2-point line. CTRL to snap, SHIFT to constrain'}


class finishLine:
	"a FreeCAD command to finish any running Line drawing operation"
	def Activated(self):
		activeCommand = FreeCADGui.activeWorkbench().activeDraftCommand
		if (activeCommand != None):
			if (activeCommand.featureName == "Line"):
				activeCommand.finish(False)
	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/finish.png',
			'MenuText': 'Finish Line',
			'ToolTip': 'Finishes a line without closing it'}

	
class closeLine:
	"a FreeCAD command to close any running Line drawing operation"
	def Activated(self):
		activeCommand = FreeCADGui.activeWorkbench().activeDraftCommand
		if (activeCommand != None):
			if (activeCommand.featureName == "Line"):
				activeCommand.finish(True)
	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/close.png',
			'MenuText': 'Close Line',
			'ToolTip': 'Closes the line being drawn'}


class undoLine:
	"a FreeCAD command to undo last drawn segment of a line"
	def Activated(self):
		activeCommand = FreeCADGui.activeWorkbench().activeDraftCommand
		if (activeCommand != None):
			if (activeCommand.featureName == "Line"):
				activeCommand.undolast()
	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/undo.png',
			'MenuText': 'Undo last segment',
			'ToolTip': 'Undoes the last drawn segment of the line being drawn'}

	
class rectangle:
	"This class creates a rectangle."
	def Activated(self):
		self.doc = FreeCAD.ActiveDocument		
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			self.constrain = None
			self.view = FreeCADGui.ActiveDocument.ActiveView
			self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
			self.featureName = "Rectangle"
			FreeCADGui.activeWorkbench().activeDraftCommand = self
			self.node = []
			self.obj = None
			self.pos = []
			self.ui.sourceCmd = self
			self.ui.lineUi()
			self.ui.cmdlabel.setText("Rectangle")
			self.call = self.view.addEventCallback("SoEvent",self.action)
			self.snap = snapTracker()
			self.rect = rectangleTracker()
			FreeCAD.Console.PrintMessage("Pick first point:\n")

	def finish(self,closed=False):
		"terminates the operation and closes the poly if asked"
		self.node=[]
		self.rect.off()
		self.view.removeEventCallback("SoEvent",self.call)
		self.ui.offUi()
		self.ui.sourceCmd=None
		del self.rect
		del self.snap
		FreeCADGui.activeWorkbench().activeDraftCommand = None

	def createObject(self):
		"creates the final object in the current doc"
		ve = []
		edges = []
		for i in range(4):
			n = self.rect.coords.point.getValues()[i]
			ve.append(FreeCAD.Vector(n[0],n[1],n[2]))
		edges.append(Part.Line(ve[0],ve[1]).toShape())
		edges.append(Part.Line(ve[1],ve[2]).toShape())
		edges.append(Part.Line(ve[2],ve[3]).toShape())
		edges.append(Part.Line(ve[3],ve[0]).toShape())
		shape=Part.Wire(edges)
		self.doc.openTransaction("Create "+self.featureName) 
		self.obj=self.doc.addObject("Part::Feature",self.featureName)
		self.obj.Shape = shape
		self.doc.commitTransaction()
		formatObject(self.obj)
		select(self.obj)
		self.finish()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			point = snapPoint(self,point,cursor,arg["CtrlDown"])
			if not self.ui.zValue.isEnabled(): point.z = float(self.ui.zValue.text())
			if (len(self.node)>0): displayPoint(point, self.node[-1])
			else: displayPoint(point)
			self.rect.update(point)

		elif (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				if (arg["Position"] == self.pos):
					self.finish()
				else:
					self.pos = arg["Position"]
					point = self.view.getPoint(self.pos[0],self.pos[1])
					point = snapPoint(self,point,self.pos,arg["CtrlDown"])
					if not self.ui.zValue.isEnabled(): point.z = float(self.ui.zValue.text())
					self.appendPoint(point)

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		point = FreeCAD.Vector(numx,numy,numz)
		self.appendPoint(point)

	def appendPoint(self,point):
		self.node.append(point)
		if (len(self.node) > 1):
			self.rect.update(point)
			self.createObject()
		else:
			FreeCAD.Console.PrintMessage("Pick opposite point:\n")
			self.rect.setorigin(point)
			self.rect.on()

	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/rectangle.png',
			'MenuText': 'Rectangle',
			'ToolTip': 'Creates a 2-point rectangle. CTRL to snap'}

class arc:
	'''
	This class creates an arc (circle feature). 

	USAGE:

	It asks the user for 4 points: the center point, another point to specify the radius, then the start and end positions
	-pick a point somewhere on screen with the mouse
	-pick a point on an existing object, 2d or 3d
	-on numeric input, pressing Enter passes from x to y, from y to z, and from z it adds the point (if valid)
	-press CTRL to snap
	-press SHIFT to constrain
	-when picking the end angle, press ALT to go counter-clockwise.
	'''
	def __init__(self):
		self.closedCircle=False
		self.featureName = "Arc"

	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			self.view = FreeCADGui.ActiveDocument.ActiveView
			self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
			self.step = 0
			FreeCADGui.activeWorkbench().activeDraftCommand = self
			self.doc.openTransaction("Create "+self.featureName)
			self.obj = self.doc.addObject("Part::Feature",self.featureName)
			self.doc.commitTransaction()
			formatObject(self.obj)
			self.center = None
			self.rad = None
			self.node = []
			self.constrain = None
			self.ui.arcUi()
			self.ui.sourceCmd = self
			self.snap = snapTracker()
			self.linetrack = lineTracker(dotted=True)
			self.constraintrack = lineTracker(dotted=True)
			self.arctrack = arcTracker()
			self.call = self.view.addEventCallback("SoEvent",self.action)
			FreeCAD.Console.PrintMessage("Pick center point:\n")

	def finish(self,closed=False):
		"finishes the arc"
		if (self.rad == None): self.doc.undo()
		elif not(self.closedCircle) and (self.step < 4): self.doc.undo()
		self.ui.offUi()
		self.view.removeEventCallback("SoEvent",self.call)
		del self.snap
		del self.linetrack
		del self.constraintrack
		del self.arctrack
		self.doc.recompute()
		FreeCADGui.activeWorkbench().activeDraftCommand = None

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"):
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			point = snapPoint(self,point,cursor,arg["CtrlDown"])
			ctrlPoint = FreeCAD.Vector(point.x,point.y,point.z)
			if (arg["ShiftDown"]): # constraining
				point = constrainPoint(self,point)
			else:
				self.constrain = None
			if not self.ui.zValue.isEnabled(): point.z = float(self.ui.zValue.text())
			if (self.step == 0):
				displayPoint(point)
			elif (self.step == 1):
				rad = fcvec.dist(point,self.center)
				self.ui.radiusValue.setText("%.2f" % rad)
				self.arctrack.trans.scaleFactor.setValue([rad,rad,rad])
				self.ui.radiusValue.setFocus()
				self.ui.radiusValue.selectAll()
				# Draw constraint tracker line.
				if (arg["ShiftDown"]):
					self.constraintrack.p1(point)
					self.constraintrack.p2(ctrlPoint)
					self.constraintrack.on()
				else: self.constraintrack.off()
				self.linetrack.p1(self.center)
				self.linetrack.p2(point)
				self.linetrack.on()
			elif (self.step == 2):
				currentrad = fcvec.dist(point,self.center)
				angle = math.asin((point.y-self.center.y)/currentrad)
				if (point.x > self.center.x):
					pc = [math.cos(angle)*self.rad,math.sin(angle)*self.rad]
					if (point.y < self.center.y):
						angle = (math.pi*2)+angle
				else:
					pc = [-math.cos(angle)*self.rad,math.sin(angle)*self.rad]
					angle = math.pi-angle
				self.linetrack.p2(FreeCAD.Vector(self.center.x+pc[0],self.center.y+pc[1],point.z))
				# Draw constraint tracker line.
				if (arg["ShiftDown"]):
					self.constraintrack.p1(point)
					self.constraintrack.p2(ctrlPoint)
					self.constraintrack.on()
				else: self.constraintrack.off()
				self.ui.radiusValue.setText("%.2f" % math.degrees(angle))
				self.firstangle = angle
				self.ui.radiusValue.setFocus()
				self.ui.radiusValue.selectAll()
			else:
				currentrad = fcvec.dist(point,self.center)
				angle = math.asin((point.y-self.center.y)/currentrad)
				if (point.x > self.center.x):
					pc = [math.cos(angle)*self.rad,math.sin(angle)*self.rad]
					if (point.y < self.center.y):
						angle = (math.pi*2)+angle
				else:
					pc = [-math.cos(angle)*self.rad,math.sin(angle)*self.rad]
					angle = math.pi-angle
				if (angle < self.firstangle): 
					sweep = (2*math.pi-self.firstangle)+angle
				else:
					sweep = angle - self.firstangle
				self.clockwise=True
				if (arg["AltDown"]):
					self.clockwise=False
					if (angle < self.firstangle):
						sweep = -(self.firstangle - angle)
					else:
						sweep = -(self.firstangle+(2*math.pi-angle))
				self.arctrack.update(sweep)
				self.linetrack.p2(FreeCAD.Vector(self.center.x+pc[0],self.center.y+pc[1],point.z))
				# Draw constraint tracker line.
				if (arg["ShiftDown"]):
					self.constraintrack.p1(point)
					self.constraintrack.p2(ctrlPoint)
					self.constraintrack.on()
				else: self.constraintrack.off()
				self.ui.radiusValue.setText("%.2f" % math.degrees(angle))
				self.lastangle = angle
				self.ui.radiusValue.setFocus()
				self.ui.radiusValue.selectAll()

		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				pos = arg["Position"]
				point = self.view.getPoint(pos[0],pos[1])
				point = snapPoint(self,point,pos,arg["CtrlDown"])
				if (arg["ShiftDown"]): # constraining
					point = constrainPoint(self,point)
				else:
					self.constrain = None
				if not self.ui.zValue.isEnabled(): point.z = float(self.ui.zValue.text())
				if (self.step == 0):
					self.center = point
					self.node = [point]
					self.arctrack.trans.translation.setValue([self.center.x,self.center.y,self.center.z])
					self.arctrack.on()
					self.ui.radiusUi()
					self.step = 1
					self.linetrack.p1(self.center)
					self.linetrack.p2(self.view.getPoint(pos[0],pos[1]))
					self.linetrack.on()
					FreeCAD.Console.PrintMessage("Pick radius:\n")
				elif (self.step == 1):
					self.rad = fcvec.dist(point,self.center)
					if self.closedCircle: self.drawArc()
					else: 
						self.ui.labelRadius.setText("Start angle")
						self.linetrack.p1(self.center)
						self.linetrack.on()
						self.step = 2
						FreeCAD.Console.PrintMessage("Pick start angle:\n")
				elif (self.step == 2):
					self.ui.labelRadius.setText("End angle")
					self.step = 3
					self.arctrack.trans.rotation.setValue(coin.SbVec3f(0,0,1),self.firstangle)
					FreeCAD.Console.PrintMessage("Pick end angle:\n")
				else:
					self.step = 4
					self.drawArc()

	def drawArc(self):
		if self.closedCircle:
			self.curve = Part.makeCircle(self.rad)
		else:
			if self.clockwise:
				self.curve = Part.makeCircle(self.rad,self.firstangle,self.lastangle)
			else:
				self.curve = Part.makeCircle(self.rad,self.lastangle,self.firstangle)
		self.curve.translate(self.center)
		self.obj.Shape = self.curve
		formatObject(self.obj)
		select(self.obj)
		self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		self.center = FreeCAD.Vector(numx,numy,numz)
		self.arctrack.trans.translation.setValue([self.center.x,self.center.y,self.center.z])
		self.arctrack.on()
		self.ui.radiusUi()
		self.step = 1
		FreeCAD.Console.PrintMessage("Pick radius")
		
	def numericRadius(self,rad):
		"this function gets called by the toolbar when valid radius have been entered there"
		if (self.step == 1):
			self.rad = rad
			if self.closedCircle: self.drawArc()
			self.step = 2
			self.arctrack.trans.scaleFactor.setValue([rad,rad,rad])
			self.ui.labelRadius.setText("Start angle")
			self.linetrack.p1(self.center)
			self.linetrack.on()
			FreeCAD.Console.PrintMessage("Pick start angle:\n")
		elif (self.step == 2):
			self.ui.labelRadius.setText("End angle")
			self.firstangle = math.radians(rad)
			self.arctrack.trans.rotation.setValue(self.firstangle)
			self.step = 3
			FreeCAD.Console.PrintMessage("Pick end angle:\n")
		else:
			self.step = 4
			self.drawArc()

	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/arc.png',
			'MenuText': 'Arc',
			'ToolTip': 'Creates an arc. CTRL to snap, SHIFT to constrain, ALT to go counter-clockwise'}


class circle(arc):
	"a modified version of the arc tool, to produce a closed circle"
	def __init__(self):
		self.closedCircle=True
		self.featureName = "Circle"
		
	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/circle.png',
			'MenuText': 'Circle',
			'ToolTip': 'Creates a circle. CTRL to snap'}

	
class annotation:
	'''
	This class creates an annotation feature. Takes 1 parameter: the name o the object.
	User is asked for a location point, then a dialog is displayed where text can be entered.
	CTRL snaps, ESC cancels.
	'''

	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			self.view = FreeCADGui.ActiveDocument.ActiveView
			self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
			self.featureName = "Text"
			FreeCADGui.activeWorkbench().activeDraftCommand = self
			self.node = []
			self.obj = None
			self.pos = []
			self.dialog = None
			self.text = ''
			self.ui.sourceCmd = self
			self.ui.pointUi()
			self.ui.cmdlabel.setText("Text")
			self.call = self.view.addEventCallback("SoEvent",self.action)
			self.ui.xValue.setFocus()
			self.ui.xValue.selectAll()
			self.snap = snapTracker()
			FreeCAD.Console.PrintMessage("Pick location point:\n")

	def finish(self,closed=False):
		"terminates the operation and closes the poly if asked"
		self.node=[]
		self.view.removeEventCallback("SoEvent",self.call)
		self.ui.offUi()
		self.ui.sourceCmd=None
		del self.snap
		del self.dialog
		FreeCADGui.activeWorkbench().activeDraftCommand = None

	def createObject(self):
		"creates an object in the current doc"
		self.doc.openTransaction("Create "+self.featureName) 
		self.obj=self.doc.addObject("App::Annotation",self.featureName)
		self.doc.commitTransaction()
		self.obj.LabelText=self.text
		self.obj.Position=self.node[0]
		formatObject(self.obj)
		select(self.obj)
		self.finish()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			point = snapPoint(self,point,cursor,arg["CtrlDown"])
			displayPoint(point)

		elif (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				self.pos = arg["Position"]
				point = self.view.getPoint(self.pos[0],self.pos[1])
				point = snapPoint(self,point,self.pos,arg["CtrlDown"])
				self.node.append(point)
				self.ui.textUi()
				self.ui.textValue.setFocus()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		point = FreeCAD.Vector(numx,numy,numz)
		self.node.append(point)
		self.ui.textUi()
		self.ui.textValue.setFocus()

	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/text.png',
			'MenuText': 'Text',
			'ToolTip': 'Creates an annotation. CTRL to snap'}

#---------------------------------------------------------------------------
# Modifier functions
#---------------------------------------------------------------------------

class move:
	'''
	This class translates the selected objects from a point to another point.

	USAGE:

	It asks the user for 2 points: the origin and the end of the translation vector.
	-pick a point somewhere on screen with the mouse
	-pick a point on an existing object, 2d or 3d
	-on numeric input, pressing Enter passes from x to y, from y to z, and from z it adds the point (if valid)
	-press CTRL to snap
	-press SHIFT to constrain
	-press ALT to create a copy
	'''

	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		self.sel = FreeCADGui.Selection.getSelection()
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			if (len(self.sel)>0):
				self.view = FreeCADGui.ActiveDocument.ActiveView
				self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui		
				self.constrain = None
				self.pos = []
				self.ui.pointUi()
				self.ui.cmdlabel.setText("Move")
				FreeCADGui.activeWorkbench().activeDraftCommand = self
				self.ui.sourceCmd = self
				self.featureName = "Move"
				self.ui.xValue.setFocus()
				self.ui.xValue.selectAll()
				self.node = []
				self.snap = snapTracker()
				self.linetrack = lineTracker()
				self.constraintrack = lineTracker(dotted=True)
				self.ghost = ghostTracker(self.sel)
				self.call = self.view.addEventCallback("SoEvent",self.action)
				FreeCAD.Console.PrintMessage("Pick start point:\n")

	def finish(self,closed=False):
		self.node=[]
		self.view.removeEventCallback("SoEvent",self.call)
		self.ui.offUi()
		self.ui.sourceCmd=None
		del self.ghost
		del self.snap
		del self.linetrack
		del self.constraintrack
		FreeCADGui.activeWorkbench().activeDraftCommand = None

	def move(self,delta,copy=False):
		"moving the real shapes"
		if copy: self.doc.openTransaction("Copy")
		else: self.doc.openTransaction("Move")
		for ob in self.sel:
			if copy: newob = self.doc.addObject("Part::Feature",ob.Name)
			else: newob=ob
			if (ob.Type == "Part::Feature"):
				sh = ob.Shape
				sh.translate(delta)
				newob.Shape = sh
		if copy: formatObject(newob,ob)
		self.doc.commitTransaction()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			point = snapPoint(self,point,cursor,arg["CtrlDown"])
			ctrlPoint = FreeCAD.Vector(point.x,point.y,point.z)
			if (arg["ShiftDown"]): # constraining
				point = constrainPoint(self,point)
			else:
				self.constrain = None
			self.linetrack.p2(point)
			# Draw constraint tracker line.
			if (arg["ShiftDown"]):
				self.constraintrack.p1(point)
				self.constraintrack.p2(ctrlPoint)
				self.constraintrack.on()
			else: self.constraintrack.off()
			if (len(self.node) > 0):
				last = self.node[len(self.node)-1]
				delta = fcvec.sub(point,last)
				self.ghost.trans.translation.setValue([delta.x,delta.y,delta.z])
				displayPoint(point, self.node[-1])
			else: displayPoint(point)

		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				self.pos = arg["Position"]
				point = self.view.getPoint(self.pos[0],self.pos[1])
				point = snapPoint(self,point,self.pos,arg["CtrlDown"])
				if (arg["ShiftDown"]): 
					point = constrainPoint(self,point)
				else:
					self.constrain = None
				if (self.node == []):
					self.node.append(point)
					self.ui.isRelative.show()
					self.ui.isCopy.show()
					self.linetrack.on()
					self.ghost.on()
					self.linetrack.p1(point)
					FreeCAD.Console.PrintMessage("Pick end point:\n")
				else:
					last = self.node[len(self.node)-1]
					if self.ui.isCopy.isChecked() or arg["AltDown"]:
						self.move(fcvec.sub(point,last),True)
					else:
						self.move(fcvec.sub(point,last))
					self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		point = FreeCAD.Vector(numx,numy,numz)
		if (self.node == []):
			self.node.append(point)
			self.ui.isRelative.show()
			self.ui.isCopy.show()
			self.linetrack.p1(point)
			self.linetrack.on()
			self.ghost.on()
			FreeCAD.Console.PrintMessage("Pick end point point:\n")
		else:
			last = self.node[len(self.node)-1]
			if self.ui.isCopy.isChecked():
				self.move(fcvec.sub(point,last),True)
			else:
				self.move(fcvec.sub(point,last))
			self.finish()

	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/move.png',
			'MenuText': 'Move',
			'ToolTip': 'Moves the selected objects between 2 points. CTRL to snap, SHIFT to constrain, ALT to copy'}

	
			
class applyStyle:
	"this class applies the current line width and line color to selected objects"

	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		self.sel = FreeCADGui.Selection.getSelection()
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			if (len(self.sel)>0):
				self.doc.openTransaction("Change style")
				for ob in self.sel:
					if (ob.Type == "App::DocumentObjectGroup"): self.formatGroup(ob)
					else: formatObject(ob)
				self.doc.commitTransaction()

	def formatGroup(self,grpob):
		for ob in grpob.Group:
			if (ob.Type == "App::DocumentObjectGroup"): self.formatGroup(ob)
			else: formatObject(ob)

	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/apply.png',
			'MenuText': 'Apply Style',
			'ToolTip': 'Applies current line width and color to selected objects'}
	
			
class rotate:
	'''
	This class rotates the selected objects.

	USAGE:

	It asks the user for 3 points: the rotation center, the start angle and the rotation.
	-pick a point somewhere on screen with the mouse
	-pick a point on an existing object, 2d or 3d
	-on numeric input, pressing Enter passes from x to y, from y to z, and from z it adds the point (if valid)
	-press CTRL to snap
	-press SHIFT to constrain
	-press ALT to create a copy
	'''

	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		self.sel = FreeCADGui.Selection.getSelection()
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			if (len(self.sel)>0):
				self.view = FreeCADGui.ActiveDocument.ActiveView
				self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
				self.step = 0
				self.featureName = "Rotate"
				FreeCADGui.activeWorkbench().activeDraftCommand = self
				self.center = None
				self.node = []
				self.constrain = None
				self.ui.arcUi()
				self.ui.cmdlabel.setText("Rotate")
				self.ui.sourceCmd = self
				self.snap = snapTracker()
				self.linetrack = lineTracker()
				self.constraintrack = lineTracker(dotted=True)
				self.arctrack = arcTracker()
				self.ghost = ghostTracker(self.sel)
				self.call = self.view.addEventCallback("SoEvent",self.action)
				FreeCAD.Console.PrintMessage("Pick rotation center:\n")

	def finish(self,closed=False):
		"finishes the arc"
		self.ui.offUi()
		self.view.removeEventCallback("SoEvent",self.call)
		del self.snap
		del self.linetrack
		del self.constraintrack
		del self.arctrack
		del self.ghost
		self.doc.recompute()
		FreeCADGui.activeWorkbench().activeDraftCommand = None

	def rot (self,angle,copy=False):
		"rotating the real shapes"
		if copy: self.doc.openTransaction("Copy")
		else: self.doc.openTransaction("Rotate")
		for ob in self.sel:
			if copy: newob = self.doc.addObject("Part::Feature",ob.Name)
			else: newob = ob
			if (ob.Type == "Part::Feature"):
				shape = ob.Shape
				shape.rotate((self.center.x,self.center.y,self.center.z),(0,0,1),angle)
				newob.Shape=shape
			if copy: formatObject(newob,ob)
		self.doc.commitTransaction()

	def rotateCopy (self,angle):
		"the real rotate/copy operation"
		self.doc.openTransaction("Copy")
		for ob in self.sel:
			if (ob.Type == "Part::Feature"):
				shape = ob.Shape
				shape.rotate((self.center.x,self.center.y,self.center.z),(0,0,1),angle)
				newob = self.doc.addObject("Part::Feature",ob.Name)
				newob.Shape=shape
			formatObject(newob,ob)
		self.doc.commitTransaction()
		self.doc.recompute()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"):
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			point = snapPoint(self,point,cursor,arg["CtrlDown"])
			ctrlPoint = FreeCAD.Vector(point.x,point.y,point.z)
			if (arg["ShiftDown"]): # constraining
				point = constrainPoint(self,point)
			else:
				self.constrain = None
			if (self.step == 0):
				displayPoint(point)
			elif (self.step == 1):
				currentrad = fcvec.dist(point,self.center)
				if (currentrad != 0): angle = math.asin((point.y-self.center.y)/currentrad)
				else: angle = 0
				if (point.x > self.center.x):
					if (point.y < self.center.y):
						angle = (math.pi*2)+angle
				else:
					angle = math.pi-angle
				self.linetrack.p2(point)
				# Draw constraint tracker line.
				if (arg["ShiftDown"]):
					self.constraintrack.p1(point)
					self.constraintrack.p2(ctrlPoint)
					self.constraintrack.on()
				else: self.constraintrack.off()
				self.ui.radiusValue.setText("%.2f" % math.degrees(angle))
				self.firstangle = angle
				self.ui.radiusValue.setFocus()
				self.ui.radiusValue.selectAll()
			else:
				currentrad = fcvec.dist(point,self.center)
				self.arctrack.trans.scaleFactor.setValue([currentrad,currentrad,currentrad])
				if (currentrad != 0): angle = math.asin((point.y-self.center.y)/currentrad)
				else: angle = 0
				if (point.x > self.center.x):
					if (point.y < self.center.y):
						angle = (math.pi*2)+angle
				else:
					angle = math.pi-angle
				if (angle < self.firstangle): 
					sweep = (2*math.pi-self.firstangle)+angle
				else:
					sweep = angle - self.firstangle
				self.arctrack.update(sweep)
				self.ghost.trans.rotation.setValue(coin.SbVec3f(0,0,1),sweep)
				self.linetrack.p2(point)
				# Draw constraint tracker line.
				if (arg["ShiftDown"]):
					self.constraintrack.p1(point)
					self.constraintrack.p2(ctrlPoint)
					self.constraintrack.on()
				else: self.constraintrack.off()
				self.ui.radiusValue.setText("%.2f" % math.degrees(sweep))
				self.lastangle = angle
				self.ui.radiusValue.setFocus()
				self.ui.radiusValue.selectAll()

		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				pos = arg["Position"]
				point = self.view.getPoint(pos[0],pos[1])
				point = snapPoint(self,point,pos,arg["CtrlDown"])
				if (arg["ShiftDown"]): # constraining
					point = constrainPoint(self,point)
				else:
					self.constrain = None
				if (self.step == 0):
					self.center = point
					self.node = [point]
					self.arctrack.trans.translation.setValue([self.center.x,self.center.y,self.center.z])
					self.ui.radiusUi()
					self.ui.labelRadius.setText("Base angle")
					self.linetrack.p1(self.center)
					self.ghost.trans.center.setValue(self.center.x,self.center.y,self.center.z)
					self.linetrack.on()
					self.step = 1
					FreeCAD.Console.PrintMessage("Pick base angle:\n")
				elif (self.step == 1):
					self.ui.labelRadius.setText("Rotation")
					self.arctrack.trans.rotation.setValue(coin.SbVec3f(0,0,1),self.firstangle)
					self.rad = fcvec.dist(point,self.center)
					self.arctrack.on()
					self.ghost.on()
					self.arctrack.trans.scaleFactor.setValue([self.rad,self.rad,self.rad])
					self.ui.isCopy.show()
					self.step = 2
					FreeCAD.Console.PrintMessage("Pick rotation angle:\n")
				else:
					currentrad = fcvec.dist(point,self.center)
					angle = math.asin((point.y-self.center.y)/currentrad)
					if (point.x > self.center.x):
						if (point.y < self.center.y):
							angle = (math.pi*2)+angle
					else:
						angle = math.pi-angle
					if (angle < self.firstangle): 
						sweep = (2*math.pi-self.firstangle)+angle
					else:
						sweep = angle - self.firstangle
					if self.ui.isCopy.isChecked() or arg["AltDown"]:
						self.rot(sweep,True)
					else:
						self.rot(sweep)
					self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		self.center = FreeCAD.Vector(numx,numy,numz)
		self.node = [self.center]
		self.arctrack.trans.translation.setValue([self.center.x,self.center.y,self.center.z])
		self.ghost.trans.center.setValue(self.center.x,self.center.y,self.center.z)
		self.linetrack.p1(self.center)
		self.arctrack.on()
		self.linetrack.on()
		self.ui.radiusUi()
		self.ui.labelRadius.setText("Base angle")
		self.step = 1
		FreeCAD.Console.PrintMessage("Pick base angle:\n")

	def numericRadius(self,rad):
		"this function gets called by the toolbar when valid radius have been entered there"
		if (self.step == 1):
			self.ui.labelRadius.setText("Rotation")
			self.firstangle = math.radians(rad)
			self.arctrack.trans.rotation.setValue(coin.SbVec3f(0,0,1),math.radians(rad))
			self.arctrack.on()
			self.ghost.on()
			self.ui.isCopy.show()
			self.step = 2
			FreeCAD.Console.PrintMessage("Pick rotation angle:\n")
		else:
			if self.ui.isCopy.isChecked():
				self.rot(math.radians(rad),True)
			else:
				self.rot(math.radians(rad))
			self.finish()

	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/rotate.png',
			'MenuText': 'Rotate',
			'ToolTip': 'Rotates the selected objects. CTRL to snap, SHIFT to constrain, ALT creates a copy'}

			
class offset:
	'''
	This class offsets the selected objects.

	USAGE:

	The user must provide a point indicating the distance to offset. The distance
	is calculated from the closest segment.
	-pick a point somewhere on screen with the mouse
	-pick a point on an existing object, 2d or 3d
	-on numeric input, the position of the mouse cursor gives the direction
	-press CTRL to snap
	-press SHIFT to constrain to the currently used segment
	-press ALT to create a copy
	'''

	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		self.sel = FreeCADGui.Selection.getSelection()
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			if (len(self.sel) == 1):
				self.view = FreeCADGui.ActiveDocument.ActiveView
				self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
				self.sel = self.sel[0]
				self.constrain = None
				self.constrainSeg = None
				self.pos = []
				self.ui.radiusUi()
				self.ui.isCopy.show()
				self.ui.labelRadius.setText("Distance")
				self.ui.cmdlabel.setText("Offset")
				FreeCADGui.activeWorkbench().activeDraftCommand = self
				self.ui.sourceCmd = self
				self.featureName = "Offset"
				self.ui.radiusValue.setFocus()
				self.ui.radiusValue.selectAll()
				self.node = []
				self.snap = snapTracker()
				self.linetrack = lineTracker()
				if (len(self.sel.Shape.Edges) > 1):
					self.ghost = ghostTracker(self.sel,"single")
					self.type="line"
				else:
					if isinstance(self.sel.Shape.Edges[0].Curve,Part.Line):
						self.ghost = ghostTracker(self.sel,"single")
						self.type="line"
					else:
						self.ghost = arcTracker()
						self.setuparc()
						self.type="arc"
				self.call = self.view.addEventCallback("SoEvent",self.action)
				FreeCAD.Console.PrintMessage("Pick distance:\n")
				if (complexity(self.sel) == 8) or (complexity(self.sel) == 6):
					FreeCAD.Console.PrintMessage("Sorry, straight+curve wires are not supported at the moment...")
					self.finish()

	def finish(self,closed=False):
		self.node=[]
		self.view.removeEventCallback("SoEvent",self.call)
		self.ui.offUi()
		self.ui.sourceCmd=None
		del self.snap
		del self.linetrack
		del self.ghost
		FreeCADGui.activeWorkbench().activeDraftCommand = None

	def findDistance(self,point,currentSeg=None):
		"finds the shortest distance between mouse cursor and selected object"
		if (self.sel.Type == "Part::Feature"):
			edges = self.sel.Shape.Edges
			if (currentSeg == None):
				valid = None
				for edge in edges:
					if (isinstance (edge.Curve,Part.Line)):
						ve1 = edge.Vertexes[0].Point
						ve2 = edge.Vertexes[1].Point
						segment = fcvec.new(ve1,ve2)
						perp = fcvec.crossproduct(segment)
						chord = fcvec.new(point,ve1)
						dist = fcvec.project(chord,perp)
						newpoint = fcvec.add(point,dist)
						if (dist.Length != 0):
							if (fcvec.new(ve1,newpoint).Length <= segment.Length) and (fcvec.new(ve2,newpoint).Length <= segment.Length):
								if not valid:
									valid = [dist,edges.index(edge)]
								else:
									if (dist.Length < valid[0].Length):
										valid = [dist,edges.index(edge)]
					elif (isinstance (edge.Curve,Part.Circle)):
						ve1 = edge.Vertexes[0].Point
						if (len(edge.Vertexes) > 1): ve2 = edge.Vertexes[-1].Point
						else: ve2 = None
						center=edge.Curve.Center
						segment = fcvec.new(point,center)
						ratio=(segment.Length-edge.Curve.Radius)/segment.Length
						dist = fcvec.scale(segment,ratio)
						newpoint = fcvec.add(point,dist)
						if (dist.Length != 0):
							if ve2:
								ang1=fcvec.angle(fcvec.new(center,ve1))
								ang2=fcvec.angle(fcvec.new(center,ve2))
								angpt=fcvec.angle(fcvec.new(center,newpoint))
								if (((angpt <= ang2) and (angpt >= ang1)) or ((angpt <= ang1) and (angpt >= ang2))):
									if not valid:
										valid = [dist,edges.index(edge)]
									else:
										if (dist.Length < valid[0].Length):
											valid = [dist,edges.index(edge)]
							else:
								if not valid:
									valid = [dist,edges.index(edge)]
								else:
									if (dist.Length < valid[0].Length):
										valid = [dist,edges.index(edge)]
				return valid
			else:
				if (isinstance (edges[currentSeg].Curve,Part.Line)):
					ve1 = edges[currentSeg].Vertexes[0].Point
					ve2 = edges[currentSeg].Vertexes[1].Point
					segment = fcvec.new(ve1,ve2)
					perp = fcvec.crossproduct(segment)
					chord = fcvec.new(point,ve1)
					dist = fcvec.project(chord,perp)
					if (dist != 0): return [dist,currentSeg]
					else: return None
				elif (isinstance (edges[currentSeg].Curve,Part.Circle)):
					center=edges[currentSeg].Curve.Center
					segment = fcvec.new(point,center)
					ratio=(segment.Length-edges[currentSeg].Curve.Radius)/segment.Length
					dist = fcvec.scale(segment,ratio)
					if (dist != 0): return [dist,currentSeg]
					else: return None

	def setuparc(self):
		edge=self.sel.Shape.Edges[0]
		self.center=edge.Curve.Center
		self.radius=edge.Curve.Radius
		self.ve1 = edge.Vertexes[0].Point
		if (len(edge.Vertexes) > 1): 
			self.ve2 = edge.Vertexes[-1].Point
			ang1=-fcvec.angle(fcvec.new(self.center,self.ve1))
			ang2=-fcvec.angle(fcvec.new(self.center,self.ve2))
			if ang1 > ang2: ang1,ang2 = ang2,ang1
			print "ang1 ", math.degrees(ang1)
			print "ang2 ", math.degrees(ang2)
			self.ghost.update(ang2-ang1)
			self.ghost.trans.rotation.setValue(coin.SbVec3f(0,0,1),ang1)
		self.ghost.trans.translation.setValue([self.center.x,self.center.y,self.center.z])

	def redrawGhost(self,dist):
		"offsets the ghost to the given distance"
		if (self.sel.Type == "Part::Feature"):
			if (self.type == "line"):
				nodeslist = []
				closed = False
				if (self.sel.Shape.Wires != []): closed = self.sel.Shape.Wires[0].isClosed()
				for v in self.sel.Shape.Vertexes:
					nodeslist.append(v.Point)
				last = len(nodeslist)-1
				if closed: nodeslist.append(nodeslist[0])
				dVector = fcvec.neg(dist[0])
				for i in range(dist[1],0,-1):
					selfSegment = fcvec.sub(nodeslist[i],nodeslist[i+1])
					nextSegment = fcvec.sub(nodeslist[i-1],nodeslist[i])
					angle = abs(fcvec.angle(selfSegment,nextSegment))
					if ((selfSegment.x * nextSegment.y - selfSegment.y * nextSegment.x) < 0): angle = -angle
					if (dVector.Length > 0): 
						distRatio = (dVector.Length/math.cos(angle/2))/dVector.Length
					else:
						distRatio = 0
					dVector = fcvec.rotate(dVector,angle/2)
					npoint = fcvec.add(nodeslist[i],fcvec.scale(dVector,distRatio))
					self.ghost.coords.point.set1Value(i,npoint.x,npoint.y,npoint.z)
					dVector = fcvec.rotate(dVector,angle/2)

				if closed:
					selfSegment = fcvec.sub(nodeslist[0],nodeslist[1])
					nextSegment = fcvec.sub(nodeslist[last],nodeslist[0])
					angle = fcvec.angle(selfSegment,nextSegment)
					if ((selfSegment.x * nextSegment.y - selfSegment.y * nextSegment.x) < 0): angle = -angle
					if (dVector.Length > 0): 
						distRatio = (dVector.Length/math.cos(angle/2))/dVector.Length
					else:
						distRatio = 0
					dVector = fcvec.rotate(dVector,angle/2)
					fpoint = fcvec.add(nodeslist[0],fcvec.scale(dVector,distRatio))
					self.ghost.coords.point.set1Value(0,fpoint.x,fpoint.y,fpoint.z)
				else:
					npoint = fcvec.add(nodeslist[0],dVector)
					self.ghost.coords.point.set1Value(0,npoint.x,npoint.y,npoint.z)
				dVector = fcvec.neg(dist[0])

				for i in range(dist[1]+1,len(nodeslist)-1):
					selfSegment = fcvec.sub(nodeslist[i],nodeslist[i-1])
					nextSegment = fcvec.sub(nodeslist[i+1],nodeslist[i])
					angle = -fcvec.angle(selfSegment,nextSegment)
					#if ((selfSegment.x * nextSegment.y - selfSegment.y * nextSegment.x) >= 0): angle = -angle
					if (dVector.Length > 0): 
						distRatio = (dVector.Length/math.cos(angle/2))/dVector.Length
					else:
						distRatio = 0
					dVector = fcvec.rotate(dVector,-angle/2)
					npoint = fcvec.add(nodeslist[i],fcvec.scale(dVector,distRatio))
					self.ghost.coords.point.set1Value(i,npoint.x,npoint.y,npoint.z)
					dVector = fcvec.rotate(dVector,-angle/2)

				if closed:
					self.ghost.coords.point.set1Value(last+1,fpoint.x,fpoint.y,fpoint.z)
				else:
					npoint = fcvec.add(nodeslist[last],dVector)
					self.ghost.coords.point.set1Value(last,npoint.x,npoint.y,npoint.z)
			else:
				rad=fcvec.new(self.center,self.node[0]).Length
				self.ghost.trans.scaleFactor.setValue([rad,rad,rad])

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			point = snapPoint(self,point,cursor,arg["CtrlDown"])
			if not self.ui.zValue.isEnabled(): point.z = float(self.ui.zValue.text())
			self.node = [point]
			if (arg["ShiftDown"]) and self.constrainSeg:
				dist = self.findDistance(point, self.constrainSeg[1])
			else:
				dist = self.findDistance(point)
			if dist:
				self.constrainSeg = dist
				self.linetrack.on()
				self.ghost.on()
				self.linetrack.p1(point)
				dvec = fcvec.add(point,dist[0])
				self.linetrack.p2(dvec)
				self.redrawGhost(dist)
				self.ui.radiusValue.setText("%.2f" % dist[0].Length)
			else:
				self.constrainSeg = None
				self.linetrack.off()
				self.ghost.off()
				self.ui.radiusValue.setText("off")
			self.ui.radiusValue.setFocus()
			self.ui.radiusValue.selectAll()

		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				if arg["AltDown"] or self.ui.isCopy.isChecked():
					targetOb = self.doc.addObject("Part::Feature",self.sel.Name)
					formatObject(targetOb,self.sel)
				else:
					targetOb = self.sel
				self.doc.openTransaction("Offset")
				if (self.sel.Type == "Part::Feature"):
					if (self.type == "line"):
						shape = self.sel.Shape
						nodes = []
						for v in shape.Vertexes: nodes.append(v.Point)
						for i in range(0,len(nodes)):
							n=self.ghost.coords.point.getValues()[i]
							nodes[i]=FreeCAD.Vector(n[0],n[1],n[2])
						newshape = Part.Line(nodes[0],nodes[1]).toShape()
						for p in range(1,len(nodes)-1):
							newseg = Part.Line(nodes[p],nodes[p+1]).toShape()
							newshape = newshape.fuse(newseg)
						if self.sel.Shape.Wires:
							if self.sel.Shape.Wires[0].isClosed():
								newseg = Part.Line(nodes[len(nodes)-1],nodes[0]).toShape()
								e=newshape.Edges
								e.append(newseg)
								newshape=Part.Wire(e)
						targetOb.Shape = newshape
					else:
						rad=fcvec.new(self.center,self.node[0]).Length
						shape = self.sel.Shape
						if (len(shape.Vertexes) == 1):
							ang1 = 0
							ang2 = 2*math.pi
						else:
							ve1 = shape.Edges[0].Vertexes[0].Point
							ve2 = shape.Edges[0].Vertexes[-1].Point
							ang1 = -fcvec.angle(fcvec.new(self.center,ve1))
							ang2 = -fcvec.angle(fcvec.new(self.center,ve2))
							if (ang2 < ang1): ang1,ang2 = ang2,ang1
						newshape = Part.makeCircle(rad,ang1,ang2)
						newshape.translate(self.center)
						targetOb.Shape = newshape
				self.doc.commitTransaction()
				self.finish()

	def numericRadius(self,scale):
		"this function gets called by the toolbar when valid distance have been entered there"
		if (self.constrainSeg != None):
			uVector = fcvec.scale(fcvec.normalized(self.constrainSeg[0]),scale)
			self.redrawGhost([uVector,self.constrainSeg[1]])
			self.doc.openTransaction("Offset")
			if (self.sel.Type == "Part::Feature"):
				if self.ui.isCopy.isChecked():
					targetOb = self.doc.addObject("Part::Feature",self.sel.Name)
					formatObject(targetOb,self.sel)
				else:
					targetOb = self.sel
				shape = self.sel.Shape
				nodes = []
				for v in shape.Vertexes: nodes.append(v.Point)
				for i in range(0,len(nodes)):
					n=self.ghost.coords.point.getValues()[i]
					nodes[i]=FreeCAD.Vector(n[0],n[1],n[2])
				newshape = Part.Line(nodes[0],nodes[1]).toShape()
				for p in range(1,len(nodes)-1):
					newseg = Part.Line(nodes[p],nodes[p+1]).toShape()
					newshape = newshape.fuse(newseg)
				targetOb.Shape = newshape
			self.doc.commitTransaction()
			self.finish()

	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/offset.png',
			'MenuText': 'Offset',
			'ToolTip': 'Offsets the active object. CTRL to snap, SHIFT to constrain, ALT to copy'}
	

class upgrade:
	'''
	This class upgrades selected objects in different ways, following this list (in order):
	- if there are more than one faces, the faces are merged (union)
	- if there is only one face, nothing is done
	- if there are closed wires, they are transformed in a face
	- otherwise join all edges into a wire (closed if applicable)
	'''
	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		self.sel = FreeCADGui.Selection.getSelection()
		self.featureName = "Upgrade"
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			if (len(self.sel) > 0):
				edges = []
				wires = []
				faces = []
				# determining which level we will have
				for ob in self.sel:
					for f in ob.Shape.Faces:
						faces.append(f)
					for w in ob.Shape.Wires:
						if w.isClosed():
							wires.append(w)
					lastob = ob
				# applying transformation
				self.doc.openTransaction("Upgrade")
				if (len(faces) > 0):
					u = faces.pop(0)
					for f in faces:
						u = u.fuse(f)
						u = concatenate(u)

					newob = self.doc.addObject("Part::Feature","Union")
					newob.Shape = u
					formatObject(newob,lastob)
				elif (len(wires) > 0):
					for w in wires:
						f = Part.Face(w)
						faces.append(f)
					for f in faces:
						newob = self.doc.addObject("Part::Feature","Face")
						newob.Shape = f
						formatObject(newob,lastob)
				else:
					for ob in self.sel:
						for e in ob.Shape.Edges:
							edges.append(e)
					w = Part.Wire(edges)
					newob = self.doc.addObject("Part::Feature","Wire")
					newob.Shape = w
					formatObject(newob,lastob)
				for ob in self.sel:
					self.doc.removeObject(ob.Name)
				self.doc.commitTransaction()
				select(newob)
	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/upgrade.png',
			'MenuText': 'Upgrade',
			'ToolTip': 'Joins the selected objects into one, or converts closed wires to filled faces, or unite faces'}
		
class downgrade:
	'''
	This class downgrades selected objects in different ways, following this list (in order):
	- if there are more than one faces, the subsequent faces are subtracted from the first one
	- if there is only one face, it gets converted to a wire
	- otherwise wires are exploded into single edges
	'''
	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		self.sel = FreeCADGui.Selection.getSelection()
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			if (len(self.sel) > 0):
				self.featureName = "Upgrade"
				edges = []
				faces = []
				for ob in self.sel:
					for f in ob.Shape.Faces:
						faces.append(f)
				for ob in self.sel:
					for e in ob.Shape.Edges:
						edges.append(e)
				# applying transformation
				self.doc.openTransaction("Downgrade")
				if (len(faces) > 1):
					u = faces.pop(0)
					for f in faces:
						u = u.cut(f)
						for ob in self.sel:
							newob = self.doc.addObject("Part::Feature","Subtraction")
							newob.Shape = u
							formatObject(newob,ob)
							self.doc.removeObject(ob.Name)
				elif (len(faces) > 0):
					w=faces[0].Wires[0]
					for ob in self.sel:
						newob = self.doc.addObject("Part::Feature","Wire")
						newob.Shape = w
						formatObject(newob,ob)
						self.doc.removeObject(ob.Name)
				else:
					for ob in self.sel:
						for e in edges:
							newob = self.doc.addObject("Part::Feature","Edge")
							newob.Shape = e
							formatObject(newob,ob)
						self.doc.removeObject(ob.Name)
				self.doc.commitTransaction()
				select(newob)
	def GetResources(self):
		return {'Pixmap'  : 'Mod/Draft/icons/downgrade.png',
			'MenuText': 'Downgrade',
			'ToolTip': 'Explodes the selected objects into simpler objects, or subtract faces'}



class trimex:
	"this tool trims or extends lines, polylines and arcs. SHIFT constrains to the last point."

	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		self.sel = FreeCADGui.Selection.getSelection()
		if (self.doc != None) and (FreeCADGui.activeWorkbench().activeDraftCommand == None):
			if (len(self.sel) == 1):
				self.sel=self.sel[0]
				self.view = FreeCADGui.ActiveDocument.ActiveView
				self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
				self.ui.radiusUi()
				self.ui.labelRadius.setText("Distance")
				self.ui.cmdlabel.setText("Trimex")
				FreeCADGui.activeWorkbench().activeDraftCommand = self
				self.ui.sourceCmd = self
				self.featureName = "Trimex"
				self.ui.radiusValue.setFocus()
				self.ui.radiusValue.selectAll()
				self.node = []
				self.snap = snapTracker()
				self.linetrack = lineTracker()
				self.ghost = ghostTracker(self.sel,"single")
				self.sel.ViewObject.Visibility = False
				self.ghost.on()
				self.activePoint = 0
				self.nodes = []
				for v in self.sel.Shape.Vertexes:
					self.nodes.append(v.Point)
				self.call = self.view.addEventCallback("SoEvent",self.action)
				FreeCAD.Console.PrintMessage("Pick distance:\n")
				print complexity(self.sel)
				if (complexity(self.sel) != 1) and (complexity(self.sel) != 4):
					FreeCAD.Console.PrintMessage("Closed wires and wires containing arcs are not supported at the moment:\n")
					self.finish()
				
	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			snapped = self.view.getObjectInfo((cursor[0],cursor[1]))
			point = snapPoint(self,point,cursor)
			if not self.ui.zValue.isEnabled(): point.z = float(self.ui.zValue.text())
			dist = self.redrawGhost(point,snapped,arg["ShiftDown"],arg["AltDown"])
			self.ui.radiusValue.setText("%.2f" % dist)
			self.ui.radiusValue.setFocus()
			self.ui.radiusValue.selectAll()
		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				shape = self.sel.Shape
				self.nodes = []
				for i in range(len(self.ghost.coords.point.getValues())):
					n=self.ghost.coords.point.getValues()[i]
					self.nodes.append(FreeCAD.Vector(n[0],n[1],n[2]))
				self.trimObject()
				self.finish()

	def redrawGhost(self,point,snapped=None,shift=False,alt=False):
		reverse = False
		for i in range(len(self.nodes)):
			self.ghost.coords.point.set1Value(i,self.nodes[i].x,self.nodes[i].y,self.nodes[i].z)
			
		if shift: npoint = self.activePoint
		else: npoint = findClosest(point,self.nodes)

		if npoint == 0:
			if alt:
				npoint = 1
				starter = self.nodes[0]
				self.ghost.coords.point.deleteValues(2)
			else:
				starter = self.nodes[npoint+1]
		elif npoint == len(self.nodes)-1:
			if alt:
				npoint = npoint -1
				starter = self.nodes[npoint+1]
				self.ghost.coords.point.deleteValues(0,npoint)
				reverse = True
			else:
				starter = self.nodes[npoint-1]
		else:			
			if npoint >= (len(self.nodes)-1)/2:
				if alt:
					npoint = npoint-1
					starter = self.nodes[npoint+1]
					self.ghost.coords.point.deleteValues(0,npoint)
					reverse = True
				else:	
					starter = self.nodes[npoint-1]
					self.ghost.coords.point.deleteValues(npoint+1)
			else:
				if alt:
					npoint = npoint+1
					starter = self.nodes[npoint-1]
					self.ghost.coords.point.deleteValues(npoint+1)
				else:
					starter = self.nodes[npoint+1]
					self.ghost.coords.point.deleteValues(0,npoint)
					reverse = True
					

		seg = fcvec.new(starter,self.nodes[npoint])
		if snapped:
			snapped = self.doc.getObject(snapped['Object'])
			ray = Part.Line(starter,self.nodes[npoint]).toShape()
			snappoint = []
			for e in snapped.Shape.Edges:
				pt = findIntersection(ray,e,True,True)
				if pt: snappoint.append(pt)
			if snappoint:
				point = snappoint[findClosest(point,snappoint)]
				
		perp = fcvec.crossproduct(seg)
		chord = fcvec.new(point,self.nodes[npoint])
		proj = fcvec.project(chord,perp)
		newpoint = fcvec.add(point,proj)
		dist = fcvec.new(newpoint,self.nodes[npoint])
		if reverse: 
			self.ghost.coords.point.set1Value(0,newpoint.x,newpoint.y,newpoint.z)
		else:
			self.ghost.coords.point.set1Value(npoint,newpoint.x,newpoint.y,newpoint.z)
		if npoint != self.activePoint:
			old=self.nodes[self.activePoint]
			self.ghost.coords.point.set1Value(self.activePoint,old.x,old.y,old.z)
			self.activePoint = npoint
		if abs(fcvec.angle(dist,seg)) > math.pi/2: return dist.Length
		else: return -dist.Length	       	
		

	def trimObject(self):
		"trims the actual object"
		edges = []
		for i in range(len(self.nodes)-1):
			edges.append(Part.Line(self.nodes[i],self.nodes[i+1]).toShape())
		newshape = Part.Wire(edges)
		self.doc.openTransaction("Trim/extend")
		self.sel.Shape = newshape
		self.doc.commitTransaction()
		self.ghost.off()

	def finish(self,closed=False):		
		self.nodes = []
		self.view.removeEventCallback("SoEvent",self.call)
		self.ui.offUi()
		self.ui.sourceCmd=None
		del self.snap
		del self.linetrack
		del self.ghost
		self.sel.ViewObject.Visibility = True
		select(self.sel)
		FreeCADGui.activeWorkbench().activeDraftCommand = None

	def numericRadius(self,dist):
		"this function gets called by the toolbar when valid distance have been entered there"
		if self.activePoint == 0: starter = self.nodes[self.activePoint+1]
		else: starter = self.nodes[self.activePoint-1]
		seg = fcvec.new(starter,self.nodes[self.activePoint])
		seg = fcvec.normalized(seg)
		seg = fcvec.scale(seg,dist)
		self.nodes[self.activePoint] = fcvec.add(self.nodes[self.activePoint],seg)
		self.trimObject()
		self.finish()
		
	def GetResources(self):
		return {'Pixmap' : 'Mod/Draft/icons/trimex.png',
			'MenuText' : 'TrimExtend',
			'ToolTip' : 'Trims or Extends the selected object. SHIFT constrains to current segment, ALT inverts'}

#---------------------------------------------------------------------------
# Adds the commands to the FreeCAD command manager
#---------------------------------------------------------------------------

# drawing commands
FreeCADGui.addCommand('Draft_Line',line())
FreeCADGui.addCommand('Draft_Polyline',polyline())
FreeCADGui.addCommand('Draft_Circle',circle())
FreeCADGui.addCommand('Draft_Arc',arc())
FreeCADGui.addCommand('Draft_Text',annotation())
FreeCADGui.addCommand('Draft_Rectangle',rectangle())

# context commands
FreeCADGui.addCommand('Draft_FinishLine',finishLine())
FreeCADGui.addCommand('Draft_CloseLine',closeLine())
FreeCADGui.addCommand('Draft_UndoLine',undoLine())

# modification commands
FreeCADGui.addCommand('Draft_Move',move())
FreeCADGui.addCommand('Draft_ApplyStyle',applyStyle())
FreeCADGui.addCommand('Draft_Rotate',rotate())
FreeCADGui.addCommand('Draft_Offset',offset())
FreeCADGui.addCommand('Draft_Upgrade',upgrade())
FreeCADGui.addCommand('Draft_Downgrade',downgrade())
FreeCADGui.addCommand('Draft_Trimex',trimex())
