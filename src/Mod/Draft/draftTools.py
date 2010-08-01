#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2009, 2010                                              *  
#*   Yorik van Havre <yorik@uncreated.net>, Ken Cline <cline@frii.com>     *  
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

__title__="FreeCAD Draft Workbench GUI Tools"
__author__ = "Yorik van Havre, Werner Mayer, Martin Burbaum, Ken Cline"
__url__ = "http://free-cad.sourceforge.net"

#---------------------------------------------------------------------------
# Generic stuff
#---------------------------------------------------------------------------

import FreeCAD, FreeCADGui, Part, WorkingPlane, math, re, importSVG, Draft
from PyQt4 import QtCore,QtGui
from draftlibs import fcvec,fcgeo
from FreeCAD import Vector

# loads a translation engine
languages = {"Swedish":"se","Spanish":"es",
             "English":"en","French":"fr",
             "German":"de","Italian":"it"}
ln = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/General").GetString("Language")
if ln in languages:
	translator = QtCore.QTranslator()
	translator.load("draft_"+languages[ln],Draft.getDraftPath("Languages"))
	QtGui.QApplication.installTranslator(translator)

def translate(context,text):
        "convenience function for Qt translator"
        return QtGui.QApplication.translate(context, text, None,
                                            QtGui.QApplication.UnicodeUTF8)

def msg(text=None):
        if not text: FreeCAD.Console.PrintMessage("")
        else: FreeCAD.Console.PrintMessage(str(text.toLatin1()))

try:
	from pivy import coin
	if FreeCADGui.getSoDBVersion() != coin.SoDB.getVersion():
		raise AssertionError("FreeCAD and Python-Pivy use different versions of Coin. This will lead to unexpected behaviour.")
except AssertionError:
	msg(translate("draft", "Error: FreeCAD and Python-Pivy use different versions of Coin. This will lead to unexpected behaviour.", None, QtGui.QApplication.UnicodeUTF8))
	raise
except ImportError:
	msg(translate("draft", "Error: The Python-Pivy package must be installed on your system to use the Draft module", None, QtGui.QApplication.UnicodeUTF8))
	raise
except:
	msg(translate("draft", "Error: Unknown error while trying to load the Python-Pivy package", None, QtGui.QApplication.UnicodeUTF8))
	raise

# loads the fill patterns
FreeCAD.svgpatterns = importSVG.getContents(Draft.getDraftPath('icons.svg'),'pattern')
altpat = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft").GetString("patternFile")
if altpat:
        altpatterns = importSVG.getContents(atlpat,'pattern')
        if altpatterns: FreeCAD.svgpatterns.update(altpatterns)

# sets the default working plane
plane = WorkingPlane.plane()
defaultWP = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft").GetInt("defaultWP")
if defaultWP == 1: plane.alignToPointAndAxis(Vector(0,0,0), Vector(0,0,1), 0)
elif defaultWP == 2: plane.alignToPointAndAxis(Vector(0,0,0), Vector(0,1,0), 0)
elif defaultWP == 3: plane.alignToPointAndAxis(Vector(0,0,0), Vector(1,0,0), 0)

# last snapped objects, for quick intersection calculation
lastObj = [0,0]

#---------------------------------------------------------------------------
# Global state
#---------------------------------------------------------------------------

class todo:
	''' static todo class, delays execution of functions.  Use todo.delay
	to schedule geometry manipulation that would crash coin if done in the
	event callback'''

	'''List of (function, argument) pairs to be executed by
	QTcore.QTimer.singleShot(0,doTodo).'''
	itinerary = []

	@staticmethod
	def doTasks():
		for f, arg in todo.itinerary:
			try:
                                # print "debug: executing",f
                                f(arg)
			except:
				wrn = "[Draft.todo] Unexpected error:" + sys.exc_info()[0]
				FreeCAD.Console.PrintWarning (wrn)
		todo.itinerary = []

	@staticmethod
	def delay (f, arg):
                # print "debug: delaying",f
		if todo.itinerary == []:
			QtCore.QTimer.singleShot(0, todo.doTasks)
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


#---------------------------------------------------------------------------
# Snapping stuff
#---------------------------------------------------------------------------

def snapPoint (target,point,cursor,ctrl=False):
	'''
	Snap function used by the Draft tools

	Currently has two modes: passive and active. Pressing CTRL while 
	clicking puts you in active mode:

	- In passive mode (an open circle appears), your point is
	  snapped to the nearest point on any underlying geometry.

	- In active mode (ctrl pressed, a filled circle appears), your point
	  can currently be snapped to the following points:
	    - Nodes and midpoints of all Part shapes
	    - Nodes and midpoints of lines/wires
	    - Centers and quadrant points of circles
	    - Endpoints of arcs
	    - Intersection between line, wires segments, arcs and circles
	    - When constrained (SHIFT pressed), Intersections between
	      constraining axis and lines/wires
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
                elif ctrl and ("Dimline" in ob.PropertiesList):
                        base = Part.Line(ob.Start,ob.End).toShape()
                        proj = fcgeo.findDistance(ob.Dimline,base)
                        if not proj: proj = Vector(0,0,0)
                        p2 = ob.Start.add(fcvec.neg(proj))
                        dv = ob.End.sub(ob.Start)
                        dv.multiply(0.5)
                        p2 = p2.add(dv)
                        snapArray.append([p2,0,p2])
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
			if i[0] == None: print "snapPoint: debug 'i[0]' is 'None'"
			sqdist = ((point.x-i[0].x)**2 + (point.y-i[0].y)**2 + (point.z-i[0].z)**2)
			if sqdist < shortest:
				shortest = sqdist
				newpoint = i
		target.snap.coords.point.setValue((newpoint[2].x,newpoint[2].y,newpoint[2].z))
		if (newpoint[1] == 1):
			target.snap.setMarker("square")
		elif (newpoint[1] == 0):
			target.snap.setMarker("point")
		elif (newpoint[1] == 3):
			target.snap.setMarker("square")
		else:
			target.snap.setMarker("circle")
		target.snap.switch.whichChild = 0
		return newpoint[2]

def constrainPoint (target,point,mobile=False,sym=False):
	'''
	Constrain function used by the Draft tools
	On commands that need to enter several points (currently only line/wire),
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

def selectObject(arg):
        '''this is a scene even handler, to be called from the Draft tools
        when they need to select an object'''
	if (arg["Type"] == "SoKeyboardEvent"):
		if (arg["Key"] == "ESCAPE"):
			FreeCAD.activeDraftCommand.finish()
			#TODO : this part raises a coin3D warning about scene traversal, to be fixed.
	if (arg["Type"] == "SoMouseButtonEvent"):
		if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
			cursor = arg["Position"]
			snapped = FreeCADGui.ActiveDocument.ActiveView.getObjectInfo((cursor[0],cursor[1]))
			if snapped:
				obj = FreeCAD.ActiveDocument.getObject(snapped['Object'])
				FreeCADGui.Selection.addSelection(obj)
				FreeCAD.activeDraftCommand.component=snapped['Component']
				FreeCAD.activeDraftCommand.proceed()


