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

	- Return in SelectPlane should changes the offset, keeping the current plane.
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

import FreeCAD, FreeCADGui, Part, math, sys, os
from FreeCAD import Vector
from draftlibs import fcvec, fcgeo
from pivy import coin

#---------------------------------------------------------------------------
# General functions
#---------------------------------------------------------------------------

def typecheck (args_and_types, name="?"):
        "typecheck([arg1,type),(arg2,type),...]): checks arguments types"
	for v,t in args_and_types:
		if not isinstance (v,t):
                        w = "typecheck[" + str(name) + "]: "
                        w += str(v) + " is not " + str(t) + "\n"
			FreeCAD.Console.PrintWarning(w)
			raise TypeError("Draft." + str(name))

def getDraftPath(pathname=""):
        '''getDraftPath([subpath]): returns the user or system path where the
        Draft module is running from. If path is supplied, the full path to the
        subpath inside the Draft module is returned'''
        path1 = FreeCAD.ConfigGet("AppHomePath") + "Mod/Draft"
	path2 = FreeCAD.ConfigGet("UserAppData") + "Mod/Draft"
	if os.path.exists(path2): return path2 + os.sep + pathname
	else: return path1 + os.sep + pathname
                
def precision():
        "precision(): returns the precision value from Draft user settings"
        params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
	return params.GetInt("precision")
        
def getRealName(name):
	"getRealName(string): strips the trailing numbers from a string name"
	for i in range(1,len(name)):
		if not name[-i] in '1234567890':
			return name[:len(name)-(i-1)]
	return name

def shapify(obj):
        '''shapify(object): transforms a parametric shape object into
        non-parametric and returns the new object'''
        if not (obj.isDerivedFrom("Part::Feature")): return None
        if not "Shape" in obj.PropertiesList: return None
        if obj.Type == "Part::Feature": return obj
        shape = obj.Shape
        name = getRealName(obj.Name)
        FreeCAD.ActiveDocument.removeObject(obj.Name)
        newobj = FreeCAD.ActiveDocument.addObject("Part::Feature",name)
        newobj.Shape = shape
        FreeCAD.ActiveDocument.recompute()
        return newobj

def getGroupContents(objectslist):
        '''getGroupContents(objectlist): if any object of the given list
        is a group, its content is appened to the list, which is returned'''
        newlist = []
        for obj in objectslist:
                if obj.Type == "App::DocumentObjectGroup":
                        newlist.extend(getGroupContents(obj.Group))
                else:
                        newlist.append(obj)
        return newlist

def formatObject(target,origin=None):
	'''
        formatObject(targetObject,[originObject]): This function applies
        to the given target object the current properties 
	set on the toolbar (line color and line width),
        or copies the properties of another object if given as origin.
	It also places the object in construction group if needed.
	'''
	obrep = target.ViewObject
	ui = FreeCADGui.activeWorkbench().draftToolBar.ui
	doc = FreeCAD.ActiveDocument
	if ui.constrButton.isChecked():
		col = ui.getDefaultColor("constr")
		gname = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft").\
		GetString("constructiongroupname")
		if gname:
			grp = doc.getObject(gname)
			if not grp: grp = doc.addObject("App::DocumentObjectGroup",gname) 
			grp.addObject(target)
	else:
		col = ui.getDefaultColor("ui")
	col = (float(col[0]),float(col[1]),float(col[2]),0.0)
	lw = float(ui.widthButton.value())
	if (target.Type == "App::Annotation"):
		obrep.TextColor=col
	else:
		if not origin:
			obrep.LineWidth = lw
                        if "PointColor" in obrep.PropertiesList: obrep.PointColor = col
			obrep.LineColor = col
		else:
			matchrep = origin.ViewObject
			obrep.LineWidth = matchrep.LineWidth
                        if ("PointColor" in obrep.PropertiesList) and \
                                    ("PointColor" in matchrep.PropertiesList):
                                obrep.PointColor = matchrep.PointColor
			obrep.LineColor = matchrep.LineColor
			if (target.Type[:4] == "Part"):
				obrep.ShapeColor = matchrep.ShapeColor
                                if matchrep.DisplayMode in obrep.listDisplayModes():
                                        obrep.DisplayMode = matchrep.DisplayMode

