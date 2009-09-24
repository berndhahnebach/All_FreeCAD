
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

__title__="FreeCAD Draft Workbench"
__author__ = "Yorik van Havre <yorik@gmx.fr>, Werner Mayer, Martin Burbaum"
__url__ = ["http://yorik.orgfree.com","http://free-cad.sourceforge.net"]

'''
General description:

	The Draft module is a FreeCAD module for drawing/editing 2D entities.
	The aim is to give FreeCAD basic 2D-CAD capabilities (similar
	to Autocad and other similar software). This modules is made to be run
	inside FreeCAD and needs the PyQt4 and pivy modules available.
	The user manual is at
	http://sourceforge.net/apps/mediawiki/free-cad/index.php?title=2d_Drafting_Module

How it works / how to extend:

	This module is written entirely in python. If you know a bit of python
	language, you are welcome to modify this module or to help us to improve it.
	Suggestions are also welcome on the FreeCAD discussion forum.

	If you want to have a look at the code, here is a general explanation. The
	Draft module is divided in two files: Draft.py (this one) and draftGui.py.
	The Draft.py file is doing all the actual cad operation, while in draftGui.py
	you have the ui part, ie. the draft command bar. Both files are loaded at
	module init by InitGui.py, which is called directly by FreeCAD. In Draft.py, you have:
	- General functions, like snap, constraint, format, that are shared by all the tools
	- Trackers, for drawing temporary stuff: snaps, lines, arcs or ghosts
	- Tools that construct geometry: line, circle, rectangle, text
	- Tools that modify geometry: move, rotate, offset, upgrade, downgrade, trimex
	- The tools are then mapped to FreeCAD commands
	The tools all have an Activated() function, which is called by FreeCAD when the
	corresponding FreeCAD command is invoked. Most tools then create the trackers they
	will need during operation, then place a callback mechanism, which will detect
	user input and do the necessary cad operations. They also send commands to the
	command bar, which will display the appropriate controls. While the scene event
	callback watches mouse events, the keyboard is being watched by the command bar.
'''

# import FreeCAD modules
import FreeCAD, FreeCADGui, Part, math, sys
sys.path.append(FreeCAD.ConfigGet("AppHomePath")+"/bin") # temporary hack for linux
from FreeCAD import Base, Vector
from pivy import coin
from draftlibs import fcvec
from draftlibs import fcgeo


# Constants

NORM = Vector(0,0,1) # temporary normal for all objects (always horiontal)
lastObj = [0,0] # last snapped objects, for quick intersection calculation

#---------------------------------------------------------------------------
# General functions
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
	    - Intersection between line, polylines segments, arcs and circles
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

		if (ctrl) and (ob.Type == "Part::Feature"):
			for i in ob.Shape.Vertexes:
				snapArray.append([i.Point,0,i.Point])
			for j in ob.Shape.Edges:
				if (isinstance (j.Curve,Part.Line)):
					p1 = j.Vertexes[0].Point
					p2 = j.Vertexes[1].Point
					midpoint = fcgeo.findMidpoint(j)
					snapArray.append([midpoint,1,midpoint])
					if (len(target.node) > 0):
						last = target.node[len(target.node)-1]
						if (target.constrain == 0):
							if ((last.y > p1.y) and (last.y < p2.y) or (last.y > p2.y) and (last.y < p1.y)):
								pc = (last.y-p1.y)/(p2.y-p1.y)
								constrainpoint = (Vector(p1.x+pc*(p2.x-p1.x),p1.y+pc*(p2.y-p1.y),p1.z+pc*(p2.z-p1.z)))
								snapArray.append([constrainpoint,1,constrainpoint]) # constrainpoint
						if (target.constrain == 1):
							if ((last.x > p1.x) and (last.x < p2.x) or (last.x > p2.x) and (last.x < p1.x)):
								pc = (last.x-p1.x)/(p2.x-p1.x)
								constrainpoint = (Vector(p1.x+pc*(p2.x-p1.x),p1.y+pc*(p2.y-p1.y),p1.z+pc*(p2.z-p1.z)))
								snapArray.append([constrainpoint,1,constrainpoint]) # constrainpoint

				elif isinstance (j.Curve,Part.Circle):
					rad = j.Curve.Radius
					pos = j.Curve.Center
					for i in [0,30,45,60,90,120,135,150,180,210,225,240,270,300,315,330]:
						ang = math.radians(i)
						cur = Vector(math.sin(ang)*rad+pos.x,math.cos(ang)*rad+pos.y,pos.z)
						snapArray.append([cur,1,cur])
					for i in [15,37.5,52.5,75,105,127.5,142.5,165,195,217.5,232.5,255,285,307.5,322.5,345]:
						ang = math.radians(i)
						cur = Vector(math.sin(ang)*rad+pos.x,math.cos(ang)*rad+pos.y,pos.z)
						snapArray.append([cur,0,pos])

				if lastObj[0]:
					if (lastObj[0].Type == "Part::Feature"):
						for k in lastObj[0].Shape.Edges:
							pt = fcgeo.findIntersection(j,k)
							if pt:
								for p in pt:
									snapArray.append([p,3,p])
			if ob.Shape.ShapeType == 'Compound':
				tempsnaps = []
				for e1 in ob.Shape.Edges:
					for e2 in ob.Shape.Edges:
						pt = fcgeo.findIntersection(e1,e2)
						if pt:
							for p in pt:
								tempsnaps.append(p)
				for i in range(len(tempsnaps)):
					p = tempsnaps.pop()
					alone = True
					for p2 in tempsnaps:
						if fcvec.equals(p,p2): alone = False
					if alone == True:
						snapArray.append([p,3,p])
					

		else:
			cur = Vector(snapped['x'],snapped['y'],snapped['z'])
			snapArray = [[cur,2,cur]]

		if not lastObj[0]:
			lastObj[0] = ob
			lastObj[1] = ob
		if (lastObj[1] != ob):
			lastObj[0] = lastObj[1]
			lastObj[1] = ob

		# calculating shortest distance
		shortest = 1000000000000000000
		newpoint = point
		for i in snapArray:
			if i[0] == None:
				FreeCAD.Console.PrintMessage("snapPoint: debug 'i[0]' is 'None'\n")
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

def constrainPoint (target,point,mobile=False):
	'''
	Constrain function
	On commands that need to enter several points (currently only line/polyline),
	you can constrain the next point to be picked to the last drawn point by
	pressing SHIFT. The vertical or horizontal constraining depends on the
	position of your mouse in relation to last point at the moment you press
	SHIFT. if mobile=True, mobile behaviour applies.
	'''
	if len(target.node) > 0:
		last = target.node[-1]
		if ((target.constrain == None) or mobile):
			if (abs(point.x-last.x) > abs(point.y-last.y)):
				point.y = last.y
				target.constrain = 0 #horizontal
				target.ui.xValue.setEnabled(True)
				target.ui.yValue.setEnabled(False)
			else:
				point.x = last.x
				target.constrain = 1 #vertical
				target.ui.xValue.setEnabled(False)
				target.ui.yValue.setEnabled(True)
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
	obrep = target.ViewObject
	ui= FreeCADGui.activeWorkbench().draftToolBar.ui
	r = float(ui.color.red()/255.0)
	g = float(ui.color.green()/255.0)
	b = float(ui.color.blue()/255.0)
	lw = float(ui.widthButton.value())
	col = (r,g,b,0.0)
	params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
	fontheight = params.GetFloat("textheight")
	if (target.Type == "App::Annotation"):
		obrep.DisplayMode="World"
		obrep.FontSize=fontheight
		obrep.TextColor=col
	else:
		if not origin:
			obrep.LineWidth = lw
			obrep.LineColor = col
		else:
			matchrep = origin.ViewObject
			obrep.LineWidth = matchrep.LineWidth
			obrep.LineColor = matchrep.LineColor
			obrep.ShapeColor = matchrep.ShapeColor
	if (target.Type == "App::FeaturePython"):
		if 'Dimline' in target.PropertiesList:
			target.ViewObject.FontSize=fontheight

def select(object):
	"deselects everything and selects only the passed object"
	FreeCADGui.Selection.clearSelection()
	FreeCADGui.Selection.addSelection(object)

def selectObject(arg):
	"this is a scene even handler, to be called from the tools when they need to select an object"
	if (arg["Type"] == "SoKeyboardEvent"):
		if (arg["Key"] == "ESCAPE"):
			FreeCADGui.activeWorkbench().activeDraftCommand.finish()
			#TODO : this part raises a coin3D warning about scene traversal, to be fixed.
	if (arg["Type"] == "SoMouseButtonEvent"):
		if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
			selection = FreeCADGui.Selection.getSelection()
			cursor = arg["Position"]
			snapped = FreeCADGui.ActiveDocument.ActiveView.getObjectInfo((cursor[0],cursor[1]))
			if snapped:
				obj = FreeCAD.ActiveDocument.getObject(snapped['Object'])
				FreeCADGui.Selection.addSelection(obj)
				FreeCADGui.activeWorkbench().activeDraftCommand.component=snapped['Component']
				FreeCADGui.activeWorkbench().activeDraftCommand.proceed()

