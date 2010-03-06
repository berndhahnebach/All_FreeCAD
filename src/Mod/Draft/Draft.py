#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2009, 2010                                              *  
#*   Yorik van Havre <yorik@gmx.fr>, Ken Cline <cline@frii.com>            *  
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

from __future__ import division

__title__="FreeCAD Draft Workbench"
__author__ = "Yorik van Havre, Werner Mayer, Martin Burbaum, Ken Cline"
__url__ = "http://free-cad.sourceforge.net"

'''
General description:

	The Draft module is a FreeCAD module for drawing/editing 2D entities.
	The aim is to give FreeCAD basic 2D-CAD capabilities (similar
	to Autocad and other similar software). This modules is made to be run
	inside FreeCAD and needs the PyQt4 and pivy modules available.

User manual:

	http://sourceforge.net/apps/mediawiki/free-cad/index.php?title=2d_Drafting_Module

How it works / how to extend:

	This module is written entirely in python. If you know a bit of python
	language, you are welcome to modify this module or to help us to improve it.
	Suggestions are also welcome on the FreeCAD discussion forum.

	If you want to have a look at the code, here is a general explanation. The
	Draft module is divided in two files: Draft.py (this one) and draftGui.py.
	The Draft.py file is doing all the actual cad operation, while in draftGui.py
	you have the ui part, ie. the draft command bar. Both files are loaded at
	module init by InitGui.py, which is called directly by FreeCAD. In Draft.py,
	you have:
	- General functions, like snap, constraint, format, that are shared by all tools
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

ToDo list:

	- Provide a method for off-plane geometry creation (e.g. a DisablePlane tool)
        - Add support for meshes in SelectPlane
	- Implement a Qt translation system
	- Pressing ctrl should immediately update the marker displayed when picking.
	- Shift tab should move focus to previous item (presumably a bug outside of Draft)
	- Picking fails to notice some endpoints and midpoints.  Sometimes
	  flashes end points on an off while the mouse moves with random state
	  when mouse stops
	- circle ignores length
	- When attempting to create a circle tangent to two existing circles, fcgeo
	  fails:  NameError: global name 'circlefrom2Circles1Point' is not defined
	- Fix offset tool geometry calculation. Should use openCascade's offset tools.
	  Can we use Part.OffsetCurve?  -- probably not :-(
	- Scale applied to lines produces BSplineCurve objects, which cause
	  fcgeo to report "Unsupported curve type" when snapping (in findIntersection)
	- DimensionViewProvider sets norm to Vector(0,0,1).  I don't know what
	  is going on here, but this seems dangerous.
	- Implement a way to specify the working plane.  Make sure all tools
	  function properly in all planes.
	- The use of angles to specify arcs is not well defined in general.
	  How do we specify u-direction (the x-axis may not always be the
	  right choice)?
	- fix arc keyboard specifications
	- is the arctracker in rotate useful?
	- Rectangle displays junk immediately after first point is selected.
	- The following exception is raised by drawing a self-intersecting
	  quadrilateral, updrading it, then trying to snap while creating a line:

	  [1;33m<type 'exceptions.ReferenceError'>
	  [0mTraceback (most recent call last):
	  File "/Users/cline/Library/Preferences/FreeCAD/Mod/Draft/Draft.py", line 926, in action
	  point,ctrlPoint = getPoint(self,arg)
	  File "/Users/cline/Library/Preferences/FreeCAD/Mod/Draft/Draft.py", line 372, in getPoint
	  point = snapPoint(target,point,args["Position"],args["CtrlDown"])
	  File "/Users/cline/Library/Preferences/FreeCAD/Mod/Draft/Draft.py", line 190, in snapPoint
	  if (lastObj[0].Type[:4] == "Part"):
	  ReferenceError: Cannot access attribute 'Type' of deleted object

	- Do we need to delay geometry creation until after the event
	  callback?  The trackers do this, but it is not done for the "real"
	  geometry.  The answer is probably yes - this could cause core dumps. Use
	  PyQt4.QtCore.QTimer.singleShot(msec,callable) to schedule geometry
	  creation (OCC geometry, too!)
	- There needs to be a global facility for finishing the active
	  command, regardless of which workbench it is in.

	- Bug in fcgeo.findIntersection for line and arc (circle?) not on the same plane:
	  [1;33m<type 'exceptions.UnboundLocalError'>
	  [0mTraceback (most recent call last):
	  File "/Users/cline/Library/Preferences/FreeCAD/Mod/Draft/Draft.py", line 1247, in action
	    point,ctrlPoint = getPoint(self,arg)
	  File "/Users/cline/Library/Preferences/FreeCAD/Mod/Draft/Draft.py", line 469, in getPoint
	      point = snapPoint(target,point,args["Position"],args["CtrlDown"])
	  File "/Users/cline/Library/Preferences/FreeCAD/Mod/Draft/Draft.py", line 289, in snapPoint
	      pt = fcgeo.findIntersection(j,k)
	  File "/Users/cline/Library/Preferences/FreeCAD/Mod/Draft/draftlibs/fcgeo.py", line 170, in findIntersection
	      d = vec1.dot(toPlane)
	  UnboundLocalError: local variable 'vec1' referenced before assignment

	- Rectangle can produce non-planar result
'''

# import FreeCAD modules

import PyQt4
import FreeCAD, FreeCADGui, Part, WorkingPlane, math, sys
sys.path.append(FreeCAD.ConfigGet("AppHomePath")+"/bin") # temporary hack for linux
from FreeCAD import Base, Vector
from draftlibs import fcvec
from draftlibs import fcgeo

# Pre-run tests

try:
	from pivy import coin
	if FreeCADGui.getSoDBVersion() != coin.SoDB.getVersion():
		raise AssertionError("FreeCAD and Python-Pivy use different versions of Coin. This will lead to unexpected behaviour.")
except AssertionError:
	FreeCAD.Console.PrintMessage("Error: FreeCAD and Python-Pivy use different versions of Coin. This will lead to unexpected behaviour.")
	raise
except ImportError:
	FreeCAD.Console.PrintMessage("Error: The Python-Pivy package must be installed on your system to use the Draft module")
	raise
except:
	FreeCAD.Console.PrintMessage("Error: Unknown error while trying to load the Python-Pivy package")
	raise


#---------------------------------------------------------------------------
# Global state
#---------------------------------------------------------------------------

lastObj = [0,0] # last snapped objects, for quick intersection calculation

class todo:
	''' static todo class, delays execution of functions.  Use todo.delay
	to schedule geometry manipulation that would crash coin if done in the
	event callback'''

	'''List of (function, argument) pairs to be executed by
	PyQt4.QTcore.QTimer.singleShot(0,doTodo).'''
	itinerary = []

	@staticmethod
	def doTasks():
		for f, arg in todo.itinerary:
			try: f(arg)
			except:
				msg = "[Draft.todo] Unexpected error:" + sys.exc_info()[0]
				FreeCAD.Console.PrintWarning (msg)
		todo.itinerary = []

	@staticmethod
	def delay (f, arg):
		if todo.itinerary == []:
			PyQt4.QtCore.QTimer.singleShot(0, todo.doTasks)
		todo.itinerary.append((f,arg))


''' plane selection code used to look like this:
	if self.ui.lockedz:
		self.axis = Vector(0,0,1)
		self.xVec = Vector(1,0,0)
		self.upVec = Vector(0,1,0)
	else:
		self.axis = fcvec.neg(self.view.getViewDirection())
		rot = self.view.getCameraNode().orientation.getValue()
		self.upVec = Vector(rot.multVec(coin.SbVec3f((0,1,0))).getValue())
		self.xVec = fcvec.rotate(self.upVec,math.pi/2,fcvec.neg(self.axis))
'''
plane = WorkingPlane.plane()

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

	# checking if alwaySnap setting is on
	if FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft").\
		GetBool("alwaysSnap"): ctrl = True
	
	snapped=target.view.getObjectInfo((cursor[0],cursor[1]))
	if (snapped == None):
		target.snap.switch.whichChild = -1
		return point
	else:
		# building snap array
		ob = target.doc.getObject(snapped['Object'])

		# snapArray contains lists of 3 items: point that activates
		# snap, type, point to snap to. Type is 0=vertex point (filled
		# circle), 1=calculated point (filled square), 2=other (open
		# circle), 3=open square (intersection)
		snapArray=[] 

		if (ctrl) and (ob.Type[:4] == "Part"):
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
					if (lastObj[0].Type[:4] == "Part"):
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
				self.ui.translate("snapPoint: debug 'i[0]' is 'None'\n")
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
		target.snap.switch.whichChild = 0
		return newpoint[2]

