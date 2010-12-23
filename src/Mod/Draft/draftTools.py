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
__author__ = "Yorik van Havre, Werner Mayer, Martin Burbaum, Ken Cline, Dmitry Chigrin"
__url__ = "http://free-cad.sourceforge.net"

#---------------------------------------------------------------------------
# Generic stuff
#---------------------------------------------------------------------------

import FreeCAD, FreeCADGui, Part, WorkingPlane, math, re, importSVG, Draft
from draftlibs import fcvec,fcgeo
from FreeCAD import Vector
from draftGui import todo,QtCore,QtGui
from pivy import coin

# loads a translation engine
locale = Draft.getTranslation(QtCore.QLocale(eval("QtCore.QLocale."+FreeCADGui.getLocale())).name())
if locale:
        translator = QtCore.QTranslator()
        translator.load(locale,Draft.getDraftPath("Languages"))
        QtGui.QApplication.installTranslator(translator)

def translate(context,text):
        "convenience function for Qt translator"
        return QtGui.QApplication.translate(context, text, None,
                                            QtGui.QApplication.UnicodeUTF8)
		

def msg(text=None,mode=None):
        "prints the given message on the FreeCAD status bar"
        if not text: FreeCAD.Console.PrintMessage("")
        else:
                if mode == 'warning':
                        FreeCAD.Console.PrintWarning(str(text.toLatin1()))
                elif mode == 'error':
                        FreeCAD.Console.PrintError(str(text.toLatin1()))
                else:
                        FreeCAD.Console.PrintMessage(str(text.toLatin1()))

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
                                        lastob = FreeCAD.ActiveDocument.getObject(lastObj[0])
                                        if lastob:
                                                if (lastob.Type[:4] == "Part"):
                                                        for k in lastob.Shape.Edges:
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
			lastObj[0] = ob.Name
			lastObj[1] = ob.Name
		if (lastObj[1] != ob.Name):
			lastObj[0] = lastObj[1]
			lastObj[1] = ob.Name

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
                                target.ui.xValue.setFocus()
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
                                target.ui.yValue.setFocus()
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
                                target.ui.zValue.setFocus()
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
	def __init__(self,dotted=False,scolor=None,swidth=None,children=[],ontop=False):
                self.ontop = ontop
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
                if self.ontop:
                        sg.insertChild(switch,0)
                else:
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
			self.marker.markerIndex = coin.SoMarkerSet.DIAMOND_FILLED_9_9
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

	def update(self,pts,p3=None):
                if (len(pts) == 2) and p3:
                        pts.append(p3)
		if len(pts) == 2:
			points = [fcvec.tup(pts[0],True),fcvec.tup(pts[0],True),\
				fcvec.tup(pts[1],True),fcvec.tup(pts[1],True)]
			self.coords.point.setValues(0,4,points)
		elif len(pts) == 3:
			p1 = pts[0]
			p4 = pts[1]
                        if fcvec.equals(p1,p4):
                                proj = None
                        else:
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
        "An arc tracker"
        def __init__(self,dotted=False,scolor=None,swidth=None,start=0,end=math.pi*2):
                self.circle = None
                self.startangle = math.degrees(start)
                self.endangle = math.degrees(end)
		self.trans = coin.SoTransform()
		self.trans.translation.setValue([0,0,0])
                self.sep = coin.SoSeparator()
                self.recompute()
		Tracker.__init__(self,dotted,scolor,swidth,[self.trans, self.sep])

        def setCenter(self,cen):
                "sets the center point"
                self.trans.translation.setValue([cen.x,cen.y,cen.z])

        def setRadius(self,rad):
                "sets the radius"
                self.trans.scaleFactor.setValue([rad,rad,rad])

        def setStartAngle(self,ang):
                "sets the start angle"
                self.startangle = math.degrees(ang)
                self.recompute()

        def setEndAngle(self,ang):
                "sets the end angle"
                self.endangle = math.degrees(ang)
                self.recompute()

        def getAngle(self,pt):
                "returns the angle of a given vector"
                c = self.trans.translation.getValue()
                center = Vector(c[0],c[1],c[2])
                base = plane.u
                rad = pt.sub(center)
                return(fcvec.angle(rad,base,plane.axis))

        def getAngles(self):
                "returns the start and end angles"
                return(self.startangle,self.endangle)
                
        def setStartPoint(self,pt):
                "sets the start angle from a point"
                self.setStartAngle(self.getAngle(pt))

        def setEndPoint(self,pt):
                "sets the end angle from a point"
                self.setEndAngle(self.getAngle(pt))
                
        def setApertureAngle(self,ang):
                "sets the end angle by giving the aperture angle"
                ap = math.degrees(ang)
                self.endangle = self.startangle + ap
                self.recompute()

        def recompute(self):
                if self.circle: self.sep.removeChild(self.circle)
                self.circle = None
                if self.endangle < self.startangle:
                        c = Part.makeCircle(1,Vector(0,0,0),plane.axis,self.endangle,self.startangle)
                else:
                        c = Part.makeCircle(1,Vector(0,0,0),plane.axis,self.startangle,self.endangle)
                ivin = coin.SoInput()
                ivin.setBuffer(c.writeInventor())
                ivob = coin.SoDB.readAll(ivin)
                if ivob:
                        self.circle = ivob.getChild(1).getChild(0)
                        self.circle.removeChild(self.circle.getChild(0))
                        self.circle.removeChild(self.circle.getChild(0))
                        self.sep.addChild(self.circle)

class ghostTracker(Tracker):
	'''A Ghost tracker, that allows to copy whole object representations.
        You can pass it an object or a list of objects, or a shape.'''
	def __init__(self,sel):
		self.trans = coin.SoTransform()
		self.trans.translation.setValue([0,0,0])
		self.children = [self.trans]
		self.ivsep = coin.SoSeparator()
                try:
                        if isinstance(sel,Part.Shape):
                                ivin = coin.SoInput()
                                ivin.setBuffer(sel.writeInventor())
                                ivob = coin.SoDB.readAll(ivin)
                                self.ivsep.addChild(ivob.getChildren()[1])
                        else:
                                if not isinstance(sel,list):
                                        sel = [sel]
                                for obj in sel:
                                        self.ivsep.addChild(obj.ViewObject.RootNode.copy())
                except:
                        print "draft: Couldn't create ghost"
                self.children.append(self.ivsep)
		Tracker.__init__(self,children=self.children)

        def update(self,obj):
                obj.ViewObject.show()
                self.finalize()
                self.ivsep = coin.SoSeparator()
                self.ivsep.addChild(obj.ViewObject.RootNode.copy())
                Tracker.__init__(self,children=[self.ivsep])
                self.on()
                obj.ViewObject.hide()

class editTracker(Tracker):
	"A node edit tracker"
	def __init__(self,pos=Vector(0,0,0),name="None",idx=0):
		color = coin.SoBaseColor()
		color.rgb = FreeCADGui.activeWorkbench().draftToolBar.\
			ui.getDefaultColor("snap")
		self.marker = coin.SoMarkerSet() # this is the marker symbol
		self.marker.markerIndex = coin.SoMarkerSet.SQUARE_FILLED_9_9
		self.coords = coin.SoCoordinate3() # this is the coordinate
		self.coords.point.setValue((pos.x,pos.y,pos.z))
                selnode = coin.SoType.fromName("SoFCSelection").createInstance()
		selnode.documentName.setValue(FreeCAD.ActiveDocument.Name)
		selnode.objectName.setValue(name)
		selnode.subElementName.setValue("EditNode"+str(idx))
		node = coin.SoAnnotation()
		selnode.addChild(self.coords)
		selnode.addChild(color)
		selnode.addChild(self.marker)
                node.addChild(selnode)
		Tracker.__init__(self,children=[node],ontop=True)
                self.on()

        def set(self,pos):
                self.coords.point.setValue((pos.x,pos.y,pos.z))

        def get(self):
                p = self.coords.point.getValues()[0]
                return Vector(p[0],p[1],p[2])

        def move(self,delta):
                self.set(self.get().add(delta))