def getPoint(target,args,mobile=False):
	"returns a constrained 3d point and its original point"
	ui = FreeCADGui.activeWorkbench().draftToolBar.ui
	view = FreeCADGui.ActiveDocument.ActiveView
	point = view.getPoint(args["Position"][0],args["Position"][1])
	point = snapPoint(target,point,args["Position"],args["CtrlDown"])
	ctrlPoint = Vector(point.x,point.y,point.z)
	if (args["ShiftDown"]): # constraining
		if mobile and (target.constrain == None):
			target.node.append(point)
		point = constrainPoint(target,point,mobile)
	else:
		target.constrain = None
		ui.xValue.setEnabled(True)
		ui.yValue.setEnabled(True)
	if not ui.zValue.isEnabled(): point.z = float(ui.zValue.text())
	if target.node:
		if target.featureName == "Rectangle":
			ui.displayPoint(point, target.node[0])
		else:
			ui.displayPoint(point, target.node[-1])
	else: ui.displayPoint(point)
	return point,ctrlPoint

				
#---------------------------------------------------------------------------
# Trackers
#---------------------------------------------------------------------------

class Tracker:
	"common settings for all trackers"
	def __init__(self,dotted=False,scolor=None,swidth=None):
		self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
		self.color = coin.SoBaseColor()
		if scolor:
			self.color.rgb = scolor
		else:
			self.color.rgb = self.ui.getDefaultColor("ui")
		self.drawstyle = coin.SoDrawStyle()
		if swidth:
			self.drawstyle.lineWidth = swidth
		if dotted:
			self.drawstyle.style = coin.SoDrawStyle.LINES
			self.drawstyle.lineWeight = 3
			self.drawstyle.linePattern = 0xaa
			# drawstyle.linePatternScaleFactor = 10
		self.node = coin.SoSeparator()
		self.node.addChild(self.drawstyle)
		self.node.addChild(self.color)
		self.switch = coin.SoSwitch() # this is the on/off switch
		self.switch.addChild(self.node)
		sg=FreeCADGui.ActiveDocument.ActiveView.getSceneGraph()
		sg.addChild(self.switch)
		self.switch.whichChild = -1
		
	def __del__(self):
		self.switch.whichChild = -1

	def on(self):
		self.switch.whichChild = 0

	def off(self):
		self.switch.whichChild = -1
				
class snapTracker:
	"a class to create a snap marker symbol, used by the functions that support snapping"
	def __init__(self):
		self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
		color = coin.SoBaseColor()
		color.rgb = self.ui.getDefaultColor("snap")
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

class lineTracker(Tracker):
	"a class to create a tracking line used by the functions that need it"
	def __init__(self,dotted=False,scolor=None,swidth=None):
		Tracker.__init__(self,dotted,scolor,swidth)
		line = coin.SoLineSet()
		line.numVertices.setValue(2)
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValues(0,2,[[0,0,0],[1,0,0]])
		self.node.addChild(self.coords)
		self.node.addChild(line)

	def p1(self,point):
		self.coords.point.set1Value(0,point.x,point.y,point.z)

	def p2(self,point):
		self.coords.point.set1Value(1,point.x,point.y,point.z)

class rectangleTracker(Tracker):
	"a tracking rectangle"
	def __init__(self,dotted=False,scolor=None,swidth=None):
		Tracker.__init__(self,dotted,scolor,swidth)
		line = coin.SoLineSet()
		line.numVertices.setValue(5)
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValues(0,5,[[0,0,0],[1,0,0],[1,1,0],[0,1,0],[0,0,0]])
		self.node.addChild(self.coords)
		self.node.addChild(line)
		self.origin = Vector(0,0,0)

	def setorigin(self,point):
		self.coords.point.set1Value(0,point.x,point.y,point.z)
		self.coords.point.set1Value(4,point.x,point.y,point.z)
		self.origin = point

	def update(self,point):
		self.coords.point.set1Value(1,point.x,self.origin.y,self.origin.z)
		self.coords.point.set1Value(2,point.x,point.y,self.origin.z)
		self.coords.point.set1Value(3,self.origin.x,point.y,self.origin.z)

class dimTracker(Tracker):
	"a tracking bridge for drawing dimensions"
	def __init__(self,dotted=False,scolor=None,swidth=None):
		Tracker.__init__(self,dotted,scolor,swidth)
		self.line = coin.SoLineSet()
		self.line.numVertices.setValue(4)
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValues(0,4,[[0,0,0],[0,0,0],[0,0,0],[0,0,0]])
		self.node.addChild(self.coords)
		self.node.addChild(self.line)

	def update(self,pts):
		if len(pts) == 2:
			points = [fcvec.tup(pts[0],True),fcvec.tup(pts[0],True),\
				  fcvec.tup(pts[1],True),fcvec.tup(pts[1],True)]
			self.coords.point.setValues(0,4,points)
		elif len(pts) == 3:
			p1 = pts[0]
			p4 = pts[1]
			base = Part.Line(p1,p4).toShape()
			proj = fcgeo.findDistance(pts[2],base)
			if not proj:
				p2 = p1
				p3 = p4
			else:
				p2 = p1.add(fcvec.neg(proj))
				p3 = p4.add(fcvec.neg(proj))
			points = [fcvec.tup(p1),fcvec.tup(p2),fcvec.tup(p3),fcvec.tup(p4)]
			self.coords.point.setValues(0,4,points)
		
class arcTracker(Tracker):
	"a class to create a tracking arc/circle, used by the functions that need it"
	def __init__(self,dotted=False,scolor=None,swidth=None):
		Tracker.__init__(self,dotted,scolor,swidth)
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
		self.node.addChild(self.coords)
		self.node.addChild(self.trans)
		self.node.addChild(self.circle)

	def startangle(self,angle):
		self.trans.rotation.setValue(coin.SbVec3f(0,0,1),-angle)

	def center(self,center):
		self.trans.translation.setValue([center.x,center.y,center.z])

	def radius(self,rad):
		self.trans.scaleFactor.setValue([rad,rad,rad])

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

class ghostTracker(Tracker):
	"this class creates a copy of the coin representation of all passed objects, to be used as ghost"
	def __init__(self,sel):
		Tracker.__init__(self)
		self.trans = coin.SoTransform()
		self.trans.translation.setValue([0,0,0])
		self.node.addChild(self.trans)
		ivsep = coin.SoSeparator()
		try:
			for ob in sel:
				ivsep.addChild(ob.ViewObject.RootNode.copy())
		except:
			print "draft: Couldn't create ghost"
		else:
			self.node.addChild(ivsep)

				
#---------------------------------------------------------------------------
# Python Features definitions
#---------------------------------------------------------------------------
		
class Dimension:
	"this class defines Dimension objects"
	def __init__(self, obj):
		obj.addProperty("App::PropertyVector","Start","Base",\
					"Startpoint of dimension").Start = FreeCAD.Vector(0,0,0)
		obj.addProperty("App::PropertyVector","End","Base",\
					"Endpoint of dimension").End = FreeCAD.Vector(1,0,0)
		obj.addProperty("App::PropertyVector","Dimline","Base",\
					"Point through which the dimension line passes").Dimline = FreeCAD.Vector(0,1,0)
		self.Type = "Dimension"
		obj.Proxy = self
		
	def onChanged(self, fp, prop):
		pass

	def execute(self, fp):
		pass