def constrainPoint (target,point,mobile=False,sym=False):
	'''
	Constrain function
	On commands that need to enter several points (currently only line/polyline),
	you can constrain the next point to be picked to the last drawn point by
	pressing SHIFT. The vertical or horizontal constraining depends on the
	position of your mouse in relation to last point at the moment you press
	SHIFT. if mobile=True, mobile behaviour applies. If sym=True, x alway = y
	'''
	if len(target.node) > 0:
		last = target.node[-1]
		maxx = abs(point.x - last.x)
		maxy = abs(point.y - last.y)
		if target.ui.zValue.isEnabled():
			maxz = abs(point.z - last.z)
		else:
			maxz = 0
		if ((target.constrain == None) or mobile):
			if ((maxx > maxy) and (maxx > maxz)):
				point.y = last.y
				point.z = last.z
				if sym:
					point.y = last.y+(point.x-last.x)
					point.z = last.z+(point.x-last.x)
				target.constrain = 0 #x direction
				target.ui.xValue.setEnabled(True)
				target.ui.yValue.setEnabled(False)
				target.ui.zValue.setEnabled(False)
			elif ((maxy > maxx) and (maxy > maxz)):
				point.x = last.x
				point.z = last.z
				if sym:
					point.x = last.x+(point.y-last.y)
					point.z = last.z+(point.y-last.y)
				target.constrain = 1 #y direction
				target.ui.xValue.setEnabled(False)
				target.ui.yValue.setEnabled(True)
				target.ui.zValue.setEnabled(False)
			elif ((maxz > maxx) and (maxz > maxy)):
				point.x = last.x
				point.y = last.y
				if sym:
					point.x = last.x+(point.z-last.z)
					point.y = last.y+(point.z-last.z)
				target.constrain = 2 #z direction
				target.ui.xValue.setEnabled(False)
				target.ui.yValue.setEnabled(False)
				target.ui.zValue.setEnabled(True)
			else: target.constrain = 3
		elif (target.constrain == 0):
			point.y = last.y
			point.z = last.z
			if sym:
				point.y = last.y+(point.x-last.x)
				point.z = last.z+(point.x-last.x)
		elif (target.constrain == 1):
			point.x = last.x
			point.z = last.z
			if sym:
				point.x = last.x+(point.y-last.y)
				point.z = last.z+(point.y-last.y)
		elif (target.constrain == 2):
			point.x = last.x
			point.y = last.y
			if sym:
				point.x = last.x+(point.z-last.z)
				point.y = last.y+(point.z-last.z)
			
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
	if (target.Type == "App::Annotation"):
		obrep.TextColor=col
	else:
		if not origin:
			obrep.LineWidth = lw
			obrep.LineColor = col
		else:
			matchrep = origin.ViewObject
			obrep.LineWidth = matchrep.LineWidth
			obrep.LineColor = matchrep.LineColor
			if (target.Type[:4] == "Part"):
				obrep.ShapeColor = matchrep.ShapeColor

def getSelection():
	return FreeCADGui.Selection.getSelection(FreeCAD.ActiveDocument.Name)

def select(object):
	"deselects everything and selects only the passed object"
	FreeCADGui.Selection.clearSelection()
	FreeCADGui.Selection.addSelection(object)

def selectObject(arg):
	"this is a scene even handler, to be called from the tools when they need to select an object"
	if (arg["Type"] == "SoKeyboardEvent"):
		if (arg["Key"] == "ESCAPE"):
			FreeCAD.activeDraftCommand.finish()
			#TODO : this part raises a coin3D warning about scene traversal, to be fixed.
	if (arg["Type"] == "SoMouseButtonEvent"):
		if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
			selection = getSelection()
			cursor = arg["Position"]
			snapped = FreeCADGui.ActiveDocument.ActiveView.getObjectInfo((cursor[0],cursor[1]))
			if snapped:
				obj = FreeCAD.ActiveDocument.getObject(snapped['Object'])
				FreeCADGui.Selection.addSelection(obj)
				FreeCAD.activeDraftCommand.component=snapped['Component']
				FreeCAD.activeDraftCommand.proceed()

def getPoint(target,args,mobile=False,sym=False):
	'''
	returns a constrained 3d point and its original point.
	if mobile=True, the constraining occurs from the location of
	mouse cursor when Shift is pressed, otherwise from last entered
	point. If sym=True, x and y values stay always equal
	'''
	ui = FreeCADGui.activeWorkbench().draftToolBar.ui
	view = FreeCADGui.ActiveDocument.ActiveView
	point = view.getPoint(args["Position"][0],args["Position"][1])
	point = snapPoint(target,point,args["Position"],args["CtrlDown"])

	
	viewDirection = view.getViewDirection()
	if FreeCADGui.ActiveDocument.ActiveView.getCameraType() == "Perspective":
		camera = FreeCADGui.ActiveDocument.ActiveView.getCameraNode()
		p = camera.getField("position").getValue()
		# view is from camera to point:
		viewDirection = point.sub(Vector(p[0],p[1],p[2]))
	point = plane.projectPoint(point, viewDirection)
	ctrlPoint = Vector(point.x,point.y,point.z)
	if (args["ShiftDown"]): # constraining
		if mobile and (target.constrain == None):
			target.node.append(point)
		point = constrainPoint(target,point,mobile=mobile,sym=sym)
	else:
		target.constrain = None
		ui.xValue.setEnabled(True)
		ui.yValue.setEnabled(True)
		if not ui.lockedz: ui.zValue.setEnabled(True)
	#if ui.lockedz: point.z = float(ui.zValue.text()) #???removed
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
	def __init__(self,dotted=False,scolor=None,swidth=None,children=[]):
		color = coin.SoBaseColor()
		color.rgb = scolor or FreeCADGui.activeWorkbench().draftToolBar.\
			ui.getDefaultColor("ui")
		drawstyle = coin.SoDrawStyle()
		if swidth:
			drawstyle.lineWidth = swidth
		if dotted:
			drawstyle.style = coin.SoDrawStyle.LINES
			drawstyle.lineWeight = 3
			drawstyle.linePattern = 0xaa
		node = coin.SoSeparator()
		for c in [drawstyle, color] + children:
			node.addChild(c)
		self.switch = coin.SoSwitch() # this is the on/off switch
		self.switch.addChild(node)
		self.switch.whichChild = -1
		todo.delay(self._insertSwitch, self.switch)

	def finalize(self):
		todo.delay(self._removeSwitch, self.switch)
		self.switch = None

	def _insertSwitch(self, switch):
		'''insert self.switch into the scene graph.  Must not be called
		from an event handler (or other scene graph traversal).'''
		sg=FreeCADGui.ActiveDocument.ActiveView.getSceneGraph()
		sg.addChild(switch)

	def _removeSwitch(self, switch):
		'''remove self.switch from the scene graph.  As with _insertSwitch,
		must not be called during scene graph traversal).'''
		sg=FreeCADGui.ActiveDocument.ActiveView.getSceneGraph()
		sg.removeChild(switch)

	def on(self):
		self.switch.whichChild = 0

	def off(self):
		self.switch.whichChild = -1

				
class snapTracker(Tracker):
	"a class to create a snap marker symbol, used by the functions that support snapping"
	def __init__(self):
		color = coin.SoBaseColor()
		color.rgb = FreeCADGui.activeWorkbench().draftToolBar.\
			ui.getDefaultColor("snap")
		self.marker = coin.SoMarkerSet() # this is the marker symbol
		self.marker.markerIndex = coin.SoMarkerSet.CIRCLE_FILLED_9_9
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValue((0,0,0))
		node = coin.SoAnnotation()
		node.addChild(self.coords)
		node.addChild(color)
		node.addChild(self.marker)
		Tracker.__init__(self,children=[node])

