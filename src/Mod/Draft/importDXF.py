
# -*- coding: utf8 -*-

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

__title__="FreeCAD Draft Workbench - DXF importer/exporter"
__author__ = "Yorik van Havre <yorik@gmx.fr>"
__url__ = ["http://yorik.orgfree.com","http://free-cad.sourceforge.net"]

'''
This script uses a DXF-parsing library created by Stani,
Kitsu and Migius for Blender

imports:
line, polylines, lwpolylines, arcs, circles, texts,
mtexts, layers (as groups), colors

exports:
lines, polylines, lwpolylines, circles, arcs,
texts, colors,layers (from groups)
'''

import FreeCAD, os, Part, math, re, string, Mesh, Draft
from draftlibs import fcvec, dxfColorMap, dxfLibrary, fcgeo
from draftlibs.dxfReader import readDXF
from Draft import Dimension, ViewProviderDimension
from FreeCAD import Vector

try: import FreeCADGui
except: gui = False
else: gui = True
try: draftui = FreeCADGui.draftToolBar
except: draftui = None

pythonopen = open # to distinguish python built-in open function from the one declared here

def prec():
    "returns the current Draft precision level"
    return Draft.getParam("precision")

def decodeName(name):
    "decodes encoded strings"
    try:
        decodedName = (name.decode("utf8"))
    except UnicodeDecodeError:
        try:
            decodedName = (name.decode("latin1"))
        except UnicodeDecodeError:
                print "dxf: error: couldn't determine character encoding"
                decodedName = name
    return decodedName

def locateLayer(wantedLayer):
    "returns layer group and creates it if needed"
    wantedLayerName = decodeName(wantedLayer)
    for l in layers:
        if wantedLayerName==l.Label:
            return l
    newLayer = doc.addObject("App::DocumentObjectGroup",wantedLayer)
    newLayer.Label = wantedLayerName
    layers.append(newLayer)
    return newLayer

def calcBulge(v1,bulge,v2):
    '''
    calculates intermediary vertex for curved segments.
    algorithm from http://www.afralisp.net/lisp/Bulges1.htm
    '''
    chord = v2.sub(v1)
    sagitta = (bulge * chord.Length)/2
    startpoint = v1.add(fcvec.scale(chord,0.5))
    perp = chord.cross(Vector(0,0,1))
    if not fcvec.isNull(perp): perp.normalize()
    endpoint = fcvec.scale(perp,sagitta)
    return startpoint.add(endpoint)

def getGroup(ob,exportList):
    "checks if the object is part of a group"
    for i in exportList:
        if (i.Type == "App::DocumentObjectGroup"):
            for j in i.Group:
                if (j == ob):
                    return i.Label
    return "0"

def getACI(ob,text=False):
    "gets the ACI color closest to the objects color"
    if not gui: return 0
    else:
        if text:
            col=ob.ViewObject.TextColor
        else:
            col=ob.ViewObject.LineColor
        aci=[0,442]
        for i in range (255,-1,-1):
            ref=dxfColorMap.color_map[i]
            dist=((ref[0]-col[0])**2 + (ref[1]-col[1])**2 + (ref[2]-col[2])**2)
            if (dist <= aci[1]): aci=[i,dist]
        return aci[0]

def rawValue(entity,code):
    "returns the  value of a DXF code in an entity section"
    value = None
    for pair in entity.data:
        if pair[0] == code:
            value = pair[1]
    return value