class DimensionViewProvider:
	"this class defines a view provider for Dimension objects"
	def __init__(self, obj):
		obj.addProperty("App::PropertyLength","FontSize","Base","Font size").FontSize=0.2
		obj.addProperty("App::PropertyLength","LineWidth","Base","Line width")
		obj.addProperty("App::PropertyColor","LineColor","Base","Line color")
		obj.addProperty("App::PropertyLength","ExtLines","Base","Ext lines ").ExtLines=0.3
		obj.Proxy = self

	def calcGeom(self,obj):
		p1 = obj.Start
		p4 = obj.End
		base = Part.Line(p1,p4).toShape()
		proj = fcgeo.findDistance(obj.Dimline,base)
		if not proj:
			p2 = p1
			p3 = p4
		else:
			p2 = p1.add(fcvec.neg(proj))
			p3 = p4.add(fcvec.neg(proj))
			dmax = obj.ViewObject.ExtLines
			if dmax and (proj.Length > dmax):
				p1 = p2.add(fcvec.scale(fcvec.normalized(proj),dmax))
				p4 = p3.add(fcvec.scale(fcvec.normalized(proj),dmax))
		midpoint = p2.add(fcvec.scale(p3.sub(p2),0.5))
		if not proj:
			ed = fcgeo.vec(base)
			proj = fcvec.crossproduct(ed)
		angle = -fcvec.angle(p3.sub(p2))
		if (angle > math.pi/2) or (angle <= -math.pi/2): angle = math.pi+angle
		# norm = p3.sub(p2).cross(proj)
		offset = fcvec.rotate(FreeCAD.Vector(obj.ViewObject.FontSize*.2,0,0),angle+math.pi/2)
		tbase = midpoint.add(offset)
		norm = Vector(0,0,1)
		return p1,p2,p3,p4,tbase,angle,norm

	def attach(self, obj):
		p1,p2,p3,p4,tbase,angle,norm = self.calcGeom(obj.Object)
		self.color = coin.SoBaseColor()
		self.color.rgb = (0,0,0)
		self.font = coin.SoFont()
		self.text = coin.SoAsciiText()
		self.text.justification = coin.SoAsciiText.CENTER
		self.text.string = ("%.2f" % p3.sub(p2).Length)
		self.textpos = coin.SoTransform()
		self.textpos.translation.setValue([tbase.x,tbase.y,tbase.z])
		self.textpos.rotation.setValue(coin.SbVec3f(norm.x,norm.y,norm.z),angle)
		label = coin.SoSeparator()
		label.addChild(self.textpos)
		label.addChild(self.color)
		label.addChild(self.font)
		label.addChild(self.text)
		marker = coin.SoMarkerSet()
		marker.markerIndex = coin.SoMarkerSet.CIRCLE_FILLED_5_5
		self.coord1 = coin.SoCoordinate3()
		self.coord1.point.setValue((p2.x,p2.y,p2.z))
		self.coord2 = coin.SoCoordinate3()
		self.coord2.point.setValue((p3.x,p3.y,p3.z))
		marks = coin.SoAnnotation()
		marks.addChild(self.color)
		marks.addChild(self.coord1)
		marks.addChild(marker)
		marks.addChild(self.coord2)
		marks.addChild(marker)       
		self.drawstyle = coin.SoDrawStyle()
		self.drawstyle.lineWidth = 1       
		line = coin.SoLineSet()
		line.numVertices.setValue(4)
		self.coords = coin.SoCoordinate3()
		self.coords.point.setValues(0,4,[[p1.x,p1.y,p1.z],[p2.x,p2.y,p2.z],[p3.x,p3.y,p3.z],[p4.x,p4.y,p4.z]])
		self.node = coin.SoGroup()
		self.node.addChild(self.color)
		self.node.addChild(self.drawstyle)
		self.node.addChild(self.coords)
		self.node.addChild(line)
		self.node.addChild(label)
		self.node.addChild(marks)
		obj.addDisplayMode(self.node,"Wireframe")
		self.onChanged(obj,"FontSize")
        
	def updateData(self, obj, prop):
		p1,p2,p3,p4,tbase,angle,norm = self.calcGeom(obj)
		self.text.string = ("%.2f" % p3.sub(p2).Length)
		self.textpos.rotation.setValue(coin.SbVec3f(norm.x,norm.y,norm.z),angle)
		self.coords.point.setValues(0,4,[[p1.x,p1.y,p1.z],[p2.x,p2.y,p2.z],[p3.x,p3.y,p3.z],[p4.x,p4.y,p4.z]])
		self.textpos.translation.setValue([tbase.x,tbase.y,tbase.z])
		self.coord1.point.setValue((p2.x,p2.y,p2.z))
		self.coord2.point.setValue((p3.x,p3.y,p3.z))

	def onChanged(self, vp, prop):
		if prop == "FontSize":
			self.font.size = vp.FontSize
		elif prop == "LineColor":
			c = vp.LineColor
			self.color.rgb.setValue(c[0],c[1],c[2])
		elif prop == "LineWidth":
			self.drawstyle.lineWidth = vp.LineWidth
		elif prop == "ExtLines":
			self.updateData(vp.Object, None)

	def getDisplayModes(self,obj):
		modes=[]
		modes.append("Wireframe")
		return modes

	def getDefaultDisplayMode(self):
		return "Wireframe"
    
	def getIcon(self):
		return """
                    /* XPM */
                    static char * path2644_xpm[] = {
                    "16 16 24 1",
                    " 	c None",
                    ".	c #040300",
                    "+	c #000000",
                    "@	c #060500",
                    "#	c #433A00",
                    "$	c #030200",
                    "%	c #D5BA00",
                    "&	c #040400",
                    "*	c #6E6000",
                    "=	c #FFDF00",
                    "-	c #010100",
                    ";	c #0F0D00",
                    ">	c #EED000",
                    ",	c #968300",
                    "'	c #2A2500",
                    ")	c #FCDC00",
                    "!	c #BFA700",
                    "~	c #050500",
                    "{	c #514700",
                    "]	c #DFC300",
                    "^	c #030300",
                    "/	c #7C6C00",
                    "(	c #9C8800",
                    "_	c #A79200",
                    "                ",
                    "       ..       ",
                    "       ++       ",
                    "      @##@      ",
                    "      $%%$      ",
                    "     &*==*&     ",
                    "    -;>==>;-    ",
                    "    -,====,-    ",
                    "   &')====)'&   ",
                    "   +!======!+   ",
                    "  ~{========{~  ",
                    "  ~]========]~  ",
                    " ^/==========/^ ",
                    "-&(__________(&-",
                    "++++++++++++++++",
                    "                "};
                    """

	def __getstate__(self):
		return None

	def __setstate__(self,state):
		return None




#---------------------------------------------------------------------------
# Geometry constructors
#---------------------------------------------------------------------------

class Creator:
	" General settings for all geometry creation tools"
	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		if not self.doc: self.finish()
		if FreeCADGui.activeWorkbench().activeDraftCommand: self.finish()
		self.view = FreeCADGui.ActiveDocument.ActiveView
		self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
		FreeCADGui.activeWorkbench().activeDraftCommand = self
		FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)
		self.ui.cross(True)
		self.node = []
		self.pos = []
		self.ui.sourceCmd = self
		self.constrain = None
		self.obj = None
		
	def finish(self):
		self.node=[]
		self.ui.offUi()
		self.ui.cross(False)
		self.ui.sourceCmd=None
		FreeCADGui.activeWorkbench().activeDraftCommand = None
		FreeCAD.Console.PrintMessage("")
		
	