class lineTracker(Tracker):
	"a class to create a tracking line used by the functions that need it"
	def __init__(self,dotted=False,scolor=None,swidth=None):
		line = coin.SoLineSet()
		line.numVertices.setValue(2)
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValues(0,2,[[0,0,0],[1,0,0]])
		Tracker.__init__(self,dotted,scolor,swidth,[self.coords,line])

	def p1(self,point):
		self.coords.point.set1Value(0,point.x,point.y,point.z)

	def p2(self,point):
		self.coords.point.set1Value(1,point.x,point.y,point.z)

class rectangleTracker(Tracker):
	"a tracking rectangle"
	def __init__(self,dotted=False,scolor=None,swidth=None):
		self.origin = Vector(0,0,0)
		line = coin.SoLineSet()
		line.numVertices.setValue(5)
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValues(0,5,[[0,0,0],[1,0,0],[1,1,0],[0,1,0],[0,0,0]])
		Tracker.__init__(self,dotted,scolor,swidth,[self.coords,line])

	def setorigin(self,point):
		self.coords.point.set1Value(0,point.x,point.y,point.z)
		self.coords.point.set1Value(4,point.x,point.y,point.z)
		self.origin = point

	def update(self,point):
		diagonal = point.sub(self.origin)
		inpoint1 = self.origin.add(fcvec.project(diagonal,plane.v))
		inpoint2 = self.origin.add(fcvec.project(diagonal,plane.u))
		self.coords.point.set1Value(1,inpoint1.x,inpoint1.y,inpoint1.z)
		self.coords.point.set1Value(2,point.x,point.y,point.z)
		self.coords.point.set1Value(3,inpoint2.x,inpoint2.y,inpoint2.z)

class dimTracker(Tracker):
	"a tracking bridge for drawing dimensions"
	def __init__(self,dotted=False,scolor=None,swidth=None):
		line = coin.SoLineSet()
		line.numVertices.setValue(4)
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValues(0,4,[[0,0,0],[0,0,0],[0,0,0],[0,0,0]])
		Tracker.__init__(self,dotted,scolor,swidth,[self.coords,line])

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
		self.center = Vector(0,0,0)
		self.start = Vector(0,0,0)
		self.angle = 0

		self.trans = coin.SoTransform()
		self.trans.translation.setValue([0,0,0])

		self.coords = coin.SoCoordinate4()
		self.circle = coin.SoNurbsCurve()

		container = coin.SoSeparator()
		container.addChild(self.coords)
		container.addChild(self.circle)

		Tracker.__init__(self,dotted,scolor,swidth,[self.trans, container])
		self.on()

	def centerPoint(self, v):
		self.center = v
		self.trans.translation.setValue(v.x, v.y, v.z)

	def startPoint(self, v):
		self.start = v

	def changeRadius(self, r):
		v = fcvec.scaleTo(self.start.sub(self.center), r)
		self.start = v.add(self.center)

	def update(self, angle):
		# This function creates a nurbs representation of the arc.
		'''
		Display the arc using a NURBS curve comprising three
		equal sections joined by double knots.  For example,
		to span 270 degrees, the contol point might be:
		[(1,0,0,1), (s,s,0,s), (0,1,0,1), (-s,s,0,s), (-1,0,0,1), (-s,-s,0,s), (0,-1,0,1)],
		where s = sqrt(2)

		For reasons I do not understand, the arc's shape
		changes when the control points are translated, so a
		coin translation is used instead.
		'''
		u = self.start.sub(self.center) # translate start for rotation
		w = math.cos(angle/6)
		def makepoint(i):
			v = fcvec.rotate(u, i*angle/6, plane.axis)
			if i%2==0:
				return (v.x, v.y, v.z, 1)
			else:
				return (v.x, v.y, v.z, w)
		control = [makepoint(i) for i in range(0,7)]
		knots = [0,0,0,1,1,2,2,3,3,3] # coin docs advise avoiding fractional knot values
		self.coords.point.setValues(control)
		self.circle.numControlPoints.setValue(len(control))
		self.circle.knotVector.setValues(0,len(knots),knots)
		self.circle.knotVector.setNum(len(knots))
		self.on()


class ghostTracker(Tracker):
	"this class creates a copy of the coin representation of all passed objects, to be used as ghost"
	def __init__(self,sel):
		self.trans = coin.SoTransform()
		self.trans.translation.setValue([0,0,0])
		children = [self.trans]
		ivsep = coin.SoSeparator()
		try:
			for ob in sel:
				ivsep.addChild(ob.ViewObject.RootNode.copy())
		except:
			print "draft: Couldn't create ghost"
		else:
			children.append(ivsep)

		Tracker.__init__(self,children=children)

				
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
		prm = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
		obj.addProperty("App::PropertyLength","FontSize","Base","Font size").FontSize=prm.GetFloat("textheight")
		obj.addProperty("App::PropertyString","FontName","Base","Font Name").FontName=prm.GetString("textfont")
		obj.addProperty("App::PropertyLength","LineWidth","Base","Line width")
		obj.addProperty("App::PropertyColor","LineColor","Base","Line color")
		obj.addProperty("App::PropertyLength","ExtLines","Base","Ext lines").ExtLines=0.3
		obj.Proxy = self
		self.Object = obj.Object
		self.ViewObject = obj

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
				p1 = p2.add(fcvec.scaleTo(proj,dmax))
				p4 = p3.add(fcvec.scaleTo(proj,dmax))
		midpoint = p2.add(fcvec.scale(p3.sub(p2),0.5))
		if not proj:
			ed = fcgeo.vec(base)
			proj = ed.cross(Vector(0,0,1))
		angle = -fcvec.angle(p3.sub(p2))
		# ???What is the conditional trying to accomplish?  The guard expression seems suspect.
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
		self.onChanged(obj,"FontName")

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
		elif prop == "FontName":
			self.font.name = str(vp.FontName)
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

	def getrgb(self,color):
		"returns a rgb value #000000 from a freecad color"
		r = str(hex(int(color[0]*255)))[2:].zfill(2)
		g = str(hex(int(color[1]*255)))[2:].zfill(2)
		b = str(hex(int(color[2]*255)))[2:].zfill(2)
		return "#"+r+g+b

	def getSVG(self):
		"returns an svg representation of the dimension"
		p1,p2,p3,p4,tbase,angle,norm = self.calcGeom(self.Object)
		svg='<g id="'+self.Object.Name+'"><path '
		svg+='d="M '+str(p1.x)+' '+str(p1.y)+' '
		svg+='L '+str(p2.x)+' '+str(p2.y)+' '
		svg+='L '+str(p3.x)+' '+str(p3.y)+' '
		svg+='L '+str(p4.x)+' '+str(p4.y)+'" '
		svg+='fill="none" stroke="'
		svg+=self.getrgb(self.ViewObject.LineColor) + '" '
		svg+='stroke-width="' + str(self.ViewObject.LineWidth/100) + ' px" '
		svg+='style="stroke-width:'+ str(self.ViewObject.LineWidth/100)
		svg+=';stroke-miterlimit:4;stroke-dasharray:none"/>\n'
		svg+='<circle cx="'+str(p2.x)+'" cy="'+str(p2.y)
		svg+='" r="'+str(self.ViewObject.FontSize/10)+'" '
		svg+='fill="'+ self.getrgb(self.ViewObject.LineColor) +'" stroke="none" '
		svg+='style="stroke-miterlimit:4;stroke-dasharray:none"/>\n'
		svg+='<circle cx="'+str(p3.x)+'" cy="'+str(p3.y)
		svg+='" r="'+str(self.ViewObject.FontSize/10)+'" '
		svg+='fill="#000000" stroke="none" '
		svg+='style="stroke-miterlimit:4;stroke-dasharray:none"/>\n'
		svg+='<text id="' + self.Object.Name + '" fill="'
		svg+=str(self.ViewObject.LineWidth/100) +'" font-size="'
		svg+=str(self.ViewObject.FontSize/10)+'" '
		svg+='style="text-anchor:middle;text-align:center" '
		svg+='transform="rotate('+str(math.degrees(angle))
		svg+=' '+str(tbase.x)+' '+str(tbase.y)+')" '
		svg+='x="' + str(tbase.x) + '" y="' + str(tbase.y) + '">\n'
		svg+="%.2f" % p3.sub(p2).Length
		svg+='</text>\n</g>\n'
		return svg