class fcformat:
    "this contains everything related to color/lineweight formatting"
    def __init__(self,drawing):
        self.dxf = drawing
        params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
        self.paramtext = params.GetBool("dxftext")
        self.paramstarblocks = params.GetBool("dxfstarblocks")
        self.dxflayout = params.GetBool("dxflayouts")
        self.paramstyle = params.GetInt("dxfstyle")
        self.join = params.GetBool("joingeometry")
        self.makeBlocks = params.GetBool("groupLayers")
        bparams = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/View")

        if self.paramstyle > 1:
            # checking if FreeCAD background is dark or bright
            if bparams.GetBool("Gradient"):
                c1 = bparams.GetUnsigned("BackgroundColor2")
                c2 = bparams.GetUnsigned("BackgroundColor3")
                r1 = float((c1>>24)&0xFF)
                g1 = float((c1>>16)&0xFF)
                b1 = float((c1>>8)&0xFF)
                r2 = float((c2>>24)&0xFF)
                g2 = float((c2>>16)&0xFF)
                b2 = float((c2>>8)&0xFF)
                v1 = FreeCAD.Vector(r1,g1,b1)
                v2 = FreeCAD.Vector(r2,g2,b2)
                v = v2.sub(v1)
                v = fcvec.scale(v,0.5)
                cv = v1.add(v)
            else:
                c1 = bparams.GetUnsigned("BackgroundColor")
                r1 = float((c1>>24)&0xFF)
                g1 = float((c1>>16)&0xFF)
                b1 = float((c1>>8)&0xFF)
                cv = FreeCAD.Vector(r1,g1,b1)
            value = cv.x*.3 + cv.y*.59 + cv.z*.11
            if value < 128: self.brightbg = False
            else:
                self.brightbg = True
	
        if gui and draftui:
            r = float(draftui.color.red()/255.0)
            g = float(draftui.color.green()/255.0)
            b = float(draftui.color.blue()/255.0)
            self.lw = float(draftui.linewidth)
        else:
            self.lw = float(params.GetInt("linewidth"))
            c = params.GetUnsigned("color")
            r = float(((c>>24)&0xFF)/255)
            g = float(((c>>16)&0xFF)/255)
            b = float(((c>>8)&0xFF)/255)
        self.col = (r,g,b,0.0)

        if self.paramstyle == 3:
            parammappingfile = params.GetString("dxfmappingfile")
            self.table = self.buildTable(parammappingfile)
		
    def buildTable(self,tablefile):
        "builds a table for converting colors into linewidths"
        try: f = pythonopen(tablefile)
        except ValueError:
			print "error: ",tablefile, " not found"
			return None
        table = {}
        header = len(f.readline().split("\t"))
        if header == 15:
            for l in f:
                s = l.split("\t")
                if "Color_" in s[0]:
                    index = int(s[0].split("_")[1])
                    if s[1] == "(Object)": color = "object"
                    else:
                        c = s[2].split(",")
                        color = [float(c[0])/255,float(c[1])/255,float(c[2])/255]
                        if (color == [0.0,0.0,0.0]) and (not self.brightbg):
                            color = [1.0,1.0,1.0]
                    if s[2] == "(Object)": width = "object"
                    else: width = float(s[10])*10
                    table[index]=[color,width]
        elif header == 3:
            for l in f:
                s = l.split("\t")
                index = int(s[0])
                c = string.replace(s[1],'"','')
                c = c.split(",")
                color = [float(c[0])/255,float(c[1])/255,float(c[2])/255]
                width = float(s[2])
                table[index]=[color,width]
            for i in range(256):
                if not i in table.keys():
                    table[i]=["object","object"]
        else:
            print "error building mapping table: file format not recognized"
            table = None
        print table
        return table
		
    def formatObject(self,obj,dxfobj,textmode=False):
        "applies color and linetype to objects"
        if self.paramstyle == 1:
            if textmode:
                obj.ViewObject.TextColor = (0.0,0.0,0.0)
            else:
                obj.ViewObject.LineColor = self.col
                obj.ViewObject.LineWidth = self.lw	
        elif self.paramstyle == 2:
            if textmode:
                if dxfobj.color_index == 256: cm = self.getGroupColor(dxfobj)[:3]
                else: cm = dxfColorMap.color_map[dxfobj.color_index]
                obj.ViewObject.TextColor = (cm[0],cm[1],cm[2])
            else:
                if dxfobj.color_index == 256: cm = self.getGroupColor(dxfobj)
                elif (dxfobj.color_index == 7) and self.brightbg: cm = [0.0,0.0,0.0]
                else: cm = dxfColorMap.color_map[dxfobj.color_index]
                obj.ViewObject.LineColor = (cm[0],cm[1],cm[2],0.0)
                obj.ViewObject.LineWidth = self.lw
        elif self.paramstyle == 3:
            if textmode:
                cm = table[dxfobj.color_index][0]
                wm = table[dxfobj.color_index][1]
                obj.ViewObject.TextColor = (cm[0],cm[1],cm[2])
            else:
                if dxfobj.color_index == 256:
                    cm = self.table[self.getGroupColor(dxfobj,index=True)][0]
                    wm = self.table[self.getGroupColor(dxfobj,index=True)][1]
                else:
                    cm = self.table[dxfobj.color_index][0]
                    wm = self.table[dxfobj.color_index][1]
                if cm == "object": cm = self.getGroupColor(dxfobj)
                else: obj.ViewObject.LineColor = (cm[0],cm[1],cm[2],0.0)
                if wm == "object": wm = self.lw
                else: obj.ViewObject.LineWidth = wm

    def getGroupColor(self,dxfobj,index=False):
        "get color of bylayer stuff"
        name = dxfobj.layer
        for table in self.dxf.tables.get_type("table"):
            if table.name == "layer":
                for l in table.get_type("layer"):
                    if l.name == name:
                        if index: return l.color
                        else:
                            if (l.color == 7) and self.brightbg: return [0.0,0.0,0.0]
                            else: return dxfColorMap.color_map[l.color]

def vec(pt):
    "returns a rounded Vector from a dxf point"
    return FreeCAD.Vector(round(pt[0],prec()),round(pt[1],prec()),round(pt[2],prec()))

def drawLine(line,shapemode=False):
    "returns a Part shape from a dxf line"
    if (len(line.points) > 1):
        v1=vec(line.points[0])
        v2=vec(line.points[1])
        if not fcvec.equals(v1,v2):
            try:
                if (fmt.paramstyle == 4) and (not fmt.makeBlocks) and (not shapemode):
                    return Draft.makeWire([v1,v2])
                else:
                    return Part.Line(v1,v2).toShape()
            except:
                warn(line)
    return None