class Line(Creator):
	'''
	This class creates a line or group of lines feature. 
	Takes 1 optional argument, the max number of points.
	'''
	def __init__(self):
		self.max=2

	def GetResources(self):
		return {'Pixmap'  : 'Draft_line',
			'MenuText': 'Line',
			'ToolTip': 'Creates a 2-point line. CTRL to snap, SHIFT to constrain'}

	def Activated(self):
		Creator.Activated(self)
		self.featureName = "Line"
		self.ui.lineUi()
		self.call = self.view.addEventCallback("SoEvent",self.action)
		self.snap = snapTracker()
		self.linetrack = lineTracker()
		self.constraintrack = lineTracker(dotted=True)
		FreeCAD.Console.PrintMessage("Pick first point:\n")

	def finish(self,closed=False):
		"terminates the operation and closes the poly if asked"
		if closed and (len(self.node) > 2):
			currentshape = self.obj.Shape
			first = self.node[0]
			last = self.node[len(self.node)-1]
			newseg = Part.Line(last,first).toShape()
			e=currentshape.Edges
			e.append(newseg)
			newshape=Part.Wire(e)
			self.obj.Shape = newshape
		Creator.finish(self)
		self.view.removeEventCallback("SoEvent",self.call)
		del self.linetrack
		del self.constraintrack
		del self.snap

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
			point,ctrlPoint = getPoint(self,arg)
			self.linetrack.p2(point)
			# Draw constraint tracker line.
			if (arg["ShiftDown"]):
				self.constraintrack.p1(point)
				self.constraintrack.p2(ctrlPoint)
				self.constraintrack.on()
			else: self.constraintrack.off()
		elif (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				if (arg["Position"] == self.pos):
					self.finish(False)
				else:
					point,ctrlPoint = getPoint(self,arg)
					self.pos = arg["Position"]
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
				newshape = Part.Wire(edges)
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
		point = Vector(numx,numy,numz)
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



class Polyline(Line):
	def __init__(self):
		self.max=256
	def GetResources(self):
		return {'Pixmap'  : 'Draft_polyline',
			'MenuText': 'Polyline',
			'ToolTip': 'Creates a 2-point line. CTRL to snap, SHIFT to constrain'}


class FinishLine:
	"a FreeCAD command to finish any running Line drawing operation"
	def Activated(self):
		activeCommand = FreeCADGui.activeWorkbench().activeDraftCommand
		if (activeCommand != None):
			if (activeCommand.featureName == "Line"):
				activeCommand.finish(False)
	def GetResources(self):
		return {'Pixmap'  : 'Draft_finish',
			'MenuText': 'Finish Line',
			'ToolTip': 'Finishes a line without closing it'}

	
class CloseLine:
	"a FreeCAD command to close any running Line drawing operation"
	def Activated(self):
		activeCommand = FreeCADGui.activeWorkbench().activeDraftCommand
		if (activeCommand != None):
			if (activeCommand.featureName == "Line"):
				activeCommand.finish(True)
	def GetResources(self):
		return {'Pixmap'  : 'Draft_lock',
			'MenuText': 'Close Line',
			'ToolTip': 'Closes the line being drawn'}


class UndoLine:
	"a FreeCAD command to undo last drawn segment of a line"
	def Activated(self):
		activeCommand = FreeCADGui.activeWorkbench().activeDraftCommand
		if (activeCommand != None):
			if (activeCommand.featureName == "Line"):
				activeCommand.undolast()
	def GetResources(self):
		return {'Pixmap'  : 'Draft_rotate',
			'MenuText': 'Undo last segment',
			'ToolTip': 'Undoes the last drawn segment of the line being drawn'}

	
class Rectangle(Creator):
	"This tool creates a rectangle."

	def GetResources(self):
		return {'Pixmap'  : 'Draft_rectangle',
			'MenuText': 'Rectangle',
			'ToolTip': 'Creates a 2-point rectangle. CTRL to snap'}

	def Activated(self):
		Creator.Activated(self)
		self.featureName = "Rectangle"
		self.refpoint = None
		self.ui.lineUi()
		self.ui.cmdlabel.setText("Rectangle")
		self.call = self.view.addEventCallback("SoEvent",self.action)
		self.snap = snapTracker()
		self.rect = rectangleTracker()
		FreeCAD.Console.PrintMessage("Pick first point:\n")

	def finish(self,closed=False):
		"terminates the operation and closes the poly if asked"
		Creator.finish(self)
		self.rect.off()
		self.view.removeEventCallback("SoEvent",self.call)
		del self.rect
		del self.snap

	def createObject(self):
		"creates the final object in the current doc"
		ve = []
		edges = []
		z = self.node[0].z
		if self.node[0].x < self.node[1].x:
			minx = self.node[0].x
			maxx = self.node[-1].x
		else:
			minx = self.node[-1].x
			maxx = self.node[0].x
		if self.node[0].y < self.node[-1].y:
			miny = self.node[0].y
			maxy = self.node[-1].y
		else:
			miny = self.node[-1].y
			maxy = self.node[0].y
		edges.append(Part.Line(Vector(minx,maxy,z),Vector(maxx,maxy,z)).toShape())
		edges.append(Part.Line(Vector(maxx,maxy,z),Vector(maxx,miny,z)).toShape())
		edges.append(Part.Line(Vector(maxx,miny,z),Vector(minx,miny,z)).toShape())
		edges.append(Part.Line(Vector(minx,miny,z),Vector(minx,maxy,z)).toShape())	
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
			point,ctrlPoint = getPoint(self,arg,mobile=True)
			self.rect.update(point)
		elif (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				if (arg["Position"] == self.pos):
					self.finish()
				else:
					point,ctrlPoint = getPoint(self,arg)
					self.appendPoint(point)

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		point = Vector(numx,numy,numz)
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


class Arc(Creator):
	'''
	This class creates an arc (circle feature). 
	'''
	def __init__(self):
		self.closedCircle=False
		self.featureName = "Arc"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_arc',
			'MenuText': 'Arc',
			'ToolTip': 'Creates an arc. CTRL to snap, SHIFT to constrain, ALT to go counter-clockwise'}

	def Activated(self):
		Creator.Activated(self)
		self.step = 0
		self.doc.openTransaction("Create "+self.featureName)
		self.obj = self.doc.addObject("Part::Feature",self.featureName)
		self.doc.commitTransaction()
		formatObject(self.obj)
		self.center = None
		self.rad = None
		self.tangents = []
		self.tanpoints = []
		if self.featureName == "Arc": self.ui.arcUi()
		else: self.ui.circleUi()
		self.altdown = False
		self.ui.sourceCmd = self
		self.snap = snapTracker()
		self.linetrack = lineTracker(dotted=True)
		self.constraintrack = lineTracker(dotted=True)
		self.arctrack = arcTracker()
		self.call = self.view.addEventCallback("SoEvent",self.action)
		FreeCAD.Console.PrintMessage("Pick center point:\n")

	def finish(self,closed=False):
		"finishes the arc"
		Creator.finish(self)
		if (self.rad == None): self.doc.undo()
		elif not(self.closedCircle) and (self.step < 4): self.doc.undo()
		self.view.removeEventCallback("SoEvent",self.call)
		del self.snap
		del self.linetrack
		del self.constraintrack
		del self.arctrack
		self.doc.recompute()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"):
			point,ctrlPoint = getPoint(self,arg)
			if (self.step == 0):
				if arg["AltDown"]:
					if not self.altdown:
						self.ui.cross(False)
						self.altdown = True
						self.ui.switchUi(True)
				else:
					if self.altdown:
						self.ui.cross(True)
						self.altdown = False
						self.ui.switchUi(False)
			elif (self.step == 1):
				if len(self.tangents) == 2:
					cir = fcgeo.circleFrom2tan1pt(self.tangents[0], self.tangents[1], point)
					self.center = fcgeo.findClosestCircle(point,cir).Center
					self.arctrack.trans.translation.setValue([self.center.x,self.center.y,self.center.z])
				elif self.tangents and self.tanpoints:
					cir = fcgeo.circleFrom1tan2pt(self.tangents[0], self.tanpoints[0], point)
					self.center = fcgeo.findClosestCircle(point,cir).Center
					self.arctrack.trans.translation.setValue([self.center.x,self.center.y,self.center.z])
				if arg["AltDown"]:
					if not self.altdown:
						self.ui.cross(False)
						self.altdown = True
					snapped = self.view.getObjectInfo((arg["Position"][0],arg["Position"][1]))
					if snapped:
						ob = self.doc.getObject(snapped['Object'])
						num = int(snapped['Component'].lstrip('Edge'))-1
						ed = ob.Shape.Edges[num]
						if len(self.tangents) == 2:
							cir = fcgeo.circleFrom3tan(self.tangents[0], self.tangents[1], ed)
							cl = fcgeo.findClosestCircle(point,cir)
							self.center = cl.Center
							self.rad = cl.Radius
							self.arctrack.trans.translation.setValue([self.center.x,self.center.y,self.center.z])
						else:
							self.rad = self.center.add(fcgeo.findDistance(self.center,ed).sub(self.center)).Length
					else:
						self.rad = fcvec.dist(point,self.center)
				else:
					if self.altdown:
						self.ui.cross(True)
						self.altdown = False
					self.rad = fcvec.dist(point,self.center)
				self.ui.radiusValue.setText("%.2f" % self.rad)
				self.arctrack.trans.scaleFactor.setValue([self.rad,self.rad,self.rad])
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
				self.linetrack.p2(Vector(self.center.x+pc[0],self.center.y+pc[1],point.z))
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
				self.linetrack.p2(Vector(self.center.x+pc[0],self.center.y+pc[1],point.z))
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
				point,ctrlPoint = getPoint(self,arg)
				if (self.step == 0):
					if arg["AltDown"]:
						snapped=self.view.getObjectInfo((arg["Position"][0],arg["Position"][1]))
						if snapped:
							ob = self.doc.getObject(snapped['Object'])
							num = int(snapped['Component'].lstrip('Edge'))-1
							ed = ob.Shape.Edges[num]
							self.tangents.append(ed)
							if len(self.tangents) == 2:
								self.arctrack.on()
								self.ui.radiusUi()
								self.step = 1
								self.linetrack.on()
								FreeCAD.Console.PrintMessage("Pick radius:\n")
					else:
						if len(self.tangents) ==1:
							self.tanpoints.append(point)
						else:
							self.center = point
							self.node = [point]
							self.arctrack.trans.translation.setValue([self.center.x,self.center.y,self.center.z])
							self.linetrack.p1(self.center)
							self.linetrack.p2(self.view.getPoint(arg["Position"][0],arg["Position"][1]))
						self.arctrack.on()
						self.ui.radiusUi()
						self.step = 1
						self.linetrack.on()
						FreeCAD.Console.PrintMessage("Pick radius:\n")
				elif (self.step == 1):
					if self.closedCircle:
						self.ui.cross(False)
						self.drawArc()
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
		"actually draws the FreeCAD object"
		if self.closedCircle:
			arc = Part.Circle(self.center,NORM,self.rad).toShape()
		else:
			radvec = Vector(self.rad,0,0)
			p1 = Vector.add(self.center,fcvec.rotate(radvec,self.firstangle))
			p3 = Vector.add(self.center,fcvec.rotate(radvec,self.lastangle))
			if self.firstangle < self.lastangle:
				mid = self.firstangle+(self.lastangle-self.firstangle)/2
			else:
				half = (self.lastangle-(math.pi*2-self.firstangle))/2
				mid = self.lastangle-half
			if not self.clockwise: mid = mid + math.pi
			p2 = Vector.add(self.center,fcvec.rotate(radvec,mid))			       
			arc = Part.Arc(p1,p2,p3).toShape()
		self.obj.Shape = arc
		formatObject(self.obj)
		select(self.obj)
		self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		self.center = Vector(numx,numy,numz)
		self.node = [self.center]
		self.arctrack.trans.translation.setValue([self.center.x,self.center.y,self.center.z])
		self.arctrack.on()
		self.ui.radiusUi()
		self.step = 1
		FreeCAD.Console.PrintMessage("Pick radius")
		
	def numericRadius(self,rad):
		"this function gets called by the toolbar when valid radius have been entered there"
		if (self.step == 1):
			self.rad = rad
			if len(self.tangents) == 2:
				cir = fcgeo.circleFrom2tan1rad(self.tangents[0], self.tangents[1], rad)
				if self.center:
					self.center = fcgeo.findClosestCircle(self.center,cir).Center
				else:
					self.center = cir[-1].Center
			elif self.tangents and self.tanpoints:
				cir = fcgeo.circleFrom1tan1pt1rad(self.tangents[0],self.tanpoints[0],rad)
				if self.center:
					self.center = fcgeo.findClosestCircle(self.center,cir).Center
				else:
					self.center = cir[-1].Center
			if self.closedCircle:
				self.drawArc()
			else:
				self.step = 2
				self.arctrack.trans.scaleFactor.setValue([rad,rad,rad])
				self.ui.labelRadius.setText("Start angle")
				self.linetrack.p1(self.center)
				self.linetrack.on()
				FreeCAD.Console.PrintMessage("Pick start angle:\n")
		elif (self.step == 2):
			self.ui.labelRadius.setText("End angle")
			self.firstangle = math.radians(rad)
			print "firstangle: ",self.firstangle
			self.arctrack.trans.rotation.setValue(coin.SbVec3f(0,0,1),self.firstangle)
			self.step = 3
			FreeCAD.Console.PrintMessage("Pick end angle:\n")
		else:
			self.lastangle = math.radians(rad)
			self.step = 4
			self.drawArc()




class Circle(Arc):
	"a modified version of the arc tool, to produce a closed circle"
	def __init__(self):
		self.closedCircle=True
		self.featureName = "Circle"
		
	def GetResources(self):
		return {'Pixmap'  : 'Draft_circle',
			'MenuText': 'Circle',
			'ToolTip': 'Creates a circle. CTRL to snap, ALT to select tangent objects'}

	
class Text(Creator):
	'''
	This class creates an annotation feature.
	'''

	def GetResources(self):
		return {'Pixmap'  : 'Draft_text',
			'MenuText': 'Text',
			'ToolTip': 'Creates an annotation. CTRL to snap'}

	def Activated(self):
		Creator.Activated(self)
		self.featureName = "Text"
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
		FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)

	def finish(self,closed=False):
		"terminates the operation"
		Creator.finish(self)
		self.view.removeEventCallback("SoEvent",self.call)
		del self.snap
		del self.dialog

	def createObject(self):
		"creates an object in the current doc"
		self.doc.openTransaction("Create "+self.featureName) 
		self.obj=self.doc.addObject("App::Annotation",self.featureName)
		self.doc.commitTransaction()
		self.obj.LabelText=self.text.encode('latin1')
		self.obj.Position=self.node[0]
		formatObject(self.obj)
		select(self.obj)
		self.finish()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg)

		elif (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				point,dtrlPoint = getPoint(self,arg)
				self.node.append(point)
				self.ui.textUi()
				self.ui.textValue.setFocus()
				self.ui.cross(False)

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		point = Vector(numx,numy,numz)
		self.node.append(point)
		self.ui.textUi()
		self.ui.textValue.setFocus()
		self.ui.cross(False)

class Dim(Creator):
	'''
	This class creates a dimension feature.
	'''
	def __init__(self):
		self.max=2

	def GetResources(self):
		return {'Pixmap'  : 'Draft_dimension',
			'MenuText': 'Dimension',
			'ToolTip': 'Creates a dimension. CTRL to snap, SHIFT to constrain, ALT to select a segment'}

	def Activated(self):
		Creator.Activated(self)
		self.featureName = "Dimension"
		self.ui.lineUi()
		self.altdown = False
		self.call = self.view.addEventCallback("SoEvent",self.action)
		self.snap = snapTracker()
		self.dimtrack = dimTracker()
		self.constraintrack = lineTracker(dotted=True)
		FreeCAD.Console.PrintMessage("Pick first point:\n")
		FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)

	def finish(self,closed=False):
		"terminates the operation"
		Creator.finish(self)
		self.view.removeEventCallback("SoEvent",self.call)
		del self.dimtrack
		del self.constraintrack
		del self.snap

	def createObject(self):
		"creates an object in the current doc"
		self.doc.openTransaction("Create "+self.featureName) 
		obj = FreeCAD.ActiveDocument.addObject("App::FeaturePython","Dimension")
		Dimension(obj)
		DimensionViewProvider(obj.ViewObject)
		obj.Start = self.node[0]
		obj.End = self.node[1]
		obj.Dimline = self.node[2]
		self.doc.commitTransaction()
		formatObject(obj)
		select(obj)
		self.doc.recompute()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg)
			if arg["AltDown"] and (len(self.node)<3):
				if not self.altdown:
					self.ui.cross(False)
					self.altdown = True
					self.ui.switchUi(True)
				snapped = self.view.getObjectInfo((arg["Position"][0],arg["Position"][1]))
				if snapped:
					ob = self.doc.getObject(snapped['Object'])
					num = int(snapped['Component'].lstrip('Edge'))-1
					ed = ob.Shape.Edges[num]
					v1 = ed.Vertexes[0].Point
					v2 = ed.Vertexes[-1].Point
					self.dimtrack.update([v1,v2])
			else:
				if self.altdown:
					self.ui.cross(True)
					self.altdown = False
					self.ui.switchUi(False)
				self.dimtrack.update(self.node+[point])
				# Draw constraint tracker line.
				if (arg["ShiftDown"]):
					self.constraintrack.p1(point)
					self.constraintrack.p2(ctrlPoint)
					self.constraintrack.on()
				else: self.constraintrack.off()
		elif (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				point,ctrlPoint = getPoint(self,arg)
				if arg["AltDown"] and (len(self.node)<3):
					snapped = self.view.getObjectInfo((arg["Position"][0],arg["Position"][1]))
					if snapped:
						ob = self.doc.getObject(snapped['Object'])
						num = int(snapped['Component'].lstrip('Edge'))-1
						ed = ob.Shape.Edges[num]
						v1 = ed.Vertexes[0].Point
						v2 = ed.Vertexes[-1].Point
						self.node = [v1,v2]
						self.dimtrack.on()
				else:
					self.node.append(point)
				self.dimtrack.update(self.node)
				if (len(self.node) == 1):
					self.dimtrack.on()
				elif (len(self.node) == 3):
					self.createObject()
					self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		point = Vector(numx,numy,numz)
		self.node.append(point)
		self.dimtrack.update(self.node)
		if (len(self.node) == 1):
			self.dimtrack.on()
		elif (len(self.node) == 3):
			self.createObject()
			self.finish()



#---------------------------------------------------------------------------
# Modifier functions
#---------------------------------------------------------------------------

class Modifier:
	"This stores general stuff for all modifier tools"

	def Activated(self):
		self.doc = FreeCAD.ActiveDocument
		if not self.doc: self.finish()
		if FreeCADGui.activeWorkbench().activeDraftCommand: self.finish()
		self.view = FreeCADGui.ActiveDocument.ActiveView
		self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
		FreeCADGui.activeWorkbench().activeDraftCommand = self
		FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)
		self.node = []
		self.ui.sourceCmd = self
		self.constrain = None
		self.obj = None
		
	def finish(self):
		self.node = []
		self.ui.offUi()
		self.ui.sourceCmd=None
		FreeCADGui.activeWorkbench().activeDraftCommand = None
		FreeCAD.Console.PrintMessage("")
		self.ui.cross(False)
			