class SelectPlane:
	"choose a plane for Draft module geometry creation"

	def GetResources(self):
		return {'MenuText': 'SelectPlane',
			'ToolTip': 'Select plane for geometry creation'}

	def Activated(self):
		if FreeCAD.activeDraftCommand:
			FreeCAD.activeDraftCommand.finish()
		self.offset = 0
		self.ui = None
		self.call = None
		self.doc = FreeCAD.ActiveDocument
		if self.doc:
			FreeCAD.activeDraftCommand = self
			self.view = FreeCADGui.ActiveDocument.ActiveView
			self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
			self.ui.selectPlaneUi()
			self.ui.translate("Pick a face to define the drawing plane\n")
			self.ui.sourceCmd = self
			if plane.alignToSelection(self.offset):
				#??? clear selection here
				self.finish()
			else:
				self.call = self.view.addEventCallback("SoEvent", self.action)

	def action(self, arg):
		if arg["Type"] == "SoKeyboardEvent" and arg["Key"] == "ESCAPE":
			self.finish()
		if arg["Type"] == "SoMouseButtonEvent":
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				cursor = arg["Position"]
				doc = FreeCADGui.ActiveDocument
				info = doc.ActiveView.getObjectInfo((cursor[0],cursor[1]))
				if info:
					try:
						shape = doc.getObject(info["Object"]).Object.Shape
						component = getattr(shape, info["Component"])
						if plane.alignToFace(component, self.offset) \
							    or plane.alignToCurve(component, self.offset):
							self.finish()
					except:
						pass

	def selectHandler(self, arg):
 		if arg == "XY":
			plane.alignToPointAndAxis(Vector(0,0,0), Vector(0,0,1), self.offset)
			self.finish()
		elif arg == "XZ":
			plane.alignToPointAndAxis(Vector(0,0,0), Vector(0,1,0), self.offset)
			self.finish()
		elif arg == "YZ":
			plane.alignToPointAndAxis(Vector(0,0,0), Vector(1,0,0), self.offset)
			self.finish()
			

	def offsetHandler(self, arg):
		self.offset = arg

	def finish(self):
		if self.call:
			self.view.removeEventCallback("SoEvent",self.call)
		FreeCAD.activeDraftCommand = None
		if self.ui:
			self.ui.offUi()


#---------------------------------------------------------------------------
# Geometry constructors
#---------------------------------------------------------------------------

class Creator:
	" General settings for all geometry creation tools"
	def Activated(self):
		if FreeCAD.activeDraftCommand:
			FreeCAD.activeDraftCommand.finish()
		self.ui = None
		self.call = None
		self.doc = None
		self.doc = FreeCAD.ActiveDocument
		self.view = FreeCADGui.ActiveDocument.ActiveView
		if not self.doc:
			self.finish()
		else:
			FreeCAD.activeDraftCommand = self
			self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
			FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)

			#??? need to get offset to calculate point from ui
			plane.setup(fcvec.neg(self.view.getViewDirection()), Vector(0,0,0))
			self.ui.cross(True)
			self.node = []
			self.pos = []
			self.ui.sourceCmd = self
			self.constrain = None
			self.obj = None
		
	def finish(self):
		self.node=[]
		FreeCAD.activeDraftCommand = None
		if self.ui:
			self.ui.offUi()
			self.ui.cross(False)
			self.ui.sourceCmd = None
			self.ui.translate("")
		if self.call:
			self.view.removeEventCallback("SoEvent",self.call)
	

class Line(Creator):
	'''
	This class creates a line or group of lines feature. 
	Takes 1 optional argument, the max number of points.
	'''
	def __init__(self, polyline=False):
		self.isPolyline = polyline

	def GetResources(self):
		return {'Pixmap'  : 'Draft_line',
			'MenuText': 'Line',
			'ToolTip': 'Creates a 2-point line. CTRL to snap, SHIFT to constrain'}

	def Activated(self):
		Creator.Activated(self)
		if self.doc:
			self.featureName = "Line"
			self.ui.lineUi()
			self.snap = snapTracker()
			self.linetrack = lineTracker()
			self.constraintrack = lineTracker(dotted=True)
			self.call = self.view.addEventCallback("SoEvent",self.action)
			self.ui.translate("Pick first point:\n")

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
		if self.ui:
			self.linetrack.finalize()
			self.constraintrack.finalize()
			self.snap.finalize()
		Creator.finish(self)

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
					if (not self.isPolyline and len(self.node) == 2):
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
			self.ui.translate("Pick next point:\n")
		elif (len(self.node) == 2):
			self.createObject()
			last = self.node[len(self.node)-2]
			newseg = Part.Line(last,point).toShape()
			self.obj.Shape = newseg
			if self.isPolyline:
				self.ui.translate("Pick next point, or (F)inish or (C)lose:\n")
		else:
			currentshape = self.obj.Shape
			last = self.node[len(self.node)-2]
			newseg = Part.Line(last,point).toShape()
			newshape=currentshape.fuse(newseg)
			self.obj.Shape = newshape
			self.ui.translate("Pick next point, or (F)inish or (C)lose:\n")

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		point = Vector(numx,numy,numz)
		self.node.append(point)
		self.linetrack.p1(point)
		self.drawSegment(point)
		if (not self.isPolyline and len(self.node) == 2):
			self.finish(False)
		if self.ui.xValue.isEnabled():
			self.ui.xValue.setFocus()
			self.ui.xValue.selectAll()
		else:
			self.ui.yValue.setFocus()
			self.ui.yValue.selectAll()



class Polyline(Line):
	def __init__(self):
		Line.__init__(self,polyline=True)
	def GetResources(self):
		return {'Pixmap'  : 'Draft_polyline',
			'MenuText': 'Polyline',
			'ToolTip': 'Creates a 2-point line. CTRL to snap, SHIFT to constrain'}


class FinishLine:
	"a FreeCAD command to finish any running Line drawing operation"
	def Activated(self):
		if (FreeCAD.activeDraftCommand != None):
			if (FreeCAD.activeDraftCommand.featureName == "Line"):
				FreeCAD.activeDraftCommand.finish(False)
	def GetResources(self):
		return {'Pixmap'  : 'Draft_finish',
			'MenuText': 'Finish Line',
			'ToolTip': 'Finishes a line without closing it'}

	
class CloseLine:
	"a FreeCAD command to close any running Line drawing operation"
	def Activated(self):
		if (FreeCAD.activeDraftCommand != None):
			if (FreeCAD.activeDraftCommand.featureName == "Line"):
				FreeCAD.activeDraftCommand.finish(True)
	def GetResources(self):
		return {'Pixmap'  : 'Draft_lock',
			'MenuText': 'Close Line',
			'ToolTip': 'Closes the line being drawn'}