def drawPolyline(polyline,shapemode=False):
    "returns a Part shape from a dxf polyline"
    if (len(polyline.points) > 1):
        edges = []
        curves = False
        verts = []
        for p in range(len(polyline.points)-1):
            p1 = polyline.points[p]
            p2 = polyline.points[p+1]
            v1 = vec(p1)
            v2 = vec(p2)
            verts.append(v1)
            if not fcvec.equals(v1,v2):
                if polyline.points[p].bulge:
                    curves = True
                    cv = calcBulge(v1,polyline.points[p].bulge,v2)
                    if fcvec.isColinear([v1,cv,v2]):
                        try: edges.append(Part.Line(v1,v2).toShape())
                        except: warn(polyline)
                    else:
                        try: edges.append(Part.Arc(v1,cv,v2).toShape())
                        except: warn(polyline)
                else:
                    try: edges.append(Part.Line(v1,v2).toShape())
                    except: warn(polyline)
        verts.append(v2)
        if polyline.closed:
            p1 = polyline.points[len(polyline.points)-1]
            p2 = polyline.points[0]
            v1 = vec(p1)
            v2 = vec(p2)
            cv = calcBulge(v1,polyline.points[-1].bulge,v2)
            if not fcvec.equals(v1,v2):
                if fcvec.isColinear([v1,cv,v2]):
                    try: edges.append(Part.Line(v1,v2).toShape())
                    except: warn(polyline)
                else:
                    try: edges.append(Part.Arc(v1,cv,v2).toShape())
                    except: warn(polyline)
        if edges:
            try:
                if (fmt.paramstyle == 4) and (not curves) and (not fmt.makeBlocks) and (not shapemode):
                    ob = Draft.makeWire(verts)
                    ob.Closed = polyline.closed
                    return ob
                else:
                    if polyline.closed:
                        w = Part.Wire(edges)
                        return(Part.Face(w))
                    else:
                        return Part.Wire(edges)                          
            except:
                warn(polyline)
    return None

def drawArc(arc,shapemode=False):
    "returns a Part shape from a dxf arc"
    v=vec(arc.loc)
    firstangle=round(arc.start_angle,prec())
    lastangle=round(arc.end_angle,prec())
    circle=Part.Circle()
    circle.Center=v
    circle.Radius=round(arc.radius,prec())
    try:
        if (fmt.paramstyle == 4) and (not fmt.makeBlocks) and (not shapemode):
            pl = FreeCAD.Placement()
            pl.move(v)
            return Draft.makeCircle(arc.radius,pl,False,firstangle,lastangle)
        else:
            return circle.toShape(math.radians(firstangle),math.radians(lastangle))
    except:
        warn(arc)
    return None

def drawCircle(circle,shapemode=False):
    "returns a Part shape from a dxf circle"
    v = vec(circle.loc)
    curve = Part.Circle()
    curve.Radius = round(circle.radius,prec())
    curve.Center = v
    try:
        if (fmt.paramstyle == 4) and (not fmt.makeBlocks) and (not shapemode):
            pl = FreeCAD.Placement()
            pl.move(v)
            return Draft.makeCircle(circle.radius,pl)
        else:
            return curve.toShape()
    except:
        warn(circle)
    return None

def drawFace(face):
    "returns a Part face from a list of points"
    pl = []
    for p in face.points:
        pl.append(vec(p))
    p1 = face.points[0]
    pl.append(vec(p1))
    try:
        pol = Part.makePolygon(pl)
        return Part.Face(pol)
    except:
        warn(face)
    return None

def drawMesh(mesh):
    "returns a Mesh from a dxf mesh"
    md = []
    if mesh.flags == 16:
        pts = mesh.points
        udim = rawValue(mesh,71)
        vdim = rawValue(mesh,72)
        for u in range(udim-1):
            for v in range(vdim-1):
                b = u+v*udim
                p1 = pts[b]
                p2 = pts[b+1]
                p3 = pts[b+udim]
                p4 = pts[b+udim+1]
                md.append([p1,p2,p4])
                md.append([p1,p4,p3])
    elif mesh.flags == 64:
        pts = []
        fcs = []
        for p in mesh.points:
            if p.flags == 192:
                pts.append(p)
            elif p.flags == 128:
                fcs.append(p)
        for f in fcs:
            p1 = pts[rawValue(f,71)-1]
            p2 = pts[rawValue(f,72)-1]
            p3 = pts[rawValue(f,73)-1]
            md.append([p1,p2,p3])
            if rawValue(f,74) != None:
                p4 = pts[rawValue(f,74)-1]
                md.append([p1,p3,p4])                                
    try:
        return Mesh.Mesh(md)
    except:
        warn(mesh)
    return None     

def drawSolid(solid):
    "returns a Part shape from a dxf solid"
    p4 = None
    p1x = rawValue(solid,10)
    p1y = rawValue(solid,20)
    p1z = rawValue(solid,30) or 0
    p2x = rawValue(solid,11)
    p2y = rawValue(solid,21)
    p2z = rawValue(solid,31) or p1z
    p3x = rawValue(solid,12)
    p3y = rawValue(solid,22)
    p3z = rawValue(solid,32) or p1z
    p4x = rawValue(solid,13)
    p4y = rawValue(solid,23)
    p4z = rawValue(solid,33) or p1z
    p1 = FreeCAD.Vector(p1x,p1y,p1z)
    p2 = FreeCAD.Vector(p2x,p2y,p2z)
    p3 = FreeCAD.Vector(p3x,p3y,p3z)
    if p4x != None: p4 = FreeCAD.Vector(p4x,p4y,p4z)
    if p4 and (p4 != p3) and (p4 != p2) and (p4 != p1):
        try:
            return Part.Face(Part.makePolygon([p1,p2,p3,p4,p1]))
        except:
            warn(solid)
    else:
        try:
            return Part.Face(Part.makePolygon([p1,p2,p3,p1]))
        except:
            warn(solid)
    return None