class Move(Modifier):
	"This class translates the selected objects from a point to another point."

	def GetResources(self):
		return {'Pixmap'  : 'Draft_move',
			'MenuText': 'Move',
			'ToolTip': 'Moves the selected objects between 2 points. CTRL to snap, SHIFT to constrain, ALT to copy'}

	def Activated(self):
		Modifier.Activated(self)
		self.ui.cmdlabel.setText("Move")
		self.featureName = "Move"
		self.call = None
		if not FreeCADGui.Selection.getSelection():
			self.ghost = None
			self.snap = None
			self.linetrack = None
			self.constraintrack = None
			self.ui.selectUi()
			FreeCAD.Console.PrintMessage("Select an object to move\n")
			self.call = self.view.addEventCallback("SoEvent",selectObject)
		else:
			self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = FreeCADGui.Selection.getSelection()
		self.ui.pointUi()
		self.ui.xValue.setFocus()
		self.ui.xValue.selectAll()
		self.snap = snapTracker()
		self.linetrack = lineTracker()
		self.constraintrack = lineTracker(dotted=True)
		self.ghost = ghostTracker(self.sel)
		self.call = self.view.addEventCallback("SoEvent",self.action)
		FreeCAD.Console.PrintMessage("Pick start point:\n")
		self.ui.cross(True)

	def finish(self,closed=False):
		Modifier.finish(self)
		self.view.removeEventCallback("SoEvent",self.call)
		del self.ghost
		del self.snap
		del self.linetrack
		del self.constraintrack

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
			elif (ob.Type == "App::Annotation"):
				ob.Position = ob.Position.add(delta)
			elif (ob.Type == "App::FeaturePython"):
				if 'Dimline' in ob.PropertiesList:
					ob.Start = ob.Start.add(delta)
					ob.End = ob.End.add(delta)
					ob.Dimline = ob.Dimline.add(delta)

		if copy: formatObject(newob,ob)
		self.doc.commitTransaction()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg)
			self.linetrack.p2(point)
			# Draw constraint tracker line.
			if (arg["ShiftDown"]):
				self.constraintrack.p1(point)
				self.constraintrack.p2(ctrlPoint)
				self.constraintrack.on()
			else: self.constraintrack.off()
			if (len(self.node) > 0):
				last = self.node[len(self.node)-1]
				delta = point.sub(last)
				self.ghost.trans.translation.setValue([delta.x,delta.y,delta.z])
		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				point,ctrlPoint = getPoint(self,arg)
				if (self.node == []):
					self.node.append(point)
					self.ui.isRelative.show()
					self.ui.isCopy.show()
					self.linetrack.on()
					self.ghost.on()
					self.linetrack.p1(point)
					FreeCAD.Console.PrintMessage("Pick end point:\n")
				else:
					last = self.node[-1]
					if self.ui.isCopy.isChecked() or arg["AltDown"]:
						self.move(point.sub(last),True)
					else:
						self.move(point.sub(last))
					self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		point = Vector(numx,numy,numz)
		if not self.node:
			self.node.append(point)
			self.ui.isRelative.show()
			self.ui.isCopy.show()
			self.linetrack.p1(point)
			self.linetrack.on()
			self.ghost.on()
			FreeCAD.Console.PrintMessage("Pick end point point:\n")
		else:
			last = self.node[-1]
			if self.ui.isCopy.isChecked():
				self.move(point.sub(last),True)
			else:
				self.move(point.sub(last))
			self.finish()

			