class UndoLine:
	"a FreeCAD command to undo last drawn segment of a line"
	def Activated(self):
		if (FreeCAD.activeDraftCommand != None):
			if (FreeCAD.activeDraftCommand.featureName == "Line"):
				FreeCAD.activeDraftCommand.undolast()
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
		if self.ui:
			self.featureName = "Rectangle"
			self.refpoint = None
			self.ui.lineUi()
			self.ui.cmdlabel.setText("Rectangle")
			self.call = self.view.addEventCallback("SoEvent",self.action)
			self.snap = snapTracker()
			self.rect = rectangleTracker()
			self.ui.translate("Pick first point:\n")

	def finish(self,closed=False):
		"terminates the operation and closes the poly if asked"
		Creator.finish(self) 
		if self.ui:
			self.rect.off()
			self.rect.finalize()
			self.snap.finalize()

	def createObject(self):
		"creates the final object in the current doc"
		p1 = self.node[0]
		p3 = self.node[-1]
		diagonal = p3.sub(p1)
		p2 = p1.add(fcvec.project(diagonal, plane.v))
		p4 = p1.add(fcvec.project(diagonal, plane.u))
		shape = Part.makePolygon([p1,p2,p3,p4,p1])
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
			self.ui.translate("Pick opposite point:\n")
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
			'ToolTip': 'Creates an arc. CTRL to snap, SHIFT to constrain'}

	def Activated(self):
		Creator.Activated(self)
		if self.ui:
			self.step = 0
			self.doc.openTransaction("Create "+self.featureName)
			self.obj = self.doc.addObject("Part::Feature",self.featureName)
			self.doc.commitTransaction()
			formatObject(self.obj)
			self.center = None
			self.rad = None
			self.angle = 0 # angle inscribed by arc
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
			self.ui.translate("Pick center point:\n")

	def finish(self,closed=False):
		"finishes the arc"
		Creator.finish(self)
		if self.ui:
			if (self.rad == None): self.doc.undo()
			elif not(self.closedCircle) and (self.step < 4): self.doc.undo()
			self.snap.finalize()
			self.linetrack.finalize()
			self.constraintrack.finalize()
			self.arctrack.finalize()
			self.doc.recompute()

	def updateAngle(self, angle):
		# previous absolute angle
		lastangle = self.firstangle + self.angle
		if lastangle <= -2*math.pi: lastangle += 2*math.pi
		if lastangle >= 2*math.pi: lastangle -= 2*math.pi
		# compute delta = change in angle:
		d0 = angle-lastangle
		d1 = d0 + 2*math.pi
		d2 = d0 - 2*math.pi
		if abs(d0) < min(abs(d1), abs(d2)):
			delta = d0
		elif abs(d1) < abs(d2):
			delta = d1
		else:
			delta = d2
		newangle = self.angle + delta
		# normalize angle, preserving direction
		if newangle >= 2*math.pi: newangle -= 2*math.pi
		if newangle <= -2*math.pi: newangle += 2*math.pi
		self.angle = newangle

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"):
			point,ctrlPoint = getPoint(self,arg)
			# this is to make sure radius is what you see on screen
			if self.center and fcvec.dist(point,self.center) > 0:
				viewdelta = fcvec.project(point.sub(self.center), plane.axis)
				if not fcvec.isNull(viewdelta):
					point = point.add(fcvec.neg(viewdelta))
			if (self.step == 0): # choose center
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
			elif (self.step == 1): # choose radius
				if len(self.tangents) == 2:
					cir = fcgeo.circleFrom2tan1pt(self.tangents[0], self.tangents[1], point)
					self.center = fcgeo.findClosestCircle(point,cir).Center
					self.arctrack.centerPoint(self.center)
				elif self.tangents and self.tanpoints:
					cir = fcgeo.circleFrom1tan2pt(self.tangents[0], self.tanpoints[0], point)
					self.center = fcgeo.findClosestCircle(point,cir).Center
					self.arctrack.centerPoint(self.center)
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
							self.arctrack.centerPoint(self.center)
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
				self.arctrack.startPoint(point)
				self.arctrack.changeRadius(self.rad)
				self.arctrack.update(math.radians(360))
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
			elif (self.step == 2): # choose first angle
				currentrad = fcvec.dist(point,self.center)
				if currentrad != 0:
					angle = fcvec.angle(plane.u, point.sub(self.center), plane.axis)
				else: angle = 0
				self.linetrack.p2(fcvec.scaleTo(point.sub(self.center),self.rad).add(self.center))
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
			else: # choose second angle
				currentrad = fcvec.dist(point,self.center)
				if currentrad != 0:
					angle = fcvec.angle(plane.u, point.sub(self.center), plane.axis)
				else: angle = 0
				self.linetrack.p2(fcvec.scaleTo(point.sub(self.center),self.rad).add(self.center))
				# Draw constraint tracker line.
				if (arg["ShiftDown"]):
					self.constraintrack.p1(point)
					self.constraintrack.p2(ctrlPoint)
					self.constraintrack.on()
				else: self.constraintrack.off()
				self.ui.radiusValue.setText("%.2f" % math.degrees(angle))
				self.updateAngle(angle)
				self.arctrack.update(self.angle)
				self.ui.radiusValue.setFocus()
				self.ui.radiusValue.selectAll()

		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				point,ctrlPoint = getPoint(self,arg)
				# this is to make sure radius is what you see on screen
				if self.center and fcvec.dist(point,self.center) > 0:
					viewdelta = fcvec.project(point.sub(self.center), plane.axis)
					if not fcvec.isNull(viewdelta):
						point = point.add(fcvec.neg(viewdelta))
				if (self.step == 0): # choose center
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
								self.ui.translate("Pick radius:\n")
					else:
						if len(self.tangents) == 1:
							self.tanpoints.append(point)
						else:
							self.center = point
							self.node = [point]
							self.arctrack.centerPoint(self.center)
							self.linetrack.p1(self.center)
							self.linetrack.p2(self.view.getPoint(arg["Position"][0],arg["Position"][1]))
						self.arctrack.on()
						self.ui.radiusUi()
						self.step = 1
						self.linetrack.on()
						self.ui.translate("Pick radius:\n")
				elif (self.step == 1): # choose radius
					if self.closedCircle:
						self.ui.cross(False)
						self.drawArc()
					else: 
						self.ui.labelRadius.setText("Start angle")
						self.linetrack.p1(self.center)
						self.linetrack.on()
						self.step = 2
						self.ui.translate("Pick start angle:\n")
				elif (self.step == 2): # choose first angle
					self.ui.labelRadius.setText("End angle")
					self.step = 3
					# scale center->point vector for proper display
					u = fcvec.scaleTo(point.sub(self.center), self.rad)
					self.arctrack.startPoint(u.add(self.center))
					self.ui.translate("Pick end angle:\n")
				else: # choose second angle
					self.step = 4
					self.drawArc()

	def drawArc(self):
		"actually draws the FreeCAD object"
		if self.closedCircle:
			arc = Part.Circle(self.center, plane.axis, self.rad).toShape()
		else:
			radvec = fcvec.scaleTo(plane.u, self.rad)
			p1 = Vector.add(self.center,fcvec.rotate(radvec, self.firstangle, plane.axis))
			p3 = Vector.add(self.center,
					fcvec.rotate(radvec, self.firstangle+self.angle, plane.axis))
			mid = self.firstangle + self.angle/2
			p2 = Vector.add(self.center, fcvec.rotate(radvec, mid, plane.axis))
			arc = Part.Arc(p1,p2,p3).toShape()
		self.obj.Shape = arc
		formatObject(self.obj)
		select(self.obj)
		self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		self.center = Vector(numx,numy,numz)
		self.node = [self.center]
		self.arctrack.centerPoint(self.center)
		self.arctrack.on()
		self.ui.radiusUi()
		self.step = 1
		self.ui.translate("Pick radius")
		
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
				self.arctrack.centerPoint(self.center)
				# synthesize a vector in the arc's plane 
				# ??? At this point, (1,0,0) might be good enough.  This code
				# shows one way to guess an orthogonal vector.
				if fcvec.equals(plane.axis, Vector(1,0,0)): ortho = Vector(0,self.rad,0)
				else: ortho = fcvec.scaleTo(Vector(1,0,0).cross(plane.axis), self.rad)
				self.arctrack.startPoint(ortho.add(self.center))
				self.arctrack.update(math.radians(360))
				self.ui.labelRadius.setText("Start angle")
				self.linetrack.p1(self.center)
				self.linetrack.on()
				self.ui.translate("Pick start angle:\n")
		elif (self.step == 2):
			self.ui.labelRadius.setText("End angle")
			self.firstangle = math.radians(rad)
			if fcvec.equals(plane.axis, Vector(1,0,0)): u = Vector(0,self.rad,0)
			else: u = fcvec.scaleTo(Vector(1,0,0).cross(plane.axis), self.rad)
			urotated = fcvec.rotate(u, math.radians(rad), plane.axis)
			self.arctrack.startPoint(urotated)
			self.step = 3
			self.ui.translate("Pick end angle:\n")
		else:
			self.updateAngle(rad)
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
		if self.ui:
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
			self.ui.translate("Pick location point:\n")
			FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)

	def finish(self,closed=False):
		"terminates the operation"
		Creator.finish(self)
		if self.ui:
			self.snap.finalize()
			del self.dialog

	def createObject(self):
		"creates an object in the current doc"
		textbuffer = []
		for l in self.text: textbuffer.append(str(l).encode('latin1'))
		self.doc.openTransaction("Create "+self.featureName) 
		self.obj=self.doc.addObject("App::Annotation",self.featureName)
		self.doc.commitTransaction()
		self.obj.LabelText=textbuffer
		self.obj.Position=self.node[0]
		params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
		self.obj.ViewObject.DisplayMode="World"
		self.obj.ViewObject.FontSize=params.GetFloat("textheight")
		self.obj.ViewObject.FontName=params.GetString("textfont")
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
		if self.ui:
			self.featureName = "Dimension"
			self.ui.lineUi()
			self.altdown = False
			self.call = self.view.addEventCallback("SoEvent",self.action)
			self.snap = snapTracker()
			self.dimtrack = dimTracker()
			self.constraintrack = lineTracker(dotted=True)
			self.ui.translate("Pick first point:\n")
			FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)

	def finish(self,closed=False):
		"terminates the operation"
		Creator.finish(self)
		if self.ui:
			self.dimtrack.finalize()
			self.constraintrack.finalize()
			self.snap.finalize()

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
		self.ui = None
		self.call = None
		self.doc = FreeCAD.ActiveDocument
		if not self.doc:
			self.finish()
		else:
			if FreeCAD.activeDraftCommand: FreeCAD.activeDraftCommand.finish()
			FreeCAD.activeDraftCommand = self
			self.view = FreeCADGui.ActiveDocument.ActiveView
			self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
			FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)
			#??? point should obey ui.
			plane.setup(fcvec.neg(self.view.getViewDirection()), Vector(0,0,0))
			self.node = []
			self.ui.sourceCmd = self
			self.constrain = None
			self.obj = None
			self.extendedCopy = False
		
	def finish(self):
		self.node = []
		FreeCAD.activeDraftCommand = None
		if self.ui:
			self.ui.offUi()
			self.ui.sourceCmd=None
			self.ui.translate("")
			self.ui.cross(False)
		if self.call:
			self.view.removeEventCallback("SoEvent",self.call)
			