class PlaneTracker(Tracker):
        "A working plane tracker"
        def __init__(self):
                self.trans = coin.SoTransform()
		self.trans.translation.setValue([0,0,0])
                m1 = coin.SoMaterial()
                m1.transparency.setValue(0.8)
                m1.diffuseColor.setValue([0.4,0.4,0.6])
                c1 = coin.SoCoordinate3()
                c1.point.setValues([[-1,-1,0],[1,-1,0],[1,1,0],[-1,1,0]])
                f = coin.SoIndexedFaceSet()
                f.coordIndex.setValues([0,1,2,3])
                m2 = coin.SoMaterial()
                m2.transparency.setValue(0.7)
                m2.diffuseColor.setValue([0.2,0.2,0.3])
                c2 = coin.SoCoordinate3()
                c2.point.setValues([[0,1,0],[0,0,0],[1,0,0],[-.05,.95,0],[0,1,0],[.05,.95,0],[.95,.05,0],[1,0,0],[.95,-.05,0]])
                l = coin.SoLineSet()
                l.numVertices.setValues([3,3,3])
                s = coin.SoSeparator()
                s.addChild(self.trans)
                s.addChild(m1)
                s.addChild(c1)
                s.addChild(f)
                s.addChild(m2)
                s.addChild(c2)
                s.addChild(l)
                Tracker.__init__(self,children=[s])

        def set(self,pos=None):
                if pos:                        
                        Q = plane.getRotation().Rotation.Q
                else:
                        plm = plane.getPlacement()
                        Q = plm.Rotation.Q
                        pos = plm.Base
                self.trans.translation.setValue([pos.x,pos.y,pos.z])
                self.trans.rotation.setValue([Q[0],Q[1],Q[2],Q[3]])
                self.on()
                        
class wireTracker(Tracker):                
        "A wire tracker"
	def __init__(self,wire):
                line = coin.SoLineSet()
                self.closed = fcgeo.isReallyClosed(wire)
                if self.closed:
                        line.numVertices.setValue(len(wire.Vertexes)+1)
                else:
                        line.numVertices.setValue(len(wire.Vertexes))
                self.coords = coin.SoCoordinate3()
                self.update(wire)
                Tracker.__init__(self,children=[self.coords,line])

        def update(self,wire):
                for i in range(len(wire.Vertexes)):
                        p=wire.Vertexes[i].Point
                        self.coords.point.set1Value(i,[p.x,p.y,p.z])
                if self.closed:
                        t = len(wire.Vertexes)
                        p = wire.Vertexes[0].Point
                        self.coords.point.set1Value(t,[p.x,p.y,p.z])
     
#---------------------------------------------------------------------------
# Helper tools
#---------------------------------------------------------------------------
                	
class SelectPlane:
	"The Draft_SelectPlane FreeCAD command definition"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_selectPlane',
			'MenuText': str(translate("draft", "SelectPlane").toLatin1()),
			'ToolTip' : str(translate("draft", "Select a working plane for geometry creation").toLatin1())}

        def IsActive(self):
                if FreeCADGui.ActiveDocument:
                        return True
                else:
                        return False
        
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
                                FreeCADGui.Selection.clearSelection()
                                self.display(plane.axis)
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
                                                        self.display(plane.axis)
							self.finish()
					except:
						pass

	def selectHandler(self, arg):
 		if arg == "XY":
			plane.alignToPointAndAxis(Vector(0,0,0), Vector(0,0,1), self.offset)
                        self.display('top')
			self.finish()
		elif arg == "XZ":
			plane.alignToPointAndAxis(Vector(0,0,0), Vector(0,-1,0), self.offset)
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
                        self.ui.cross(True)
                        self.ui.sourceCmd = self
                        self.ui.cmdlabel.setText(name)
			FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)
			plane.setup(fcvec.neg(self.view.getViewDirection()), Vector(0,0,0))
			self.node = []
			self.pos = []
			self.constrain = None
			self.obj = None
                        self.planetrack = PlaneTracker()

        def IsActive(self):
                if FreeCADGui.ActiveDocument:
                        return True
                else:
                        return False
		
	def finish(self):
		self.node=[]
                self.planetrack.finalize()
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
                        self.obj = None
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
                        Draft.makeWire(self.node,closed,face=self.ui.hasFill.isChecked())
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
			if self.obj.Shape.Edges:
				edges = self.obj.Shape.Edges
                                if len(edges) > 1:
                                        edges.pop()
                                        newshape = Part.Wire(edges)
                                else:
                                        newshape = Part.Shape()
				self.obj.Shape = newshape
                print self.node

	def drawSegment(self,point):
		"draws a new segment"
		if (len(self.node) == 1):
			self.linetrack.on()
			msg(translate("draft", "Pick next point:\n"))
                        self.planetrack.set(self.node[0])
		elif (len(self.node) == 2):
			if not self.obj: self.createTempObject()
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
		elif self.ui.yValue.isEnabled():
			self.ui.yValue.setFocus()
			self.ui.yValue.selectAll()
                else:
                        self.ui.zValue.setFocus()
			self.ui.zValue.selectAll()



class Wire(Line):
        "a FreeCAD command for creating a wire"
	def __init__(self):
		Line.__init__(self,wiremode=True)
	def GetResources(self):
		return {'Pixmap'  : 'Draft_wire',
			'MenuText': str(translate("draft", "Wire").toLatin1()),
			'ToolTip': str(translate("draft", "Creates a multiple-point wire. CTRL to snap, SHIFT to constrain").toLatin1())}


# DNC: class BSpline
class BSpline(Line):
        "a FreeCAD command for creating a b-spline"
	def __init__(self):
		Line.__init__(self,wiremode=True)
	def GetResources(self):
		return {'Pixmap'  : 'Draft_bspline',
			'MenuText': str(translate("draft", "B-Spline").toLatin1()),
			'ToolTip': str(translate("draft", "Creates a multiple-point b-spline. CTRL to snap, SHIFT to constrain").toLatin1())}

	def drawSegment(self,point):
		"draws a new segment"
		if (len(self.node) == 1):
			self.linetrack.on()
			msg(translate("draft", "Pick next point:\n"))
                        self.planetrack.set(self.node[0])
		elif (len(self.node) == 2):
			self.createTempObject()
			last = self.node[len(self.node)-2]
			newseg = Part.Line(last,point).toShape()
			self.obj.Shape = newseg
			if self.isWire:
				msg(translate("draft", "Pick next point, or (F)inish or (C)lose:\n"))
		else:
			self.obj.Shape = Part.BSplineCurve(self.node).toShape()
			msg(translate("draft", "Pick next point, or (F)inish or (C)lose:\n"))
			
	def finish(self,closed=False):
		"terminates the operation and closes the poly if asked"
		if (len(self.node) > 1):
                        old = self.obj.Name
                        self.doc.removeObject(old)
                        self.doc.openTransaction("Create "+self.featureName)
                        Draft.makeBSpline(self.node,closed)
                        self.doc.commitTransaction()
		if self.ui:
			self.linetrack.finalize()
			self.constraintrack.finalize()
			self.snap.finalize()
		Creator.finish(self)
        
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
        def IsActive(self):
                if FreeCAD.activeDraftCommand:
                        if FreeCAD.activeDraftCommand.featureName == "Line":
                                return True
                return False
	
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
        def IsActive(self):
                if FreeCAD.activeDraftCommand:
                        if FreeCAD.activeDraftCommand.featureName == "Line":
                                return True
                return False


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
        def IsActive(self):
                if FreeCAD.activeDraftCommand:
                        if FreeCAD.activeDraftCommand.featureName == "Line":
                                return True
                return False
	
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
			self.ui.pointUi()
                        self.ui.hasFill.show()
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
                        self.ui.isRelative.show()
			self.rect.setorigin(point)
			self.rect.on()
                        self.planetrack.set(point)


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
					self.arctrack.setCenter(self.center)
				elif self.tangents and self.tanpoints:
					cir = fcgeo.circleFrom1tan2pt(self.tangents[0], self.tanpoints[0], point)
					self.center = fcgeo.findClosestCircle(point,cir).Center
					self.arctrack.setCenter(self.center)
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
							self.arctrack.setCenter(self.center)
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
				self.arctrack.setRadius(self.rad)
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
				self.arctrack.setApertureAngle(self.angle)
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
							self.arctrack.setCenter(self.center)
							self.linetrack.p1(self.center)
							self.linetrack.p2(self.view.getPoint(arg["Position"][0],arg["Position"][1]))
						self.arctrack.on()
						self.ui.radiusUi()
						self.step = 1
						self.linetrack.on()
						msg(translate("draft", "Pick radius:\n"))
                                                self.planetrack.set(point)
                                                
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
					self.arctrack.setStartAngle(self.firstangle)
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
                        print "debug:",sta, end
                        if end < sta: sta,end = end,sta
                        Draft.makeCircle(self.rad,p,self.ui.hasFill.isChecked(),sta,end)
                self.doc.commitTransaction()
                self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		self.center = Vector(numx,numy,numz)
		self.node = [self.center]
		self.arctrack.setCenter(self.center)
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
				self.arctrack.setCenter(self.center)
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
			self.arctrack.setStartAngle(self.firstangle)
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


