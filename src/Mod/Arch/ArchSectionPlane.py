#***************************************************************************
#*                                                                         *
#*   Copyright (c) 2011                                                    *  
#*   Yorik van Havre <yorik@uncreated.net>                                 *  
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU Lesser General Public License (LGPL)    *
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

import FreeCAD,FreeCADGui,ArchComponent,WorkingPlane,Drawing,math,Draft,ArchCommands
from FreeCAD import Vector
from PyQt4 import QtCore
from pivy import coin
from draftlibs import fcvec


class _CommandSectionPlane:
    "the Arch SectionPlane command definition"
    def GetResources(self):
        return {'Pixmap'  : 'Arch_SectionPlane',
                'Accel': "S, P",
                'MenuText': QtCore.QT_TRANSLATE_NOOP("Arch_SectionPlane","Section Plane"),
                'ToolTip': QtCore.QT_TRANSLATE_NOOP("Arch_SectionPlane","Adds a section plane object to the document")}

    def Activated(self):
        sel = FreeCADGui.Selection.getSelection()
        FreeCAD.ActiveDocument.openTransaction("Section Plane")
        obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython","Section")
        _SectionPlane(obj)
        _ViewProviderSectionPlane(obj.ViewObject)
        FreeCAD.ActiveDocument.commitTransaction()
        g = []
        for o in sel:
            if o.isDerivedFrom("Part::Feature"):
                g.append(o)
        obj.Objects = g
        page = FreeCAD.ActiveDocument.addObject("Drawing::FeaturePage","Page")
        template = Draft.getParam("template")
        if not template:
            template = FreeCAD.getResourceDir()+'Mod/Drawing/Templates/A3_Landscape.svg'
        page.ViewObject.HintOffsetX = 200
        page.ViewObject.HintOffsetY = 100
        page.ViewObject.HintScale = 20
        page.Template = template
        view = FreeCAD.ActiveDocument.addObject("Drawing::FeatureViewPython","View")
        page.addObject(view)
        _ArchDrawingView(view)
        view.Source = obj
        FreeCAD.ActiveDocument.recompute()

class _SectionPlane:
    "A section plane object"
    def __init__(self,obj):
        obj.Proxy = self
        obj.addProperty("App::PropertyLinkList","Objects","Base",
                        "The objects that must be considered by this section plane. Empty means all document")
        self.Type = "SectionPlane"
        
    def execute(self,obj):
        import Part
        pl = obj.Placement
        l = obj.ViewObject.DisplaySize
        p = Part.makePlane(l,l,Vector(l/2,-l/2,0),Vector(0,0,-1))
        obj.Shape = p
        obj.Placement = pl

    def onChanged(self,obj,prop):
        pass

    def getNormal(self,obj):
        return obj.Shape.Faces[0].normalAt(0,0)

