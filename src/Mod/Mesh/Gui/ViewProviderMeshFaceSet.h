/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#ifndef MESHGUI_VIEWPROVIDERMESHOBJECT_H
#define MESHGUI_VIEWPROVIDERMESHOBJECT_H

#include <Mod/Mesh/App/Core/Elements.h>
#include <Mod/Mesh/Gui/ViewProvider.h>

#include <vector>
#include <Inventor/fields/SoSFVec2f.h>

class SbViewVolume;
class SoBaseColor;
class SoShapeHints;
class SoEventCallback;
class SoMaterialBinding;

namespace Gui {
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
 * The ViewProviderMeshFaceSet class creates a node representing the mesh data structure.
 * It does basically the same ViewProviderMeshNode by rendering directly the FreeCAD mesh
 * structure whereas this class follows the Inventor way. For more details @see 
 * SoFCMeshNode and SoFCMeshFaceSet.
 * @author Werner Mayer
 */
class MeshGuiExport ViewProviderMeshFaceSet : public ViewProviderMesh
{
    PROPERTY_HEADER(MeshGui::ViewProviderMeshFaceSet);

public:
    ViewProviderMeshFaceSet();
    virtual ~ViewProviderMeshFaceSet();

    void attach(App::DocumentObject *pcFeat);
    virtual void updateData(const App::Property*);

protected:
    SoShape* getShapeNode() const;
    void showOpenEdges(bool);

protected:
    SoFCMeshObjectNode  * pcMeshNode;
    SoFCMeshObjectShape * pcMeshShape;
};

} // namespace MeshGui


#endif // MESHGUI_VIEWPROVIDERMESHOBJECT_H