def drawSpline(spline,shapemode=False):
    "returns a Part Shape from a dxf spline"
    flag = rawValue(spline,70)
    if flag == 1:
        closed = True
    else:
        closed = False
    verts = []
    knots = []
    for dline in spline.data:
        if dline[0] == 10:
            cp = [dline[1]]
        elif dline[0] == 20:
            cp.append(dline[1])
        elif dline[0] == 30:
            cp.append(dline[1])
            pt = Vector(cp[0],cp[1],cp[2])
            if verts:
                if pt != verts[-1]:
                    verts.append(pt)
            else:
                verts.append(pt)
        elif dline[0] == 40:
            knots.append(dline[1])
    try:
        if (fmt.paramstyle == 4) and (not fmt.makeBlocks) and (not shapemode):
            ob = Draft.makeSpline(verts)
            ob.Closed = closed
            return ob
        else:
            sp = Part.BSplineCurve()
            print knots
            sp.interpolate(verts)
            sh = Part.Wire(sp.toShape())
            if closed:
                return Part.Face(sh)
            else:
                return sh                          
    except:
        warn(spline)
    return None
    
def drawBlock(blockref):
    "returns a shape from a dxf block reference"
    shapes = []
    for line in blockref.entities.get_type('line'):
        s = drawLine(line,shapemode=True)
        if s: shapes.append(s)
    for polyline in blockref.entities.get_type('polyline'):
        s = drawPolyline(polyline,shapemode=True)
        if s: shapes.append(s)
    for polyline in blockref.entities.get_type('lwpolyline'):
        s = drawPolyline(polyline,shapemode=True)
        if s: shapes.append(s)
    for arc in blockref.entities.get_type('arc'):
        s = drawArc(arc,shapemode=True)
        if s: shapes.append(s)
    for circle in blockref.entities.get_type('circle'):
        s = drawCircle(circle,shapemode=True)
        if s: shapes.append(s)
    for insert in blockref.entities.get_type('insert'):
        s = drawInsert(insert)
        if s: shapes.append(s)
    for solid in blockref.entities.get_type('solid'):
        s = drawSolid(solid)
        if s: shapes.append(s)
    for spline in blockref.entities.get_type('spline'):
        s = drawSpline(spline,shapemode=True)
        if s: shapes.append(s)
    for text in blockref.entities.get_type('text'):
        if fmt.dxflayout or (not rawValue(text,67)):
            addText(text)
    for text in blockref.entities.get_type('mtext'):
        if fmt.dxflayout or (not rawValue(text,67)):
            addText(text)
    try: shape = Part.makeCompound(shapes)
    except: warn(blockref)
    if shape:
        blockshapes[blockref.name]=shape
        return shape
    return None

def drawInsert(insert):
    if blockshapes.has_key(insert):
        shape = blockshapes[insert.block]
    else:
        shape = None
        for b in drawing.blocks.data:
            if b.name == insert.block:
                shape = drawBlock(b)
    if fmt.paramtext:
        attrs = attribs(insert)
        for a in attrs:
            addText(a,attrib=True)
    if shape:
        pos = vec(insert.loc)
        rot = math.radians(insert.rotation)
        scale = insert.scale
        tsf = FreeCAD.Matrix()
        tsf.scale(vec(scale))
        tsf.rotateZ(rot)
        shape = shape.transformGeometry(tsf)
        shape.translate(pos)
        return shape
    return None

def drawLayerBlock(shapeslist):
    "draws a compound with the given shapes"
    shape = None
    try:
        shape = Part.makeCompound(shapeslist)
    except:
        pass
    return shape

def attribs(insert):
    "checks if an insert has attributes, and returns the values if yes"
    atts = []
    if rawValue(insert,66) != 1: return []
    index = None
    for i in range(len(drawing.entities.data)):
        if drawing.entities.data[i] == insert:
            index = i
            break
    if index == None: return []
    j = index+1
    while True:
        ent = drawing.entities.data[j]
        print str(ent)
        if str(ent) == 'seqend':
            return atts
        elif str(ent) == 'attrib':
            atts.append(ent)
            j += 1

def addObject(shape,name="Shape",layer=None):
    "adds a new object to the document with passed arguments"
    if isinstance(shape,Part.Shape):
        newob=doc.addObject("Part::Feature",name)
        newob.Shape = shape
    else:
        newob = shape
    if layer:
        lay=locateLayer(layer)
        lay.addObject(newob)
    return newob

def addText(text,attrib=False):
    "adds a new text to the document"
    if attrib:
        lay = locateLayer(rawValue(text,8))
        val = rawValue(text,1)
        pos = FreeCAD.Vector(rawValue(text,10),rawValue(text,20),rawValue(text,30))
        hgt = rawValue(text,40)
    else:
        lay = locateLayer(text.layer)
        val = text.value
        pos = FreeCAD.Vector(text.loc[0],text.loc[1],text.loc[2])
        hgt = text.height
    if val:
        newob=doc.addObject("App::Annotation","Text")
        lay.addObject(newob)
        val = re.sub('{([^!}]([^}]|\n)*)}', '', val)
        val = re.sub('%%d','°',val)
        val = re.sub('%%c','Ø',val)
        val = val.decode("Latin1").encode("Latin1")
        newob.LabelText = val
        newob.Position = pos
        if gui:
            newob.ViewObject.FontSize=float(hgt)
            newob.ViewObject.DisplayMode = "World"
            fmt.formatObject(newob,text,textmode=True)

