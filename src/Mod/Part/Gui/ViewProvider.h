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

#include <Gui/ViewProviderGeometryObject.h>
#include <Standard_Boolean.hxx>
#include <map>

class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SoVertexShape;
class SoPickedPoint;
class SoEventCallback;
class SbVec3f;

namespace PartGui {


class AppPartGuiExport ViewProviderPart:public Gui::ViewProviderGeometryObject
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
    App::PropertyFloatConstraint Deviation;
    App::PropertyBool NoPerVertexNormals;
    App::PropertyBool QualityNormals;
    App::PropertyColor LineColor;
    App::PropertyColor PointColor;
    App::PropertyMaterial LineMaterial;
    App::PropertyMaterial PointMaterial;


    virtual void attach(App::DocumentObject *);
    virtual void setDisplayMode(const char* ModeName);
    /// returns a list of all possible modes
    virtual std::vector<std::string> getDisplayModes(void) const;
    /// Update the Part representation
    //virtual void update(const ChangeType&);

    virtual void updateData(const App::Property*);
    TopoDS_Shape getShape(const SoPickedPoint*) const;
    static void shapeInfoCallback(void * ud, SoEventCallback * n);

protected:
    /// get called by the container whenever a property has been changed
    virtual void onChanged(const App::Property* prop);
    Standard_Boolean computeFaces   (SoSeparator* root, const TopoDS_Shape &myShape);
    Standard_Boolean computeEdges   (SoSeparator* root, const TopoDS_Shape &myShape);
    Standard_Boolean computeVertices(SoSeparator* root, const TopoDS_Shape &myShape);

    void transferToArray(const TopoDS_Face& aFace,SbVec3f** vertices,SbVec3f** vertexnormals, long** cons,int &nbNodesInFace,int &nbTriInFace );

    // nodes for the data representation
    SoSeparator *EdgeRoot;
    SoSeparator *FaceRoot;
    SoSeparator *VertexRoot;
    SoMaterial  *pcLineMaterial;
    SoMaterial  *pcPointMaterial;
    SoDrawStyle *pcLineStyle;
    SoDrawStyle *pcPointStyle;

private:
    static App::PropertyFloatConstraint::Constraints floatRange;
    static App::PropertyFloatConstraint::Constraints floatRangeDeviation;
    std::map<SoVertexShape*, TopoDS_Shape> vertexShapeMap;
};

} // namespace PartGui


#endif // __VIEWPROVIDERPART_H__