class ApplyStyle(Modifier):
	"this class applies the current line width and line color to selected objects"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_apply',
			'MenuText': 'Apply Style',
			'ToolTip': 'Applies current line width and color to selected objects'}

	def Activated(self):
		Modifier.Activated(self)
		self.sel = FreeCADGui.Selection.getSelection()
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


	
			
class Rotate(Modifier):
	'''
	This class rotates the selected objects.
	'''


	def GetResources(self):
		return {'Pixmap'  : 'Draft_rotate',
			'MenuText': 'Rotate',
			'ToolTip': 'Rotates the selected objects. CTRL to snap, SHIFT to constrain, ALT creates a copy'}

	def Activated(self):
		Modifier.Activated(self)
		self.ui.cmdlabel.setText("Rotate")
		self.featureName = "Rotate"
		self.call = None
		if not FreeCADGui.Selection.getSelection():
			self.ghost = None
			self.snap = None
			self.linetrack = None
			self.arctrack = None
			self.constraintrack = None
			self.ui.selectUi()
			FreeCAD.Console.PrintMessage("Select an object to rotate\n")
			self.call = self.view.addEventCallback("SoEvent",selectObject)
		else:
			self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = FreeCADGui.Selection.getSelection()
		self.step = 0
		self.center = None
		self.ui.arcUi()
		self.ui.cmdlabel.setText("Rotate")
		self.snap = snapTracker()
		self.linetrack = lineTracker()
		self.constraintrack = lineTracker(dotted=True)
		self.arctrack = arcTracker()
		self.ghost = ghostTracker(self.sel)
		self.call = self.view.addEventCallback("SoEvent",self.action)
		FreeCAD.Console.PrintMessage("Pick rotation center:\n")
		self.ui.cross(True)
				
	def finish(self,closed=False):
		"finishes the arc"
		Modifier.finish(self)
		self.view.removeEventCallback("SoEvent",self.call)
		del self.snap
		del self.linetrack
		del self.constraintrack
		del self.arctrack
		del self.ghost
		self.doc.recompute()

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
			point,ctrlPoint = getPoint(self,arg)
			if (self.step == 0):
				pass
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
				point,ctrlPoint = getPoint(self,arg)
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
		self.center = Vector(numx,numy,numz)
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



class Offset(Modifier):
	"This class offsets the selected objects."

	def GetResources(self):
		return {'Pixmap'  : 'Draft_offset',
			'MenuText': 'Offset',
			'ToolTip': 'Offsets the active object. CTRL to snap, SHIFT to constrain, ALT to copy'}

	def Activated(self):
		Modifier.Activated(self)
		self.ui.cmdlabel.setText("Offset")
		self.featureName = "Offset"
		self.call = None
		if not FreeCADGui.Selection.getSelection():
			self.ghost = None
			self.snap = None
			self.linetrack = None
			self.arctrack = None
			self.constraintrack = None
			self.ui.selectUi()
			FreeCAD.Console.PrintMessage("Select an object to offset\n")
			self.call = self.view.addEventCallback("SoEvent",selectObject)
		elif len(FreeCADGui.Selection.getSelection()) > 1:
			FreeCAD.Console.PrintWarning("Offset only works on one object at a time\n")
		else:
			self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = FreeCADGui.Selection.getSelection()[0]
		self.step = 0
		self.constrainSeg = None
		self.ui.radiusUi()
		self.ui.isCopy.show()
		self.ui.labelRadius.setText("Distance")
		self.ui.cmdlabel.setText("Offset")
		self.ui.radiusValue.setFocus()
		self.ui.radiusValue.selectAll()
		self.snap = snapTracker()
		self.linetrack = lineTracker()
		self.constraintrack = lineTracker(dotted=True)
		self.faces = False
		self.edges = []
		c = fcgeo.complexity(self.sel)	
		if (c >= 7): 
			self.edges = fcgeo.getBoundary(self.sel.Shape)
			self.faces = True
			self.edges = fcgeo.sortEdges(self.edges)
		elif (c >=4): 
			self.edges = self.sel.Shape.Wires[0].Edges
			self.edges = fcgeo.sortEdges(self.edges)
		else: self.edges = self.sel.Shape.Edges	
		self.ghost = []
		for e in self.edges:
			if isinstance(e.Curve,Part.Line): self.ghost.append(lineTracker())
			else: self.ghost.append(arcTracker())
		self.call = self.view.addEventCallback("SoEvent",self.action)
		FreeCAD.Console.PrintMessage("Pick distance:\n")
		self.ui.cross(True)

	def finish(self,closed=False):
		Modifier.finish(self)
		self.view.removeEventCallback("SoEvent",self.call)
		del self.snap
		del self.linetrack
		del self.constraintrack
		del self.ghost

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"):
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			point = snapPoint(self,point,cursor,arg["CtrlDown"])
			if not self.ui.zValue.isEnabled(): point.z = float(self.ui.zValue.text())
			self.node = [point]
			if (arg["ShiftDown"]) and self.constrainSeg:
				dist = fcgeo.findPerpendicular(point,self.edges,self.constrainSeg[1])
				self.constraintrack.p1(self.edges[self.constrainSeg[1]].Vertexes[0].Point)
				self.constraintrack.p2(Vector.add(point,dist[0]))
				self.constraintrack.on()
			else:
				dist = fcgeo.findPerpendicular(point,self.edges)
				self.constraintrack.off()
			if dist:
				self.constrainSeg = dist
				self.linetrack.on()
				for g in self.ghost: g.on()
				self.linetrack.p1(point)
				dvec = Vector.add(point,dist[0])
				self.linetrack.p2(dvec)
				self.redraw(dist)
				self.ui.radiusValue.setText("%.2f" % dist[0].Length)
			else:
				self.constrainSeg = None
				self.linetrack.off()
				for g in self.ghost: g.off()
				self.ui.radiusValue.setText("off")
			self.ui.radiusValue.setFocus()
			self.ui.radiusValue.selectAll()

		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				self.doc.openTransaction("Offset")
				if arg["AltDown"] or self.ui.isCopy.isChecked():
					targetOb = self.doc.addObject("Part::Feature",self.sel.Name)
					formatObject(targetOb,self.sel)
				else:
					targetOb = self.sel
				cursor = arg["Position"]
				point = self.view.getPoint(cursor[0],cursor[1])
				point = snapPoint(self,point,cursor,arg["CtrlDown"])
				if not self.ui.zValue.isEnabled(): point.z = float(self.ui.zValue.text())
				self.node = [point]
				if (arg["ShiftDown"]) and self.constrainSeg:
					dist = fcgeo.findPerpendicular(point,self.edges,self.constrainSeg[1])
				else:
					dist = fcgeo.findPerpendicular(point,self.edges)
				if dist:
					newedges = self.redraw(dist,True)
					if self.faces:
						targetOb.Shape = Part.Face(Part.Wire(newedges))
					else:
						targetOb.Shape = Part.Wire(newedges)
					self.doc.commitTransaction()
				self.finish()




	def redraw(self,dist,real=False):
		"offsets the ghost to the given distance. if real=True, the shape itself will be redrawn too"
		closed = Part.Wire(self.edges).isClosed()
		offsetVec = fcvec.neg(dist[0])
		if real: newedges=[]

		#finding the base vector, which is the segment at base of the offset vector
		base = self.edges[dist[1]]
		if (isinstance(base.Curve,Part.Line)):
			baseVec = fcgeo.vec(base)
		else:
			baseVec = fcvec.crossproduct(offsetVec)
			if fcvec.new(base.Curve.Center,self.node[0]).Length > base.Curve.Radius:
				baseVec = fcvec.neg(baseVec)

		# offsetting the first vertex
		edge = self.edges[0]
		if closed and (len(self.edges)>1): prev = self.edges[-1]
		else: prev = None
		if isinstance(edge.Curve,Part.Line): perp = fcgeo.vec(edge)
		else: perp = fcvec.crossproduct(fcvec.new(edge.Vertexes[0].Point,edge.Curve.Center))
		angle = fcvec.angle(baseVec,perp)
		offset1 = fcvec.rotate(offsetVec,angle)
		offedge1 = fcgeo.offset(edge,offset1)
		if prev:
			if (isinstance(prev.Curve,Part.Line)): perp = fcgeo.vec(prev)
			else: perp = fcvec.crossproduct(fcvec.new(prev.Vertexes[0].Point,prev.Curve.Center))
			angle = fcvec.angle(baseVec,perp)
			offset2 = fcvec.rotate(offsetVec,angle)
			offedge2 = fcgeo.offset(prev,offset2)
			inter = fcgeo.findIntersection(offedge1,offedge2,True,True)
			first = inter[fcgeo.findClosest(edge.Vertexes[0].Point,inter)]
		else: first = Vector.add(edge.Vertexes[0].Point,offset1)
					
		# iterating throught edges, offsetting the last vertex
		for i in range(len(self.edges)):
			edge = self.edges[i]
			if (i < len(self.edges)-1): next = self.edges[i+1]
			else:
				if closed and (len(self.edges)>1): next = self.edges[0]
				else: next = None
			if isinstance(edge.Curve,Part.Line): perp = fcgeo.vec(edge)
			else: perp = fcvec.crossproduct(fcvec.new(edge.Vertexes[0].Point,edge.Curve.Center))
			angle = fcvec.angle(baseVec,perp)
			offset1 = fcvec.rotate(offsetVec,angle)
			offedge1 = fcgeo.offset(edge,offset1)
			if next:
				if (isinstance(next.Curve,Part.Line)): perp = fcgeo.vec(next)
				else: perp = fcvec.crossproduct(fcvec.new(next.Vertexes[0].Point,next.Curve.Center))
				angle = fcvec.angle(baseVec,perp)
				offset2 = fcvec.rotate(offsetVec,angle)
				offedge2 = fcgeo.offset(next,offset2)
				inter = fcgeo.findIntersection(offedge1,offedge2,True,True)
				last = inter[fcgeo.findClosest(edge.Vertexes[-1].Point,inter)]
			else: last = Vector.add(edge.Vertexes[-1].Point,offset1)
			
			if isinstance(edge.Curve,Part.Line):
				self.ghost[i].p1(first)
				self.ghost[i].p2(last)
				if real: newedges.append(Part.Line(first,last).toShape())
			else:
				center = edge.Curve.Center
				rad = offedge1.Curve.Radius
				if len(edge.Vertexes) > 1:
					chord = fcvec.new(first,last)
					perp = fcvec.crossproduct(chord)
					scaledperp = fcvec.scale(fcvec.normalized(perp),rad)
					midpoint = Vector.add(center,scaledperp)
					ang1=fcvec.angle(fcvec.new(center,first))
					ang2=fcvec.angle(fcvec.new(center,last))
					if ang1 > ang2: ang1,ang2 = ang2,ang1
					self.ghost[i].update(ang1-ang2)
					self.ghost[i].trans.rotation.setValue(coin.SbVec3f(0,0,1),-ang1)
				self.ghost[i].trans.translation.setValue([center.x,center.y,center.z])
				self.ghost[i].trans.scaleFactor.setValue([rad,rad,rad])
				if real: 
					if (len(edge.Vertexes) > 1):
						newedges.append(Part.Arc(first,midpoint,last).toShape())
					else:
						newedges.append(Part.Circle(center,Vector(0,0,1),rad).toShape())
			first = last
		if real: return newedges
				
	def numericRadius(self,scale):
		"this function gets called by the toolbar when valid distance have been entered there"
		if self.constrainSeg:
			scaledOffset = fcvec.scale(fcvec.normalized(self.constrainSeg[0]),scale)
			self.doc.openTransaction("Offset")
			if self.ui.isCopy.isChecked():
				targetOb = self.doc.addObject("Part::Feature",self.sel.Name)
				formatObject(targetOb,self.sel)
			else:
				targetOb = self.sel
			newedges = self.redraw([scaledOffset,self.constrainSeg[1]],True)
			if self.faces:
				targetOb.Shape = Part.Face(Part.Wire(newedges))
			else:
				targetOb.Shape = Part.Wire(newedges)
			self.doc.commitTransaction()
		else: FreeCAD.Console.PrintMessage("Couldn't determine where to apply distance!\n")
		self.finish()
			