def getPoint(target,args,mobile=False,sym=False):
	'''
        Function used by the Draft Tools.
	returns a constrained 3d point and its original point.
	if mobile=True, the constraining occurs from the location of
	mouse cursor when Shift is pressed, otherwise from last entered
	point. If sym=True, x and y values stay always equal
	'''
	ui = FreeCADGui.activeWorkbench().draftToolBar.ui
	view = FreeCADGui.ActiveDocument.ActiveView
	point = view.getPoint(args["Position"][0],args["Position"][1])
	point = snapPoint(target,point,args["Position"],args["CtrlDown"])

	if not plane.weak:
		# working plane was explicitely selected - project onto it
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
		ui.zValue.setEnabled(True)
                if not plane.weak:
                        if fcvec.equals(plane.axis,Vector(0,0,1)):
                                ui.zValue.setEnabled(False)
                        elif fcvec.equals(plane.axis,Vector(0,1,0)):
                                ui.yValue.setEnabled(False)
                        elif fcvec.equals(plane.axis,Vector(1,0,0)):
                                ui.xValue.setEnabled(False)
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
	"A generic Draft Tracker, to be used by other specific trackers"
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
	"A Snap Mark tracker, used by tools that support snapping"
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

	def setMarker(self,style):
		if (style == "point"):
			self.marker.markerIndex = coin.SoMarkerSet.CIRCLE_FILLED_9_9
		elif (style == "square"):
			self.marker.markerIndex = coin.SoMarkerSet.SQUARE_LINE_9_9
		elif (style == "circle"):
			self.marker.markerIndex = coin.SoMarkerSet.CIRCLE_LINE_9_9

class lineTracker(Tracker):
	"A Line tracker, used by the tools that need to draw temporary lines"
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
	"A Rectangle tracker, used by the rectangle tool"
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
	"A Dimension tracker, used by the dimension tool"
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
	"An Arc tracker, used by tools that need to draw temporary arcs"
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
		[(1,0,0,1), (s,s,0,s), (0,1,0,1), (-s,s,0,s), (-1,0,0,1),
                (-s,-s,0,s), (0,-1,0,1)], where s = sqrt(2)
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
		knots = [0,0,0,1,1,2,2,3,3,3]
		self.coords.point.setValues(control)
		self.circle.numControlPoints.setValue(len(control))
		self.circle.knotVector.setValues(0,len(knots),knots)
		self.circle.knotVector.setNum(len(knots))
		self.on()


class ghostTracker(Tracker):
	"A Ghost tracker, that allows to copy whole object representations"
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

class pointEditTracker(Tracker):
	"A point edit tracker"
	def __init__(self,pos=Vector(0,0,0)):
                trm = coin.SoTransform()
                trm.rotation.setValue(coin.SbVec3f(1,0,0),math.pi/2)
                trm.scaleFactor.setValue([.2,.2,.2])
                trm.translation.setValue(coin.SbVec3f(pos.x,pos.y,pos.z))
                color = coin.SoBaseColor()
		color.rgb = FreeCADGui.activeWorkbench().draftToolBar.\
			ui.getDefaultColor("snap")
                t1 = coin.SoTransform()
                t1.rotation.setValue(coin.SbVec3f(0,0,1),math.pi/2)
                t1.scaleFactor.setValue([.2,.2,.2])
                t1.translation.setValue([-1,0,0])
                c1 = coin.SoCone()
                c2 = c1.copy()
                t2 = coin.SoTransform()
                t2.rotation.setValue(coin.SbVec3f(0,0,1),math.pi)
                t2.translation.setValue([2,0,0])
                s1 = coin.SoSeparator()
                s1.addChild(color)
                s1.addChild(t1)
                s1.addChild(c1)
                s1.addChild(t2)
                s1.addChild(c2)
                xydragger = coin.SoTranslate2Dragger()
                zdragger = coin.SoTranslate1Dragger()
                zdragger.setPart("translator",s1)
                self.marker = coin.SoDragPointDragger()
                # self.marker.translation.setValue(coin.SbVec3f(pos.x,pos.y,pos.z))
                # self.marker.setPart("xzTranslator",xydragger)
                # self.marker.setPart("yTranslator",zdragger)
                Tracker.__init__(self,scolor=color.rgb,children=[trm,self.marker])

        def setpos(self,pos):
                self.marker.translation.setValue(coin.SbVec3f(pos.x,pos.y,pos.z))

     
#---------------------------------------------------------------------------
# Helper tools
#---------------------------------------------------------------------------

def edit():
        obj = FreeCAD.ActiveDocument.ActiveObject
        if obj:
                trackers = []
                if "Points" in obj.PropertiesList:
                        for p in obj.Points:
                                trackers.append(pointEditTracker(p))
                        for t in trackers:
                                t.on()
                        return trackers
                	
class SelectPlane:
	"The Draft_SelectPlane FreeCAD command definition"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_selectPlane',
			'MenuText': str(translate("draft", "SelectPlane").toLatin1()),
			'ToolTip' : str(translate("draft", "Select a working plane for geometry creation").toLatin1())}

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
			msg(translate("draft", "Pick a face to define the drawing plane\n"))
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
                        self.display('top')
			self.finish()
		elif arg == "XZ":
			plane.alignToPointAndAxis(Vector(0,0,0), Vector(0,1,0), self.offset)
                        self.display('front')
			self.finish()
		elif arg == "YZ":
			plane.alignToPointAndAxis(Vector(0,0,0), Vector(1,0,0), self.offset)
                        self.display('side')
			self.finish()
		elif arg == "currentView":
			viewDirection = fcvec.neg(self.view.getViewDirection())
			plane.alignToPointAndAxis(Vector(0,0,0), viewDirection, self.offset)
                        self.display(viewDirection)
			self.finish()
		elif arg == "reset":
			plane.reset()
                        self.display('None')
			self.finish()

	def offsetHandler(self, arg):
		self.offset = arg

        def display(self,arg):
                if self.offset:
                        if self.offset > 0: suffix = ' + '+str(self.offset)
                        else: suffix = ' - '+str(self.offset)
                else: suffix = ''
                if type(arg).__name__  == 'str':
                        self.ui.wplabel.setText(arg+suffix)
                elif type(arg).__name__ == 'Vector':
                        plv = 'd('+str(arg.x)+','+str(arg.y)+','+str(arg.z)+')'
                        self.ui.wplabel.setText(plv+suffix)

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
	"A generic Draft Creator Tool used by creation tools such as line or arc"
	def Activated(self,name="None"):
		if FreeCAD.activeDraftCommand:
			FreeCAD.activeDraftCommand.finish()
		self.ui = None
		self.call = None
		self.doc = None
		self.doc = FreeCAD.ActiveDocument
		self.view = FreeCADGui.ActiveDocument.ActiveView
                self.featureName = name
		if not self.doc:
			self.finish()
		else:
			FreeCAD.activeDraftCommand = self
			self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
			FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)
			plane.setup(fcvec.neg(self.view.getViewDirection()), Vector(0,0,0))
			self.ui.cross(True)
			self.node = []
			self.pos = []
			self.ui.sourceCmd = self
                        self.ui.cmdlabel.setText(name)
			self.constrain = None
			self.obj = None
		
	def finish(self):
		self.node=[]
		FreeCAD.activeDraftCommand = None
		if self.ui:
			self.ui.offUi()
			self.ui.cross(False)
			self.ui.sourceCmd = None
			msg("")
		if self.call:
			self.view.removeEventCallback("SoEvent",self.call)
	