def getSelection():
        "getSelection(): returns the current FreeCAD selection"
	return FreeCADGui.Selection.getSelection()

def select(obj):
	"select(object): deselects everything and selects only the passed object"
	FreeCADGui.Selection.clearSelection()
	FreeCADGui.Selection.addSelection(obj)

def makeCircle(radius, placement=None, face=True, startangle=None, endangle=None):
        '''makeCircle(radius,[placement,face,startangle,endangle]): Creates a circle
        object with given radius. If placement is given, it is
        used. If face is False, the circle is shown as a
        wireframe, otherwise as a face. If startangle AND endangle are given
        (in degrees), they are used and the object appears as an arc.'''
        if placement: typecheck([(placement,FreeCAD.Placement)], "makeCircle")
        obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython","Circle")
        Circle(obj)
        ViewProviderCircle(obj.ViewObject)
        obj.Radius = radius
        if not face: obj.ViewObject.DisplayMode = "Wireframe"
        if (startangle != None) and (endangle != None):
                if startangle == -0: startangle = 0
                obj.StartAngle = startangle
                obj.EndAngle = endangle
        if placement: obj.Placement = placement
        formatObject(obj)
        select(obj)
        FreeCAD.ActiveDocument.recompute()
        return obj
        
def makeRectangle(length, height, placement=None, face=True):
        '''makeRectangle(length,width,[placement,face]): Creates a Rectangle
        object with length in X direction and height in Y direction.
        If a placement is given, it is used. If face is False, the
        rectangle is shown as a wireframe, otherwise as a face.'''
        if placement: typecheck([(placement,FreeCAD.Placement)], "makeRectangle")
        obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython","Rectangle")
        Rectangle(obj)
        ViewProviderRectangle(obj.ViewObject)
        obj.Length = length
        obj.Height = height
        if not face: obj.ViewObject.DisplayMode = "Wireframe"
        if placement: obj.Placement = placement
        formatObject(obj)
        select(obj)
        FreeCAD.ActiveDocument.recompute()
        return obj
        
def makeDimension(p1,p2,p3=None):
        '''makeDimension(p1,p2,[p3]): Creates a Dimension object
        measuring distance between p1 and p2, with the dimension
        line passign through p3.The current line width and color
        will be used.'''
        typecheck([(p1,Vector), (p2,Vector), (p3,Vector)], "makeDimension")
        obj = FreeCAD.ActiveDocument.addObject("App::FeaturePython","Dimension")
	Dimension(obj)
	ViewProviderDimension(obj.ViewObject)
	obj.Start = p1
	obj.End = p2
        if not p3:
            p3 = p2.sub(p1)
            p3.multiply(0.5)
            p3 = p1.add(p3)
	obj.Dimline = p3
	formatObject(obj)
	select(obj)
	FreeCAD.ActiveDocument.recompute()
        return obj

def makeWire(pointslist,closed=False,placement=None,face=True):
        '''makeWire(pointslist,[closed],[placement]): Creates a Wire object
        from the given list of vectors. If closed is True or first
        and last points are identical, the wire is closed. If face is
        true (and wire is closed), the wire will appear filled. Instead of
        a pointslist, you can also pass a Part Wire.'''
        if not isinstance(pointslist,list):
                nlist = []
                for v in pointslist.Vertexes:
                        nlist.append(v.Point)
                pointslist = nlist
        if placement: typecheck([(placement,FreeCAD.Placement)], "makeRectangle")
        if len(pointslist) == 2: fname = "Line"
        else: fname = "Wire"
        obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython",fname)
        Wire(obj)
        ViewProviderWire(obj.ViewObject)
        obj.Points = pointslist
        obj.Closed = closed
        if not face: obj.ViewObject.DisplayMode = "Wireframe"
        if placement: obj.Placement = placement
        formatObject(obj)
	select(obj)
	FreeCAD.ActiveDocument.recompute()
        return obj