class _ViewProviderSectionPlane(ArchComponent.ViewProviderComponent):
    "A View Provider for Section Planes"
    def __init__(self,vobj):
        vobj.addProperty("App::PropertyLength","DisplaySize","Base",
                        "The display size of the section plane")
        vobj.DisplaySize = 1
        vobj.Transparency = 85
        vobj.LineWidth = 1
        vobj.LineColor = (0.0,0.0,0.4,1.0)
        vobj.Proxy = self
        self.Object = vobj.Object

    def getIcon(self):
        return ":/icons/Arch_SectionPlane_Tree.svg"

    def claimChildren(self):
        return []

    def attach(self,vobj):
        self.Object = vobj.Object
        # adding arrows
        rn = vobj.RootNode
        self.col = coin.SoBaseColor()
        self.setColor()
        ds = coin.SoDrawStyle()
        ds.style = coin.SoDrawStyle.LINES
        self.lcoords = coin.SoCoordinate3()
        ls = coin.SoLineSet()
        ls.numVertices.setValues([2,4,4,2,4,4,2,4,4,2,4,4])
        pt = coin.SoAnnotation()
        pt.addChild(self.col)
        pt.addChild(ds)
        pt.addChild(self.lcoords)
        pt.addChild(ls)
        rn.addChild(pt)
        self.setVerts()

    def setColor(self):
        self.col.rgb.setValue(self.Object.ViewObject.LineColor[0],
                              self.Object.ViewObject.LineColor[1],
                              self.Object.ViewObject.LineColor[2])

    def setVerts(self):
        def extendVerts(x,y):
            l1 = hd/3
            l2 = l1/3
            verts.extend([[x,y,0],[x,y,-l1]])
            verts.extend([[x,y,-l1],[x-l2,y,-l1+l2],[x+l2,y,-l1+l2],[x,y,-l1]])
            verts.extend([[x,y,-l1],[x,y-l2,-l1+l2],[x,y+l2,-l1+l2],[x,y,-l1]])
        hd = self.Object.ViewObject.DisplaySize/2
        verts = []
        extendVerts(-hd,-hd)
        extendVerts(hd,-hd)
        extendVerts(hd,hd)
        extendVerts(-hd,hd)
        self.lcoords.point.setValues(verts)

    def updateData(self,obj,prop):
        if prop in ["Shape","Placement"]:
            self.setVerts()
        return

    def onChanged(self,vobj,prop):
        if prop == "LineColor":
            self.setColor()
        elif prop == "DisplaySize":
            vobj.Object.Proxy.execute(vobj.Object)
        return