class Polygon(Creator):
	"the Draft_Polygon FreeCAD command definition"
        
	def GetResources(self):
		return {'Pixmap'  : 'Draft_polygon',
			'MenuText': str(translate("draft", "Polygon").toLatin1()),
			'ToolTip': str(translate("draft", "Creates a regular polygon. CTRL to snap, SHIFT to constrain").toLatin1())}

	def Activated(self):
		Creator.Activated(self,"Polygon")
		if self.ui:
			self.step = 0
			self.center = None
			self.rad = None
			self.tangents = []
			self.tanpoints = []
			self.ui.pointUi()
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
			self.snap.finalize()
			self.linetrack.finalize()
			self.constraintrack.finalize()
			self.arctrack.finalize()
			self.doc.recompute()

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
			else: # choose radius
				if len(self.tangents) == 2:
					cir = fcgeo.circleFrom2tan1pt(self.tangents[0], self.tangents[1], point)
					self.center = fcgeo.findClosestCircle(point,cir).Center
					self.arctrack.setCenter(self.center)
				elif self.tangents and self.tanpoints:
					cir = fcgeo.circleFrom1tan2pt(self.tangents[0], self.tanpoints[0], point)
					self.center = fcgeo.findClosestCircle(point,cir).Center
					self.arctrack.setCenter(self.center)
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
							self.arctrack.setCenter(self.center)
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
				self.arctrack.setRadius(self.rad)
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
							self.arctrack.setCenter(self.center)
							self.linetrack.p1(self.center)
							self.linetrack.p2(self.view.getPoint(arg["Position"][0],arg["Position"][1]))
						self.arctrack.on()
						self.ui.radiusUi()
                                                self.ui.numFaces.show()
						self.step = 1
						self.linetrack.on()
						msg(translate("draft", "Pick radius:\n"))
                                                self.planetrack.set(point)
				elif (self.step == 1): # choose radius
                                        self.ui.cross(False)
                                        self.drawPolygon()

	def drawPolygon(self):
		"actually draws the FreeCAD object"
                p = plane.getRotation()
                p.move(self.center)
                self.doc.openTransaction("Create Polygon")                     
                Draft.makePolygon(self.ui.numFaces.value(),self.rad,True,p,face=self.ui.hasFill.isChecked())
                self.doc.commitTransaction()
                self.finish()

	def numericInput(self,numx,numy,numz):
		"this function gets called by the toolbar when valid x, y, and z have been entered there"
		self.center = Vector(numx,numy,numz)
		self.node = [self.center]
		self.arctrack.setCenter(self.center)
		self.arctrack.on()
		self.ui.radiusUi()
		self.step = 1
                self.ui.radiusValue.setFocus()
		msg(translate("draft", "Pick radius:\n"))
		
	def numericRadius(self,rad):
		"this function gets called by the toolbar when valid radius have been entered there"
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
                self.drawPolygon()
        
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
                self.cont = None
                self.dir = None

	def GetResources(self):
		return {'Pixmap'  : 'Draft_dimension',
			'MenuText': str(translate("draft", "Dimension").toLatin1()),
			'ToolTip': str(translate("draft", "Creates a dimension. CTRL to snap, SHIFT to constrain, ALT to select a segment").toLatin1())}

	def Activated(self):
                if self.cont: self.finish()
                else:
                        Creator.Activated(self,"Dimension")
                        if self.ui:
                                self.ui.pointUi()
                                self.ui.continueCmd.show()
                                self.altdown = False
                                self.call = self.view.addEventCallback("SoEvent",self.action)
                                self.snap = snapTracker()
                                self.dimtrack = dimTracker()
                                self.arctrack = arcTracker()
                                self.link = None
                                self.edges = []
                                self.pts = []
                                self.angledata = None
                                self.center = None
                                self.constraintrack = lineTracker(dotted=True)
                                msg(translate("draft", "Pick first point:\n"))
                                FreeCADGui.activeWorkbench().draftToolBar.draftWidget.setVisible(True)

	def finish(self,closed=False):
		"terminates the operation"
                self.cont = None
                self.dir = None
		Creator.finish(self)
		if self.ui:
			self.dimtrack.finalize()
			self.constraintrack.finalize()
			self.snap.finalize()

	def createObject(self):
		"creates an object in the current doc"
		self.doc.openTransaction("Create "+self.featureName)
                if self.angledata:
                        Draft.makeAngularDimension(self.center,self.angledata)
                elif self.link:
                        Draft.makeDimension(self.link[0],self.link[1],self.link[2],self.node[2])
                else:
                        Draft.makeDimension(self.node[0],self.node[1],self.node[2])
		self.doc.commitTransaction()
                if self.ui.continueCmd.isChecked():
                        self.cont = self.node[2]
                        if not self.dir:
                                if self.link:
                                        v1 = self.link[0].Shape.Vertexes[self.link[1]].Point
                                        v2 = self.link[0].Shape.Vertexes[self.link[2]].Point
                                        self.dir = v2.sub(v1)
                                else:
                                        self.dir = self.node[1].sub(self.node[0])
                        self.node = [self.node[1]]
                self.link = None

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg)
                        self.ui.cross(True)
			if arg["AltDown"] and (len(self.node)<3):
                                self.ui.cross(False)
                                self.dimtrack.off()
				if not self.altdown:
					self.altdown = True
					self.ui.switchUi(True)
				snapped = self.view.getObjectInfo((arg["Position"][0],arg["Position"][1]))
				if snapped:
					ob = self.doc.getObject(snapped['Object'])
                                        if "Edge" in snapped['Component']:
                                                num = int(snapped['Component'].lstrip('Edge'))-1
                                                ed = ob.Shape.Edges[num]
                                                v1 = ed.Vertexes[0].Point
                                                v2 = ed.Vertexes[-1].Point
                                                self.dimtrack.update([v1,v2],self.cont)
			else:
                                self.ui.cross(True)
                                if self.node and (len(self.edges) < 2):
                                        self.dimtrack.on()
                                if len(self.edges) == 2:
                                        self.dimtrack.off()
                                        r = point.sub(self.center)
                                        self.arctrack.setRadius(r.Length)
                                        a = self.arctrack.getAngle(point)
                                        pair = fcgeo.getBoundaryAngles(a,self.pts)
                                        if not (pair[0] < a < pair[1]):
                                                self.angledata = [4*math.pi-pair[0],2*math.pi-pair[1]]
                                        else:
                                                self.angledata = [2*math.pi-pair[0],2*math.pi-pair[1]]
                                        self.arctrack.setStartAngle(self.angledata[0])
                                        self.arctrack.setEndAngle(self.angledata[1])
				if self.altdown:
					self.altdown = False
					self.ui.switchUi(False)
                                if self.dir:
                                        point = self.node[0].add(fcvec.project(point.sub(self.node[0]),self.dir))
				if len(self.node) == 2:
                                        self.dimtrack.update(self.node+[point],self.cont)
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
                                                if 'Edge' in snapped['Component']:
                                                        num = int(snapped['Component'].lstrip('Edge'))-1
                                                        ed = ob.Shape.Edges[num]
                                                        v1 = ed.Vertexes[0].Point
                                                        v2 = ed.Vertexes[-1].Point
                                                        i1 = i2 = None
                                                        for i in range(len(ob.Shape.Vertexes)):
                                                                if v1 == ob.Shape.Vertexes[i].Point:
                                                                        i1 = i
                                                                if v2 == ob.Shape.Vertexes[i].Point:
                                                                        i2 = i
                                                        if (i1 != None) and (i2 != None):
                                                                if not self.edges:
                                                                        self.node = [v1,v2]
                                                                        self.link = [ob,i1,i2]
                                                                        self.edges.append(ed)
                                                                else:
                                                                        self.edges.append(ed)
                                                                        self.node.extend([v1,v2])
                                                                        c = fcgeo.findIntersection(self.node[0],
                                                                                                             self.node[1],
                                                                                                             self.node[2],
                                                                                                             self.node[3],
                                                                                                             True,True)
                                                                        if c:
                                                                                self.center = c[0]
                                                                                self.arctrack.setCenter(self.center)
                                                                                self.arctrack.on()
                                                                                for e in self.edges:
                                                                                        for v in e.Vertexes:
                                                                                                self.pts.append(self.arctrack.getAngle(v.Point))
                                                                        else:
                                                                                msg(translate("draft", "Edges don't intersect!\n"))
                                                                                self.finish()
                                                                        
                                                        self.dimtrack.on()
				else:
                                        if self.dir:
                                                point = self.node[0].add(fcvec.project(point.sub(self.node[0]),self.dir))
					self.node.append(point)
				self.dimtrack.update(self.node)
				if (len(self.node) == 1):
					self.dimtrack.on()
                                        self.planetrack.set(self.node[0])
                                elif (len(self.node) == 2) and self.cont:
                                        self.node.append(self.cont)
                                        self.createObject()
                                        if not self.cont: self.finish()
				elif (len(self.node) == 3):
					self.createObject()
					if not self.cont: self.finish()
                                elif self.angledata:
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
			if not self.cont: self.finish()



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
                        self.planetrack = PlaneTracker()

        def IsActive(self):
                if FreeCADGui.ActiveDocument:
                        return True
                else:
                        return False
		
	def finish(self):
		self.node = []
		FreeCAD.activeDraftCommand = None
		if self.ui:
			self.ui.offUi()
			self.ui.sourceCmd=None
			self.ui.cross(False)
                msg("")
                self.planetrack.finalize()
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
                self.doc.recompute()

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
                                        self.planetrack.set(point)
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
			'MenuText': str(translate("draft", "Apply Current Style").toLatin1()),
			'ToolTip': str(translate("draft", "Applies current line width and color to selected objects").toLatin1())}

        def IsActive(self):
                if Draft.getSelection():
                        return True
                else:
                        return False

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
				if not arg["AltDown"]:
                                        self.step = 3
                                        self.finish()
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
			elif (self.step == 2):
				currentrad = fcvec.dist(point,self.center)
				if (currentrad != 0):
					angle = fcvec.angle(plane.u, point.sub(self.center), plane.axis)
				else: angle = 0
				if (angle < self.firstangle): 
					sweep = (2*math.pi-self.firstangle)+angle
				else:
					sweep = angle - self.firstangle
				self.arctrack.setApertureAngle(sweep)
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
					self.ui.radiusUi()
					self.ui.labelRadius.setText("Base angle")
					self.linetrack.p1(self.center)
					self.arctrack.setCenter(self.center)
					self.ghost.trans.center.setValue(self.center.x,self.center.y,self.center.z)
					self.linetrack.on()
					self.step = 1
					msg(translate("draft", "Pick base angle:\n"))
                                        self.planetrack.set(point)
				elif (self.step == 1):
					self.ui.labelRadius.setText("Rotation")
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
                self.running = False
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
				msg(translate("draft", "Offset only works on one object at a time\n"),"warning")
			else:
				self.proceed()

	def proceed(self):
		if self.call: self.view.removeEventCallback("SoEvent",self.call)
		self.sel = Draft.getSelection()[0]
                if not self.sel.isDerivedFrom("Part::Feature"):
                        msg(translate("draft", "Cannot offset this object type\n"),"warning")
                        self.finish()
                else:
                        self.step = 0
                        self.dvec = None
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
                        self.shape = self.sel.Shape
                        self.mode = None
                        if "Radius" in self.sel.PropertiesList:
                                self.ghost = arcTracker()
                                self.mode = "Circle"
                                self.center = self.shape.Edges[0].Curve.Center
                                self.ghost.setCenter(self.center)
                                self.ghost.setStartAngle(math.radians(self.sel.FirstAngle))
                                self.ghost.setEndAngle(math.radians(self.sel.LastAngle))
                        else:
                                self.ghost = wireTracker(self.shape)
                                self.mode = "Wire"
                        self.call = self.view.addEventCallback("SoEvent",self.action)
                        msg(translate("draft", "Pick distance:\n"))
                        self.ui.cross(True)
                        self.planetrack.set(self.shape.Vertexes[0].Point)
                        self.running = True

	def action(self,arg):
		"scene event handler"
		if (arg["Type"] == "SoLocation2Event"):
                        self.ui.cross(True)
                        point,ctrlPoint = getPoint(self,arg)
			if (arg["ShiftDown"]) and self.constrainSeg:
				dist = fcgeo.findPerpendicular(point,self.shape,self.constrainSeg[1])
                                e = self.shape.Edges[self.constrainSeg[1]]
				self.constraintrack.p1(e.Vertexes[0].Point)
				self.constraintrack.p2(point.add(dist[0]))
				self.constraintrack.on()
			else:
				dist = fcgeo.findPerpendicular(point,self.shape.Edges)
				self.constraintrack.off()
			if dist:
                                self.ghost.on()
                                if self.mode == "Wire":
                                        d = fcvec.neg(dist[0])
                                        v1 = fcgeo.getTangent(self.shape.Edges[0],point)
                                        v2 = fcgeo.getTangent(self.shape.Edges[dist[1]],point)
                                        a = -fcvec.angle(v1,v2)
                                        self.dvec = fcvec.rotate(d,a,plane.axis)
                                        self.ghost.update(fcgeo.offsetWire(self.shape,self.dvec))
                                elif self.mode == "Circle":
                                        self.dvec = point.sub(self.center).Length
                                        self.ghost.setRadius(self.dvec)
                                self.constrainSeg = dist
				self.linetrack.on()
				self.linetrack.p1(point)
				self.linetrack.p2(point.add(dist[0]))
				self.ui.radiusValue.setText("%.2f" % dist[0].Length)
			else:
                                self.dvec = None
                                self.ghost.off()
				self.constrainSeg = None
				self.linetrack.off()
				self.ui.radiusValue.setText("off")
			self.ui.radiusValue.setFocus()
			self.ui.radiusValue.selectAll()
			if self.extendedCopy:
				if not arg["AltDown"]: self.finish()
                if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
                                copymode = False
                                if arg["AltDown"] or self.ui.isCopy.isChecked(): copymode = True
                                if self.dvec:
                                        self.doc.openTransaction("Offset")
                                        nwire = Draft.offset(self.sel,self.dvec,copymode)
                                        self.doc.commitTransaction()
				if arg["AltDown"]:
					self.extendedCopy = True
				else:
					self.finish()
                                        
	def finish(self,closed=False):
		Modifier.finish(self)
		if self.ui and self.running:
			self.snap.finalize()
			self.linetrack.finalize()
			self.constraintrack.finalize()
			self.ghost.finalize()


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
                newob = None
                nodelete = False
		edges = []
		wires = []
		openwires = []
		faces = []
                groups = []
                curves = []
                facewires = []
		# determining what we have in our selection
		for ob in self.sel:
			if ob.Type == "App::DocumentObjectGroup":
                                groups.append(ob)
                        else:
                                if ob.Shape.ShapeType == 'Edge': openwires.append(ob.Shape)
                                for f in ob.Shape.Faces:
                                        faces.append(f)
                                        facewires.extend(f.Wires)
                                for w in ob.Shape.Wires:
                                        if w.isClosed():
                                                wires.append(w)
                                        else:
                                                openwires.append(w)
                                for e in ob.Shape.Edges:
                                        if not isinstance(e.Curve,Part.Line):
                                                curves.append(e)
                                lastob = ob
                # print "objects:",self.sel," edges:",edges," wires:",wires," openwires:",openwires," faces:",faces," groups:",groups," curves:",curves," facewires:",facewires
		# applying transformation
		self.doc.openTransaction("Upgrade")
                if groups:
                        # if we have a group: turn each closed wire inside into a face
                        msg(translate("draft", "Found groups: closing each open object inside\n"))
                        for grp in groups: 
                                for ob in grp.Group:
                                        if not ob.Shape.Faces:
                                                for w in ob.Shape.Wires:
                                                        newob = Draft.makeWire(w,closed=w.isClosed())
                                                        self.sel.append(ob)
                                                        grp.addObject(newob)
                elif faces and (len(wires)+len(openwires)==len(facewires)):
                        # we have exactly 2 objects: we fuse them
                        if (len(self.sel) == 2) and (not curves):
                                msg(translate("draft", "Found 2 objects: fusing them\n"))
                                newob = Draft.fuse(self.sel[0],self.sel[1])
                                nodelete = True
                        elif (len(self.sel) > 2) or (len(faces) > 1):
                                # more than 2 objects or faces: we try the draft way: make one face out of them
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
                                                msg(translate("draft", "Found 2 objects or faces: making a parametric face\n"))
                                                newob = Draft.makeWire(u.Wires[0],closed=True,face=f)
                                                Draft.formatObject(newob,lastob)
                                        else:
                                                # if not possible, we do a non-parametric union
                                                msg(translate("draft", "Found 2 objects containing curves: fusing them\n"))
                                                newob = self.doc.addObject("Part::Feature","Union")
                                                newob.Shape = u
                                                Draft.formatObject(newob,lastob)
                                else:
                                        # if not possible, we do a non-parametric union
                                        msg(translate("draft", "Found 2 objects: fusing them\n"))
                                        newob = self.doc.addObject("Part::Feature","Union")
                                        newob.Shape = u
                                        Draft.formatObject(newob,lastob)
                        elif len(self.sel) == 1:
                                # only one object: if not parametric, we "draftify" it
                                nodelete = True
                                if (not("Base" in self.sel[0].PropertiesList)) and (not curves):
                                        msg(translate("draft", "Found 1 non-parametric objects: draftifying it\n"))
                                        Draft.draftify(self.sel[0])
                                        
                elif wires and (not faces) and (not openwires):
                        if (len(self.sel) == 1) and self.sel[0].isDerivedFrom("Sketcher::SketchObject") and (not curves):
                                msg(translate("draft", "Found 1 closed sketch object: making a face from it\n"))
                                newob = Draft.makeWire(self.sel[0].Shape.Wires[0],closed=True)
                                self.sel[0].ViewObject.Visibility = False
                                nodelete = True
                        else:
                                # only closed wires
                                for w in wires:
                                        f = Part.Face(w)
                                        faces.append(f)
                                for f in faces:
                                        if not curves: 
                                                newob = Draft.makeWire(f.Wire,closed=True)
                                        else:
                                                # if there are curved segments, we do a non-parametric face
                                                msg(translate("draft", "Found closed wires: making faces\n"))
                                                newob = self.doc.addObject("Part::Feature","Face")
                                                newob.Shape = f
                                                Draft.formatObject(newob,lastob)
                elif (len(openwires) == 1) and (not faces) and (not wires):
                        # special case, we have only one open wire. We close it!"
                        p0 = openwires[0].Vertexes[0].Point
                        p1 = openwires[0].Vertexes[-1].Point
                        edges = openwires[0].Edges
                        edges.append(Part.Line(p1,p0).toShape())
                        w = Part.Wire(fcgeo.sortEdges(edges))
                        msg(translate("draft", "Found 1 open wire: closing it\n"))
                        if not curves:
                                newob = Draft.makeWire(w,closed=True)
                        else:
                                # if not possible, we do a non-parametric union
                                newob = self.doc.addObject("Part::Feature","Wire")
                                newob.Shape = w
                                Draft.formatObject(newob,lastob)
                elif openwires and (not wires) and (not faces):
                        # only open wires and edges: we try to join their edges
                        for ob in self.sel:
                                for e in ob.Shape.Edges:
                                        edges.append(e)
                        newob = None
                        w = Part.Wire(fcgeo.sortEdges(edges[:]))
                        if len(w.Edges) == len(edges):
                                msg(translate("draft", "Found several edges: wiring them\n"))
                                if not curves:
                                        newob = Draft.makeWire(w)
                                else:
                                        newob = self.doc.addObject("Part::Feature","Wire")
                                        newob.Shape = w
                                        Draft.formatObject(newob,lastob)
                        if not newob:
                                print "no new object found"
                                msg(translate("draft", "Found several non-connected edges: making compound\n"))
                                newob = self.compound()
                                Draft.formatObject(newob,lastob)
                else:
                        msg(translate("draft", "Found several non-treatable objects: making compound\n"))
                        newob = self.compound()
                        Draft.formatObject(newob,lastob) 
                if not nodelete:
                        for ob in self.sel:
                                if not ob.Type == "App::DocumentObjectGroup":
                                        self.doc.removeObject(ob.Name)
		self.doc.commitTransaction()
		if newob: Draft.select(newob)
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
                        lastob = ob
		# applying transformation
		self.doc.openTransaction("Downgrade")
                if len(self.sel) == 1:
                        # special case, we have one parametric object: we "de-parametrize" it
                        if (self.sel[0].isDerivedFrom("Part::Feature")) and ("Base" in self.sel[0].PropertiesList):
                                msg(translate("draft", "Found 1 parametric object: breaking its dependencies\n"))
                                Draft.shapify(self.sel[0])
                                self.doc.commitTransaction()
                                Modifier.finish(self)
                                return
                if len(self.sel) == 2:
                        # we have only 2 objects: cut 2nd from 1st
                        msg(translate("draft", "Found 2 objects: subtracting them\n"))
                        newob = Draft.cut(self.sel[0],self.sel[1])
		elif (len(faces) > 1):
			if len(self.sel) == 1:
                                # one object with several faces: split it
				for f in faces:
                                        msg(translate("draft", "Found several faces: splitting them\n"))
					newob = self.doc.addObject("Part::Feature","Face")
					newob.Shape = f
					Draft.formatObject(newob,self.sel[0])
				self.doc.removeObject(ob.Name)
			else:
                                # several objects: remove all the faces from the first one
                                msg(translate("draft", "Found several objects: subtracting them from the first one\n"))
				u = faces.pop(0)
				for f in faces:
					u = u.cut(f)
				newob = self.doc.addObject("Part::Feature","Subtraction")
				newob.Shape = u
				for ob in self.sel:
					Draft.formatObject(newob,ob)
					self.doc.removeObject(ob.Name)
		elif (len(faces) > 0):
                        # only one face: we extract its wires
                        msg(translate("draft", "Found 1 face: extracting its wires\n"))
			for w in faces[0].Wires:
                                newob = self.doc.addObject("Part::Feature","Wire")
				newob.Shape = w
                                Draft.formatObject(newob,lastob)
			for ob in self.sel:
				self.doc.removeObject(ob.Name)
		else:
                        # no faces: split wire into single edges
                        msg(translate("draft", "Found only wires: extracting their edges\n"))
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
                        self.point = getPoint(self,arg)[0]
			self.shift = arg["ShiftDown"]
			self.alt = arg["AltDown"]
			if arg["CtrlDown"]: self.snapped = None
			else: self.snapped = self.view.getObjectInfo((arg["Position"][0],arg["Position"][1]))
			if self.extrudeMode:
				dist = self.extrude(self.point,self.shift)
			else:
				dist = self.redraw(self.point,self.snapped,self.shift,self.alt)
			self.constraintrack.p1(self.point)
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
                if self.force:
                        ratio = self.force/delta.Length
                        delta.multiply(ratio)
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
                self.force = None
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