def makeLine(p1,p2):
        '''makeLine(p1,p2): Creates a line between p1 and p2.'''
        obj = makeWire([p1,p2])
        return obj

def makeText(stringslist,point=Vector(0,0,0),screen=False):
        '''makeText(strings,[point],[screen]): Creates a Text object at the given point,
        containing the strings given in the strings list, one string by line (strings
        can also be one single string). The current color and text height and font
        specified in preferences are used.
        If screen is True, the text always faces the view direction.'''
        typecheck([(point,Vector)], "makeText")
        if not isinstance(stringslist,list): stringslist = [stringslist]
        textbuffer = []
        for l in stringslist: textbuffer.append(str(l).encode('latin1'))
        obj=FreeCAD.ActiveDocument.addObject("App::Annotation","Text")
        obj.LabelText=textbuffer
        obj.Position=point
        params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
        if not screen: obj.ViewObject.DisplayMode="World"
        h = params.GetFloat("textheight")
        if screen: h = h*10
        obj.ViewObject.FontSize = h
        obj.ViewObject.FontName = params.GetString("textfont")
        formatObject(obj)
        select(obj)
	return obj

def extrude(obj,vector):
        '''makeExtrusion(object,vector): extrudes the given object
        in the direction given by the vector. The original object
        gets hidden.'''
        newobj = FreeCAD.ActiveDocument.addObject("Part::Extrusion","Extrusion")
        newobj.Base = obj
        newobj.Dir = vector
        obj.ViewObject.Visibility = False
        formatObject(newobj,obj)
        FreeCAD.ActiveDocument.recompute()
        return newobj

def fuse(object1,object2):
        '''fuse(oject1,object2): returns an object made from
        the union of the 2 given objects. If the objects are
        coplanar, a special Draft Wire is used, otherwise we use
        a standard Part fuse.'''
        if fcgeo.isCoplanar(object1.Shape.fuse(object2.Shape).Faces):
                obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython","Fusion")
                Wire(obj)
                ViewProviderWire(obj.ViewObject)
        else:
                obj = FreeCAD.ActiveDocument.addObject("Part::Fuse","Fusion")
        obj.Base = object1
        obj.Tool = object2
        object1.ViewObject.Visibility = False
        object2.ViewObject.Visibility = False
        formatObject(obj,object1)
        FreeCAD.ActiveDocument.recompute()
        return obj

def cut(object1,object2):
        '''cut(oject1,object2): returns a cut object made from
        the difference of the 2 given objects.'''
        obj = FreeCAD.ActiveDocument.addObject("Part::Cut","Cut")
        obj.Base = object1
        obj.Tool = object2
        object1.ViewObject.Visibility = False
        object2.ViewObject.Visibility = False
        formatObject(obj,object1)
        FreeCAD.ActiveDocument.recompute()
        return obj

def move(objectslist,vector,copy=False):
        '''move(objects,vector,[copy]): Moves the objects contained
        in objects (that can be an object or a list of objects)
        in the direction and distance indicated by the given
        vector. If copy is True, the actual objects are not moved, but copies
        are created instead.he objects (or their copies) are returned.'''
        typecheck([(vector,Vector), (copy,bool)], "move")
        if not isinstance(objectslist,list): objectslist = [objectslist]
        newobjlist = []
        for obj in objectslist:
                if (obj.isDerivedFrom("Part::Feature")):
                        if copy:
                                newobj = FreeCAD.ActiveDocument.addObject("Part::Feature",getRealName(obj.Name))
                        else:
                                newobj = obj
                        sh = obj.Shape
                        sh.translate(vector)
                        newobj.Shape = sh
                elif (obj.Type == "App::Annotation"):
                        if copy:
                                newobj = FreeCAD.ActiveDocument.addObject("App::Annotation",getRealName(obj.Name))
                                newobj.LabelText = obj.LabelText
                        else:
                                newobj = obj
                        newobj.Position = obj.Position.add(vector)
                elif (obj.Type == "App::FeaturePython"):
                        if 'Dimline' in obj.PropertiesList:
                                if copy:
                                        newobj = FreeCAD.ActiveDocument.addObject("App::FeaturePython",getRealName(obj.Name))
                                        Dimension(newobj)
                                        DimensionViewProvider(newobj.ViewObject)
                                else:
                                        newobj = obj
                                newobj.Start = obj.Start.add(vector)
                                newobj.End = obj.End.add(vector)
                                newobj.Dimline = obj.Dimline.add(vector)
                else:
                        if "Placement" in obj.PropertiesList:
                                pla = obj.Placement
                                pla.move(vector)
                                obj.Placement = pla
		if copy: formatObject(newobj,obj)
                newobjlist.append(newobj)
        if len(newobjlist) == 1: return newobjlist[0]
        return newobjlist