def addToBlock(shape,layer):
    "adds given shape to the layer dict"
    if layer in layerBlocks:
        layerBlocks[layer].append(shape)
    else:
        layerBlocks[layer] = [shape]

def processdxf(document,filename):
    "this does the translation of the dxf contents into FreeCAD Part objects"
    global drawing # for debugging - so drawing is still accessible to python after the script
    FreeCAD.Console.PrintMessage("opening "+filename+"...\n")
    drawing = readDXF(filename)
    global layers
    layers = []
    global doc
    doc = document
    global blockshapes
    blockshapes = {}
    global badobjects
    badobjects = []
    global layerBlocks
    layerBlocks = {}
    
    # getting config parameters
    
    global fmt
    fmt = fcformat(drawing)
    shapes = []

    # drawing lines

    lines = drawing.entities.get_type("line")
    if lines: FreeCAD.Console.PrintMessage("drawing "+str(len(lines))+" lines...\n")
    for line in lines:
        if fmt.dxflayout or (not rawValue(line,67)):
            shape = drawLine(line)
            if shape:
                if fmt.join:
                    if isinstance(shape,Part.Shape):
                        shapes.append(shape)
                    else:                                        
                        shapes.append(shape.Shape)
                elif fmt.makeBlocks:
                    if isinstance(shape,Part.Shape):
                        addToBlock(shape,line.layer)
                    else:                                        
                        addToBlock(shape.Shape,line.layer)       
                else:
                    newob = addObject(shape,"Line",line.layer)
                    if gui: fmt.formatObject(newob,line)
						
    # drawing polylines

    pls = drawing.entities.get_type("lwpolyline")
    pls.extend(drawing.entities.get_type("polyline"))
    polylines = []
    meshes = []
    for p in pls:
        if hasattr(p,"flags"):
            if p.flags in [16,64]:
                meshes.append(p)
            else:
                polylines.append(p)
        else:
            polylines.append(p)
    if polylines: FreeCAD.Console.PrintMessage("drawing "+str(len(polylines))+" polylines...\n")
    for polyline in polylines:
        if fmt.dxflayout or (not rawValue(polyline,67)):
            shape = drawPolyline(polyline)
            if shape:
                if fmt.join:
                    if isinstance(shape,Part.Shape):
                        shapes.append(shape)
                    else:
                        shapes.append(shape.Shape)
                elif fmt.makeBlocks:
                    if isinstance(shape,Part.Shape):
                        addToBlock(shape,polyline.layer)
                    else:                                        
                        addToBlock(shape.Shape,polyline.layer)              
                else:
                    newob = addObject(shape,"Polyline",polyline.layer)
                    if gui: fmt.formatObject(newob,polyline)
				
    # drawing arcs

    arcs = drawing.entities.get_type("arc")
    if arcs: FreeCAD.Console.PrintMessage("drawing "+str(len(arcs))+" arcs...\n")
    for arc in arcs:
        if fmt.dxflayout or (not rawValue(arc,67)):
            shape = drawArc(arc)
            if shape:
                if fmt.join:
                    if isinstance(shape,Part.Shape):
                        shapes.append(shape)
                    else:
                        shapes.append(shape.Shape)
                elif fmt.makeBlocks:
                    if isinstance(shape,Part.Shape):
                        addToBlock(shape,arc.layer)
                    else:                                        
                        addToBlock(shape.Shape,arc.layer)              
                else:
                    newob = addObject(shape,"Arc",arc.layer)
                    if gui: fmt.formatObject(newob,arc)

    # joining lines, polylines and arcs if needed

    if fmt.join and shapes:
        FreeCAD.Console.PrintMessage("Joining geometry...\n")
        edges = []
        for s in shapes:
            edges.extend(s.Edges)
        shapes = fcgeo.findWires(edges)
        for s in shapes:
            newob = addObject(s)

    # drawing circles

    circles = drawing.entities.get_type("circle")
    if circles: FreeCAD.Console.PrintMessage("drawing "+str(len(circles))+" circles...\n")
    for circle in circles:
        if fmt.dxflayout or (not rawValue(circle,67)):
            shape = drawCircle(circle)
            if shape:
                if fmt.makeBlocks:
                    if isinstance(shape,Part.Shape):
                        addToBlock(shape,circle.layer)
                    else:                                        
                        addToBlock(shape.Shape,circle.layer)
                else:
                    newob = addObject(shape,"Circle",circle.layer)
                    if gui: fmt.formatObject(newob,circle)

    # drawing solids

    solids = drawing.entities.get_type("solid")
    if solids: FreeCAD.Console.PrintMessage("drawing "+str(len(circles))+" solids...\n")
    for solid in solids:
        lay = rawValue(solid,8)
        if fmt.dxflayout or (not rawValue(solid,67)):
            shape = drawSolid(solid)
            if shape:
                if fmt.makeBlocks:
                    if isinstance(shape,Part.Shape):
                        addToBlock(shape,lay)
                    else:                                        
                        addToBlock(shape.Shape,lay)
                else:
                    newob = addObject(shape,"Solid",lay)
                    if gui: fmt.formatObject(newob,solid)

    # drawing splines

    splines = drawing.entities.get_type("spline")
    if splines: FreeCAD.Console.PrintMessage("drawing "+str(len(splines))+" splines...\n")
    for spline in splines:
        lay = rawValue(spline,8)
        if fmt.dxflayout or (not rawValue(spline,67)):
            shape = drawSpline(spline)
            if shape:
                if fmt.makeBlocks:
                    if isinstance(shape,Part.Shape):
                        addToBlock(shape,lay)
                    else:
                        addToBlock(shape.Shape,lay)
                else:
                    newob = addObject(shape,"Spline",lay)
                    if gui: fmt.formatObject(newob,spline)

    # drawing texts

    if fmt.paramtext:
        texts = drawing.entities.get_type("mtext")
        texts.extend(drawing.entities.get_type("text"))
        if texts: FreeCAD.Console.PrintMessage("drawing "+str(len(texts))+" texts...\n")
        for text in texts:
            if fmt.dxflayout or (not rawValue(text,67)):
                addText(text)
					
    else: FreeCAD.Console.PrintMessage("skipping texts...\n")

    # drawing 3D objects

    faces3d = drawing.entities.get_type("3dface")
    if faces3d: FreeCAD.Console.PrintMessage("drawing "+str(len(faces3d))+" 3dfaces...\n")
    for face3d in faces3d:
        shape = drawFace(face3d)
        if shape:
            newob = addObject(shape,"Face",face3d.layer)
            if gui: fmt.formatObject(newob,face3d)
    if meshes: FreeCAD.Console.PrintMessage("drawing "+str(len(meshes))+" 3dmeshes...\n")
    for mesh in meshes:
        me = drawMesh(mesh)
        if me:
            newob = doc.addObject("Mesh::Feature","Mesh")
            lay = locateLayer(rawValue(mesh,8))
            lay.addObject(newob)
            newob.Mesh = me
            if gui: fmt.formatObject(newob,mesh)

    # drawing dims

    if fmt.paramtext:
        dims = drawing.entities.get_type("dimension")
        FreeCAD.Console.PrintMessage("drawing "+str(len(dims))+" dimensions...\n")
        for dim in dims:
            if fmt.dxflayout or (not rawValue(dim,67)):
                try:
                    layer = rawValue(dim,8)
                    x1 = float(rawValue(dim,10))
                    y1 = float(rawValue(dim,20))
                    z1 = float(rawValue(dim,30))
                    x2 = float(rawValue(dim,13))
                    y2 = float(rawValue(dim,23))
                    z2 = float(rawValue(dim,33))
                    x3 = float(rawValue(dim,14))
                    y3 = float(rawValue(dim,24))
                    z3 = float(rawValue(dim,34))
                    d = rawValue(dim,70)
                    if d: align = int(d)
                    else: align = 0
                    d = rawValue(dim,50)
                    if d: angle = float(d)
                    else: angle = 0
                except:
                    warn(dim)
                else:
                    lay=locateLayer(layer)
                    pt = FreeCAD.Vector(x1,y1,z1)
                    p1 = FreeCAD.Vector(x2,y2,z2)
                    p2 = FreeCAD.Vector(x3,y3,z3)
                    if align == 0:
                        if angle == 0:
                            p2 = FreeCAD.Vector(x3,y2,z2)
                        else:
                            p2 = FreeCAD.Vector(x2,y3,z2)
                    newob = doc.addObject("App::FeaturePython","Dimension")
                    lay.addObject(newob)
                    Dimension(newob)
                    ViewProviderDimension(newob.ViewObject)
                    newob.Start = p1
                    newob.End = p2
                    newob.Dimline = pt
                    if gui:
                        dim.layer = layer
                        dim.color_index = 256
                        fmt.formatObject (newob,dim)
						
    else: FreeCAD.Console.PrintMessage("skipping dimensions...\n")

    # drawing blocks

    inserts = drawing.entities.get_type("insert")
    if not fmt.paramstarblocks:
        FreeCAD.Console.PrintMessage("skipping *blocks...\n")
        newinserts = []
        for i in inserts:
            if fmt.dxflayout or (not rawValue(i,67)):
                if i.block[0] != '*':
                    newinserts.append(i)
        inserts = newinserts
    if inserts:
        FreeCAD.Console.PrintMessage("drawing "+str(len(inserts))+" blocks...\n")
        blockrefs = drawing.blocks.data
        for ref in blockrefs:
            drawBlock(ref)
        for insert in inserts:
            shape = drawInsert(insert)
            if shape:
                if fmt.makeBlocks:
                    if isinstance(shape,Part.Shape):
                        addToBlock(shape,block.layer)
                    else:                                        
                        addToBlock(shape.Shape,block.layer)
                else:
                    newob = addObject(shape,"Block."+insert.block,insert.layer)
                    if gui: fmt.formatObject(newob,insert)

    # make blocks, if any

    if fmt.makeBlocks:
        for k,l in layerBlocks.iteritems():
            shape = drawLayerBlock(l)
            if shape:
                newob = addObject(shape,k)
    del layerBlocks
                                        
    # finishing

    doc.recompute()
    FreeCAD.Console.PrintMessage("successfully imported "+filename+"\n")
    if badobjects: print "dxf: ",len(badobjects)," objects were not imported"
    del fmt
    del doc
    del blockshapes