class Line(Creator):
	"The Line FreeCAD command definition"

	def __init__(self, wiremode=False):
		self.isWire = wiremode

	def GetResources(self):
		return {'Pixmap'  : 'Draft_line',
			'MenuText': str(translate("draft", "Line").toLatin1()),
			'ToolTip': str(translate("draft", "Creates a 2-point line. CTRL to snap, SHIFT to constrain").toLatin1())}

	def Activated(self):
		Creator.Activated(self,"Line")
		if self.doc:
			self.ui.lineUi()
			self.snap = snapTracker()
			self.linetrack = lineTracker()
			self.constraintrack = lineTracker(dotted=True)
			self.call = self.view.addEventCallback("SoEvent",self.action)
			msg(translate("draft", "Pick first point:\n"))

	def finish(self,closed=False):
		"terminates the operation and closes the poly if asked"
		if (len(self.node) > 1):
                        old = self.obj.Name
                        self.doc.removeObject(old)
                        self.doc.openTransaction("Create "+self.featureName)
                        Draft.makeWire(self.node,closed)
                        self.doc.commitTransaction()
		if self.ui:
			self.linetrack.finalize()
			self.constraintrack.finalize()
			self.snap.finalize()
		Creator.finish(self)

	def createTempObject(self):
		"creates a temporary object in the current doc so we can snap to it" 
		self.obj=self.doc.addObject("Part::Feature",self.featureName)

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg)
                        self.ui.cross(True)
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
					if (not self.isWire and len(self.node) == 2):
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
			msg(translate("draft", "Pick next point:\n"))
		elif (len(self.node) == 2):
			self.createTempObject()
			last = self.node[len(self.node)-2]
			newseg = Part.Line(last,point).toShape()
			self.obj.Shape = newseg
			if self.isWire:
				msg(translate("draft", "Pick next point, or (F)inish or (C)lose:\n"))
		else:
			currentshape = self.obj.Shape
			last = self.node[len(self.node)-2]
			newseg = Part.Line(last,point).toShape()
			newshape=currentshape.fuse(newseg)
			self.obj.Shape = newshape
			msg(translate("draft", "Pick next point, or (F)inish or (C)lose:\n"))

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		point = Vector(numx,numy,numz)
		self.node.append(point)
		self.linetrack.p1(point)
		self.drawSegment(point)
		if (not self.isWire and len(self.node) == 2):
			self.finish(False)
		if self.ui.xValue.isEnabled():
			self.ui.xValue.setFocus()
			self.ui.xValue.selectAll()
		else:
			self.ui.yValue.setFocus()
			self.ui.yValue.selectAll()



class Wire(Line):
        "a FreeCAD command for creating a wire"
	def __init__(self):
		Line.__init__(self,wiremode=True)
	def GetResources(self):
		return {'Pixmap'  : 'Draft_wire',
			'MenuText': str(translate("draft", "Wire").toLatin1()),
			'ToolTip': str(translate("draft", "Creates a multiple-point wire. CTRL to snap, SHIFT to constrain").toLatin1())}


class FinishLine:
	"a FreeCAD command to finish any running Line drawing operation"
	def Activated(self):
		if (FreeCAD.activeDraftCommand != None):
			if (FreeCAD.activeDraftCommand.featureName == "Line"):
				FreeCAD.activeDraftCommand.finish(False)
	def GetResources(self):
		return {'Pixmap'  : 'Draft_finish',
			'MenuText': str(translate("draft", "Finish line").toLatin1()),
			'ToolTip': str(translate("draft", "Finishes a line without closing it").toLatin1())}

	
class CloseLine:
	"a FreeCAD command to close any running Line drawing operation"
	def Activated(self):
		if (FreeCAD.activeDraftCommand != None):
			if (FreeCAD.activeDraftCommand.featureName == "Line"):
				FreeCAD.activeDraftCommand.finish(True)
	def GetResources(self):
		return {'Pixmap'  : 'Draft_lock',
			'MenuText': str(translate("draft", "Close Line").toLatin1()),
			'ToolTip': str(translate("draft", "Closes the line being drawn").toLatin1())}


class UndoLine:
	"a FreeCAD command to undo last drawn segment of a line"
	def Activated(self):
		if (FreeCAD.activeDraftCommand != None):
			if (FreeCAD.activeDraftCommand.featureName == "Line"):
				FreeCAD.activeDraftCommand.undolast()
	def GetResources(self):
		return {'Pixmap'  : 'Draft_rotate',
			'MenuText': str(translate("draft", "Undo last segment").toLatin1()),
			'ToolTip': str(translate("draft", "Undoes the last drawn segment of the line being drawn").toLatin1())}

	
class Rectangle(Creator):
	"the Draft_Rectangle FreeCAD command definition"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_rectangle',
			'MenuText': str(translate("draft", "Rectangle").toLatin1()),
			'ToolTip': str(translate("draft", "Creates a 2-point rectangle. CTRL to snap").toLatin1())}

	def Activated(self):
		Creator.Activated(self,"Rectangle")
		if self.ui:
			self.refpoint = None
			self.ui.lineUi()
			self.call = self.view.addEventCallback("SoEvent",self.action)
			self.snap = snapTracker()
			self.rect = rectangleTracker()
			msg(translate("draft", "Pick first point:\n"))

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
                length = p4.sub(p1).Length
                if abs(fcvec.angle(p4.sub(p1),plane.u,plane.axis)) > 1: length = -length
                height = p2.sub(p1).Length
                if abs(fcvec.angle(p2.sub(p1),plane.v,plane.axis)) > 1: height = -height
                p = plane.getRotation()
                p.move(p1)
                self.doc.openTransaction("Create "+self.featureName)
                Draft.makeRectangle(length,height,p,self.ui.hasFill.isChecked())
                self.doc.commitTransaction()
                self.finish()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg,mobile=True)
			self.rect.update(point)
                        self.ui.cross(True)
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
			msg(translate("draft", "Pick opposite point:\n"))
			self.rect.setorigin(point)
			self.rect.on()


class Arc(Creator):
	"the Draft_Arc FreeCAD command definition"
        
	def __init__(self):
		self.closedCircle=False
		self.featureName = "Arc"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_arc',
			'MenuText': str(translate("draft", "Arc").toLatin1()),
			'ToolTip': str(translate("draft", "Creates an arc. CTRL to snap, SHIFT to constrain").toLatin1())}

	def Activated(self):
		Creator.Activated(self,self.featureName)
		if self.ui:
			self.step = 0
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
			msg(translate("draft", "Pick center point:\n"))

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
                        self.ui.cross(True)
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
								msg(translate("draft", "Pick radius:\n"))
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
						msg(translate("draft", "Pick radius:\n"))
				elif (self.step == 1): # choose radius
					if self.closedCircle:
						self.ui.cross(False)
						self.drawArc()
					else: 
						self.ui.labelRadius.setText("Start angle")
						self.linetrack.p1(self.center)
						self.linetrack.on()
						self.step = 2
						msg(translate("draft", "Pick start angle:\n"))
				elif (self.step == 2): # choose first angle
					self.ui.labelRadius.setText("Aperture")
					self.step = 3
					# scale center->point vector for proper display
					u = fcvec.scaleTo(point.sub(self.center), self.rad)
					self.arctrack.startPoint(u.add(self.center))
					msg(translate("draft", "Pick aperture:\n"))
				else: # choose second angle
					self.step = 4
					self.drawArc()

	def drawArc(self):
		"actually draws the FreeCAD object"
                p = plane.getRotation()
                p.move(self.center)
                self.doc.openTransaction("Create "+self.featureName)
		if self.closedCircle:                       
			Draft.makeCircle(self.rad,p,self.ui.hasFill.isChecked())
		else:
                        sta = math.degrees(self.firstangle)
                        end = math.degrees(self.firstangle+self.angle)
                        print sta, end
                        if end < sta: sta,end = end,sta
                        Draft.makeCircle(self.rad,p,self.ui.hasFill.isChecked(),sta,end)
                self.doc.commitTransaction()
                self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		self.center = Vector(numx,numy,numz)
		self.node = [self.center]
		self.arctrack.centerPoint(self.center)
		self.arctrack.on()
		self.ui.radiusUi()
		self.step = 1
                self.ui.radiusValue.setFocus()
		msg(translate("draft", "Pick radius:\n"))
		
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
                                self.ui.radiusValue.setText("")
                                self.ui.radiusValue.setFocus()
				msg(translate("draft", "Pick start angle:\n"))
		elif (self.step == 2):
			self.ui.labelRadius.setText("Aperture")
			self.firstangle = math.radians(rad)
			if fcvec.equals(plane.axis, Vector(1,0,0)): u = Vector(0,self.rad,0)
			else: u = fcvec.scaleTo(Vector(1,0,0).cross(plane.axis), self.rad)
			urotated = fcvec.rotate(u, math.radians(rad), plane.axis)
			self.arctrack.startPoint(urotated)
			self.step = 3
                        self.ui.radiusValue.setText("")
                        self.ui.radiusValue.setFocus()
			msg(translate("draft", "Aperture angle:\n"))
		else:
			self.updateAngle(rad)
                        self.angle = math.radians(rad)
			self.step = 4
			self.drawArc()


