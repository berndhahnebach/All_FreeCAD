/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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


#ifndef MESHGUI_VIEWPROVIDERPYTHON_H
#define MESHGUI_VIEWPROVIDERPYTHON_H

#include <vector>
#include <Inventor/fields/SoSFVec2f.h>

#include <Gui/ViewProviderPythonFeature.h>
#include <App/PropertyStandard.h>

class SoSeparator;
class SoEventCallback;
class SbViewVolume;
class SoBaseColor;
class SoShape;
class SoCoordinate3;
class SoIndexedFaceSet;
class SoShapeHints;
class SoMaterialBinding;

namespace MeshCore {
  class MeshKernel;
}

namespace MeshGui {


class MeshGuiExport ViewProviderPython : public Gui::ViewProviderPythonGeometry
{
    PROPERTY_HEADER(MeshGui::ViewProviderPython);

public:
    ViewProviderPython();
    virtual ~ViewProviderPython();

    // Display properties
    App::PropertyPercent LineTransparency;
    App::PropertyFloatConstraint LineWidth;
    App::PropertyFloatConstraint PointSize;
    App::PropertyFloatConstraint CreaseAngle;
    App::PropertyBool OpenEdges;
    App::PropertyEnumeration Lighting;
    App::PropertyColor LineColor;

    virtual void attach(App::DocumentObject *);
    virtual void setDisplayMode(const char* ModeName);
    /// returns a list of all possible modes
    virtual std::vector<std::string> getDisplayModes(void) const;
    virtual void updateData(const App::Property*);

protected:
    /// get called by the container whenever a property has been changed
    virtual void onChanged(const App::Property* prop);
    void createMesh(const MeshCore::MeshKernel& pcMesh);
    virtual void showOpenEdges(bool);
    void setOpenEdgeColorFrom(const App::Color& col);

    // nodes for the data representation
    SoDrawStyle         * pcLineStyle;
    SoDrawStyle         * pcPointStyle;
    SoSeparator         * pcOpenEdge;
    SoBaseColor         * pOpenColor;
    SoMaterial          * pLineColor;
    SoShapeHints        * pShapeHints;
    SoMaterialBinding   * pcMatBinding;
    SoCoordinate3       * pcMeshCoord;
    SoIndexedFaceSet    * pcMeshFaces;

private:
    static App::PropertyFloatConstraint::Constraints floatRange;
    static App::PropertyFloatConstraint::Constraints angleRange;
    static App::PropertyIntegerConstraint::Constraints intPercent;
    static const char* LightingEnums[];
};

} // namespace MeshGui


#endif // MESHGUI_VIEWPROVIDERPYTHON_H

