/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef __VIEWPROVIDERPART_H__
#define __VIEWPROVIDERPART_H__

#include <Standard_math.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <Gui/ViewProviderGeometryObject.h>
#include <map>

class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Wire;
class TopoDS_Face;
class SoSeparator;
class SoSwitch;
class SoVertexShape;
class SoPickedPoint;
class SoShapeHints;
class SoEventCallback;
class SbVec3f;

namespace PartGui {


class AppPartGuiExport ViewProviderPart : public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER(PartGui::ViewProviderPart);

public:
    /// constructor
    ViewProviderPart();
    /// destructor
    virtual ~ViewProviderPart();

    // Display properties
    App::PropertyFloatConstraint LineWidth;
    App::PropertyFloatConstraint PointSize;
    App::PropertyColor LineColor;
    App::PropertyColor PointColor;
    App::PropertyMaterial LineMaterial;
    App::PropertyMaterial PointMaterial;
    App::PropertyBool ControlPoints;
    App::PropertyEnumeration Lighting;


    virtual void attach(App::DocumentObject *);
    virtual void setDisplayMode(const char* ModeName);
    /// returns a list of all possible modes
    virtual std::vector<std::string> getDisplayModes(void) const;
    /// Update the view representation
    void reload();

    virtual void updateData(const App::Property*);
    TopoDS_Shape getShape(const SoPickedPoint*) const;
    static void shapeInfoCallback(void * ud, SoEventCallback * n);

protected:
    /// get called by the container whenever a property has been changed
    virtual void onChanged(const App::Property* prop);
    bool loadParameter();
    Standard_Boolean computeFaces   (SoSeparator* root, const TopoDS_Shape &myShape);
    Standard_Boolean computeEdges   (SoSeparator* root, const TopoDS_Shape &myShape);
    Standard_Boolean computeVertices(SoSeparator* root, const TopoDS_Shape &myShape);

    void transferToArray(const TopoDS_Face& aFace,SbVec3f** vertices,SbVec3f** vertexnormals,
         int32_t** cons,int &nbNodesInFace,int &nbTriInFace );
    void showControlPoints(bool);
    void showControlPointsOfEdge(const TopoDS_Edge&);
    void showControlPointsOfWire(const TopoDS_Wire&);
    void showControlPointsOfFace(const TopoDS_Face&);

    // nodes for the data representation
    SoSeparator  *EdgeRoot;
    SoSeparator  *FaceRoot;
    SoSeparator  *VertexRoot;
    SoMaterial   *pcLineMaterial;
    SoMaterial   *pcPointMaterial;
    SoDrawStyle  *pcLineStyle;
    SoDrawStyle  *pcPointStyle;
    SoSwitch     *pcControlPoints;
    SoShapeHints *pShapeHints;

private:
    // settings stuff
    float meshDeviation;
    bool noPerVertexNormals;
    bool qualityNormals;
    static App::PropertyFloatConstraint::Constraints floatRange;
    static const char* LightingEnums[];
    std::map<SoVertexShape*, TopoDS_Shape> vertexShapeMap;
};

} // namespace PartGui


#endif // __VIEWPROVIDERPART_H__