class Circle(Arc):
	"The Draft_Circle FreeCAD command definition"
        
	def __init__(self):
		self.closedCircle=True
		self.featureName = "Circle"
		
	def GetResources(self):
		return {'Pixmap'  : 'Draft_circle',
			'MenuText': str(translate("draft", "Circle").toLatin1()),
			'ToolTip': str(translate("draft", "Creates a circle. CTRL to snap, ALT to select tangent objects").toLatin1())}

	
class Text(Creator):
	'''
	This class creates an annotation feature.
	'''

	def GetResources(self):
		return {'Pixmap'  : 'Draft_text',
			'MenuText': str(translate("draft", "Text").toLatin1()),
			'ToolTip': str(translate("draft", "Creates an annotation. CTRL to snap").toLatin1())}

	def Activated(self):
		Creator.Activated(self,"Text")
		if self.ui:
			self.dialog = None
			self.text = ''
			self.ui.sourceCmd = self
			self.ui.pointUi()
			self.call = self.view.addEventCallback("SoEvent",self.action)
			self.ui.xValue.setFocus()
			self.ui.xValue.selectAll()
			self.snap = snapTracker()
			msg(translate("draft", "Pick location point:\n"))
			FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)

	def finish(self,closed=False):
		"terminates the operation"
		Creator.finish(self)
		if self.ui:
			self.snap.finalize()
			del self.dialog

	def createObject(self):
		"creates an object in the current doc"
		self.doc.openTransaction("Create "+self.featureName)
                Draft.makeText(self.text,self.node[0])
		self.doc.commitTransaction()
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

class Dimension(Creator):
	"The Draft_Dimension FreeCAD command definition"
        
	def __init__(self):
		self.max=2

	def GetResources(self):
		return {'Pixmap'  : 'Draft_dimension',
			'MenuText': str(translate("draft", "Dimension").toLatin1()),
			'ToolTip': str(translate("draft", "Creates a dimension. CTRL to snap, SHIFT to constrain, ALT to select a segment").toLatin1())}

	def Activated(self):
		Creator.Activated(self,"Dimension")
		if self.ui:
			self.ui.lineUi()
			self.altdown = False
			self.call = self.view.addEventCallback("SoEvent",self.action)
			self.snap = snapTracker()
			self.dimtrack = dimTracker()
			self.constraintrack = lineTracker(dotted=True)
			msg(translate("draft", "Pick first point:\n"))
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
		Draft.makeDimension(self.node[0],self.node[1],self.node[2])
		self.doc.commitTransaction()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg)
                        self.ui.cross(True)
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
	"A generic Modifier Tool, used by modification tools such as move"

	def Activated(self,name="None"):
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
			plane.setup(fcvec.neg(self.view.getViewDirection()), Vector(0,0,0))
			self.node = []
			self.ui.sourceCmd = self
			self.constrain = None
			self.obj = None
			self.extendedCopy = False
                        self.ui.cmdlabel.setText(name)
			self.featureName = name
		
	def finish(self):
		self.node = []
		FreeCAD.activeDraftCommand = None
		if self.ui:
			self.ui.offUi()
			self.ui.sourceCmd=None
			msg("")
			self.ui.cross(False)
		if self.call:
			self.view.removeEventCallback("SoEvent",self.call)
			
class Move(Modifier):
	"The Draft_Move FreeCAD command definition"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_move',
			'MenuText': str(translate("draft", "Move").toLatin1()),
			'ToolTip': str(translate("draft", "Moves the selected objects between 2 points. CTRL to snap, SHIFT to constrain, ALT to copy").toLatin1())}

	def Activated(self):
		Modifier.Activated(self,"Move")
		if self.ui:
			if not Draft.getSelection():
				self.ghost = None
				self.snap = None
				self.linetrack = None
				self.constraintrack = None
				self.ui.selectUi()
				msg(translate("draft", "Select an object to move\n"))
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = Draft.getSelection()
                self.sel = Draft.getGroupContents(self.sel)
		self.ui.pointUi()
		self.ui.xValue.setFocus()
		self.ui.xValue.selectAll()
		self.snap = snapTracker()
		self.linetrack = lineTracker()
		self.constraintrack = lineTracker(dotted=True)
		self.ghost = ghostTracker(self.sel)
		self.call = self.view.addEventCallback("SoEvent",self.action)
		msg(translate("draft", "Pick start point:\n"))
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
		Draft.move(self.sel,delta,copy)
		self.doc.commitTransaction()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg)
			self.linetrack.p2(point)
                        self.ui.cross(True)
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
					msg(translate("draft", "Pick end point:\n"))
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
			msg(translate("draft", "Pick end point:\n"))
		else:
			last = self.node[-1]
			if self.ui.isCopy.isChecked():
				self.move(point.sub(last),True)
			else:
				self.move(point.sub(last))
			self.finish()

			
class ApplyStyle(Modifier):
	"The Draft_ApplyStyle FreeCA command definition"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_apply',
			'MenuText': str(translate("draft", "Apply Style").toLatin1()),
			'ToolTip': str(translate("draft", "Applies current line width and color to selected objects").toLatin1())}

	def Activated(self):
		Modifier.Activated(self)
		if self.ui:
			self.sel = Draft.getSelection()
			if (len(self.sel)>0):
				self.doc.openTransaction("Change style")
				for ob in self.sel:
					if (ob.Type == "App::DocumentObjectGroup"): self.formatGroup(ob)
					else: Draft.formatObject(ob)
				self.doc.commitTransaction()

	def formatGroup(self,grpob):
		for ob in grpob.Group:
			if (ob.Type == "App::DocumentObjectGroup"): self.formatGroup(ob)
			else: Draft.formatObject(ob)

			
class Rotate(Modifier):
	"The Draft_Rotate FreeCAD command definition"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_rotate',
			'MenuText': str(translate("draft", "Rotate").toLatin1()),
			'ToolTip': str(translate("draft", "Rotates the selected objects. CTRL to snap, SHIFT to constrain, ALT creates a copy").toLatin1())}

	def Activated(self):
		Modifier.Activated(self,"Rotate")
		if self.ui:
			if not Draft.getSelection():
				self.ghost = None
				self.snap = None
				self.linetrack = None
				self.arctrack = None
				self.constraintrack = None
				self.ui.selectUi()
				msg(translate("draft", "Select an object to rotate\n"))
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = Draft.getSelection()
                self.sel = Draft.getGroupContents(self.sel)
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
		msg(translate("draft", "Pick rotation center:\n"))
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
                Draft.rotate(self.sel,math.degrees(angle),self.center,plane.axis,copy)
		self.doc.commitTransaction()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"):
			point,ctrlPoint = getPoint(self,arg)
                        self.ui.cross(True)
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
					msg(translate("draft", "Pick base angle:\n"))
				elif (self.step == 1):
					self.ui.labelRadius.setText("Rotation")
					self.arctrack.startPoint(point)
					self.rad = fcvec.dist(point,self.center)
					self.arctrack.on()
					self.ghost.on()
					self.ui.isCopy.show()
					self.step = 2
					msg(translate("draft", "Pick rotation angle:\n"))
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
		msg(translate("draft", "Pick base angle:\n"))

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
			msg(translate("draft", "Pick rotation angle:\n"))
		else:
			self.rot(math.radians(rad),self.ui.isCopy.isChecked())
			self.finish()