def rotate(objectslist,angle,center=Vector(0,0,0),axis=Vector(0,0,1),copy=False):
        '''rotate(objects,angle,[center,axis,copy]): Rotates the objects contained
        in objects (that can be a list of objects or an object) of the given angle
        around the center, using axis as a rotation axis. If axis is omitted,
        the rotation will be around the vertical Z axis.
        If copy is True, the actual objects are not moved, but copies
        are created instead. The objects (or their copies) are returned.'''
        typecheck([(copy,bool)], "rotate")
        if not isinstance(objectslist,list): objectslist = [objectslist]
        newobjlist = []
        for obj in objectslist:
                if copy:
                        newobj = FreeCAD.ActiveDocument.addObject("Part::Feature",getRealName(obj.Name))
                else:
                        newobj = obj
                if (obj.isDerivedFrom("Part::Feature")):
                        shape = obj.Shape
                        shape.rotate(fcvec.tup(center), fcvec.tup(axis), angle)
                        newobj.Shape=shape
                if copy:
                        formatObject(newobj,obj)
                newobjlist.append(newobj)
        if len(newobjlist) == 1: return newobjlist[0]
        return newobjlist


def scale(objectslist,delta,center=Vector(0,0,0),copy=False):
        '''scale(objects,vector,[center,copy]): Scales the objects contained
        in objects (that can be a list of objects or an object) of the given scale
        factors defined by the given vector (in X, Y and Z directions) around
        given center. If copy is True, the actual objects are not moved, but copies
        are created instead. The objects (or their copies) are returned.'''
        if not isinstance(objectslist,list): objectslist = [objectslist]
        newobjlist = []
        for obj in objectslist:
                if copy:
                        newobj = FreeCAD.ActiveDocument.addObject("Part::Feature",getRealName(obj.Name))
                else:
                        newobj = shapify(obj)
                if (obj.isDerivedFrom("Part::Feature")):
                        sh = obj.Shape
                        m = FreeCAD.Matrix()
                        m.scale(delta)
                        sh = sh.transformGeometry(m)
                        corr = Vector(center.x,center.y,center.z)
                        corr.scale(delta.x,delta.y,delta.z)
                        corr = fcvec.neg(corr.sub(center))
                        sh.translate(corr)
                        newobj.Shape = sh
                elif (obj.Type == "App::Annotation"):
                        factor = delta.x * delta.y * delta.z * obj.ViewObject.FontSize
                        obj.ViewObject.Fontsize = factor
		if copy: formatObject(newobj,obj)
                newobjlist.append(newobj)
        if len(newobjlist) == 1: return newobjlist[0]
        return newobjlist