class Move(Modifier):
	"This class translates the selected objects from a point to another point."

	def GetResources(self):
		return {'Pixmap'  : 'Draft_move',
			'MenuText': 'Move',
			'ToolTip': 'Moves the selected objects between 2 points. CTRL to snap, SHIFT to constrain, ALT to copy'}

	def Activated(self):
		Modifier.Activated(self)
		if self.ui:
			self.ui.cmdlabel.setText("Move")
			self.featureName = "Move"
			self.call = None
			if not getSelection():
				self.ghost = None
				self.snap = None
				self.linetrack = None
				self.constraintrack = None
				self.ui.selectUi()
				self.ui.translate("Select an object to move\n")
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = getSelection()
		self.ui.pointUi()
		self.ui.xValue.setFocus()
		self.ui.xValue.selectAll()
		self.snap = snapTracker()
		self.linetrack = lineTracker()
		self.constraintrack = lineTracker(dotted=True)
		self.ghost = ghostTracker(self.sel)
		self.call = self.view.addEventCallback("SoEvent",self.action)
		self.ui.translate("Pick start point:\n")
		self.ui.cross(True)

	def finish(self,closed=False):
		Modifier.finish(self)
		if self.ui:
			self.ghost.finalize()
			self.snap.finalize()
			self.linetrack.finalize()
			self.constraintrack.finalize()

	def move(self,delta,copy=False):
		"moving the real shapes"
		if copy: self.doc.openTransaction("Copy")
		else: self.doc.openTransaction("Move")
		for ob in self.sel:
			if (ob.Type[:4] == "Part"):
				if copy: newob = self.doc.addObject("Part::Feature",ob.Name)
				else: newob = ob
				sh = ob.Shape
				sh.translate(delta)
				newob.Shape = sh
			elif (ob.Type == "App::Annotation"):
				if copy:
					newob = self.doc.addObject("App::Annotation",ob.Name)
					newob.LabelText = ob.LabelText
				else: newob = ob
				newob.Position = ob.Position.add(delta)
			elif (ob.Type == "App::FeaturePython"):
				if 'Dimline' in ob.PropertiesList:
					if copy:
						newob = self.doc.addObject("App::FeaturePython",ob.Name)
						Dimension(newob)
						DimensionViewProvider(newob.ViewObject)
					else: newob = ob
					newob.Start = ob.Start.add(delta)
					newob.End = ob.End.add(delta)
					newob.Dimline = ob.Dimline.add(delta)
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
			if self.extendedCopy:
				if not arg["AltDown"]: self.finish()
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
					self.ui.translate("Pick end point:\n")
				else:
					last = self.node[0]
					if self.ui.isCopy.isChecked() or arg["AltDown"]:
						self.move(point.sub(last),True)
					else:
						self.move(point.sub(last))
					if arg["AltDown"]:
						self.extendedCopy = True
					else:
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
			self.ui.translate("Pick end point point:\n")
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
		if self.ui:
			self.sel = getSelection()
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
		if self.ui:
			self.ui.cmdlabel.setText("Rotate")
			self.featureName = "Rotate"
			self.call = None
			if not getSelection():
				self.ghost = None
				self.snap = None
				self.linetrack = None
				self.arctrack = None
				self.constraintrack = None
				self.ui.selectUi()
				self.ui.translate("Select an object to rotate\n")
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = getSelection()
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
		self.ui.translate("Pick rotation center:\n")
		self.ui.cross(True)
				
	def finish(self,closed=False):
		"finishes the arc"
		Modifier.finish(self)
		if self.ui:
			self.snap.finalize()
			self.linetrack.finalize()
			self.constraintrack.finalize()
			self.arctrack.finalize()
			self.ghost.finalize()
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
				shape.rotate(fcvec.tup(self.center), fcvec.tup(plane.axis), math.degrees(angle))
				newob.Shape=shape
			if copy: formatObject(newob,ob)
		self.doc.commitTransaction()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"):
			point,ctrlPoint = getPoint(self,arg)
			# this is to make sure radius is what you see on screen
			if self.center and fcvec.dist(point,self.center):
				viewdelta = fcvec.project(point.sub(self.center), plane.axis)
				if not fcvec.isNull(viewdelta):
					point = point.add(fcvec.neg(viewdelta))
			if self.extendedCopy:
				if not arg["AltDown"]: self.finish()
			if (self.step == 0):
				pass
			elif (self.step == 1):
				currentrad = fcvec.dist(point,self.center)
				if (currentrad != 0):
					angle = fcvec.angle(plane.u, point.sub(self.center), plane.axis)
				else: angle = 0
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
				if (currentrad != 0):
					angle = fcvec.angle(plane.u, point.sub(self.center), plane.axis)
				else: angle = 0
				if (angle < self.firstangle): 
					sweep = (2*math.pi-self.firstangle)+angle
				else:
					sweep = angle - self.firstangle
				self.arctrack.update(sweep)
				self.ghost.trans.rotation.setValue(coin.SbVec3f(fcvec.tup(plane.axis)),sweep)
				self.linetrack.p2(point)
				# Draw constraint tracker line.
				if (arg["ShiftDown"]):
					self.constraintrack.p1(point)
					self.constraintrack.p2(ctrlPoint)
					self.constraintrack.on()
				else: self.constraintrack.off()
				self.ui.radiusValue.setText("%.2f" % math.degrees(sweep))
				self.updateAngle(angle) #??? what does this do.  can we remove it?
				self.ui.radiusValue.setFocus()
				self.ui.radiusValue.selectAll()

		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				point,ctrlPoint = getPoint(self,arg)
				if self.center and fcvec.dist(point,self.center):
					viewdelta = fcvec.project(point.sub(self.center), plane.axis)
					if not fcvec.isNull(viewdelta): point = point.add(fcvec.neg(viewdelta))
				if (self.step == 0):
					self.center = point
					self.node = [point]
					self.arctrack.startPoint(point)
					self.ui.radiusUi()
					self.ui.labelRadius.setText("Base angle")
					self.linetrack.p1(self.center)
					self.arctrack.centerPoint(self.center)
					self.ghost.trans.center.setValue(self.center.x,self.center.y,self.center.z)
					self.linetrack.on()
					self.step = 1
					self.ui.translate("Pick base angle:\n")
				elif (self.step == 1):
					self.ui.labelRadius.setText("Rotation")
					self.arctrack.startPoint(point)
					self.rad = fcvec.dist(point,self.center)
					self.arctrack.on()
					self.ghost.on()
					self.ui.isCopy.show()
					self.step = 2
					self.ui.translate("Pick rotation angle:\n")
				else:
					currentrad = fcvec.dist(point,self.center)
					angle = point.sub(self.center).getAngle(plane.u)
					if fcvec.project(point.sub(self.center), plane.v).getAngle(plane.v) > 1:
						angle = -angle
					if (angle < self.firstangle): 
						sweep = (2*math.pi-self.firstangle)+angle
					else:
						sweep = angle - self.firstangle
					if self.ui.isCopy.isChecked() or arg["AltDown"]:
						self.rot(sweep,True)
					else:
						self.rot(sweep)
					if arg["AltDown"]:
						self.extendedCopy = True
					else:
						self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		self.center = Vector(numx,numy,numz)
		self.node = [self.center]
		self.arctrack.centerPoint(self.center)
		self.ghost.trans.center.setValue(self.center.x,self.center.y,self.center.z)
		self.linetrack.p1(self.center)
		self.arctrack.on()
		self.linetrack.on()
		self.ui.radiusUi()
		self.ui.labelRadius.setText("Base angle")
		self.step = 1
		self.ui.translate("Pick base angle:\n")

	def numericRadius(self,rad):
		"this function gets called by the toolbar when valid radius have been entered there"
		if (self.step == 1):
			self.ui.labelRadius.setText("Rotation")
			self.firstangle = math.radians(rad)
			self.arctrack.startPoint(fcvec.rotate(math.radians(rad)))
			self.arctrack.on()
			self.ghost.on()
			self.ui.isCopy.show()
			self.step = 2
			self.ui.translate("Pick rotation angle:\n")
		else:
			self.rot(math.radians(rad),self.ui.isCopy.isChecked())
			self.finish()