class Offset(Modifier):
	"The Draft_Offset FreeCAD command definition"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_offset',
			'MenuText': str(translate("draft", "Offset").toLatin1()),
			'ToolTip': str(translate("draft", "Offsets the active object. CTRL to snap, SHIFT to constrain, ALT to copy").toLatin1())}

	def Activated(self):
		Modifier.Activated(self,"Offset")
		if self.ui:
			if not Draft.getSelection():
				self.ghost = None
				self.snap = None
				self.linetrack = None
				self.arctrack = None
				self.constraintrack = None
				self.ui.selectUi()
				msg(translate("draft", "Select an object to offset\n"))
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			elif len(Draft.getSelection()) > 1:
				msg(translate("draft", "Offset only works on one object at a time\n"))
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = Draft.getSelection()[0]
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
			msg(translate("draft", "The offset tool cannot currently work on multi-face objects\n"))
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
		msg(translate("draft", "Pick distance:\n"))
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
                        self.ui.cross(True)
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
                                        p = []
                                        for v in Part.Wire(newedges).Vertexes: p.append(v.Point)
                                        self.doc.openTransaction("Offset")
                                        if arg["AltDown"] or self.ui.isCopy.isChecked():
                                                obj = Draft.makeWire(p)
                                                Draft.formatObject(obj,self.sel)
                                        else:
                                                obj = self.sel
                                                if "Points" in obj.PropertiesList:
                                                        obj.Points = p
					if self.faces:
                                                if "Closed" in obj.PropertiesList:
                                                        obj.Closed = True
                                                        obj.ViewObject.DisplayMode = "Flat Lines"
					else:
                                                if "Closed" in obj.PropertiesList:
                                                        obj.Closed = False
                                                        obj.ViewObject.DisplayMode = "Wireframe"
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
				targetOb = self.doc.addObject("Part::Feature",Draft.getRealName(self.sel.Name))
				Draft.formatObject(targetOb,self.sel)
			else:
				targetOb = self.sel
			newedges = self.redraw([scaledOffset,self.constrainSeg[1]],True)
			if self.faces:
				targetOb.Shape = Part.Face(Part.Wire(newedges))
			else:
				targetOb.Shape = Part.Wire(newedges)
			self.doc.commitTransaction()
		else: msg(translate("draft", "Couldn't determine where to apply distance!\n"))
		self.finish()
			

class Upgrade(Modifier):
	'''The Draft_Upgrade FreeCAD command definition.
	This class upgrades selected objects in different ways,
        following this list (in order):
	- if there are more than one faces, the faces are merged (union)
	- if there is only one face, nothing is done
	- if there are closed wires, they are transformed in a face
	- otherwise join all edges into a wire (closed if applicable)
	- if nothing of the above is possible, a Compound is created
	'''

	def GetResources(self):
		return {'Pixmap'  : 'Draft_upgrade',
			'MenuText': str(translate("draft", "Upgrade").toLatin1()),
			'ToolTip': str(translate("draft", "Joins the selected objects into one, or converts closed wires to filled faces, or unite faces").toLatin1())}

	def Activated(self):
		Modifier.Activated(self,"Upgrade")
		if self.ui:
			if not Draft.getSelection():
				self.ui.selectUi()
				msg(translate("draft", "Select an object to upgrade\n"))
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
		self.sel = Draft.getSelection()
		loneEdges = False
		loneFaces = False
                nodelete = False
		edges = []
		wires = []
		openwires = []
		faces = []
                groups = []
                curves = []
		# determining which level we will have
		for ob in self.sel:
			if ob.Type == "App::DocumentObjectGroup":
                                groups.append(ob)
                        else:
                                if ob.Shape.ShapeType == 'Edge': loneEdges = True
                                if ob.Shape.ShapeType == 'Face': loneFaces = True
                                for f in ob.Shape.Faces:
                                        faces.append(f)
                                for w in ob.Shape.Wires:
                                        if w.isClosed():
                                                wires.append(w)
                                        else:
                                                openwires.append(w)
                                for e in ob.Shape.Edges:
                                        if not isinstance(e.Curve,Part.Line):
                                                curves.append(e)
                                lastob = ob
		# applying transformation
		self.doc.openTransaction("Upgrade")
                if groups:
                        # if we have a group: turn each closed wire inside into a face
                        for grp in groups: 
                                for ob in grp.Group:
                                        if not ob.Shape.Faces:
                                                for w in ob.Shape.Wires:
                                                        newob = Draft.makeWire(w,closed=w.isClosed())
                                                        self.sel.append(ob)
                                                        grp.addObject(newob)
                else:
                        if faces:
                                if loneEdges:
                                        # if we have a mix of all kind of things, we just make a compound
                                        newob = self.compound()
                                        Draft.formatObject(newob,lastob)
                                else:
                                        if len(self.sel) == 2:
                                                # 2 objects: we fuse them
                                                newob = Draft.fuse(self.sel[0],self.sel[1])
                                                nodelete = True
                                        else:
                                                # we try the draft way: make one face out of them
                                                u = faces.pop(0)
                                                for f in faces:
                                                        u = u.fuse(f)
                                                if fcgeo.isCoplanar(faces):
                                                        if self.sel[0].ViewObject.DisplayMode == "Wireframe":
                                                                f = False
                                                        else:
                                                                f = True
                                                        u = fcgeo.concatenate(u)
                                                        if not curves:
                                                                newob = Draft.makeWire(u.Wires[0],closed=True,face=f)
                                                                Draft.formatObject(newob,lastob)
                                                        else:
                                                                # if not possible, we do a non-parametric union
                                                                newob = self.doc.addObject("Part::Feature","Union")
                                                                newob.Shape = u
                                                                Draft.formatObject(newob,lastob)
                                                else:
                                                        # if not possible, we do a non-parametric union
                                                        newob = self.doc.addObject("Part::Feature","Union")
                                                        newob.Shape = u
                                                        Draft.formatObject(newob,lastob)
                        elif wires:
                                if loneEdges or loneFaces or openwires:
                                        # if we have mixed stuff, we do a compound
                                        newob = self.compound()
                                        Draft.formatObject(newob,lastob)
                                else:
                                        # only closed wires? we make faces
                                        for w in wires:
                                                f = Part.Face(w)
                                                faces.append(f)
                                        for f in faces:
                                                if not curves: 
                                                        newob = Draft.makeWire(f.Wire,closed=True)
                                                else:
                                                        # if not possible, we do a non-parametric face
                                                        newob = self.doc.addObject("Part::Feature","Face")
                                                        newob.Shape = f
                                                        Draft.formatObject(newob,lastob)
                        elif (len(openwires) == 1):
                                if loneEdges or loneFaces:
                                        newob = self.compound()
                                        Draft.formatObject(newob,lastob)
                                else:
                                        # special case, we have one open wire. We close it!"
                                        p0 = openwires[0].Vertexes[0].Point
                                        p1 = openwires[0].Vertexes[-1].Point
                                        edges = openwires[0].Edges
                                        edges.append(Part.Line(p1,p0).toShape())
                                        w = Part.Wire(fcgeo.sortEdges(edges))
                                        if not curves:
                                                newob = Draft.makeWire(w,closed=True)
                                        else:
                                                # if not possible, we do a non-parametric union
                                                newob = self.doc.addObject("Part::Feature","Wire")
                                                newob.Shape = w
                                                Draft.formatObject(newob,lastob)
                        else:
                                for ob in self.sel:
                                        for e in ob.Shape.Edges:
                                                edges.append(e)
                                newob = None
                                try:
                                        w = Part.Wire(fcgeo.sortEdges(edges))
                                        if len(w.Edges) == len(ob.Shape.Edges):
                                                newob = Draft.makeWire(w)
                                except:
                                        pass
                                if not newob: newob = self.compound()
                                Draft.formatObject(newob,lastob)
                if not nodelete:
                        for ob in self.sel:
                                if not ob.Type == "App::DocumentObjectGroup":
                                        self.doc.removeObject(ob.Name)
		self.doc.commitTransaction()
		Draft.select(newob)
		Modifier.finish(self)
				