class Upgrade(Modifier):
	'''
	This class upgrades selected objects in different ways, following this list (in order):
	- if there are more than one faces, the faces are merged (union)
	- if there is only one face, nothing is done
	- if there are closed wires, they are transformed in a face
	- otherwise join all edges into a wire (closed if applicable)
	- if nothing of the above is possible, a Compound is created
	'''

	def GetResources(self):
		return {'Pixmap'  : 'Draft_upgrade',
			'MenuText': 'Upgrade',
			'ToolTip': 'Joins the selected objects into one, or converts closed wires to filled faces, or unite faces'}

	def Activated(self):
		Modifier.Activated(self)
		self.ui.cmdlabel.setText("Upgrade")			
		self.featureName = "Upgrade"
		self.call = None
		if not FreeCADGui.Selection.getSelection():
			self.ui.selectUi()
			FreeCAD.Console.PrintMessage("Select an object to upgrade\n")
			self.call = self.view.addEventCallback("SoEvent",selectObject)
		else:
			self.proceed()

	def compound(self):
		shapeslist = []
		for ob in self.sel: shapeslist.append(ob.Shape)
		newob = self.doc.addObject("Part::Feature","Compound")
		newob.Shape = Part.makeCompound(shapeslist)
		return newob
		
	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = FreeCADGui.Selection.getSelection()
		loneEdges = False
		loneFaces = False
		edges = []
		wires = []
		openwires = []
		faces = []
		# determining which level we will have
		for ob in self.sel:
			if ob.Shape.ShapeType == 'Edge': loneEdges = True
			if ob.Shape.ShapeType == 'Face': loneFaces = True
			for f in ob.Shape.Faces:
				faces.append(f)
			for w in ob.Shape.Wires:
				if w.isClosed():
					wires.append(w)
				else:
					openwires.append(w)
			lastob = ob
		# applying transformation
		self.doc.openTransaction("Upgrade")
		if faces:
			if loneEdges:
				newob = self.compound()
				formatObject(newob,lastob)
			else:
				u = faces.pop(0)
				for f in faces:
					u = u.fuse(f)
					u = fcgeo.concatenate(u)
				newob = self.doc.addObject("Part::Feature","Union")
				newob.Shape = u
				formatObject(newob,lastob)
		elif wires:
			if loneEdges or loneFaces or openwires:
				newob = self.compound()
				formatObject(newob,lastob)
			else:
				for w in wires:
					f = Part.Face(w)
					faces.append(f)
				for f in faces:
					newob = self.doc.addObject("Part::Feature","Face")
					newob.Shape = f
					formatObject(newob,lastob)
		elif (len(openwires) == 1):
			if loneEdges or loneFaces:
				newob = self.compound()
				formatObject(newob,lastob)
			else:
				p0 = openwires[0].Vertexes[0].Point
				p1 = openwires[0].Vertexes[-1].Point
				edges = openwires[0].Edges
				edges.append(Part.Line(p1,p0).toShape())
				w = Part.Wire(fcgeo.sortEdges(edges))
				newob = self.doc.addObject("Part::Feature","Wire")
				newob.Shape = w
				formatObject(newob,lastob)
		else:
			for ob in self.sel:
				for e in ob.Shape.Edges:
					edges.append(e)
			newob = None
			try:
				w = Part.Wire(fcgeo.sortEdges(edges))
				if len(w.Edges) == len(ob.Shape.Edges):
					newob = self.doc.addObject("Part::Feature","Wire")
					newob.Shape = w
			except:
				pass
			if not newob: newob = self.compound()
			formatObject(newob,lastob)
		for ob in self.sel:
			self.doc.removeObject(ob.Name)
		self.doc.commitTransaction()
		select(newob)
		Modifier.finish(self)
				
class Downgrade(Modifier):
	'''
	This class downgrades selected objects in different ways, following this list (in order):
	- if there are more than one faces, the subsequent faces are subtracted from the first one
	- if there is only one face, it gets converted to a wire
	- otherwise wires are exploded into single edges
	'''

	def GetResources(self):
		return {'Pixmap'  : 'Draft_downgrade',
			'MenuText': 'Downgrade',
			'ToolTip': 'Explodes the selected objects into simpler objects, or subtract faces'}

	def Activated(self):
		Modifier.Activated(self)
		self.ui.cmdlabel.setText("Downgrade")		
		self.featureName = "Downgrade"
		self.call = None
		if not FreeCADGui.Selection.getSelection():
			self.ui.selectUi()
			FreeCAD.Console.PrintMessage("Select an object to upgrade\n")
			self.call = self.view.addEventCallback("SoEvent",selectObject)
		else:
			self.proceed()


	def proceed(self):
		self.sel = FreeCADGui.Selection.getSelection()
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
			if len(self.sel) == 1:
				for f in faces:
					newob = self.doc.addObject("Part::Feature","Face")
					newob.Shape = f
					formatObject(newob,self.sel[0])
				self.doc.removeObject(ob.Name)
			else:
				u = faces.pop(0)
				for f in faces:
					u = u.cut(f)
				newob = self.doc.addObject("Part::Feature","Subtraction")
				newob.Shape = u
				for ob in self.sel:
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
		Modifier.finish(self)
		




