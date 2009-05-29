/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <wmayer@users.sourceforge.net>        *
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

#ifndef MESHGUI_VIEWPROVIDERMESH_H
#define MESHGUI_VIEWPROVIDERMESH_H

#include <vector>
#include <Inventor/fields/SoSFVec2f.h>

#include <Mod/Mesh/App/Core/Elements.h>
#include <Gui/ViewProviderGeometryObject.h>
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

namespace App {
  class Color;
}

namespace Gui {
  class View3DInventorViewer;
  class SoFCSelection;
  class AbstractMouseModel;
}


namespace MeshCore {
  class MeshKernel;
}


namespace MeshGui {
class SoFCMeshObjectNode;
class SoFCMeshObjectShape;

/**
 * The ViewProviderExport class creates an empty node.
 * @author Werner Mayer
 */
class MeshGuiExport ViewProviderExport : public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER(MeshGui::ViewProviderExport);

public:
    ViewProviderExport();
    virtual ~ViewProviderExport();

    virtual QIcon getIcon() const;
    SoSeparator* getRoot(void){return 0;}
    std::vector<std::string> getDisplayModes(void) const;
    const char* getDefaultDisplayMode() const;
};

/**
 * The ViewProviderMesh class offers the visualization of the mesh data structure
 * and many algorithms to work on or edit the mesh.
 * @author Werner Mayer
 */
class MeshGuiExport ViewProviderMesh : public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER(MeshGui::ViewProviderMesh);

public:
    ViewProviderMesh();
    virtual ~ViewProviderMesh();

    // Display properties
    App::PropertyFloatConstraint LineWidth;
    App::PropertyFloatConstraint PointSize;
    App::PropertyFloatConstraint CreaseAngle;
    App::PropertyBool OpenEdges;
    App::PropertyEnumeration Lighting;
    App::PropertyColor LineColor;
    //App::PropertyColor BacksideColor;

    virtual void attach(App::DocumentObject *);
    virtual QIcon getIcon() const;
    /// Sets the correct display mode
    virtual void setDisplayMode(const char* ModeName);
    /// returns a list of all possible modes
    virtual std::vector<std::string> getDisplayModes(void) const;

    /** @name Editing */
    //@{
    bool doubleClicked(void){return false;}
    /// Sets the edit mode
    bool setEdit(int ModNum=0);
    /// Unsets the edit mode
    void unsetEdit(void);
    /// Returns the edit mode
    const char* getEditModeName(void);
    void markPart(unsigned long facet);
    bool isMarked(unsigned long facet) const;
    void unmarkParts();
    void removePart();
    unsigned long countMarkedFacets() const;
    void getFacetsFromPolygon(const std::vector<SbVec2f>& picked,
                              Gui::View3DInventorViewer &Viewer, SbBool inner,
                              std::vector<unsigned long>& indices) const;
    //@}

protected:
    /// get called by the container whenever a property has been changed
    void onChanged(const App::Property* prop);
    virtual void showOpenEdges(bool);
    void setOpenEdgeColorFrom(const App::Color& col);
    virtual void cutMesh(const std::vector<SbVec2f>& picked, Gui::View3DInventorViewer &Viewer, SbBool inner);
    virtual void splitMesh(const MeshCore::MeshKernel& toolMesh, const Base::Vector3f& normal, SbBool inner);
    virtual void segmentMesh(const MeshCore::MeshKernel& toolMesh, const Base::Vector3f& normal, SbBool inner);
    virtual void faceInfo(unsigned long facet);
    virtual void fillHole(unsigned long facet);

    virtual SoShape* getShapeNode() const;

public:
    static void faceInfoCallback(void * ud, SoEventCallback * n);
    static void fillHoleCallback(void * ud, SoEventCallback * n);
    static void markPartCallback(void * ud, SoEventCallback * n);
    static void clipMeshCallback(void * ud, SoEventCallback * n);
    static void partMeshCallback(void * ud, SoEventCallback * n);
    static void segmMeshCallback(void * ud, SoEventCallback * n);
    /// Creates a tool mesh from the previous picked polygon on the viewer
    static bool createToolMesh(const std::vector<SbVec2f>& rclPoly, const SbViewVolume& vol,
            const Base::Vector3f& rcNormal, std::vector<MeshCore::MeshGeomFacet>&);

protected:
    SoDrawStyle         * pcLineStyle;
    SoDrawStyle         * pcPointStyle;
    SoSeparator         * pcOpenEdge;
    SoBaseColor         * pOpenColor;
    SoBaseColor         * pLineColor;
    SoShapeHints        * pShapeHints;
    //SoShapeHints        * pBackHints;
    SoMaterialBinding   * pcMatBinding;
    //SoMaterial          * pcBacksideMaterial;

private:
    std::vector<unsigned long> _markedFacets;
    bool m_bEdit;

    static App::PropertyFloatConstraint::Constraints floatRange;
    static App::PropertyFloatConstraint::Constraints angleRange;
    static const char* LightingEnums[];
};

/**
 * The ViewProviderIndexedFaceSet class creates an indexed faceset node in order
 * to render the mesh data structure.
 * @author Werner Mayer
 */
class MeshGuiExport ViewProviderIndexedFaceSet : public ViewProviderMesh
{
    PROPERTY_HEADER(MeshGui::ViewProviderIndexedFaceSet);

public:
    ViewProviderIndexedFaceSet();
    virtual ~ViewProviderIndexedFaceSet();

    virtual void attach(App::DocumentObject *);
    /// Update the Mesh representation
    virtual void updateData(const App::Property*);

protected:
    void showOpenEdges(bool);
    SoShape* getShapeNode() const;
    /// helper method to build up the FaceSet
    void createMesh(const MeshCore::MeshKernel& pcMesh);

private:
    SoCoordinate3       * pcMeshCoord;
    SoIndexedFaceSet    * pcMeshFaces;
};

/**
 * The ViewProviderIndexedFaceSet class creates an own node in order
 * to directly render the mesh data structure.
 * @author Werner Mayer
 */
class MeshGuiExport ViewProviderMeshObject : public ViewProviderMesh
{
    PROPERTY_HEADER(MeshGui::ViewProviderMeshObject);

public:
    ViewProviderMeshObject();
    virtual ~ViewProviderMeshObject();

    void attach(App::DocumentObject *pcFeat);
    virtual void updateData(const App::Property*);

protected:
    SoShape* getShapeNode() const;
    void showOpenEdges(bool);

private:
    SoFCMeshObjectNode  * pcMeshNode;
    SoFCMeshObjectShape * pcMeshShape;
};

} // namespace MeshGui


#endif // MESHGUI_VIEWPROVIDERMESH_H