class Downgrade(Modifier):
	'''
        The Draft_Downgrade FreeCAD command definition.
	This class downgrades selected objects in different ways,
        following this list (in order):
	- if there are more than one faces, the subsequent
          faces are subtracted from the first one
	- if there is only one face, it gets converted to a wire
	- otherwise wires are exploded into single edges
	'''

	def GetResources(self):
		return {'Pixmap'  : 'Draft_downgrade',
			'MenuText': str(translate("draft", "Downgrade").toLatin1()),
			'ToolTip': str(translate("draft", "Explodes the selected objects into simpler objects, or subtract faces").toLatin1())}

	def Activated(self):
		Modifier.Activated(self,"Downgrade")
		if self.ui:
			if not Draft.getSelection():
				self.ui.selectUi()
				msg(translate("draft", "Select an object to upgrade\n"))
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			else:
				self.proceed()


	def proceed(self):
		self.sel = Draft.getSelection()
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
                if len(self.sel) == 2:
                        # we have only 2 objects: cut 2nd from 1st
                        newob = Draft.cut(self.sel[0],self.sel[1])
		elif (len(faces) > 1):
			if len(self.sel) == 1:
                                # one object with several faces: split it
				for f in faces:
					newob = self.doc.addObject("Part::Feature","Face")
					newob.Shape = f
					Draft.formatObject(newob,self.sel[0])
				self.doc.removeObject(ob.Name)
			else:
                                # several objects: remove all the faces from the first one
				u = faces.pop(0)
				for f in faces:
					u = u.cut(f)
				newob = self.doc.addObject("Part::Feature","Subtraction")
				newob.Shape = u
				for ob in self.sel:
					Draft.formatObject(newob,ob)
					self.doc.removeObject(ob.Name)
		elif (len(faces) > 0):
                        # no faces: split wire into single edges
			w=faces[0].Wires[0]
			for ob in self.sel:
				newob = self.doc.addObject("Part::Feature","Wire")
				newob.Shape = w
				Draft.formatObject(newob,ob)
				self.doc.removeObject(ob.Name)
		else:
			for ob in self.sel:
				for e in edges:
					newob = self.doc.addObject("Part::Feature","Edge")
					newob.Shape = e
					Draft.formatObject(newob,ob)
				self.doc.removeObject(ob.Name)
		self.doc.commitTransaction()
		Draft.select(newob)
		Modifier.finish(self)


class Trimex(Modifier):
	''' The Draft_Trimex FreeCAD command definition.
        This tool trims or extends lines, wires and arcs,
	or extrudes single faces. SHIFT constrains to the last point
	or extrudes in direction to the face normal.'''

	def GetResources(self):
		return {'Pixmap' : 'Draft_trimex',
			'MenuText' : str(translate("draft", "Trimex").toLatin1()),
			'ToolTip' : str(translate("draft", "Trims or Extends the selected object, or extrudes single faces. CTRL snaps, SHIFT constrains to current segment or to normal, ALT inverts").toLatin1())}

	def Activated(self):
		Modifier.Activated(self,"Trimex")
		if self.ui:
			if not Draft.getSelection():
				self.ghost = None
				self.snap = None
				self.linetrack = None
				self.constraintrack = None
				self.ui.selectUi()
				msg(translate("draft", "Select an object to trim/extend\n"))
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = Draft.getSelection()[0]
		if self.sel.Shape.isClosed(): self.finish()
		self.ui.radiusUi()
		self.ui.labelRadius.setText("Distance")
		self.ui.radiusValue.setFocus()
		self.ui.radiusValue.selectAll()
		self.snap = snapTracker()
		self.linetrack = lineTracker()
		self.constraintrack = lineTracker(dotted=True)
		self.edges = []
		if len(self.sel.Shape.Faces) == 1:
			self.extrudeMode = True
			self.ghost = [ghostTracker([self.sel])]
			self.normal = self.sel.Shape.Faces[0].normalAt(.5,.5)
			for v in self.sel.Shape.Vertexes:
				self.ghost.append(lineTracker())
			for g in self.ghost: g.on()
		else:
			self.sel.ViewObject.Visibility = False
			self.extrudeMode = False
			c = fcgeo.complexity(self.sel)
			if (c >= 7):
				msg(translate("draft", "The selected object cannot be extended\n"))
				self.finish()
			elif (c >= 4): 
				self.edges = self.sel.Shape.Wires[0].Edges
				self.edges = fcgeo.sortEdges(self.edges)
			else: self.edges = self.sel.Shape.Edges	
			self.ghost = []
			lc = self.sel.ViewObject.LineColor
			sc = (lc[0],lc[1],lc[2])
			sw = self.sel.ViewObject.LineWidth
			for e in self.edges:
				if isinstance(e.Curve,Part.Line):
					self.ghost.append(lineTracker(scolor=sc,swidth=sw))
				else:
					self.ghost.append(arcTracker(scolor=sc,swidth=sw))
			for g in self.ghost: g.on()

		self.activePoint = 0
		self.nodes = []
		self.shift = False
		self.alt = False
		self.force = None
		self.call = self.view.addEventCallback("SoEvent",self.action)
		msg(translate("draft", "Pick distance:\n"))
		self.ui.cross(True)

				
	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
                        self.ui.cross(True)
			cursor = arg["Position"]
			point = self.view.getPoint(cursor[0],cursor[1])
			self.shift = arg["ShiftDown"]
			self.alt = arg["AltDown"]
			if arg["CtrlDown"]: self.snapped = None
			else: self.snapped = self.view.getObjectInfo((cursor[0],cursor[1]))
			self.point = snapPoint(self,point,cursor,arg["CtrlDown"])
			if not self.ui.zValue.isEnabled(): self.point.z = float(self.ui.zValue.text())
			if self.extrudeMode:
				dist = self.extrude(self.point,self.shift)
			else:
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

	def extrude(self,point,shift=False,real=False):
		"redraws the ghost in extrude mode"
		self.newpoint = self.sel.Shape.Faces[0].CenterOfMass
		dvec = point.sub(self.newpoint)
		if shift: delta = fcvec.project(dvec,self.normal)
		else: delta = dvec
		if real: return delta
		self.ghost[0].trans.translation.setValue([delta.x,delta.y,delta.z])
		for i in range(1,len(self.ghost)):
			base = self.sel.Shape.Vertexes[i-1].Point
			self.ghost[i].p1(base)
			self.ghost[i].p2(base.add(delta))
		return delta.Length

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
		if self.extrudeMode:
			delta = self.extrude(self.point,self.shift,real=True)
                        self.doc.openTransaction("Extrude")
                        obj = Draft.extrude(self.sel,delta)
                        self.doc.commitTransaction()
                        self.sel = obj
		else:
			edges = self.redraw(self.point,self.snapped,self.shift,self.alt,real=True)
			newshape = Part.Wire(edges)
                        self.doc.openTransaction("Trim/extend")
                        if 'Points' in self.sel.PropertiesList:
                                p = []
                                for v in newshape.Vertexes: p.append(v.Point)
                                self.sel.Points = p
                        else:
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
			Draft.select(self.sel)

	def numericRadius(self,dist):
		"this function gets called by the toolbar when valid distance have been entered there"
		self.force = dist
		self.trimObject()
		self.finish()