class Drawing(Modifier):
        "The Draft Drawing command definition"

        def GetResources(self):
		return {'Pixmap'  : 'Draft_putOnSheet',
			'MenuText': str(translate("draft", "Drawing").toLatin1()),
			'ToolTip': str(translate("draft", "Puts the selected objects on a Drawing sheet.").toLatin1())}

        def IsActive(self):
                if Draft.getSelection():
                        return True
                else:
                        return False

	def Activated(self):
		Modifier.Activated(self,"Drawing")
                sel = Draft.getSelection()
                if not sel:
                        page = self.createDefaultPage()
                else:
                        page = None
                        for obj in sel:
                                if obj.isDerivedFrom("Drawing::FeaturePage"):
                                        page = obj
                                        sel.pop(sel.index(obj))
                        if not page:
                                for obj in self.doc.Objects:
                                        if obj.isDerivedFrom("Drawing::FeaturePage"):
                                                page = obj
                        if not page:
                                page = self.createDefaultPage()
                        sel.reverse()
                        for obj in sel:
                                if obj.ViewObject.isVisible():
                                        name = 'View'+obj.Name
                                        oldobj = page.getObject(name)
                                        if oldobj: self.doc.removeObject(oldobj.Name)
                                        Draft.makeDrawingView(obj,page)
                        self.doc.recompute()

        def createDefaultPage(self):
                template = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft").GetString("template")
                if not template:
                        template = FreeCAD.getResourceDir()+'Mod/Drawing/Templates/A3_Landscape.svg'
                page = self.doc.addObject('Drawing::FeaturePage','Page')
                page.ViewObject.HintOffsetX = 200
                page.ViewObject.HintOffsetY = 100
                page.ViewObject.HintScale = 20
                page.Template = template
                self.doc.recompute()
                return page
        