class _ArchDrawingView:
    def __init__(self, obj):
        obj.addProperty("App::PropertyLink","Source","Base","The linked object")
        obj.addProperty("App::PropertyEnumeration","RenderingMode","Drawing View","The rendering mode to use")
        obj.addProperty("App::PropertyFloat","LineWidth","Drawing View","The line width of the rendered objects")
        obj.RenderingMode = ["Solid","Wireframe"]
        obj.RenderingMode = "Solid"
        obj.LineWidth = 0.35
        obj.Proxy = self
        self.Type = "DrawingView"

    def execute(self, obj):
        if obj.Source:
            obj.ViewResult = self.updateSVG(obj)
            
    def onChanged(self, obj, prop):
        if prop in ["Source","RenderingMode"]:
            obj.ViewResult = self.updateSVG(obj)

    def updateSVG(self, obj,join=False):
        "encapsulates a svg fragment into a transformation node"
        import Part
        from draftlibs import fcgeo
        if hasattr(obj,"Source"):
            if obj.Source:
                if obj.Source.Objects:
                    svg = ''

                    # getting section plane
                    cp = ArchCommands.getCutVolume(obj.Source.Objects,obj.Source.Placement)
                    self.sections = []
                    if cp:
                        cutvolume = cp[0].extrude(cp[1])
                    shapes = []
                    colors = []

                    # sorting
                    if join:
                        walls = []
                        structs = []
                        objs = []
                        for o in obj.Source.Objects:
                            t = Draft.getType(o)
                            if t == "Wall":
                                walls.append(o)
                            elif t == "Structure":
                                structs.append(o)
                            else:
                                objs.append(o)
                        for g in [walls,structs]:
                            if g:
                                print "group:",g
                                col = g[0].ViewObject.DiffuseColor[0]
                                s = g[0].Shape
                                for o in g[1:]:
                                    try:
                                        fs = s.fuse(o.Shape)
                                        fs = fs.removeSplitter()
                                    except:
                                        print "shape fusion failed"
                                        objs.append([o.Shape,o.ViewObject.DiffuseColor[0]])
                                    else:
                                        s = fs
                                objs.append([s,col])
                    else:
                        objs = obj.Source.Objects

                    # shapes extraction    
                    for o in objs:
                        print "object:",o
                        if isinstance(o,list):
                            shape = o[0]
                            color = o[1]
                        else:
                            shape = o.Shape
                            color = o.ViewObject.DiffuseColor[0]
                        if cp:
                            for s in shape.Solids:
                                shapes.append(s.cut(cutvolume))
                                colors.append(color)
                            sec = shape.section(cp[0])
                            if sec.Edges:
                                wires = fcgeo.findWires(sec.Edges)
                                for w in wires:
                                    sec = Part.Wire(fcgeo.sortEdges(w.Edges))
                                    sec = Part.Face(sec)
                                    self.sections.append(sec)
                        else:
                            shapes.append(shape)
                            colors.append(color)

                    # generating SVG
                    linewidth = obj.LineWidth/obj.Scale        
                    if obj.RenderingMode == "Solid":
                        svg += self.renderVRM(shapes,obj.Source.Placement,colors,linewidth)
                    else:
                        svg += self.renderOCC(shapes,obj.Source.Proxy.getNormal(obj.Source),linewidth)
                    for s in self.sections:
                        svg += self.renderSection(s,obj.Source.Placement,linewidth*2)
                    result = ''
                    result += '<g id="' + obj.Name + '"'
                    result += ' transform="'
                    result += 'rotate('+str(obj.Rotation)+','+str(obj.X)+','+str(obj.Y)+') '
                    result += 'translate('+str(obj.X)+','+str(obj.Y)+') '
                    result += 'scale('+str(obj.Scale)+','+str(-obj.Scale)+')'
                    result += '">\n'
                    result += svg
                    result += '</g>\n'
                    #print "complete node:",result
                    return result
        return ''

    def renderOCC(self,shapes,direction,linewidth):
        "renders an SVG fragment with the OCC method"
        shapes = shapes[:]
        if shapes:
            base = shape.pop()
            for sh in shapes:
                base = base.fuse(sh)
        result = Drawing.projectToSVG(base,fcvec.neg(direction))
        if result:
            result = result.replace('stroke-width="0.35"','stroke-width="' + str(linewidth) + 'px"')
            return result
        return ''

    def renderVRM(self,shapes,placement,colors,linewidth):
        "renders an SVG fragment with the ArchVRM method"
        import ArchVRM
        render = ArchVRM.Renderer()
        render.setWorkingPlane(FreeCAD.Placement(placement))
        for i in range(len(shapes)):
            if colors:
                render.add(shapes[i],colors[i])
            else:
                render.add(shapes[i])
        svg = render.getSVG(linewidth=linewidth)
        #print render.info()
        return svg

    def renderSection(self,shape,placement,linewidth):
        "renders a plane parallel to the section plane"
        placement = FreeCAD.Placement(placement)
        u = placement.Rotation.multVec(FreeCAD.Vector(1,0,0))
        v = placement.Rotation.multVec(FreeCAD.Vector(0,1,0))
        pts  = []
        for vt in shape.Vertexes:
            vu = fcvec.project(vt.Point,u)
            if vu.getAngle(u) > 1:
                x = -vu.Length
            else:
                x = vu.Length
            vv = fcvec.project(vt.Point,v)
            if vv.getAngle(v) > 1:
                y = -vv.Length
            else:
                y = vv.Length
            pts.append([x,y])
        svg ='<path '
        svg += 'd="M '+ str(pts[0][0]) +' '+ str(pts[0][1]) + ' '
        for p in pts[1:]:
            svg += 'L '+ str(p[0]) +' '+ str(p[1]) + ' '
        svg += 'z '
        svg += '" '
        svg += 'stroke="#000000" '
        svg += 'stroke-width="' + str(linewidth) + 'px" '
        svg += 'style="stroke-width:' + str(linewidth) + ';'
        svg += 'stroke-miterlimit:1;'
        svg += 'stroke-linejoin:round;'
        svg += 'stroke-dasharray:none;'
        svg += 'fill:#ffffff;'
        svg += 'fill-rule: evenodd'
        svg += '"/>\n'
        return svg
            
        
                
FreeCADGui.addCommand('Arch_SectionPlane',_CommandSectionPlane())