def draftify(objectslist):
        '''draftify(objectslist): turns each object of the given list
        (objectslist can also be a single object) into a Draft parametric
        wire'''
        if not isinstance(objectslist,list): objectslist = [objectslist]
        newobjlist = []
        for obj in objectslist:
                verts = []
                for v in obj.Shape.Vertexes:
                        verts.append(v.Point)
                newobj = makeWire(verts)
                if obj.Shape.Faces:
                        newobj.Closed = True
                        newobj.ViewObject.DisplayMode = "Flat Lines"
                else:
                        newobj.ViewObject.DisplayMode = "Wireframe"
                        if obj.Shape.Wires:
                                if obj.Shape.Wires[0].isClosed:
                                        newobj.Closed = True
                FreeCAD.ActiveDocument.removeObject(obj.Name)
                newobjlist.append(newobj)
        if len(newobjlist) == 1: return newobjlist[0]
        return newobjlist
				
#---------------------------------------------------------------------------
# Python Features definitions
#---------------------------------------------------------------------------

class ViewProviderDraft:
        "A generic View Provider for Draft objects"
        
	def __init__(self, obj):
                obj.addProperty("App::PropertyEnumeration","LineStyle",
                                "SVG Output","Line Style")
                obj.addProperty("App::PropertyEnumeration","FillStyle",
                                "SVG Output","Fill Style")
                obj.LineStyle = ['continuous','dashed','dashdotted','dotted']
                fills = ['shape color']
                for f in FreeCAD.svgpatterns.keys():
                        fills.append(f+' hatch')
                obj.FillStyle = fills
                obj.Proxy = self

	def attach(self, obj):
		return

	def updateData(self, fp, prop):
		return

	def getDisplayModes(self,obj):
		modes=[]
		return modes

	def setDisplayMode(self,mode):
		return mode

	def onChanged(self, vp, prop):
		return

        def __getstate__(self):
		return None

	def __setstate__(self,state):
		return None
		
class Dimension:
	"The Dimension object"
	def __init__(self, obj):
		obj.addProperty("App::PropertyVector","Start","Base",
					"Startpoint of dimension")
		obj.addProperty("App::PropertyVector","End","Base",
					"Endpoint of dimension")
		obj.addProperty("App::PropertyVector","Dimline","Base",
					"Point through which the dimension line passes")
		self.Type = "Dimension"
                obj.Start = FreeCAD.Vector(0,0,0)
                obj.End = FreeCAD.Vector(1,0,0)
                obj.Dimline = FreeCAD.Vector(0,1,0)
		obj.Proxy = self
		
	def onChanged(self, fp, prop):
		pass

	def execute(self, fp):
		pass