class ToggleDisplayMode():
	"The ToggleDisplayMode FreeCAD command definition"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_switchMode',
                        'MenuText': str(translate("draft", "Toggle display mode").toLatin1()),
			'ToolTip': str(translate("draft", "Swaps display mode of selected objects between wireframe and flatlines").toLatin1())}

        def IsActive(self):
                if Draft.getSelection():
                        return True
                else:
                        return False
        
	def Activated(self):
                for obj in Draft.getSelection():
                        if obj.ViewObject.DisplayMode == "Flat Lines":
                                if "Wireframe" in obj.ViewObject.listDisplayModes():
                                        obj.ViewObject.DisplayMode = "Wireframe"
                        elif obj.ViewObject.DisplayMode == "Wireframe":
                                if "Flat Lines" in obj.ViewObject.listDisplayModes():
                                        obj.ViewObject.DisplayMode = "Flat Lines"


class Edit(Modifier):
	"The Draft_Edit FreeCAD command definition"

	def __init__(self):
		self.running = False

	def GetResources(self):
		return {'Pixmap'  : 'Draft_Edit',
			'MenuText': str(translate("draft", "Edit").toLatin1()),
			'ToolTip': str(translate("draft", "Edits the active object").toLatin1())}

        def IsActive(self):
                if Draft.getSelection():
                        self.selection = Draft.getSelection()
                        if "Proxy" in self.selection[0].PropertiesList:
                                if hasattr(self.selection[0].Proxy,"Type"):
                                        return True
                        else:
                                return False
                else:
                        return False

	def Activated(self):
                if self.running:
                        self.finish()
                else:
                        Modifier.Activated(self,"Edit")
                        self.ui.editUi()
                        if self.doc:
                                self.obj = Draft.getSelection()
                                if self.obj:
                                        self.obj = self.obj[0]
                                        self.editing = None
                                        self.editpoints = []
                                        self.pl = None
                                        if "Placement" in self.obj.PropertiesList:
                                                self.pl = self.obj.Placement
                                                self.invpl = self.pl.inverse()
                                        if Draft.getType(self.obj) in ["Wire","BSpline"]:
                                                for p in self.obj.Points:
                                                        if self.pl: p = self.pl.multVec(p)
                                                        self.editpoints.append(p)
                                        elif Draft.getType(self.obj) == "Circle":
                                                self.editpoints.append(self.obj.Placement.Base)
                                                if self.obj.FirstAngle == self.obj.LastAngle:
                                                        self.editpoints.append(self.obj.Shape.Vertexes[0].Point)
                                        elif Draft.getType(self.obj) == "Rectangle":
                                                self.editpoints.append(self.obj.Placement.Base)
                                                self.editpoints.append(self.obj.Shape.Vertexes[2].Point)
                                                v = self.obj.Shape.Vertexes
                                                self.bx = v[1].Point.sub(v[0].Point)
                                                if self.obj.Length < 0: self.bx = fcvec.neg(self.bx)
                                                self.by = v[2].Point.sub(v[1].Point)
                                                if self.obj.Height < 0: self.by = fcvec.neg(self.by)
                                        elif Draft.getType(self.obj) == "Polygon":
                                                self.editpoints.append(self.obj.Placement.Base)
                                                self.editpoints.append(self.obj.Shape.Vertexes[0].Point)
                                        elif Draft.getType(self.obj) == "Dimension":
                                                p = self.obj.ViewObject.Proxy.textpos.translation.getValue()
                                                self.editpoints.append(self.obj.Start)
                                                self.editpoints.append(self.obj.End)
                                                self.editpoints.append(self.obj.Dimline)
                                                self.editpoints.append(Vector(p[0],p[1],p[2]))
                                        self.trackers = []
                                        self.snap = None
                                        self.constraintrack = None
                                        if self.editpoints:
                                                for ep in range(len(self.editpoints)):
                                                        self.trackers.append(editTracker(self.editpoints[ep],self.obj.Name,ep))
                                                self.snap = snapTracker()
                                                self.constraintrack = lineTracker(dotted=True)
                                                self.call = self.view.addEventCallback("SoEvent",self.action)
                                                self.running = True
                                                plane.save()
                                                if "Shape" in self.obj.PropertiesList:
                                                        plane.alignToFace(self.obj.Shape)
                                                self.planetrack.set(self.editpoints[0])
                                        else:
                                                msg(translate("draft", "This object type is not editable\n"),'warning')
                                                self.finish()
                                else:
                                        self.finish()

	def finish(self,closed=False):
		"terminates the operation"
                if self.ui:
                        if self.snap:
                                self.snap.finalize()
                        if self.trackers:
                                for t in self.trackers:
                                        t.finalize()
                        if self.constraintrack:
                                self.constraintrack.finalize()
		Modifier.finish(self)
                plane.restore()
                self.running = False

	def action(self,arg):
		"scene event handler"
                if arg["Type"] == "SoKeyboardEvent" and arg["Key"] == "ESCAPE":
			self.finish()
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
                        if self.editing != None:
                                point,ctrlPoint = getPoint(self,arg)
                                # Draw constraint tracker line.
                                if (arg["ShiftDown"]):
                                        self.constraintrack.p1(point)
                                        self.constraintrack.p2(ctrlPoint)
                                        self.constraintrack.on()
                                else: self.constraintrack.off()
                                self.trackers[self.editing].set(point)
                                self.update(self.trackers[self.editing].get())
		elif (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
                                if self.editing == None:
                                        snapped = self.view.getObjectInfo((arg["Position"][0],arg["Position"][1]))
                                        if snapped:
                                                if snapped['Object'] == self.obj.Name:
                                                        if 'EditNode' in snapped['Component']:
                                                                self.ui.pointUi()
                                                                self.ui.isRelative.show()
                                                                self.editing = int(snapped['Component'][8:])
                                                                if "Points" in self.obj.PropertiesList:
                                                                        self.node.append(self.obj.Points[self.editing])
                                else:
                                        self.numericInput(self.trackers[self.editing].get())

        def update(self,v):
                if Draft.getType(self.obj) in ["Wire","BSpline"]:
                        pts = self.obj.Points
                        pts[self.editing] = self.invpl.multVec(v)
                        self.obj.Points = pts
                        self.trackers[self.editing].set(v)
                elif Draft.getType(self.obj) == "Circle":
                        delta = v.sub(self.obj.Placement.Base)
                        if self.editing == 0:
                                p = self.obj.Placement
                                p.move(delta)
                                self.obj.Placement = p
                                self.trackers[0].set(self.obj.Placement.Base)
                        elif self.editing == 1:
                                self.obj.Radius = delta.Length
                        self.trackers[1].set(self.obj.Shape.Vertexes[0].Point)
                elif Draft.getType(self.obj) == "Rectangle":
                        delta = v.sub(self.obj.Placement.Base)
                        if self.editing == 0:
                                p = self.obj.Placement
                                p.move(delta)
                                self.obj.Placement = p
                        elif self.editing == 1:
                                diag = v.sub(self.obj.Placement.Base)
                                nx = fcvec.project(diag,self.bx)
                                ny = fcvec.project(diag,self.by)
                                ax = nx.Length
                                ay = ny.Length
                                if ax and ay:
                                        if abs(nx.getAngle(self.bx)) > 0.1:
                                                ax = -ax
                                        if abs(ny.getAngle(self.by)) > 0.1:
                                                ay = -ay
                                        self.obj.Length = ax
                                        self.obj.Height = ay
                        self.trackers[0].set(self.obj.Placement.Base)
                        self.trackers[1].set(self.obj.Shape.Vertexes[2].Point)
                elif Draft.getType(self.obj) == "Polygon":
                        delta = v.sub(self.obj.Placement.Base)
                        if self.editing == 0:
                                p = self.obj.Placement
                                p.move(delta)
                                self.obj.Placement = p
                                self.trackers[0].set(self.obj.Placement.Base)
                        elif self.editing == 1:
                                if self.obj.DrawMode == 'inscribed':
                                        self.obj.Radius = delta.Length
                                else:
                                        halfangle = ((math.pi*2)/self.obj.FacesNumber)/2
                                        rad = math.cos(halfangle)*delta.Length
                                        self.obj.Radius = rad
                        self.trackers[1].set(self.obj.Shape.Vertexes[0].Point)
                elif Draft.getType(self.obj) == "Dimension":
                        if self.editing == 0:
                                self.obj.Start = v
                        elif self.editing == 1:
                                self.obj.End = v
                        elif self.editing == 2:
                                self.obj.Dimline = v
                        elif self.editing == 3:
                                self.obj.ViewObject.TextPosition = v
                        

	def numericInput(self,v,numy=None,numz=None):
		'''this function gets called by the toolbar
                when valid x, y, and z have been entered there'''
                if (numy != None):
                        v = Vector(v,numy,numz)
                self.doc.openTransaction("Edit "+self.obj.Name)
                self.update(v)
                self.doc.commitTransaction()
                self.editing = None
                self.ui.editUi()
                self.node = []

class AddToGroup():
	"The AddToGroup FreeCAD command definition"

	def GetResources(self):
		return {'Pixmap'  : 'Draft_addToGroup',
                        'MenuText': str(translate("draft", "Add to group").toLatin1()),
			'ToolTip': str(translate("draft", "Adds the selected object(s) to an existing group").toLatin1())}

        def IsActive(self):
                if Draft.getSelection():
                        return True
                else:
                        return False
        
	def Activated(self):
                groups = Draft.getGroupNames()
                if groups:
                        self.ui = FreeCADGui.activeWorkbench().draftToolBar.ui
                        self.ui.sourceCmd = self
                        self.ui.popupMenu(groups)

        def proceed(self,groupname):
                self.ui.sourceCmd = None
                g = FreeCAD.ActiveDocument.getObject(groupname)
                for obj in Draft.getSelection():
                        try:
                                g.addObject(obj)
                        except:
                                pass

class AddPoint(Modifier):
	"The Draft_AddPoint FreeCAD command definition"

	def __init__(self):
		self.running = False

	def GetResources(self):
		return {'Pixmap'  : 'Draft_addpoint',
			'MenuText': str(translate("draft", "Add Point").toLatin1()),
			'ToolTip': str(translate("draft", "Adds a point to an existing wire/bspline").toLatin1())}

        def IsActive(self):
		self.selection = Draft.getSelection()
		if (Draft.getType(self.selection[0]) in ['Wire','BSpline']):
			return True
		else:
			return False

	def Activated(self):
                if self.running:
                        self.finish()
                else:
                        Modifier.Activated(self,"Add Point")
                        self.ui.editUi()
                        if self.doc:
                                self.obj = Draft.getSelection()
                                if self.obj:
                                        self.obj = self.obj[0]
                                        self.editing = None
					self.pl = None
                                        if "Placement" in self.obj.PropertiesList:
                                                self.pl = self.obj.Placement
                                                self.invpl = self.pl.inverse()
					self.trackers = []
					self.resetTrackers()
					self.snap = snapTracker()
					self.call = self.view.addEventCallback("SoEvent",self.action)
					self.running = True
					plane.save()
					self.planetrack.set(self.obj.Points[0])
                                else:
                                        self.finish()

	def finish(self,closed=False):
		"terminates the operation"
                if self.ui:
                        if self.snap:
                                self.snap.finalize()
                        if self.trackers:
                                for t in self.trackers:
                                        t.finalize()
		Modifier.finish(self)
                plane.restore()
                self.running = False

	def action(self,arg):
		"scene event handler"
                if arg["Type"] == "SoKeyboardEvent" and arg["Key"] == "ESCAPE":
			self.finish()
		if (arg["Type"] == "SoLocation2Event"): #mouse movement detection
			point,ctrlPoint = getPoint(self,arg)
                        self.ui.cross(True)
		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				snapped = self.view.getObjectInfo((arg["Position"][0],arg["Position"][1]))
				if snapped:
					if snapped['Object'] == self.obj.Name:
						point,ctrlPoint = getPoint(self,arg)
						self.pos = arg["Position"]
						self.update(point)
				else:
					self.finish()

        def update(self,point):
                if Draft.getType(self.obj) == "Wire":
                        pts = self.obj.Points
                        vertexes = fcgeo.getVerts(self.obj.Shape)
                        self.edges = []
                        self.edges = self.obj.Shape.Wires[0].Edges
                        for e in self.edges:
                                if ((point in vertexes) == False) and (fcgeo.isPtOnEdge(point,e)):
                                        i = pts.index(self.invpl.multVec(e.Vertexes[1].Point))
                                        pts.insert(i, self.invpl.multVec(point))
                        self.obj.Points = pts
                        self.resetTrackers()
                if Draft.getType(self.obj) == "BSpline":
                        uNewPoint = self.obj.Shape.Curve.parameter(point)
                        pts = self.obj.Points
                        uPoints = []
                        for p in self.obj.Points:
                                uPoints.append(self.obj.Shape.Curve.parameter(p))
                        for i in range(len(uPoints)-1):
                                if ( uNewPoint > uPoints[i] ) and ( uNewPoint < uPoints[i+1] ):
                                        pts.insert(i+1, self.invpl.multVec(point))
                                        break
                        self.obj.Points = pts
                        self.resetTrackers()

	def resetTrackers(self):
		for t in self.trackers:
			t.finalize()
		self.trackers = []
		for ep in range(len(self.obj.Points)):
			objPoints = self.obj.Points[ep]
			if self.pl: objPoints = self.pl.multVec(objPoints)
			self.trackers.append(editTracker(objPoints,self.obj.Name,ep))

class DelPoint(Modifier):
	"The Draft_DelPoint FreeCAD command definition"

	def __init__(self):
		self.running = False

	def GetResources(self):
		return {'Pixmap'  : 'Draft_delpoint',
			'MenuText': str(translate("draft", "Remove Point").toLatin1()),
			'ToolTip': str(translate("draft", "Removes a point from an existing wire or bspline").toLatin1())}

        def IsActive(self):
		self.selection = Draft.getSelection()
		if (Draft.getType(self.selection[0]) in ['Wire','BSpline']):
			return True
		else:
			return False

	def Activated(self):
                if self.running:
                        self.finish()
                else:
                        Modifier.Activated(self,"Remove Point")
                        self.ui.editUi()
                        if self.doc:
                                self.obj = Draft.getSelection()
                                if self.obj:
                                        self.obj = self.obj[0]
                                        self.editing = None
                                        self.pl = None
                                        if "Placement" in self.obj.PropertiesList:
                                                self.pl = self.obj.Placement
                                        self.trackers = []
					self.resetTrackers()
					if len(self.obj.Points) > 2:
						self.call = self.view.addEventCallback("SoEvent",self.action)
						self.running = True
						plane.save()
						self.planetrack.set(self.obj.Points[0])
					else:
						msg(translate("draft", "Active object must have more than two points/nodes\n"),'warning')
						self.finish()
                                else:
                                        self.finish()

	def finish(self,closed=False):
		"terminates the operation"
                if self.ui:
                        if self.trackers:
                                for t in self.trackers:
                                        t.finalize()
		Modifier.finish(self)
                plane.restore()
                self.running = False

	def action(self,arg):
		"scene event handler"
                if arg["Type"] == "SoKeyboardEvent" and arg["Key"] == "ESCAPE":
			self.finish()
		if (arg["Type"] == "SoMouseButtonEvent"):
			if (arg["State"] == "DOWN") and (arg["Button"] == "BUTTON1"):
				snapped = self.view.getObjectInfo((arg["Position"][0],arg["Position"][1]))
				if snapped:
					if snapped['Object'] == self.obj.Name:
						if 'EditNode' in snapped['Component']:
							self.editing = int(snapped['Component'][8:])
							self.update()
				else:
					self.finish()

        def update(self):
		if len(self.obj.Points) <= 2:
			msg(translate("draft", "Active object must have more than two points/nodes\n"),'warning')
			self.finish()
		else: 
			pts = self.obj.Points
			pts.pop(self.editing)
			self.obj.Points = pts
			self.resetTrackers()

	def resetTrackers(self):
		for t in self.trackers:
			t.finalize()
		self.trackers = []
		for ep in range(len(self.obj.Points)):
			objPoints = self.obj.Points[ep]
			if self.pl: objPoints = self.pl.multVec(objPoints)
			self.trackers.append(editTracker(objPoints,self.obj.Name,ep))

class WireToBSpline(Modifier):
	"The Draft_Wire2BSpline FreeCAD command definition"

	def __init__(self):
		self.running = False

	def GetResources(self):
		return {'Pixmap'  : 'Draft_wirebspline',
			'MenuText': str(translate("draft", "Wire to BSpline").toLatin1()),
			'ToolTip': str(translate("draft", "Converts between Wire and BSpline").toLatin1())}

	def IsActive(self):
		self.selection = Draft.getSelection()
		if (Draft.getType(self.selection[0]) in ['Wire','BSpline']):
			return True
		else:
			return False

	def Activated(self):
                if self.running:
                        self.finish()
                else:
                        Modifier.Activated(self,"Convert Curve Type")
                        if self.doc:
                                self.obj = Draft.getSelection()
                                if self.obj:
                                        self.obj = self.obj[0]
                                        self.pl = None
                                        if "Placement" in self.obj.PropertiesList:
                                                self.pl = self.obj.Placement
                                        self.Points = self.obj.Points
                                        self.closed = self.obj.Closed
					if (Draft.getType(self.selection[0]) == 'Wire'):
						Draft.makeBSpline(self.Points, self.closed, self.pl)
                                        elif (Draft.getType(self.selection[0]) == 'BSpline'):
                                                Draft.makeWire(self.Points, self.closed, self.pl)
                                else:
                                        self.finish()
	def finish(self):
		Modifier.finish(self)

                        

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
FreeCADGui.addCommand('Draft_Polygon',Polygon())
FreeCADGui.addCommand('Draft_BSpline',BSpline())

# modification commands
FreeCADGui.addCommand('Draft_Move',Move())
FreeCADGui.addCommand('Draft_Rotate',Rotate())
FreeCADGui.addCommand('Draft_Offset',Offset())
FreeCADGui.addCommand('Draft_Upgrade',Upgrade())
FreeCADGui.addCommand('Draft_Downgrade',Downgrade())
FreeCADGui.addCommand('Draft_Trimex',Trimex())
FreeCADGui.addCommand('Draft_Scale',Scale())
FreeCADGui.addCommand('Draft_Drawing',Drawing())
FreeCADGui.addCommand('Draft_Edit',Edit())
FreeCADGui.addCommand('Draft_AddPoint',AddPoint())
FreeCADGui.addCommand('Draft_DelPoint',DelPoint())
FreeCADGui.addCommand('Draft_WireToBSpline',WireToBSpline())

# context commands
FreeCADGui.addCommand('Draft_FinishLine',FinishLine())
FreeCADGui.addCommand('Draft_CloseLine',CloseLine())
FreeCADGui.addCommand('Draft_UndoLine',UndoLine())
FreeCADGui.addCommand('Draft_ToggleConstructionMode',ToggleConstructionMode())
FreeCADGui.addCommand('Draft_ApplyStyle',ApplyStyle())
FreeCADGui.addCommand('Draft_ToggleDisplayMode',ToggleDisplayMode())
FreeCADGui.addCommand('Draft_AddToGroup',AddToGroup())