def warn(dxfobject):
    "outputs a warning if a dxf object couldn't be imported"
    print "dxf: couldn't import ", dxfobject
    badobjects.append(dxfobject)

def open(filename):
    "called when freecad opens a file."
    docname = os.path.splitext(os.path.basename(filename))[0]
    doc = FreeCAD.newDocument(docname)
    doc.Label = decodeName(docname)
    processdxf(doc,filename)
    return doc

def insert(filename,docname):
    "called when freecad imports a file"
    groupname = os.path.splitext(os.path.basename(filename))[0]
    try:
        doc=FreeCAD.getDocument(docname)
    except:
        doc=FreeCAD.newDocument(docname)
    importgroup = doc.addObject("App::DocumentObjectGroup",groupname)
    importgroup.Label = decodeName(groupname)
    processdxf(doc,filename)
    for l in layers:
        importgroup.addObject(l)

		
# EXPORT ########################################################################


def getSplineSegs(edge):
    "returns an array of vectors from a bSpline edge"
    params = FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft")
    seglength = params.GetInt("maxsplinesegment")
    points = []
    if seglength == 0:
        points.append(edge.Vertexes[0].Point)
        points.append(edge.Vertexes[-1].Point)
    else:
        l = edge.Length
        points.append(edge.valueAt(0))
        if l > seglength:
            nbsegs = int(math.ceil(l/seglength))
            step = l/nbsegs
            for nv in range(1,nbsegs):
                v = edge.valueAt(nv*step)
                points.append(v)
        points.append(edge.valueAt(edge.Length))
    return points