class ViewProviderDimension:
	"A View Provider for the Dimension object"
	def __init__(self, obj):
		prm = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
		obj.addProperty("App::PropertyLength","FontSize","Base","Font size")
		obj.addProperty("App::PropertyString","FontName","Base","Font Name")
		obj.addProperty("App::PropertyLength","LineWidth","Base","Line width")
		obj.addProperty("App::PropertyColor","LineColor","Base","Line color")
		obj.addProperty("App::PropertyLength","ExtLines","Base","Ext lines")
		obj.Proxy = self
		self.Object = obj.Object
                obj.FontSize=prm.GetFloat("textheight")
                obj.FontName=prm.GetString("textfont")
                obj.ExtLines=0.3
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
		if (angle >= math.pi/2) or (angle < -math.pi/2): angle = math.pi+angle
		offset = fcvec.rotate(FreeCAD.Vector(obj.ViewObject.FontSize*.2,0,0),
                                      angle+math.pi/2)
		tbase = midpoint.add(offset)
		if not proj: norm = Vector(0,0,1)
                else: norm = fcvec.neg(p3.sub(p2).cross(proj))
		return p1,p2,p3,p4,tbase,angle,norm

	def attach(self, obj):
		p1,p2,p3,p4,tbase,angle,norm = self.calcGeom(obj.Object)
		self.color = coin.SoBaseColor()
		self.color.rgb.setValue(obj.LineColor[0],
                                        obj.LineColor[1],
                                        obj.LineColor[2])
		self.font = coin.SoFont()
		self.text = coin.SoAsciiText()
		self.text.justification = coin.SoAsciiText.CENTER
		self.text.string = ("%.2f" % p3.sub(p2).Length)
		self.textpos = coin.SoTransform()
		self.textpos.translation.setValue([tbase.x,tbase.y,tbase.z])
                tm = fcvec.getPlaneRotation(p3.sub(p2),norm)
                rm = coin.SbRotation()
                self.textpos.rotation = rm
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
		selnode=coin.SoType.fromName("SoFCSelection").createInstance()
		selnode.documentName.setValue(FreeCAD.ActiveDocument.Name)
		selnode.objectName.setValue(obj.Object.Name)
		selnode.subElementName.setValue("Line")
		selnode.addChild(line)
		self.node = coin.SoGroup()
		self.node.addChild(self.color)
		self.node.addChild(self.drawstyle)
		self.node.addChild(self.coords)
		self.node.addChild(selnode)
		self.node.addChild(marks)
		self.node.addChild(label)
		obj.addDisplayMode(self.node,"Wireframe")
		self.onChanged(obj,"FontSize")
		self.onChanged(obj,"FontName")

	def updateData(self, obj, prop):
		p1,p2,p3,p4,tbase,angle,norm = self.calcGeom(obj)
		self.text.string = ("%.2f" % p3.sub(p2).Length)
		# self.textpos.rotation.setValue(coin.SbVec3f(norm.x,norm.y,norm.z),angle)
                u = p3.sub(p2)
                v = p2.sub(p1)
                u.normalize()
                v.normalize()
                u = fcvec.reorient(u,"x")
                v = fcvec.reorient(v,"y")
                w = fcvec.reorient(u.cross(v),"z")
                print u,v,w
                tm = FreeCAD.Placement(fcvec.getPlaneRotation(u,v,w)).Rotation.Q
                self.textpos.rotation = coin.SbRotation(tm[0],tm[1],tm[2],tm[3])
		self.coords.point.setValues(0,4,[[p1.x,p1.y,p1.z],
                                                 [p2.x,p2.y,p2.z],
                                                 [p3.x,p3.y,p3.z],
                                                 [p4.x,p4.y,p4.z]])
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
                        static char * dim_xpm[] = {
                        "16 16 4 1",
                        " 	c None",
                        ".	c #000000",
                        "+	c #FFFF00",
                        "@	c #FFFFFF",
                        "                ",
                        "                ",
                        "     .    .     ",
                        "    ..    ..    ",
                        "   .+.    .+.   ",
                        "  .++.    .++.  ",
                        " .+++. .. .+++. ",
                        ".++++. .. .++++.",
                        " .+++. .. .+++. ",
                        "  .++.    .++.  ",
                        "   .+.    .+.   ",
                        "    ..    ..    ",
                        "     .    .     ",
                        "                ",
                        "                ",
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

	def getSVG(self,modifier=100):
		"returns an svg representation of the dimension"
		p1,p2,p3,p4,tbase,angle,norm = self.calcGeom(self.Object)
		svg='<g id="'+self.Object.Name+'"><path '
		svg+='d="M '+str(p1.x)+' '+str(p1.y)+' '
		svg+='L '+str(p2.x)+' '+str(p2.y)+' '
		svg+='L '+str(p3.x)+' '+str(p3.y)+' '
		svg+='L '+str(p4.x)+' '+str(p4.y)+'" '
		svg+='fill="none" stroke="'
		svg+=self.getrgb(self.ViewObject.LineColor) + '" '
		svg+='stroke-width="' + str(self.ViewObject.LineWidth/modifier) + ' px" '
		svg+='style="stroke-width:'+ str(self.ViewObject.LineWidth/modifier)
		svg+=';stroke-miterlimit:4;stroke-dasharray:none"/>\n'
		svg+='<circle cx="'+str(p2.x)+'" cy="'+str(p2.y)
		svg+='" r="'+str(self.ViewObject.FontSize/(modifier/10))+'" '
		svg+='fill="'+ self.getrgb(self.ViewObject.LineColor) +'" stroke="none" '
		svg+='style="stroke-miterlimit:4;stroke-dasharray:none"/>\n'
		svg+='<circle cx="'+str(p3.x)+'" cy="'+str(p3.y)
		svg+='" r="'+str(self.ViewObject.FontSize/(modifier/10))+'" '
		svg+='fill="#000000" stroke="none" '
		svg+='style="stroke-miterlimit:4;stroke-dasharray:none"/>\n'
		svg+='<text id="' + self.Object.Name + '" fill="'
		svg+=str(self.ViewObject.LineWidth/modifier) +'" font-size="'
		svg+=str(self.ViewObject.FontSize*(modifier/5))+'" '
		svg+='style="text-anchor:middle;text-align:center" '
		svg+='transform="rotate('+str(math.degrees(angle))
		svg+=','+ str(tbase.x) + ',' + str(tbase.y) + ') '
		svg+='translate(' + str(tbase.x) + ',' + str(tbase.y) + ') '
                svg +='scale('+str(modifier/2000)+','+str(modifier/2000)+')">\n'
		svg+="%.2f" % p3.sub(p2).Length
		svg+='</text>\n</g>\n'
		return svg

class Rectangle:
        "The Rectangle object"
        
	def __init__(self, obj):
		obj.addProperty("App::PropertyDistance","Length","Base","Length of the rectangle")
		obj.addProperty("App::PropertyDistance","Height","Base","Height of the rectange")
		obj.Proxy = self
                obj.Length=1
                obj.Height=1

	def execute(self, fp):
                self.createGeometry(fp)

        def onChanged(self, fp, prop):
                if prop in ["Length","Height"]:
                        self.createGeometry(fp)
                        
        def createGeometry(self,fp):
                plm = fp.Placement
                p1 = Vector(0,0,0)
                p2 = Vector(p1.x+fp.Length,p1.y,p1.z)
		p3 = Vector(p1.x+fp.Length,p1.y+fp.Height,p1.z)
                p4 = Vector(p1.x,p1.y+fp.Height,p1.z)
		shape = Part.makePolygon([p1,p2,p3,p4,p1])
                shape = Part.Face(shape)
		fp.Shape = shape
                fp.Placement = plm

class ViewProviderRectangle(ViewProviderDraft):
        "A View Provider for the Rectangle object"

	def getIcon(self):
		return """
                        /* XPM */
                        static char * rec_xpm[] = {
                        "16 16 5 1",
                        " 	c None",
                        ".	c #000000",
                        "+	c #0000FF",
                        "@	c #141010",
                        "#	c #615BD2",
                        "        @@@@@@@@",
                        "        @##@@#@@",
                        "  ......@##@@##@",
                        "  .+++++@##@@##@",
                        "  .+++++@######@",
                        "  .+++++@##@@##@",
                        "  .+++++@##@@##@",
                        "  .+++++@######@",
                        "  .+++++@@@@@@@@"
                        "  .++++++++++.  ",
                        "  .++++++++++.  ",
                        "  .++++++++++.  ",
                        "  .++++++++++.  ",
                        "  ............  ",
                        "                ",
                        "                "};
			"""

class Circle:
        "The Circle object"
        
	def __init__(self, obj):
		obj.addProperty("App::PropertyDistance","Radius","Base","Radius of the circle")
                obj.addProperty("App::PropertyAngle","StartAngle","Arc","Start angle of the arc")
                obj.addProperty("App::PropertyAngle","EndAngle","Arc",
                                "End angle of the arc (for a full circle, give it same value as Start Angle)")
		obj.Proxy = self
                obj.Radius = 1
                obj.StartAngle = obj.EndAngle = 0

	def execute(self, fp):
                self.createGeometry(fp)

        def onChanged(self, fp, prop):
                if prop in ["Radius","StartAngle","EndAngle"]:
                        self.createGeometry(fp)
                        
        def createGeometry(self,fp):
                plm = fp.Placement
                if fp.StartAngle == fp.EndAngle:
                        shape = Part.Circle(Vector(0,0,0),Vector(0,0,1),fp.Radius).toShape()
                        shape = Part.Wire(shape)
                        shape = Part.Face(shape)
                else:
                        shape = Part.makeCircle(fp.Radius,Vector(0,0,0),
                                                Vector(0,0,1),fp.StartAngle,fp.EndAngle)
		fp.Shape = shape
                fp.Placement = plm

class ViewProviderCircle(ViewProviderDraft):
        "A View Provider for the Circle object"

	def getIcon(self):
		return """
                        /* XPM */
                        static char * circ_xpm[] = {
                        "16 16 5 1",
                        " 	c None",
                        ".	c #000000",
                        "+	c #0000FF",
                        "@	c #141010",
                        "#	c #615BD2",
                        "        @@@@@@@@",
                        "      ..@##@@#@@",
                        "    ..++@##@@##@",
                        "   .++++@##@@##@",
                        "  .+++++@######@",
                        "  .+++++@##@@##@",
                        " .++++++@##@@##@",
                        " .++++++@######@",
                        " .++++++@@@@@@@@",
                        " .++++++++++++. ",
                        "  .++++++++++.  ",
                        "  .++++++++++.  ",
                        "   .++++++++.   ",
                        "    ..++++..    ",
                        "      ....      ",
                        "                "};
			"""

class Wire:
        "The Wire object"
        
	def __init__(self, obj):
		obj.addProperty("App::PropertyVectorList","Points","Base",
                                "The vertices of the wire")
                obj.addProperty("App::PropertyBool","Closed","Base",
                                "If the wire is closed or not")
                obj.addProperty("App::PropertyLink","Base","Base",
                                "The base object is the wire is formed from 2 objects")
                obj.addProperty("App::PropertyLink","Tool","Base",
                                "The tool object is the wire is formed from 2 objects")
		obj.Proxy = self
                obj.Closed = False

	def execute(self, fp):
                self.createGeometry(fp)

        def onChanged(self, fp, prop):
                if prop in ["Points","Closed","Base","Tool"]:
                        self.createGeometry(fp)
                        
        def createGeometry(self,fp):
                plm = fp.Placement
                if fp.Base and fp.Tool:
                        if ('Shape' in fp.Base.PropertiesList) and ('Shape' in fp.Tool.PropertiesList):
                                sh1 = fp.Base.Shape
                                sh2 = fp.Tool.Shape
                                shape = sh1.fuse(sh2)
                                if fcgeo.isCoplanar(shape.Faces):
                                        shape = fcgeo.concatenate(shape)
                                        fp.Shape = shape
                elif fp.Points:
                        if fp.Points[0] == fp.Points[-1]:
                                fp.Closed = True
                                fp.Points.pop()
                        if fp.Closed and (len(fp.Points) > 2):
                                shape = Part.makePolygon(fp.Points+[fp.Points[0]])
                                shape = Part.Face(shape)
                        else:
                                edges = []
                                pts = fp.Points[1:]
                                lp = fp.Points[0]
                                for p in pts:
                                        edges.append(Part.Line(lp,p).toShape())
                                        lp = p
                                shape = Part.Wire(edges)
                        fp.Shape = shape
                fp.Placement = plm

class ViewProviderWire(ViewProviderDraft):
        "A View Provider for the Wire object"

	def getIcon(self):
		return """
                       /* XPM */
                       static char * wire_xpm[] = {
                       "16 16 5 1",
                       " 	c None",
                       ".	c #000000",
                       "+	c #0000FF",
                       "@	c #141010",
                       "#	c #615BD2",
                       "        @@@@@@@@",
                       "        @##@@#@@",
                       "        @##@@##@",
                       "        @##@@##@",
                       "    ....@######@",
                       "....++++@##@@##@",
                       ".+++++++@##@@##@",
                       " .++++++@######@",
                       " .++++++@@@@@@@@"
                       "  .++++++++++.  ",
                       "  .+++++++++++. ",
                       "   .++++++++++..",
                       "   .++++++++..  ",
                       "    .+++++..    ",
                       "    .+++..      ",
                       "     ...        "};
		       """
