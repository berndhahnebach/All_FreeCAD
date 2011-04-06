/***************************************************************************
 *   Copyright (c) 2011 Juergen Riegel <juergen.riegel@web.de>             *
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
# include <Poly_Polygon3D.hxx>
# include <BRepBndLib.hxx>
# include <BRepMesh.hxx>
# include <BRepMesh_IncrementalMesh.hxx>
# include <BRep_Tool.hxx>
# include <BRepTools.hxx>
# include <BRepAdaptor_Curve.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <GeomAbs_CurveType.hxx>
# include <GeomAbs_SurfaceType.hxx>
# include <Geom_BezierCurve.hxx>
# include <Geom_BSplineCurve.hxx>
# include <Geom_BezierSurface.hxx>
# include <Geom_BSplineSurface.hxx>
# include <GeomAPI_ProjectPointOnSurf.hxx>
# include <GeomLProp_SLProps.hxx>
# include <gp_Trsf.hxx>
# include <Poly_Array1OfTriangle.hxx>
# include <Poly_Triangulation.hxx>
# include <TColgp_Array1OfPnt.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Edge.hxx>
# include <TopoDS_Wire.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Shape.hxx>
# include <TopoDS_Iterator.hxx>
# include <TopExp_Explorer.hxx>
# include <TopExp.hxx>
# include <TopTools_IndexedMapOfShape.hxx>
# include <Poly_PolygonOnTriangulation.hxx>
# include <TColStd_Array1OfInteger.hxx>
# include <TopTools_ListOfShape.hxx>
# include <Inventor/SoPickedPoint.h>
# include <Inventor/events/SoMouseButtonEvent.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoIndexedLineSet.h>
# include <Inventor/nodes/SoLocateHighlight.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoNormal.h>
# include <Inventor/nodes/SoNormalBinding.h>
# include <Inventor/nodes/SoPointSet.h>
# include <Inventor/nodes/SoPolygonOffset.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/nodes/SoSwitch.h>
# include <Inventor/nodes/SoGroup.h>
# include <Inventor/nodes/SoSphere.h>
# include <Inventor/nodes/SoScale.h>
# include <Inventor/nodes/SoLightModel.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/TimeInfo.h>

#include <App/Application.h>
#include <App/Document.h>

#include <Gui/SoFCSelection.h>
#include <Gui/Selection.h>
#include <Gui/View3DInventorViewer.h>

#include "ViewProviderExt.h"
//#include "SoFCShapeObject.h"

#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/PrimitiveFeature.h>


using namespace PartGui;

PROPERTY_SOURCE(PartGui::ViewProviderPartExt, Gui::ViewProviderGeometryObject)


//**************************************************************************
// Construction/Destruction

App::PropertyFloatConstraint::Constraints ViewProviderPartExt::floatRange = {1.0f,64.0f,1.0f};
const char* ViewProviderPartExt::LightingEnums[]= {"One side","Two side",NULL};

ViewProviderPartExt::ViewProviderPartExt() 
{
    VisualTouched = true;

    App::Material mat;
    mat.ambientColor.set(0.2f,0.2f,0.2f);
    mat.diffuseColor.set(0.1f,0.1f,0.1f);
    mat.specularColor.set(0.0f,0.0f,0.0f);
    mat.emissiveColor.set(0.0f,0.0f,0.0f);
    mat.shininess = 1.0f;
    mat.transparency = 0.0f;
    ADD_PROPERTY(LineMaterial,(mat));
    ADD_PROPERTY(PointMaterial,(mat));
    ADD_PROPERTY(LineColor,(mat.diffuseColor));
    ADD_PROPERTY(PointColor,(mat.diffuseColor));
    ADD_PROPERTY(LineWidth,(2.0f));
    LineWidth.setConstraints(&floatRange);
    PointSize.setConstraints(&floatRange);
    ADD_PROPERTY(PointSize,(2.0f));
    ADD_PROPERTY(ControlPoints,(false));
    ADD_PROPERTY(Lighting,(1));
    Lighting.setEnums(LightingEnums);

    EdgeRoot = new SoSeparator();
    EdgeRoot->ref();
    FaceRoot = new SoSeparator();
    FaceRoot->ref();
    VertexRoot = new SoSeparator();
    VertexRoot->ref();
    coords = new SoCoordinate3();
    coords->ref();
    faceset = new SoIndexedFaceSet();
    faceset->ref();
    norm = new SoNormal;
    norm->ref();
    normb = new SoNormalBinding;
    normb->value = SoNormalBinding::PER_VERTEX_INDEXED;
    normb->ref();
    lineset = new SoIndexedLineSet();
    lineset->ref();

    pcLineMaterial = new SoMaterial;
    pcLineMaterial->ref();
    LineMaterial.touch();

    pcPointMaterial = new SoMaterial;
    pcPointMaterial->ref();
    PointMaterial.touch();

    pcLineStyle = new SoDrawStyle();
    pcLineStyle->ref();
    pcLineStyle->style = SoDrawStyle::LINES;
    pcLineStyle->lineWidth = LineWidth.getValue();

    pcPointStyle = new SoDrawStyle();
    pcPointStyle->ref();
    pcPointStyle->style = SoDrawStyle::POINTS;
    pcPointStyle->pointSize = PointSize.getValue();

    pShapeHints = new SoShapeHints;
    pShapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    pShapeHints->ref();
    Lighting.touch();

    sPixmap = "Tree_Part";
    loadParameter();
}

ViewProviderPartExt::~ViewProviderPartExt()
{
    EdgeRoot->unref();
    FaceRoot->unref();
    VertexRoot->unref();
    pcLineMaterial->unref();
    pcPointMaterial->unref();
    pcLineStyle->unref();
    pcPointStyle->unref();
    pShapeHints->unref();
    coords->unref();
    faceset->unref();
    norm->unref();
    lineset->unref();

}
void ViewProviderPartExt::onChanged(const App::Property* prop)
{
    if (prop == &LineWidth) {
        pcLineStyle->lineWidth = LineWidth.getValue();
    }
    else if (prop == &PointSize) {
        pcPointStyle->pointSize = PointSize.getValue();
    }
    else if (prop == &LineColor) {
        const App::Color& c = LineColor.getValue();
        pcLineMaterial->diffuseColor.setValue(c.r,c.g,c.b);
        if (c != LineMaterial.getValue().diffuseColor)
        LineMaterial.setDiffuseColor(c);
    }
    else if (prop == &PointColor) {
        const App::Color& c = PointColor.getValue();
        pcPointMaterial->diffuseColor.setValue(c.r,c.g,c.b);
        if (c != PointMaterial.getValue().diffuseColor)
        PointMaterial.setDiffuseColor(c);
    }
    else if (prop == &LineMaterial) {
        const App::Material& Mat = LineMaterial.getValue();
        if (LineColor.getValue() != Mat.diffuseColor)
        LineColor.setValue(Mat.diffuseColor);
        pcLineMaterial->ambientColor.setValue(Mat.ambientColor.r,Mat.ambientColor.g,Mat.ambientColor.b);
        pcLineMaterial->diffuseColor.setValue(Mat.diffuseColor.r,Mat.diffuseColor.g,Mat.diffuseColor.b);
        pcLineMaterial->specularColor.setValue(Mat.specularColor.r,Mat.specularColor.g,Mat.specularColor.b);
        pcLineMaterial->emissiveColor.setValue(Mat.emissiveColor.r,Mat.emissiveColor.g,Mat.emissiveColor.b);
        pcLineMaterial->shininess.setValue(Mat.shininess);
        pcLineMaterial->transparency.setValue(Mat.transparency);
    }
    else if (prop == &PointMaterial) {
        const App::Material& Mat = PointMaterial.getValue();
        if (PointColor.getValue() != Mat.diffuseColor)
        PointColor.setValue(Mat.diffuseColor);
        pcPointMaterial->ambientColor.setValue(Mat.ambientColor.r,Mat.ambientColor.g,Mat.ambientColor.b);
        pcPointMaterial->diffuseColor.setValue(Mat.diffuseColor.r,Mat.diffuseColor.g,Mat.diffuseColor.b);
        pcPointMaterial->specularColor.setValue(Mat.specularColor.r,Mat.specularColor.g,Mat.specularColor.b);
        pcPointMaterial->emissiveColor.setValue(Mat.emissiveColor.r,Mat.emissiveColor.g,Mat.emissiveColor.b);
        pcPointMaterial->shininess.setValue(Mat.shininess);
        pcPointMaterial->transparency.setValue(Mat.transparency);
    }
    else if (prop == &Lighting) {
        if (Lighting.getValue() == 0)
            pShapeHints->vertexOrdering = SoShapeHints::UNKNOWN_ORDERING;
        else
            pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    }
    else {
        // if the object was invisibly and chnaged while, recreate the visual
        if (prop == &Visibility && Visibility.getValue() && VisualTouched) 
            updateVisual(dynamic_cast<Part::Feature*>(pcObject)->Shape.getValue());

        ViewProviderGeometryObject::onChanged(prop);
    }
}

void ViewProviderPartExt::attach(App::DocumentObject *pcFeat)
{
    // call parent attach method
    ViewProviderGeometryObject::attach(pcFeat);

    SoGroup* pcNormalRoot = new SoGroup();
    SoGroup* pcFlatRoot = new SoGroup();
    SoGroup* pcWireframeRoot = new SoGroup();
    SoGroup* pcPointsRoot = new SoGroup();

    // enable two-side rendering
    pShapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    pShapeHints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;

    // Avoid any Z-buffer artefacts, so that the lines always appear on top of the faces
    // The correct order is Edges, Polygon offset, Faces.
    SoPolygonOffset* offset = new SoPolygonOffset();

    // normal viewing with edges and points
    pcNormalRoot->addChild(pShapeHints);
    pcNormalRoot->addChild(pcShapeMaterial);
    pcNormalRoot->addChild(norm);
    pcNormalRoot->addChild(normb);
    pcNormalRoot->addChild(coords);
    pcNormalRoot->addChild(faceset);
    pcNormalRoot->addChild(offset);
    pcNormalRoot->addChild(pcLineMaterial);  
    pcNormalRoot->addChild(pcLineStyle);
    //SoLightModel        *lightModel = new SoLightModel();
    //lightModel->model = SoLightModel::BASE_COLOR;
    //pcNormalRoot->addChild(lightModel);
    pcNormalRoot->addChild(lineset);

    //pcNormalRoot->addChild(FaceRoot);
    //pcNormalRoot->addChild(VertexRoot);

    //// just faces with no edges or points
    //pcFlatRoot->addChild(pShapeHints);
    //pcFlatRoot->addChild(FaceRoot);

    //// only edges
    //pcWireframeRoot->addChild(EdgeRoot);
    //pcWireframeRoot->addChild(VertexRoot);

    //// normal viewing with edges and points
    //pcPointsRoot->addChild(VertexRoot);

    // putting all together with the switch
    addDisplayMaskMode(pcNormalRoot, "Flat Lines");
    addDisplayMaskMode(pcFlatRoot, "Shaded");
    addDisplayMaskMode(pcWireframeRoot, "Wireframe");
    addDisplayMaskMode(pcPointsRoot, "Point");
}

void ViewProviderPartExt::setDisplayMode(const char* ModeName)
{
    if ( strcmp("Flat Lines",ModeName)==0 )
        setDisplayMaskMode("Flat Lines");
    else if ( strcmp("Shaded",ModeName)==0 )
        setDisplayMaskMode("Shaded");
    else if ( strcmp("Wireframe",ModeName)==0 )
        setDisplayMaskMode("Wireframe");
    else if ( strcmp("Points",ModeName)==0 )
        setDisplayMaskMode("Point");

    ViewProviderGeometryObject::setDisplayMode( ModeName );
}

std::vector<std::string> ViewProviderPartExt::getDisplayModes(void) const
{
    // get the modes of the father
    std::vector<std::string> StrList = ViewProviderGeometryObject::getDisplayModes();

    // add your own modes
    StrList.push_back("Flat Lines");
    StrList.push_back("Shaded");
    StrList.push_back("Wireframe");
    StrList.push_back("Points");

    return StrList;
}

void ViewProviderPartExt::shapeInfoCallback(void * ud, SoEventCallback * n)
{
    const SoMouseButtonEvent * mbe = (SoMouseButtonEvent *)n->getEvent();
    Gui::View3DInventorViewer* view  = reinterpret_cast<Gui::View3DInventorViewer*>(n->getUserData());

    // Mark all incoming mouse button events as handled, especially, to deactivate the selection node
    n->getAction()->setHandled();
    if (mbe->getButton() == SoMouseButtonEvent::BUTTON2 && mbe->getState() == SoButtonEvent::UP) {
        n->setHandled();
        view->setEditing(false);
        view->getWidget()->setCursor(QCursor(Qt::ArrowCursor));
        view->removeEventCallback(SoMouseButtonEvent::getClassTypeId(), shapeInfoCallback);
    }
    else if (mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
        const SoPickedPoint * point = n->getPickedPoint();
        if (point == NULL) {
            Base::Console().Message("No point picked.\n");
            return;
        }

        n->setHandled();

        // By specifying the indexed mesh node 'pcFaceSet' we make sure that the picked point is
        // really from the mesh we render and not from any other geometry
        Gui::ViewProvider* vp = static_cast<Gui::ViewProvider*>(view->getViewProviderByPath(point->getPath()));
        if (!vp || !vp->getTypeId().isDerivedFrom(ViewProviderPartExt::getClassTypeId()))
            return;
        ViewProviderPartExt* that = static_cast<ViewProviderPartExt*>(vp);
        TopoDS_Shape sh = that->getShape(point);
        if (!sh.IsNull()) {
            SbVec3f pt = point->getPoint();
            Base::Console().Message("(%.6f, %.6f, %.6f, %d)\n", pt[0], pt[1], pt[2], sh.HashCode(IntegerLast()));
        }
    }
}

TopoDS_Shape ViewProviderPartExt::getShape(const SoPickedPoint* point) const
{
    if (point && point->getPath()->getTail()->getTypeId().isDerivedFrom(SoVertexShape::getClassTypeId())) {
        SoVertexShape* vs = static_cast<SoVertexShape*>(point->getPath()->getTail());
        std::map<SoVertexShape*, TopoDS_Shape>::const_iterator it = vertexShapeMap.find(vs);
        if (it != vertexShapeMap.end())
            return it->second;
    }

    return TopoDS_Shape();
}

bool ViewProviderPartExt::loadParameter()
{
    bool changed = false;
    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath
        ("User parameter:BaseApp/Preferences/Mod/Part");
    float deviation = hGrp->GetFloat("MeshDeviation",0.2);
    bool novertexnormals = hGrp->GetBool("NoPerVertexNormals",false);
    bool qualitynormals = hGrp->GetBool("QualityNormals",false);

    if (this->meshDeviation != deviation) {
        this->meshDeviation = deviation;
        changed = true;
    }
    if (this->noPerVertexNormals != novertexnormals) {
        this->noPerVertexNormals = novertexnormals;
        changed = true;
    }
    if (this->qualityNormals != qualitynormals) {
        this->qualityNormals = qualitynormals;
        changed = true;
    }

    return changed;
}

void ViewProviderPartExt::reload()
{
    if (loadParameter()) {
        App::Property* shape     = pcObject->getPropertyByName("Shape");
        if (shape) update(shape);
    }
}

void ViewProviderPartExt::updateData(const App::Property* prop)
{
    Gui::ViewProviderGeometryObject::updateData(prop);
    if (prop->getTypeId() == Part::PropertyPartShape::getClassTypeId()) {
        // get the shape to show
        const TopoDS_Shape &cShape = static_cast<const Part::PropertyPartShape*>(prop)->getValue();

        // do nothing if shape is empty
        if (cShape.IsNull())
            return;

        // calculate the visual only if visibel
        if(Visibility.getValue())
            updateVisual(cShape);
        else
            VisualTouched = true;
 
    }
}

void ViewProviderPartExt::updateVisual(const TopoDS_Shape &inputShape)
{
    TopoDS_Shape cShape(inputShape);
    // time measurement and book keeping
    Base::TimeInfo start_time;
    int nbrTriangles=0,nbrNodes=0,nbrFaces=0,nbrEdges=0,endxVecSize=0;

    try {
        // calculating the deflection value
        Bnd_Box bounds;
        BRepBndLib::Add(cShape, bounds);
        bounds.SetGap(0.0);
        Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
        bounds.Get(xMin, yMin, zMin, xMax, yMax, zMax);
        Standard_Real deflection = ((xMax-xMin)+(yMax-yMin)+(zMax-zMin))/300.0 *
            this->meshDeviation;

        // create or use the mesh on the data structure
        BRepMesh_IncrementalMesh MESH(cShape,deflection);
        // We must reset the location here because the transformation data
        // are set in the placement property
        TopLoc_Location aLoc;
        cShape.Location(aLoc);

        // count triangles and nodes in the mesh
        TopExp_Explorer Ex;
        for(Ex.Init(cShape,TopAbs_FACE);Ex.More();Ex.Next()) {
               Handle (Poly_Triangulation) mesh = BRep_Tool::Triangulation(TopoDS::Face(Ex.Current()),TopLoc_Location());
               if (mesh.IsNull()) continue;
               nbrTriangles += mesh->NbTriangles();
               nbrNodes     += mesh->NbNodes();
               nbrFaces++;
        }

        // get a indexed map of edges
        TopTools_IndexedMapOfShape M;
        TopExp::MapShapes(cShape, TopAbs_EDGE, M);

        std::set<int>         edgeIdxSet;
        std::vector<int32_t>  indxVector;
        // count and index the edges
        for (int i=1; i <= M.Extent(); i++) {
            const TopoDS_Edge &actEdge = TopoDS::Edge(M(i));
            edgeIdxSet.insert(i);
            nbrEdges++;
        }
        // reserve some memory
        indxVector.reserve(nbrEdges*8);

        // create memory for the nodes and indexes
        coords  ->point      .setNum(nbrNodes);
        norm    ->vector     .setNum(nbrNodes);
        faceset ->coordIndex .setNum(nbrTriangles*4);
        // get the raw memory for fast fill up
        SbVec3f* verts = coords  ->point       .startEditing();
        SbVec3f* norms = norm    ->vector      .startEditing();
        int32_t* index = faceset ->coordIndex  .startEditing();

        // preset the normal vector with null vector
        for(int i=0;i < nbrNodes;i++) 
            norms[i]= SbVec3f(0.0,0.0,0.0);

        int i = 1,FaceNodeOffset=0,FaceTriaOffset=0;
        for (Ex.Init(cShape, TopAbs_FACE); Ex.More(); Ex.Next(),i++) {
            TopLoc_Location aLoc;
            const TopoDS_Face &actFace = TopoDS::Face(Ex.Current());
            // get the mesh of the shape
            Handle (Poly_Triangulation) mesh = BRep_Tool::Triangulation(actFace,aLoc);
            if (mesh.IsNull()) continue;

            // getting the transformation of the shape/face
            //gp_Trsf myTransf;
            //Standard_Boolean identity = true;
            //if (!aLoc.IsIdentity()) {
            //    identity = false;
            //    myTransf = aLoc.Transformation();
            //}

            // geting size of node and triangle aray of this Face
            int nbNodesInFace = mesh->NbNodes();
            int nbTriInFace   = mesh->NbTriangles();
            // check orientation
            TopAbs_Orientation orient = actFace.Orientation();


            // cycling through the poly mesh
            const Poly_Array1OfTriangle& Triangles = mesh->Triangles();
            const TColgp_Array1OfPnt& Nodes = mesh->Nodes();
            for (int g=1;g<=nbTriInFace;g++) {
                // Get the triangle
                Standard_Integer N1,N2,N3;
                Triangles(g).Get(N1,N2,N3);

                // change orientation of the triangle if the face is reversed
                if ( orient != TopAbs_FORWARD ) {
                    Standard_Integer tmp = N1;
                    N1 = N2;
                    N2 = tmp;
                }

                // get the 3 points of this triangle
                gp_Pnt V1(Nodes(N1)), V2(Nodes(N2)), V3(Nodes(N3));

                // transform the vertices to the place of the face
                /*if (!identity) {
                    V1.Transform(myTransf);
                    V2.Transform(myTransf);
                    V3.Transform(myTransf);
                }*/
                
                // calculating per vertex normals                    
                // Calculate triangle normal
                gp_Vec v1(V1.X(),V1.Y(),V1.Z()),v2(V2.X(),V2.Y(),V2.Z()),v3(V3.X(),V3.Y(),V3.Z());
                gp_Vec Normal = (v2-v1)^(v3-v1); 

                // add the triangle normal to the vertex normal for all points of this triangle
                norms[FaceNodeOffset+N1-1] += SbVec3f(Normal.X(),Normal.Y(),Normal.Z());
                norms[FaceNodeOffset+N2-1] += SbVec3f(Normal.X(),Normal.Y(),Normal.Z());
                norms[FaceNodeOffset+N3-1] += SbVec3f(Normal.X(),Normal.Y(),Normal.Z());                 

                // set the vertices
                verts[FaceNodeOffset+N1-1].setValue((float)(V1.X()),(float)(V1.Y()),(float)(V1.Z()));
                verts[FaceNodeOffset+N2-1].setValue((float)(V2.X()),(float)(V2.Y()),(float)(V2.Z()));
                verts[FaceNodeOffset+N3-1].setValue((float)(V3.X()),(float)(V3.Y()),(float)(V3.Z()));

                // set the index vector with the 3 point indexes and the end delimiter
                index[FaceTriaOffset*4+4*(g-1)]   = FaceNodeOffset+N1-1; 
                index[FaceTriaOffset*4+4*(g-1)+1] = FaceNodeOffset+N2-1; 
                index[FaceTriaOffset*4+4*(g-1)+2] = FaceNodeOffset+N3-1; 
                index[FaceTriaOffset*4+4*(g-1)+3] = SO_END_FACE_INDEX;
            }

            // handling the edges lye on this face
            
            TopExp_Explorer Exp;
            for(Exp.Init(actFace,TopAbs_EDGE);Exp.More();Exp.Next()) {
                const TopoDS_Edge &actEdge = TopoDS::Edge(Exp.Current());
                // get the overall index of this edge
                int idx = M.FindIndex(actEdge);
                // already processed this index ?
                if(edgeIdxSet.find(idx)!=edgeIdxSet.end()) {
                    
                    // this holds the indices of the edge's triangulation to the actual points
                    Handle(Poly_PolygonOnTriangulation) aPoly = BRep_Tool::PolygonOnTriangulation(actEdge, mesh, aLoc);
                    if (aPoly.IsNull())
                        assert(0); // polygon does not exist

                    // getting size and create the array
                    //nbNodesInFace = aPoly->NbNodes();
                    //vertices = new SbVec3f[nbNodesInFace];
                    
                    // geting the indexes of the edge polygon
                    const TColStd_Array1OfInteger& indices = aPoly->Nodes();
                    //const TColgp_Array1OfPnt& Nodes = aPolyTria->Nodes();
                    // go through the index array
                    for (Standard_Integer i=indices.Lower();i <= indices.Upper();i++) {
                        int inx = indices(i);
                        indxVector.push_back(FaceNodeOffset+inx-1);
                    }
                    indxVector.push_back(-1);


                    // remove the handled edge index from the set
                    edgeIdxSet.erase(idx);
                }
                
            }
            
            // counting up the per Face offsets
            FaceNodeOffset += nbNodesInFace;
            FaceTriaOffset += nbTriInFace;

        }
        // normalize all normals 
        for(int i = 0; i< nbrNodes ;i++)
            norms[i].normalize();
        
        // preset the index vector size
        endxVecSize =  indxVector.size();
        lineset ->coordIndex  .setNum(endxVecSize);

        int l=0;
        for(std::vector<int32_t>::const_iterator it=indxVector.begin();it!=indxVector.end();++it,l++)
            lineset ->coordIndex.set1Value(l,*it);

        // end the editing of the nodes
        coords  ->point       .finishEditing();
        norm    ->vector      .finishEditing();
        faceset ->coordIndex  .finishEditing();

    }
    catch (...){
        Base::Console().Error("Cannot compute Inventor representation for the shape of %s.\n",pcObject->getNameInDocument());
    }

    // printing some informations
    Base::Console().Message("ViewProvider update time: %f s\n",Base::TimeInfo::diffTimeF(start_time,Base::TimeInfo()));
    Base::Console().Message("Shape tria info: Faces:%d Edges:%d Nodes:%d Triangles:%d IdxVec:%d\n",nbrFaces,nbrEdges,nbrNodes,nbrTriangles,endxVecSize);

    VisualTouched = false;
}