def getWire(wire,nospline=False):
    "returns an array of dxf-ready points and bulges from a wire"
    edges = fcgeo.sortEdges(wire.Edges)
    points = []
    for edge in edges:
        v1 = edge.Vertexes[0].Point
        if (isinstance(edge.Curve,Part.Circle)):
            mp = fcgeo.findMidpoint(edge)
            v2 = edge.Vertexes[-1].Point
            c = edge.Curve.Center
            angle = abs(fcvec.angle(v1.sub(c),v2.sub(c)))
            # if (fcvec.angle(v2.sub(c)) < fcvec.angle(v1.sub(c))):
            #    angle = -angle
            # polyline bulge -> negative makes the arc go clockwise
            bul = math.tan(angle/4)
            # the next bit of code is for finding the direction of the arc
            # a negative cross product means the arc is clockwise
            tang1 = edge.Curve.tangent(edge.ParameterRange[0])
            tang2 = edge.Curve.tangent(edge.ParameterRange[1])
            cross1 = Vector.cross(Vector(tang1[0][0],tang1[0][1],tang1[0][2]),Vector(tang2[0][0],tang2[0][1],tang2[0][2]))
            if cross1[2] < 0:
                # polyline bulge -> negative makes the arc go clockwise 
                bul = -bul
            points.append((v1.x,v1.y,v1.z,None,None,bul))
        elif (isinstance(edge.Curve,Part.BSplineCurve)) and (not nospline):
            bul = 0.0
            spline = getSplineSegs(edge)
            spline.pop()
            for p in spline:
                points.append((p.x,p.y,p.z,None,None,bul))
        else:
            bul = 0.0
            points.append((v1.x,v1.y,v1.z,None,None,bul))
    if not fcgeo.isReallyClosed(wire):
        v = edges[-1].Vertexes[-1].Point
        points.append(fcvec.tup(v))
    # print "wire verts: ",points
    return points

def getBlock(obj):
    "returns a dxf block with the contents of the object"
    block = dxfLibrary.Block(name=obj.Name,layer=getGroup(obj,exportList))
    writeShape(obj,block)	
    return block

def writeShape(ob,dxfobject,nospline=False):
    "writes the object's shape contents in the given dxf object"
    processededges = []
    for wire in ob.Shape.Wires: # polylines
        for e in wire.Edges: processededges.append(e.hashCode())
        if (len(wire.Edges) == 1) and isinstance(wire.Edges[0].Curve,Part.Circle):
            center = fcvec.tup(wire.Edges[0].Curve.Center)
            radius = wire.Edges[0].Curve.Radius
            dxfobject.append(dxfLibrary.Circle(center, radius,
                                               color=getACI(ob),
                                               layer=getGroup(ob,exportList)))
        else:
            dxfobject.append(dxfLibrary.PolyLine(getWire(wire,nospline), [0.0,0.0,0.0],
                                                 int(fcgeo.isReallyClosed(wire)), color=getACI(ob),
                                                 layer=getGroup(ob,exportList)))
    if len(processededges) < len(ob.Shape.Edges): # lone edges
        loneedges = []
        for e in ob.Shape.Edges:
            if not(e.hashCode() in processededges): loneedges.append(e)
        # print "lone edges ",loneedges
        edge_index = 0
        for edge in loneedges:
            if (isinstance(edge.Curve,Part.BSplineCurve)) and ((not nospline) or (len(edge.Vertexes) == 1)): # splines
                points = []
                spline = getSplineSegs(edge)
                for p in spline:
                    points.append((p.x,p.y,p.z,None,None,0.0))
                dxfobject.append(dxfLibrary.PolyLine(points, [0.0,0.0,0.0],
                                                     0, color=getACI(ob),
                                                     layer=getGroup(ob,exportList)))
            elif (len(edge.Vertexes) == 1) and (isinstance(edge.Curve,Part.Circle)): # circles
                center = fcvec.tup(edge.Curve.Center)
                radius = ob.Shape.Edges[edge_index].Curve.Radius
                dxfobject.append(dxfLibrary.Circle(center, radius,
                                                   color=getACI(ob),
                                                   layer=getGroup(ob,exportList)))
            elif (len(edge.Vertexes) == 3) or (isinstance (edge.Curve,Part.Circle)): # arcs
                ce = ob.Shape.Edges[edge_index].Curve.Center
                radius = ob.Shape.Edges[edge_index].Curve.Radius
                #find direction of arc
                tang1 = edge.Curve.tangent(edge.ParameterRange[0])
                tang2 = edge.Curve.tangent(edge.ParameterRange[1])
                cross1 = Vector.cross(Vector(tang1[0][0],tang1[0][1],tang1[0][2]),Vector(tang2[0][0],tang2[0][1],tang2[0][2]))
                if cross1[2] > 0: # >0 ccw <0 cw
                    ve1 = edge.Vertexes[0].Point
                    ve2 = edge.Vertexes[-1].Point
                else:
                    ve1 = edge.Vertexes[-1].Point
                    ve2 = edge.Vertexes[0].Point
                ang1=-math.degrees(fcvec.angle(ve1.sub(ce)))
                ang2=-math.degrees(fcvec.angle(ve2.sub(ce)))
                dxfobject.append(dxfLibrary.Arc(fcvec.tup(ce), radius,
                                                ang1, ang2, color=getACI(ob),
                                                layer=getGroup(ob,exportList)))
            else: # anything else is treated as lines
            # if (len(edge.Vertexes) == 2) and (isinstance (edge.Curve,Part.Line)): # lines
                ve1=edge.Vertexes[0].Point
                ve2=edge.Vertexes[1].Point
                dxfobject.append(dxfLibrary.Line([fcvec.tup(ve1), fcvec.tup(ve2)],
                                                 color=getACI(ob),
                                                 layer=getGroup(ob,exportList)))
            edge_index = edge_index+1
