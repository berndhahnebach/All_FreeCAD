/***************************************************************************
 *   Copyright (c) 2008 Jürgen Riegel (juergen.riegel@web.de)              *
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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <Standard_math.hxx>
# include <Inventor/SoDB.h>
# include <Inventor/SoInput.h>
# include <Inventor/SbVec3f.h>
# include <Inventor/actions/SoSearchAction.h>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoLightModel.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoTransform.h>
# include <Inventor/nodes/SoRotation.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <QFile>
#endif

#include "ViewProviderFemMesh.h"

#include <Mod/Fem/App/FemMeshObject.h>
#include <Mod/Fem/App/FemMesh.h>
#include <Gui/SoFCSelection.h>
#include <App/Document.h>
#include <Base/FileInfo.h>
#include <Base/Stream.h>
#include <Base/Console.h>
#include <sstream>

#include <SMESH_Mesh.hxx>
#include <SMESHDS_Mesh.hxx>

using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemMesh, Gui::ViewProviderGeometryObject)

ViewProviderFemMesh::ViewProviderFemMesh()
{
    pcDrawStyle = new SoDrawStyle();
    pcDrawStyle->ref();
    pcDrawStyle->style = SoDrawStyle::LINES;
    pcDrawStyle->lineWidth = 2;

    pShapeHints = new SoShapeHints;
    pShapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    pShapeHints->vertexOrdering = SoShapeHints::UNKNOWN_ORDERING;
    //pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    pShapeHints->ref();

    pcMatBinding = new SoMaterialBinding;
    pcMatBinding->value = SoMaterialBinding::OVERALL;
    pcMatBinding->ref();

    pcCoords = new SoCoordinate3();
    pcCoords->ref();

    pcFaces = new SoIndexedFaceSet;
    pcFaces->ref();
}

ViewProviderFemMesh::~ViewProviderFemMesh()
{
    pcCoords->unref();
    pcDrawStyle->unref();
    pcFaces->unref();
    pShapeHints->unref();
    pcMatBinding->unref();
}

void ViewProviderFemMesh::attach(App::DocumentObject *pcObj)
{
    ViewProviderGeometryObject::attach(pcObj);

    // flat
    SoGroup* pcFlatRoot = new SoGroup();
    pcFlatRoot->addChild(pShapeHints);
    pcFlatRoot->addChild(pcShapeMaterial);
    pcFlatRoot->addChild(pcMatBinding);
    pcFlatRoot->addChild(pcHighlight);
    //addDisplayMaskMode(pcFlatRoot, "Flat");

    // line
    SoLightModel* pcLightModel = new SoLightModel();
    pcLightModel->model = SoLightModel::BASE_COLOR;
    SoGroup* pcWireRoot = new SoGroup();
    pcWireRoot->addChild(pcDrawStyle);
    pcWireRoot->addChild(pcLightModel);
    SoBaseColor* color = new SoBaseColor();
    color->rgb.setValue(0.0f,0.0f,0.0f);
    pcWireRoot->addChild(color);
    pcWireRoot->addChild(pcHighlight);
    //addDisplayMaskMode(pcWireRoot, "Wireframe");

    // flat+line
    //SoPolygonOffset* offset = new SoPolygonOffset();
    //offset->styles = SoPolygonOffset::LINES;
    //offset->factor = -2.0f;
    //offset->units = 1.0f;
    SoGroup* pcFlatWireRoot = new SoGroup();
    pcFlatWireRoot->addChild(pcFlatRoot);
    //pcFlatWireRoot->addChild(offset);
    pcFlatWireRoot->addChild(pcWireRoot);
    addDisplayMaskMode(pcFlatWireRoot, "Flat Lines");

    pcHighlight->addChild(pcCoords);
    pcHighlight->addChild(pcFaces);
}

void ViewProviderFemMesh::setDisplayMode(const char* ModeName)
{
    if (strcmp("Flat Lines",ModeName)==0)
        setDisplayMaskMode("Flat Lines");
    ViewProviderGeometryObject::setDisplayMode( ModeName );
}

std::vector<std::string> ViewProviderFemMesh::getDisplayModes(void) const
{
    std::vector<std::string> StrList;
    StrList.push_back("Flat Lines");
    return StrList;
}

void ViewProviderFemMesh::updateData(const App::Property* prop)
{
    if (prop->isDerivedFrom(Fem::PropertyFemMesh::getClassTypeId())) {
        ViewProviderFEMMeshBuilder builder;
        builder.createMesh(prop, pcCoords, pcFaces);
    }
    else {
        Gui::ViewProviderGeometryObject::updateData(prop);
    }
}

// ----------------------------------------------------------------------------

void ViewProviderFEMMeshBuilder::buildNodes(const App::Property* prop, std::vector<SoNode*>& nodes) const
{
    SoCoordinate3 *pcPointsCoord=0;
    SoIndexedFaceSet *pcFaces=0;

    if (nodes.empty()) {
        pcPointsCoord = new SoCoordinate3();
        nodes.push_back(pcPointsCoord);
        pcFaces = new SoIndexedFaceSet();
        nodes.push_back(pcFaces);
    }
    else if (nodes.size() == 2) {
        if (nodes[0]->getTypeId() == SoCoordinate3::getClassTypeId())
            pcPointsCoord = static_cast<SoCoordinate3*>(nodes[0]);
        if (nodes[1]->getTypeId() == SoIndexedFaceSet::getClassTypeId())
            pcFaces = static_cast<SoIndexedFaceSet*>(nodes[1]);
    }

    if (pcPointsCoord && pcFaces)
        createMesh(prop, pcPointsCoord, pcFaces);
}

void ViewProviderFEMMeshBuilder::createMesh(const App::Property* prop, SoCoordinate3* coords, SoIndexedFaceSet* faces) const
{
    const Fem::PropertyFemMesh* mesh = static_cast<const Fem::PropertyFemMesh*>(prop);

    SMESHDS_Mesh* data = const_cast<SMESH_Mesh*>(mesh->getValue().getSMesh())->GetMeshDS();
    const SMDS_MeshInfo& info = data->GetMeshInfo();
    int numNode = info.NbNodes();
    int numTria = info.NbTriangles();
    int numQuad = info.NbQuadrangles();
    int numPoly = info.NbPolygons();
    int numVolu = info.NbVolumes();
    int numTetr = info.NbTetras();
    int numHexa = info.NbHexas();
    int numPyrd = info.NbPyramids();
    int numPris = info.NbPrisms();
    int numHedr = info.NbPolyhedrons();

    int index=0;
    std::map<const SMDS_MeshNode*, int> mapNodeIndex;

    // set the point coordinates
    coords->point.setNum(numNode);
    SMDS_NodeIteratorPtr aNodeIter = data->nodesIterator();
    unsigned int i=0;
    SbVec3f* verts = coords->point.startEditing();
    for (;aNodeIter->more();) {
        const SMDS_MeshNode* aNode = aNodeIter->next();
        verts[i++].setValue((float)aNode->X(),(float)aNode->Y(),(float)aNode->Z());
        mapNodeIndex[aNode] = index++;
    }
    coords->point.finishEditing();

    // set the face indices
    index=0;
    faces->coordIndex.setNum(4*numTria + 5*numQuad);
    int32_t* indices = faces->coordIndex.startEditing();
    SMDS_FaceIteratorPtr aFaceIter = data->facesIterator();
    for (;aFaceIter->more();) {
        const SMDS_MeshFace* aFace = aFaceIter->next();
        int num = aFace->NbNodes();
        if (num != 3 && num != 4)
            continue;
        for (int j=0; j<num;j++) {
            const SMDS_MeshNode* node = aFace->GetNode(j);
            indices[index++] = mapNodeIndex[node];
        }
        indices[index++] = SO_END_FACE_INDEX;
    }
    faces->coordIndex.finishEditing();
}