class Scale(Modifier):
	'''The Draft_Scale FreeCAD command definition.
        This tool scales the selected objects from a base point.'''

	def GetResources(self):
		return {'Pixmap'  : 'Draft_scale',
			'MenuText': str(translate("draft", "Scale").toLatin1()),
			'ToolTip': str(translate("draft", "Scales the selected objects from a base point. CTRL to snap, SHIFT to constrain, ALT to copy").toLatin1())}

	def Activated(self):
		Modifier.Activated(self,"Scale")
		if self.ui:
			if not Draft.getSelection():
				self.ghost = None
				self.snap = None
				self.linetrack = None
				self.constraintrack = None
				self.ui.selectUi()
				msg(translate("draft", "Select an object to scale\n"))
				self.call = self.view.addEventCallback("SoEvent",selectObject)
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = Draft.getSelection()
                self.sel = Draft.getGroupContents(self.sel)
		self.ui.pointUi()
		self.ui.xValue.setFocus()
		self.ui.xValue.selectAll()
		self.snap = snapTracker()
		self.linetrack = lineTracker()
		self.constraintrack = lineTracker(dotted=True)
		self.ghost = ghostTracker(self.sel)
		self.call = self.view.addEventCallback("SoEvent",self.action)
		msg(translate("draft", "Pick base point:\n"))
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
                Draft.scale(self.sel,delta,self.node[0],copy)
		self.doc.commitTransaction()

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg,sym=True)
			self.linetrack.p2(point)
                        self.ui.cross(True)
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
					msg(translate("draft", "Pick scale factor:\n"))
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
			msg(translate("draft", "Pick scale factor:\n"))
		else:
			last = self.node[-1]
			if self.ui.isCopy.isChecked():
				self.scale(point.sub(last),True)
			else:
				self.scale(point.sub(last))
			self.finish()

class ToggleConstructionMode():
	"The Draft_ToggleConstructionMode FreeCAD command definition"

	def GetResources(self):
		return {'MenuText': str(translate("draft", "Toggle construcion Mode").toLatin1()),
			'ToolTip': str(translate("draft", "Toggles the Construction Mode for next objects.").toLatin1())}

	def Activated(self):
		FreeCADGui.activeWorkbench().draftToolBar.ui.constrButton.toggle()