class Trimex(Modifier):
	"this tool trims or extends lines, polylines and arcs. SHIFT constrains to the last point."

	def GetResources(self):
		return {'Pixmap' : 'Draft_trimex',
			'MenuText' : 'TrimExtend',
			'ToolTip' : 'Trims or Extends the selected object. SHIFT constrains to current segment, ALT inverts'}

	def Activated(self):
		Modifier.Activated(self)
		self.ui.cmdlabel.setText("Trimex")
		self.featureName = "Trimex"
		self.call = None
		if not FreeCADGui.Selection.getSelection():
			self.ghost = None
			self.snap = None
			self.linetrack = None
			self.constraintrack = None
			self.ui.selectUi()
			FreeCAD.Console.PrintMessage("Select an object to trim/extend\n")
			self.call = self.view.addEventCallback("SoEvent",selectObject)
		else:
			self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = FreeCADGui.Selection.getSelection()[0]
		if self.sel.Shape.isClosed(): self.finish()
		self.ui.radiusUi()
		self.ui.labelRadius.setText("Distance")
		self.ui.radiusValue.setFocus()
		self.ui.radiusValue.selectAll()
		self.snap = snapTracker()
		self.linetrack = lineTracker()
		self.constraintrack = lineTracker(dotted=True)
		self.sel.ViewObject.Visibility = False

		self.edges = []
		c = fcgeo.complexity(self.sel)	
		if (c >= 7): 
			self.finish()
		elif (c >= 4): 
			self.edges = self.sel.Shape.Wires[0].Edges
			self.edges = fcgeo.sortEdges(self.edges)
		else: self.edges = self.sel.Shape.Edges	
		self.ghost = []
		lc = self.sel.ViewObject.LineColor
		scolor = (lc[0],lc[1],lc[2])
		swidth = self.sel.ViewObject.LineWidth
		for e in self.edges:
			if isinstance(e.Curve,Part.Line): self.ghost.append(lineTracker(scolor=scolor,swidth=swidth))
			else: self.ghost.append(arcTracker(scolor=scolor,swidth=swidth))
		for g in self.ghost: g.on()

		self.activePoint = 0
		self.nodes = []
		self.shift = False
		self.alt = False
		self.force = None
		self.call = self.view.addEventCallback("SoEvent",self.action)
		FreeCAD.Console.PrintMessage("Pick distance:\n")
		self.ui.cross(True)

				
	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			self.shift = arg["ShiftDown"]
			self.alt = arg["AltDown"]
			if arg["CtrlDown"]: self.snapped = None
			else: self.snapped = self.view.getObjectInfo((cursor[0],cursor[1]))
			self.point = snapPoint(self,point,cursor,arg["CtrlDown"])
			if not self.ui.zValue.isEnabled(): self.point.z = float(self.ui.zValue.text())
			dist = self.redraw(self.point,self.snapped,self.shift,self.alt)
			self.constraintrack.p1(point)
			self.constraintrack.p2(self.newpoint)
			self.constraintrack.on()
			self.ui.radiusValue.setText("%.2f" % dist)
			self.ui.radiusValue.setFocus()
			self.ui.radiusValue.selectAll()
		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				cursor = arg["Position"]
				point = self.view.getPoint(cursor[0],cursor[1])
				self.shift = arg["ShiftDown"]
				self.alt = arg["AltDown"]
				if arg["CtrlDown"]: self.snapped = None
				else: self.snapped = self.view.getObjectInfo((cursor[0],cursor[1]))
				self.point = snapPoint(self,point,cursor,arg["CtrlDown"])
				if not self.ui.zValue.isEnabled(): self.point.z = float(self.ui.zValue.text())
				self.trimObject()
				self.finish()

	def redraw(self,point,snapped=None,shift=False,alt=False,real=None):
		"redraws the ghost"
		#initializing
		reverse = False
		for g in self.ghost: g.off()
		if real: newedges = []
		
		# finding the active point
		vlist = []
		for e in self.edges: vlist.append(e.Vertexes[0].Point)
		vlist.append(self.edges[-1].Vertexes[-1].Point)
		if shift: npoint = self.activePoint
		else: npoint = fcgeo.findClosest(point,vlist)
		if npoint > len(self.edges)/2: reverse = True
		if alt: reverse = not reverse
		self.activePoint = npoint
		
		# sorting out directions
		if reverse and (npoint > 0): npoint = npoint-1
		if (npoint > len(self.edges)-1):
			edge = self.edges[-1]
			ghost = self.ghost[-1]
		else:
			edge = self.edges[npoint]
			ghost = self.ghost[npoint]
		if reverse:
			v1 = edge.Vertexes[-1].Point
			v2 = edge.Vertexes[0].Point
		else:
			v1 = edge.Vertexes[0].Point
			v2 = edge.Vertexes[-1].Point

		# snapping
		if snapped:
			snapped = self.doc.getObject(snapped['Object'])
			pts = []
			for e in snapped.Shape.Edges:
				int = fcgeo.findIntersection(edge,e,True,True)
				if int: pts.extend(int)
			if pts:
				point = pts[fcgeo.findClosest(point,pts)]

		# modifying active edge
		if isinstance(edge.Curve,Part.Line):
			perp = fcvec.crossproduct(fcgeo.vec(edge))
			chord = fcvec.new(point,v1)
			proj = fcvec.project(chord,perp)
			self.newpoint = Vector.add(point,proj)
			dist = fcvec.new(self.newpoint,v1).Length
			ghost.p1(self.newpoint)
			ghost.p2(v2)
			self.ui.labelRadius.setText("Distance")
			if real:
				if self.force:
					ray = fcvec.new(v1,self.newpoint)
					ray = fcvec.scale(ray,self.force/ray.Length)
					self.newpoint = Vector.add(v1,ray)
				newedges.append(Part.Line(self.newpoint,v2).toShape())
		else:
			center = edge.Curve.Center
			rad = edge.Curve.Radius
			ang1 = fcvec.angle(fcvec.new(center,v2))
			ang2 = fcvec.angle(fcvec.new(center,point))
			self.newpoint=Vector.add(center,fcvec.rotate(Vector(rad,0,0),-ang2))
			self.ui.labelRadius.setText("Angle")
			dist = math.degrees(-ang2)
			if ang1 > ang2: ang1,ang2 = ang2,ang1
			ghost.update(ang1-ang2)
			ghost.startangle(ang1)
			ghost.center(center)
			ghost.radius(rad)
			if real:
				if self.force:
					angle = math.radians(self.force)
					newray = fcvec.rotate(Vector(rad,0,0),angle)
					self.newpoint = Vector.add(center,newray)
				chord = fcvec.new(self.newpoint,v2)
				perp = fcvec.crossproduct(chord)
				scaledperp = fcvec.scale(fcvec.normalized(perp),rad)
				midpoint = Vector.add(center,scaledperp)
				newedges.append(Part.Arc(self.newpoint,midpoint,v2).toShape())
		ghost.on()

		# resetting the visible edges
		if not reverse: list = range(npoint+1,len(self.edges))
		else: list = range(npoint-1,-1,-1)
		for i in list:
			edge = self.edges[i]
			ghost = self.ghost[i]
			if isinstance(edge.Curve,Part.Line):
				ghost.p1(edge.Vertexes[0].Point)
				ghost.p2(edge.Vertexes[-1].Point)
			else:
				ang1 = fcvec.angle(fcvec.new(center,edge.Vertexes[0].Point))
				ang2 = fcvec.angle(fcvec.new(center,edge.Vertexes[-1].Point))
				if ang1 > ang2: ang1,ang2 = ang2,ang1
				ghost.update(ang1-ang2)
				ghost.startangle(ang1)
				ghost.center(edge.Curve.Center)
				ghost.radius(edge.Curve.Radius)
			if real: newedges.append(edge)
			ghost.on()
			
		# finishing
		if real: return newedges
		else: return dist

	def trimObject(self):
		"trims the actual object"
		edges = self.redraw(self.point,self.snapped,self.shift,self.alt,real=True)
		newshape = Part.Wire(edges)
		self.doc.openTransaction("Trim/extend")
		self.sel.Shape = newshape
		self.doc.commitTransaction()
		for g in self.ghost: g.off()

	def finish(self,closed=False):		
		Modifier.finish(self)
		self.view.removeEventCallback("SoEvent",self.call)
		self.ui.labelRadius.setText("Distance")
		del self.snap
		del self.linetrack
		del self.constraintrack
		del self.ghost
		self.sel.ViewObject.Visibility = True
		select(self.sel)

	def numericRadius(self,dist):
		"this function gets called by the toolbar when valid distance have been entered there"
		self.force = dist
		self.trimObject()
		self.finish()
		


#---------------------------------------------------------------------------
# Adds the icons & commands to the FreeCAD command manager
#---------------------------------------------------------------------------


		
# drawing commands
FreeCADGui.addCommand('Draft_Line',Line())
FreeCADGui.addCommand('Draft_Polyline',Polyline())
FreeCADGui.addCommand('Draft_Circle',Circle())
FreeCADGui.addCommand('Draft_Arc',Arc())
FreeCADGui.addCommand('Draft_Text',Text())
FreeCADGui.addCommand('Draft_Rectangle',Rectangle())
FreeCADGui.addCommand('Draft_Dimension',Dim())

# context commands
FreeCADGui.addCommand('Draft_FinishLine',FinishLine())
FreeCADGui.addCommand('Draft_CloseLine',CloseLine())
FreeCADGui.addCommand('Draft_UndoLine',UndoLine())

# modification commands
FreeCADGui.addCommand('Draft_Move',Move())
FreeCADGui.addCommand('Draft_ApplyStyle',ApplyStyle())
FreeCADGui.addCommand('Draft_Rotate',Rotate())
FreeCADGui.addCommand('Draft_Offset',Offset())
FreeCADGui.addCommand('Draft_Upgrade',Upgrade())
FreeCADGui.addCommand('Draft_Downgrade',Downgrade())
FreeCADGui.addCommand('Draft_Trimex',Trimex())