def writeMesh(ob,dxfobject):
    "export a shape as a polyface mesh"
    meshdata = ob.Shape.tessellate(0.5)
    # print meshdata
    points = []
    faces = []
    for p in meshdata[0]:
        points.append([p.x,p.y,p.z])
    for f in meshdata[1]:
        faces.append([f[0]+1,f[1]+1,f[2]+1])
    # print len(points),len(faces)
    dxfobject.append(dxfLibrary.PolyLine([points,faces], [0.0,0.0,0.0],
                                         64, color=getACI(ob),
                                         layer=getGroup(ob,exportList)))
                                
def export(objectslist,filename,nospline=False):
    "called when freecad exports a file. If nospline=True, bsplines are exported as straight segs"
    global exportList
    exportList = objectslist
    dxf = dxfLibrary.Drawing()
    if (len(exportList) == 1) and (exportList[0].isDerivedFrom("Drawing::FeaturePage")):
        exportPage(exportList[0],filename)
        return
    for ob in exportList:
        print "processing ",ob.Name
        if ob.isDerivedFrom("Part::Feature"):
            if not ob.Shape.isNull():
                if FreeCAD.ParamGet("User parameter:BaseApp/Preferences/Mod/Draft").GetBool("dxfmesh"):
                    writeMesh(ob,dxf)
                else:
                    if ob.Shape.ShapeType == 'Compound':
                        if (len(ob.Shape.Wires) == 1):
                            # only one wire in this compound, no lone edge -> polyline
                            if (len(ob.Shape.Wires[0].Edges) == len(ob.Shape.Edges)):
                                writeShape(ob,dxf,nospline)
                            else:
                                # 1 wire + lone edges -> block
                                block = getBlock(ob)
                                dxf.blocks.append(block)
                                dxf.append(dxfLibrary.Insert(name=ob.Name.upper()))
                        else:
                            # all other cases: block
                            block = getBlock(ob)
                            dxf.blocks.append(block)
                            dxf.append(dxfLibrary.Insert(name=ob.Name.upper()))
                    else:
                        writeShape(ob,dxf,nospline)
				
        elif (ob.Type == "App::Annotation"):

            # texts

            # temporary - as dxfLibrary doesn't support mtexts well, we use several single-line texts
            # well, anyway, at the moment, Draft only writes single-line texts, so...
            for text in ob.LabelText:
                point = fcvec.tup(FreeCAD.Vector(ob.Position.x,
                                                 ob.Position.y-ob.LabelText.index(text),
                                                 ob.Position.z))
                if gui: height = float(ob.ViewObject.FontSize)
                else: height = 1
                dxf.append(dxfLibrary.Text(text,point,height=height,
                                           color=getACI(ob,text=True),
                                           style='STANDARD',
                                           layer=getGroup(ob,exportList)))

        elif 'Dimline' in ob.PropertiesList:
            p1 = fcvec.tup(ob.Start)
            p2 = fcvec.tup(ob.End)
            base = Part.Line(ob.Start,ob.End).toShape()
            proj = fcgeo.findDistance(ob.Dimline,base)
            if not proj:
                pbase = fcvec.tup(ob.End)
            else:
                pbase = fcvec.tup(ob.End.add(fcvec.neg(proj)))
            dxf.append(dxfLibrary.Dimension(pbase,p1,p2,color=getACI(ob),
                                            layer=getGroup(ob,exportList)))
					
    dxf.saveas(filename)
    FreeCAD.Console.PrintMessage("successfully exported "+filename+"\r\n")

def exportPage(page,filename):
    "special export for pages"
    import importSVG
    tempdoc = importSVG.open(page.PageResult)
    tempobj = tempdoc.Objects
    export(tempobj,filename,nospline=True)
    FreeCAD.closeDocument(tempdoc.Name)
        