class SendToDrawing(Modifier):
        "The Draft_SendToDrawing FreeCAD command definition"

        def __init__(self):
                self.interactive = True
        
	def GetResources(self):
		return {'Pixmap'  : 'Draft_sendToDrawing',
			'MenuText': str(translate("draft", "Drawing").toLatin1()),
			'ToolTip': str(translate("draft", "Sends the selected objects to the active Drawing sheet.").toLatin1())}

	def Activated(self):
		Modifier.Activated(self,"Send to Drawing")
                import Drawing
		if self.ui:
                        oldindex = self.ui.pageBox.currentIndex()
                        if oldindex == 0:
                                oldindex = None
                        else:
                                oldindex = str(self.ui.pageBox.itemText(oldindex))
                        self.ui.pageBox.clear()
                        self.ui.pageBox.addItem("Add New")
                        existingpages = self.doc.findObjects('Drawing::FeaturePage')
                        for p in existingpages:
                                self.ui.pageBox.addItem(p.Name)
                        if oldindex:
                                for i in range(len(existingpages)):
                                        if existingpages[i].Name == oldindex:
                                                self.ui.pageBox.setCurrentIndex(i+1)
                        if self.interactive:
                                self.ui.pageUi()
                        else:
                                self.draw()
                else: self.finish()
                                
        def draw(self):
                modifier = self.ui.LWModValue.value()
                template = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft").GetString("template")
                if not template: template = FreeCAD.getResourceDir()+'Mod/Drawing/Templates/A3_Landscape.svg'
                offset = self.ui.marginValue.value()
                scale = int(self.ui.scaleBox.itemText(self.ui.scaleBox.currentIndex()))
                if self.ui.pageBox.currentIndex() == 0:
                        pagename = str(self.ui.pageBox.itemText(0))
                        if pagename == 'Add New': pagename = 'Page'
                        page = self.doc.addObject('Drawing::FeaturePage',pagename)
                        page.Template = template
                        self.doc.recompute()
                else:
                        pagename = str(self.ui.pageBox.itemText(self.ui.pageBox.currentIndex()))
                        page = self.doc.findObjects('Drawing::FeaturePage',pagename)[0]
                page.ViewObject.HintScale = str(scale)
                page.ViewObject.HintOffset = str(offset)
                pb = open(str(page.PageResult))
                fb = pb.read()
                pageheight = re.findall("height=\"(.*?)\"",fb)
                if pageheight: pageheight = float(pageheight[0])
                pb.close()
                del pb
                sel = Draft.getSelection()
                for obj in sel:
                        name = 'View'+obj.Name
                        svg = None
                        if 'Dimline' in obj.PropertiesList:
                                svg = obj.ViewObject.Proxy.getSVG(modifier)
                        elif obj.isDerivedFrom('Part::Feature'):
                                fill = self.getFill(obj.ViewObject,page)
                                svg = self.writeShape(obj,modifier,fill)
                        elif obj.Type == 'App::Annotation':
                                svg = self.writeText(obj,modifier)
                        if svg:
                                oldobj = page.getObject(name)
                                if oldobj: self.doc.removeObject(oldobj.Name)
                                view = self.doc.addObject('Drawing::FeatureView',name)
                                view.ViewResult = self.transformSVG(name,svg,offset,scale,pageheight)
                                view.X = offset
                                view.Y = offset
                                view.Scale = scale
                                page.addObject(view)
                self.doc.recompute()
                self.finish()

        def writeText(self,obj,modifier=100):
                "returns an svg representation of a document annotation"
                textcontents = ''
                for l in obj.LabelText:
                        textcontents+=l
                        textcontents+='\n'
                svg = '<text id="' + obj.Name + '" fill="'
                svg += self.getrgb(obj.ViewObject.TextColor)
                svg += '" font-size="'
                svg += str(obj.ViewObject.FontSize*(modifier/5))+'" '
                svg += 'style="text-anchor:middle;text-align:center" '
                svg += 'transform="'
                if obj.ViewObject.RotationAxis == 'Z':
                        if obj.ViewObject.Rotation != 0:
                                svg += 'rotate('+str(obj.ViewObject.Rotation)
                                svg += ','+ str(obj.Position.x) + ',' + str(obj.Position.y) + ') '
                svg += 'translate(' + str(obj.Position.x) + ',' + str(obj.Position.y) + ') '
                svg +='scale('+str(modifier/2000)+','+str(modifier/2000)+')">\n'
                svg += textcontents
                svg += '</text>\n'
                return svg

        def writeShape(self,obj,modifier=100,fill='none'):
                "returns a svg representation of a planar shape"
                name = obj.Name
                stroke = self.getrgb(obj.ViewObject.LineColor)
                width = obj.ViewObject.LineWidth/modifier
                if len(obj.Shape.Vertexes) > 1:
                        svg ='<path id="' + name + '" '
                        edges = fcgeo.sortEdges(obj.Shape.Edges)
                        v = edges[0].Vertexes[0].Point
                        svg += 'd="M '+ str(v.x) +' '+ str(v.y) + ' '
                        for e in edges:
                                if isinstance(e.Curve,Part.Line):
                                        v = e.Vertexes[-1].Point
                                        svg += 'L '+ str(v.x) +' '+ str(v.y) + ' '
                                elif isinstance(e.Curve,Part.Circle):
                                        r = e.Curve.Radius
                                        v = e.Vertexes[-1].Point
                                        svg += 'A '+ str(r) + ' '+ str(r) +' 0 0 0 '+ str(v.x) +' '
                                        svg += str(v.y) + ' '
                        if fill != "none": svg += 'Z'
                        svg += '" '
                else:
                        cen = obj.Shape.Edges[0].Curve.Center
                        rad = obj.Shape.Edges[0].Curve.Radius
                        svg = '<circle cx="'+str(cen.x)
                        svg += '" cy="'+str(cen.y)
                        svg += '" r="'+str(rad)+'" '
                svg += 'stroke="' + stroke + '" '
                svg += 'stroke-width="' + str(width) + ' px" '
                svg += 'style="stroke-width:'+ str(width)
                svg += ';stroke-miterlimit:4'
                svg += ';stroke-dasharray:'+self.getLineStyle(obj.ViewObject)
                svg += ';fill:'+fill+'"'
                svg += '/>\n'
                return svg

        def getLineStyle(self,viewobj):
                "returns a svg dash array"
                if "LineStyle" in viewobj.PropertiesList:
                        if viewobj.LineStyle == "dashed":
                                return "0.09,0.05"
                        elif viewobj.LineStyle == "dashdotted":
                                return "0.09,0.05,0.02,0.05"
                        elif viewobj.LineStyle == "dotted":
                                return "0.02,0.02"
                return "none"

        def getFill(self,viewobj,page):
                "returns a svg fill value"
                color = self.getrgb(viewobj.LineColor)
                if viewobj.Object.Shape.Faces and (viewobj.DisplayMode != "Wireframe"):
                        if 'FillStyle' in viewobj.PropertiesList:
                                if viewobj.FillStyle == 'shape color':
                                        fill = self.getrgb(viewobj.ShapeColor)
                                else:
                                        hatch = viewobj.FillStyle[:-6]
                                        fill = 'url(#'+hatch+')'
                                        self.addPattern(hatch,page,color)
                        else:
                                fill = self.getrgb(viewobj.ShapeColor)   
                else: fill = 'none'
                return fill

        def addPattern(self,pattern,page,svgcolor):
                "adds a given pattern to the page"
                vobj = page.getObject('Pattern'+pattern)
                if not vobj:
                        view = self.doc.addObject('Drawing::FeatureView','Pattern'+pattern)
                        svg = FreeCAD.svgpatterns[pattern]
                        svg = svg.replace('DefaultColor',svgcolor)
                        view.ViewResult = svg
                        view.X = 0
                        view.Y = 0
                        view.Scale = 1
                        page.addObject(view)

        def getrgb(self,color):
		"returns a rgb value #000000 from a freecad color"
		r = str(hex(int(color[0]*255)))[2:].zfill(2)
		g = str(hex(int(color[1]*255)))[2:].zfill(2)
		b = str(hex(int(color[2]*255)))[2:].zfill(2)
		return "#"+r+g+b

        def formatSVG(self,svg,obj):
                "applies freecad object's color and linewidth to a svg fragment"
                svg = svg.replace('stroke-width="0.35"','stroke-width="'+str(obj.ViewObject.LineWidth/100)+'px"')
                lc = obj.ViewObject.LineColor
                sc = (int(math.ceil(lc[0]*255)),int(math.ceil(lc[1]*255)),int(math.ceil(lc[2]*255)))
                svg = svg.replace('stroke="rgb(0, 0, 0)"','stroke="rgb('+str(sc[0])+','+str(sc[1])+','+str(sc[2])+')"')
                return svg
                                
        def transformSVG(self,name, svg,offset=0,scale=1,pageheight=None):
                "encapsulates a svg fragment into a transformation node"
                result = '<g id="' + name + '"'
                result += ' transform="'
                if pageheight:
                        result += ' translate('+str(offset)+','+str(pageheight-offset)+')'
                        result += ' scale('+str(scale)+','+str(-scale)+')">'
                else:
                        result += ' translate('+str(offset)+','+str(offset)+')'
                        result += ' scale('+str(scale)+','+str(scale)+')">'
                result += svg
                result += '</g>'
                return result

class SendToDrawingDirect(Modifier):
        "The Draft_SendToDrawingDirect FreeCAD command definition"

        def __init__(self):
                self.interactive = False
        
	def GetResources(self):
		return {'Pixmap'  : 'Draft_sendToDrawing',
			'MenuText': str(translate("draft", "DirectDrawing").toLatin1()),
			'ToolTip': str(translate("draft", "Sends the selected objects to the active Drawing sheet without asking anything, using last settings").toLatin1())}
        

class MakeDraftWire():
	"The MakeDraft FreeCAD command definition"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_makeDraftWire',
                        'MenuText': str(translate("draft", "Make Draft Wire").toLatin1()),
			'ToolTip': str(translate("draft", "Turns selected objects to Draft Wires").toLatin1())}

	def Activated(self):
                Draft.draftify(Draft.getSelection())



#---------------------------------------------------------------------------
# Adds the icons & commands to the FreeCAD command manager, and sets defaults
#---------------------------------------------------------------------------
		
# drawing commands
FreeCADGui.addCommand('Draft_SelectPlane',SelectPlane())
FreeCADGui.addCommand('Draft_Line',Line())
FreeCADGui.addCommand('Draft_Wire',Wire())
FreeCADGui.addCommand('Draft_Circle',Circle())
FreeCADGui.addCommand('Draft_Arc',Arc())
FreeCADGui.addCommand('Draft_Text',Text())
FreeCADGui.addCommand('Draft_Rectangle',Rectangle())
FreeCADGui.addCommand('Draft_Dimension',Dimension())

# context commands
FreeCADGui.addCommand('Draft_FinishLine',FinishLine())
FreeCADGui.addCommand('Draft_CloseLine',CloseLine())
FreeCADGui.addCommand('Draft_UndoLine',UndoLine())
FreeCADGui.addCommand('Draft_ToggleConstructionMode',ToggleConstructionMode())
FreeCADGui.addCommand('Draft_MakeDraftWire',MakeDraftWire())

# modification commands
FreeCADGui.addCommand('Draft_Move',Move())
FreeCADGui.addCommand('Draft_ApplyStyle',ApplyStyle())
FreeCADGui.addCommand('Draft_Rotate',Rotate())
FreeCADGui.addCommand('Draft_Offset',Offset())
FreeCADGui.addCommand('Draft_Upgrade',Upgrade())
FreeCADGui.addCommand('Draft_Downgrade',Downgrade())
FreeCADGui.addCommand('Draft_Trimex',Trimex())
FreeCADGui.addCommand('Draft_Scale',Scale())
FreeCADGui.addCommand('Draft_SendToDrawing',SendToDrawing())
FreeCADGui.addCommand('Draft_SendToDrawingDirect',SendToDrawingDirect())