class Offset(Modifier):
	"This class offsets the selected objects."

	def GetResources(self):
		return {'Pixmap'  : 'Draft_offset',
			'MenuText': 'Offset',
			'ToolTip': 'Offsets the active object. CTRL to snap, SHIFT to constrain, ALT to copy'}

	def Activated(self):
		Modifier.Activated(self)
		if self.ui:
			self.ui.cmdlabel.setText("Offset")
			self.featureName = "Offset"
			self.call = None
			if not getSelection():
				self.ghost = None
				self.snap = None
				self.linetrack = None
				self.arctrack = None
				self.constraintrack = None
				self.ui.selectUi()
				self.ui.translate("Select an object to offset\n")
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			elif len(getSelection()) > 1:
				self.ui.translate("Offset only works on one object at a time\n")
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = getSelection()[0]
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
		if len(self.sel.Shape.Faces)>1:
			self.ui.translate("The offset tool cannot currently work on multi-face objects\n")
			self.finish()
		c = fcgeo.complexity(self.sel)
		self.closed = False
		if (c >= 7): 
			self.edges = fcgeo.getBoundary(self.sel.Shape)
			self.faces = True
			self.closed = True
			self.edges = fcgeo.sortEdges(self.edges)
		elif (c >=4): 
			self.edges = self.sel.Shape.Wires[0].Edges
			self.edges = fcgeo.sortEdges(self.edges)
			self.closed = self.sel.Shape.Wires[0].isClosed()
		else: self.edges = self.sel.Shape.Edges
		self.ghost = []
		self.buildBissectArray()
		self.redraw = self.redrawFast
		for e in self.edges:
			if isinstance(e.Curve,Part.Line): self.ghost.append(lineTracker())
			else:
				self.ghost.append(arcTracker())
				self.redraw = self.redrawSlow
		self.call = self.view.addEventCallback("SoEvent",self.action)
		self.ui.translate("Pick distance:\n")
		self.ui.cross(True)

	def finish(self,closed=False):
		Modifier.finish(self)
		if self.ui:
			self.snap.finalize()
			self.linetrack.finalize()
			self.constraintrack.finalize()
			for g in self.ghost: g.finalize()

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
			if self.extendedCopy:
				if not arg["AltDown"]: self.finish()

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
				if arg["AltDown"]:
					self.extendedCopy = True
				else:
					self.finish()


	def buildBissectArray(self):
		'''builds an array of displacement vectors for each vertex,
		corresponding to an offset of 1 unit'''
		self.bissectors=[]
		basevec = fcgeo.vec(self.edges[0])
		baseoffset = basevec.cross(plane.axis); baseoffset.normalize()
		if self.closed: prev = self.edges[-1]
		else: prev = None

		# finding the first vector
		if prev:
			prevvec = fcgeo.vec(prev)
			angle = -fcvec.angle(prevvec, basevec, plane.axis)/2
			offset = fcvec.rotate(baseoffset, angle, plane.axis)
			sfact = math.sqrt(math.tan(angle)**2+1)
			self.bissectors.append(fcvec.scale(offset,sfact))
		else:
			self.bissectors.append(baseoffset)

		for e in range(len(self.edges)):
			if (e < len(self.edges)-1):
				next = self.edges[e+1]
			else:
				if self.closed and (len(self.edges)>1): next = self.edges[0]
				else: next = None
			edgevec = fcgeo.vec(self.edges[e])
			angle = -fcvec.angle(edgevec, basevec, plane.axis)
			offset = fcvec.rotate(baseoffset, angle, plane.axis)
			if next:
				nextvec = fcgeo.vec(next)
				angle2 = -fcvec.angle(nextvec, edgevec, plane.axis)/2
				offset2 = fcvec.rotate(offset, angle2, plane.axis)
				sfact = math.sqrt(math.tan(angle2)**2+1)
				self.bissectors.append(fcvec.scale(offset2,sfact))
			else:
				self.bissectors.append(offset)
							
	def redrawFast(self,dist,real=False):
		"hopefully faster method"
		distance = dist[0].Length
		angle = fcvec.angle(fcvec.neg(dist[0]),self.bissectors[dist[1]])
		if abs(angle) > math.pi/2: distance = -distance
		if real: newedges = []
		for e in range(len(self.edges)):
			edge = self.edges[e]
			first = edge.Vertexes[0].Point.add(fcvec.scale(self.bissectors[e],distance))
			last = edge.Vertexes[-1].Point.add(fcvec.scale(self.bissectors[e+1],distance))
			self.ghost[e].p1(first)
			self.ghost[e].p2(last)
			if real: newedges.append(Part.Line(first,last).toShape())
		if real: return newedges

	def redrawSlow(self,dist,real=False):
		"offsets the ghost to the given distance. if real=True, the shape itself will be redrawn too"
		offsetVec = fcvec.neg(dist[0])
		if real: newedges=[]

		# finding the base vector, which is the segment at base of the offset vector
		base = self.edges[dist[1]]
		if (isinstance(base.Curve,Part.Line)):
			baseVec = fcgeo.vec(base)
		else:
			baseVec = offsetVec.cross(Vector(0,0,1))
			if self.node[0].sub(base.Curve.Center).Length > base.Curve.Radius:
				baseVec = fcvec.neg(baseVec)

		# offsetting the first vertex
		edge = self.edges[0]
		if self.closed and (len(self.edges)>1): prev = self.edges[-1]
		else: prev = None
		if isinstance(edge.Curve,Part.Line): perp = fcgeo.vec(edge)
		else: perp = edge.Curve.Center.sub(edge.Vertexes[0].Point).cross(Vector(0,0,1))
		angle = fcvec.angle(baseVec,perp)
		offset1 = fcvec.rotate(offsetVec, -angle, axis=plane.axis)
		offedge1 = fcgeo.offset(edge,offset1)
		if prev:
			if (isinstance(prev.Curve,Part.Line)): perp = fcgeo.vec(prev)
			else: perp = prev.Curve.Center.sub(prev.Vertexes[0].Point).cross(Vector(0,0,1))
			angle = -fcvec.angle(baseVec,perp)
			offset2 = fcvec.rotate(offsetVec, angle, axis=plane.axis)
			offedge2 = fcgeo.offset(prev,offset2)
			inter = fcgeo.findIntersection(offedge1,offedge2,True,True)
			first = inter[fcgeo.findClosest(edge.Vertexes[0].Point,inter)]
		else: first = Vector.add(edge.Vertexes[0].Point,offset1)
					
		# iterating throught edges, offsetting the last vertex
		for i in range(len(self.edges)):
			edge = self.edges[i]
			if (i < len(self.edges)-1):
				next = self.edges[i+1]
			else:
				if self.closed and (len(self.edges)>1): next = self.edges[0]
				else: next = None
			if isinstance(edge.Curve,Part.Line): perp = fcgeo.vec(edge)
			else: perp = edge.Curve.Center.sub(edge.Vertexes[0].Point).cross(Vector(0,0,1))
			angle = fcvec.angle(baseVec,perp)
			offset1 = fcvec.rotate(offsetVec, -angle, axis=plane.axis)
			offedge1 = fcgeo.offset(edge,offset1)				     
			if next:
				if (isinstance(next.Curve,Part.Line)): perp = fcgeo.vec(next)
				else: perp = next.Curve.Center.sub(next.Vertexes[0].Point).cross(Vector(0,0,1))
				angle = fcvec.angle(baseVec,perp)
				offset2 = fcvec.rotate(offsetVec, -angle, axis=plane.axis)
				offedge2 = fcgeo.offset(next,offset2)
				inter = fcgeo.findIntersection(offedge1,offedge2,True,True)
				if inter: last = inter[fcgeo.findClosest(edge.Vertexes[-1].Point,inter)]
				else: print "debug: offset: intersection missing!"
			else: last = Vector.add(edge.Vertexes[-1].Point,offset1)
			
			if isinstance(edge.Curve,Part.Line):
				self.ghost[i].p1(first)
				self.ghost[i].p2(last)
				if real: newedges.append(Part.Line(first,last).toShape())
			else:
				center = edge.Curve.Center
				rad = offedge1.Curve.Radius
				if len(edge.Vertexes) > 1:
					chord = last.sub(first)
					perp = chord.cross(Vector(0,0,1))
					scaledperp = fcvec.scaleTo(perp,rad)
					midpoint = Vector.add(center,scaledperp)
					ang1=fcvec.angle(first.sub(center))
					ang2=fcvec.angle(last.sub(center))
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
			scaledOffset = fcvec.scaleTo(self.constrainSeg[0],scale)
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
		else: self.ui.translate("Couldn't determine where to apply distance!\n")
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
		if self.ui:
			self.ui.cmdlabel.setText("Upgrade")			
			self.featureName = "Upgrade"
			self.call = None
			if not getSelection():
				self.ui.selectUi()
				self.ui.translate("Select an object to upgrade\n")
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
		self.sel = getSelection()
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
		if self.ui:
			self.ui.cmdlabel.setText("Downgrade")		
			self.featureName = "Downgrade"
			self.call = None
			if not getSelection():
				self.ui.selectUi()
				self.ui.translate("Select an object to upgrade\n")
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			else:
				self.proceed()


	def proceed(self):
		self.sel = getSelection()
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
		if self.ui:
			self.ui.cmdlabel.setText("Trimex")
			self.featureName = "Trimex"
			self.sel = None
			if not getSelection():
				self.ghost = None
				self.snap = None
				self.linetrack = None
				self.constraintrack = None
				self.ui.selectUi()
				self.ui.translate("Select an object to trim/extend\n")
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = getSelection()[0]
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
		print c
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
		self.ui.translate("Pick distance:\n")
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
			perp = fcgeo.vec(edge).cross(Vector(0,0,1))
			chord = v1.sub(point)
			proj = fcvec.project(chord,perp)
			self.newpoint = Vector.add(point,proj)
			dist = v1.sub(self.newpoint).Length
			ghost.p1(self.newpoint)
			ghost.p2(v2)
			self.ui.labelRadius.setText("Distance")
			if real:
				if self.force:
					ray = self.newpoint.sub(v1)
					ray = fcvec.scale(ray,self.force/ray.Length)
					self.newpoint = Vector.add(v1,ray)
				newedges.append(Part.Line(self.newpoint,v2).toShape())
		else:
			center = edge.Curve.Center
			rad = edge.Curve.Radius
			ang1 = fcvec.angle(v2.sub(center))
			ang2 = fcvec.angle(point.sub(center))
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
				chord = v2.sub(self.newpoint)
				perp = chord.cross(Vector(0,0,1))
				scaledperp = fcvec.scaleTo(perp,rad)
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
				ang1 = fcvec.angle(edge.Vertexes[0].Point.sub(center))
				ang2 = fcvec.angle(edge.Vertexes[-1].Point.sub(center))
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
		if self.ui:
			self.ui.labelRadius.setText("Distance")
			self.snap.finalize()
			self.linetrack.finalize()
			self.constraintrack.finalize()
			for g in self.ghost: g.finalize()
			self.sel.ViewObject.Visibility = True
			select(self.sel)

	def numericRadius(self,dist):
		"this function gets called by the toolbar when valid distance have been entered there"
		self.force = dist
		self.trimObject()
		self.finish()

class Scale(Modifier):
	"This class scales the selected objects from a base point."

	def GetResources(self):
		return {'Pixmap'  : 'Draft_scale',
			'MenuText': 'Scale',
			'ToolTip': 'Scales the selected objects from a base point. CTRL to snap, SHIFT to constrain, ALT to copy'}

	def Activated(self):
		Modifier.Activated(self)
		if self.ui:
			self.ui.cmdlabel.setText("Scale")
			self.featureName = "Scale"
			self.call = None
			if not getSelection():
				self.ghost = None
				self.snap = None
				self.linetrack = None
				self.constraintrack = None
				self.ui.selectUi()
				self.ui.translate("Select an object to scale\n")
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = getSelection()
		self.ui.pointUi()
		self.ui.xValue.setFocus()
		self.ui.xValue.selectAll()
		self.snap = snapTracker()
		self.linetrack = lineTracker()
		self.constraintrack = lineTracker(dotted=True)
		self.ghost = ghostTracker(self.sel)
		self.call = self.view.addEventCallback("SoEvent",self.action)
		self.ui.translate("Pick base point:\n")
		self.ui.cross(True)

	def finish(self,closed=False):
		Modifier.finish(self)
		if self.ui:
			self.ghost.finalize()
			self.snap.finalize()
			self.linetrack.finalize()
			self.constraintrack.finalize()

	def scale(self,delta,copy=False):
		"moving the real shapes"
		if copy: self.doc.openTransaction("Copy")
		else: self.doc.openTransaction("Move")
		for ob in self.sel:
			if copy: newob = self.doc.addObject("Part::Feature",ob.Name)
			else: newob=ob
			if (ob.Type == "Part::Feature"):
				sh = ob.Shape
				m = FreeCAD.Matrix()
				m.scale(delta)
				sh = sh.transformGeometry(m)
				corr = Vector(self.node[0].x,self.node[0].y,self.node[0].z)
				corr.scale(delta.x,delta.y,delta.z)
				corr = fcvec.neg(corr.sub(self.node[0]))
				sh.translate(corr)
				newob.Shape = sh
			elif (ob.Type == "App::Annotation"):
				factor = delta.x * delta.y * delta.z * ob.ViewObject.FontSize
				ob.ViewObject.Fontsize = factor

		if copy: formatObject(newob,ob)
		self.doc.commitTransaction()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg,sym=True)
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
				self.ghost.trans.scaleFactor.setValue([delta.x,delta.y,delta.z])
				corr = Vector(self.node[0].x,self.node[0].y,self.node[0].z)
				corr.scale(delta.x,delta.y,delta.z)
				corr = fcvec.neg(corr.sub(self.node[0]))
				self.ghost.trans.translation.setValue([corr.x,corr.y,corr.z])
			if self.extendedCopy:
				if not arg["AltDown"]: self.finish()
		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				point,ctrlPoint = getPoint(self,arg,sym=True)
				if (self.node == []):
					self.node.append(point)
					self.ui.isRelative.show()
					self.ui.isCopy.show()
					self.linetrack.on()
					self.ghost.on()
					self.linetrack.p1(point)
					self.ui.translate("Pick scale factor:\n")
				else:
					last = self.node[0]
					if self.ui.isCopy.isChecked() or arg["AltDown"]:
						self.scale(point.sub(last),True)
					else:
						self.scale(point.sub(last))
					if arg["AltDown"]:
						self.extendedCopy = True
					else:
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
			self.ui.translate("Pick scale factor:\n")
		else:
			last = self.node[-1]
			if self.ui.isCopy.isChecked():
				self.scale(point.sub(last),True)
			else:
				self.scale(point.sub(last))
			self.finish()



#---------------------------------------------------------------------------
# Adds the icons & commands to the FreeCAD command manager
#---------------------------------------------------------------------------


		
# drawing commands
FreeCADGui.addCommand('Draft_SelectPlane',SelectPlane())
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
FreeCADGui.addCommand('Draft_Scale',Scale())
