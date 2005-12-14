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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qlistview.h>
# include <Inventor/actions/SoWriteAction.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoDrawStyle.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoLineSet.h>
# include <Inventor/nodes/SoLocateHighlight.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoNormal.h>
# include <Inventor/nodes/SoNormalBinding.h>
# include <Inventor/nodes/SoPointSet.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoSwitch.h>
# include <BRepMesh_IncrementalMesh.hxx>
# include <BRep_Tool.hxx>
# include <GeomAPI_ProjectPointOnSurf.hxx>
# include <GeomLProp_SLProps.hxx>
# include <gp_Trsf.hxx>
# include <Poly_Array1OfTriangle.hxx>
# include <Poly_Triangulation.hxx>
# include <TColgp_Array1OfPnt.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Edge.hxx>
# include <TopoDS_Face.hxx>
# include <TopoDS_Shape.hxx>
# include <TopExp_Explorer.hxx>
# include <TopExp.hxx>
# include <Poly_PolygonOnTriangulation.hxx>
# include <TColStd_Array1OfInteger.hxx>
# include <TopTools_ListOfShape.hxx>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/Console.h>
#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/SoFCSelection.h>
#include <Gui/Selection.h>
#include <Gui/Tree.h>


#include "ViewProvider.h"

#include <Mod/Part/App/PartFeature.h>

#include <Poly_Polygon3D.hxx>
#include <BRepMesh.hxx>

#include "Icons/PartFeature.xpm"

//#include "Tree.h"



using namespace PartGui;


//**************************************************************************
// Construction/Destruction

       
ViewProviderPart::ViewProviderPart()
{
  EdgeRoot = new SoSeparator();
  EdgeRoot->ref();
  FaceRoot = new SoSeparator();
  FaceRoot->ref();
  VertexRoot = new SoSeparator();
  VertexRoot->ref();

  hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Part");

  fMeshDeviation      = hGrp->GetFloat("MeshDeviation",0.2);
  bNoPerVertexNormals = hGrp->GetBool("NoPerVertexNormals",false);
//  lHilightColor       = hGrp->GetInt ("HilightColor",0);
  bQualityNormals     = hGrp->GetBool("QualityNormals",false);

  sPixmap = "PartFeature";

}

ViewProviderPart::~ViewProviderPart()
{
  EdgeRoot->unref();
  FaceRoot->unref();
  VertexRoot->unref();

}

std::vector<std::string> ViewProviderPart::getModes(void)
{
  // get the modes of the father
  vector<string> StrList = ViewProviderFeature::getModes();

  // add your own modes
  StrList.push_back("Normal");
  StrList.push_back("Flat");
  StrList.push_back("Wireframe");
  StrList.push_back("Points");

  return StrList;
}

/*
void ViewProviderPart::update(const ChangeType& Reason)
{
  Reason;
  // set new view modes
  setMode(pcFeature->getShowMode());
  // copy the material properties of the feature
  setMatFromFeature();

}
*/
void ViewProviderPart::updateData(void)
{
  Base::Console().Log("ViewProviderPart::updateData() for %s called\n",pcFeature->getName()); 

  // geting actual setting values...
  fMeshDeviation      = hGrp->GetFloat("MeshDeviation",0.2);
  bNoPerVertexNormals = hGrp->GetBool("NoPerVertexNormals",false);
//  lHilightColor       = hGrp->GetInt ("HilightColor",0);
  bQualityNormals     = hGrp->GetBool("QualityNormals",false);


  // if the Feature not valid, do nothing at all
  if ( pcFeature->getStatus() !=  App::Feature::Valid )
    return; // feature is invalid

  TopoDS_Shape cShape = (dynamic_cast<Part::PartFeature*>(pcFeature))->getShape();

  // clear anchor nodes
  EdgeRoot->removeAllChildren();
  FaceRoot->removeAllChildren();
  VertexRoot->removeAllChildren();


  // creating the mesh on the data structure
  BRepMesh::Mesh(cShape,fMeshDeviation);
  //	BRepMesh_Discret MESH(fMeshDeviation,cShape,20.0,false,true,true);
	//BRepMesh_IncrementalMesh MESH(cShape,fMeshDeviation);

  try{
    computeFaces   (FaceRoot,cShape);
    computeEdges   (EdgeRoot,cShape);
    computeVertices(VertexRoot,cShape);
  } catch (...){
    Base::Console().Error("ViewProviderPart::create() Cannot compute Inventor representation for the actual shape");
  }

}

void ViewProviderPart::attach(App::Feature *pcFeat)
{
  // call father (set material and feature pointer)
  ViewProviderFeature::attach(pcFeat);

  // Build up the view represetation from the shape
  updateData();

  SoGroup* pcNormalRoot = new SoGroup();
  SoGroup* pcFlatRoot = new SoGroup();
  SoGroup* pcWireframeRoot = new SoGroup();
  SoGroup* pcPointsRoot = new SoGroup();


  // normal viewing with edges and points
  pcNormalRoot->addChild(FaceRoot);
  pcNormalRoot->addChild(EdgeRoot);
  pcNormalRoot->addChild(VertexRoot);

  // just faces with no edges or points
  pcFlatRoot->addChild(FaceRoot);

  // only edges
  pcWireframeRoot->addChild(EdgeRoot);
//  pcWireframeRoot->addChild(VertexRoot);

  // normal viewing with edges and points
  pcPointsRoot->addChild(VertexRoot);

   // puting all togetern with the switch
  pcModeSwitch->addChild(pcNormalRoot);
  pcModeSwitch->addChild(pcFlatRoot);
  pcModeSwitch->addChild(pcWireframeRoot);
  pcModeSwitch->addChild(pcPointsRoot);

  // standard viewing (flat)
  pcModeSwitch->whichChild = 0; 


}


#define MAX2(X, Y)	(  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)	( MAX2 ( MAX2(X,Y) , Z) )


// buffer acrobatics for inventor ****************************************************
//static char * buffer;
//static size_t buffer_size = 0;
//static std::string cReturnString;
/*
static void *
buffer_realloc(void * bufptr, size_t size)
{
  buffer = (char *)realloc(bufptr, size);
  buffer_size = size;
  return buffer;
}*/
/*
static const std::string&
buffer_writeaction(SoNode * root)
{
  SoOutput out;
  buffer = (char *)malloc(1024);
  buffer_size = 1024;
  out.setBuffer(buffer, buffer_size, buffer_realloc);

  SoWriteAction wa(&out);
  wa.apply(root);

  cReturnString =buffer;
  free(buffer);
  return cReturnString;
}
*/
// **********************************************************************************

Standard_Boolean ViewProviderPart::computeEdges   (SoSeparator* EdgeRoot, const TopoDS_Shape &myShape)
{
  TopExp_Explorer ex;

  EdgeRoot->addChild(pcLineMaterial);  
  EdgeRoot->addChild(pcLineStyle);

  // build up map edge->face
  TopTools_IndexedDataMapOfShapeListOfShape edge2Face;
  TopExp::MapShapesAndAncestors(myShape, TopAbs_EDGE, TopAbs_FACE, edge2Face);

  int i=0;
  for (ex.Init(myShape, TopAbs_EDGE); ex.More(); ex.Next(),i++)
  {
    // get the shape and mesh it
    const TopoDS_Edge& aEdge = TopoDS::Edge(ex.Current());

    // getting the transformation of the shape/face
    gp_Trsf myTransf;
    Standard_Boolean identity = true;
    TopLoc_Location aLoc;

    if(!aLoc.IsIdentity())  {
      identity = false;
      myTransf = aLoc.Transformation();
    }

    // try to triangulate the edge
    Handle(Poly_Polygon3D) aPoly = BRep_Tool::Polygon3D(aEdge, aLoc);

    SbVec3f* vertices;
    Standard_Integer nbNodesInFace;

    // triangulation succeeded?
    if( !aPoly.IsNull() )
    {
      // take the edge's triangulation
      //
      // getting size and create the array
      nbNodesInFace = aPoly->NbNodes();
      vertices = new SbVec3f[nbNodesInFace];

      const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();

      gp_Pnt V;
      for( Standard_Integer i=0;i < nbNodesInFace;i++ ) {
        V = Nodes(i+1);
        V.Transform(myTransf);
        vertices[i].setValue((float)(V.X()),(float)(V.Y()),(float)(V.Z()));
      }
    }
    else
    {
      // the edge has not its own triangulation, but then a face the edge is attached to
      // must provide this triangulation

      // Look for one face in our map (it doesn't care which one we take)
      const TopoDS_Face& aFace = TopoDS::Face(edge2Face.FindFromKey(aEdge).First());

      // take the face's triangulation instead
	    Handle(Poly_Triangulation) aPolyTria = BRep_Tool::Triangulation(aFace,aLoc);

      if(aPolyTria.IsNull()) // actually this shouldn't happen at all
        throw Base::Exception("Empty face trianglutaion\n");

      // this holds the indices of the edge's triangulation to the actual points
      Handle(Poly_PolygonOnTriangulation) aPoly = BRep_Tool::PolygonOnTriangulation(aEdge, aPolyTria, aLoc);

      // getting size and create the array
      nbNodesInFace = aPoly->NbNodes();
      vertices = new SbVec3f[nbNodesInFace];

      const TColStd_Array1OfInteger& indices = aPoly->Nodes();
      const TColgp_Array1OfPnt& Nodes = aPolyTria->Nodes();

      gp_Pnt V;
      int pos = 0;
      // go through the index array
      for( Standard_Integer i=indices.Lower();i <= indices.Upper();i++ ) {
        V = Nodes( indices(i) );
        V.Transform(myTransf);
        vertices[pos++].setValue((float)(V.X()),(float)(V.Y()),(float)(V.Z()));
      }
    }


    // define vertices
    SoCoordinate3 * coords = new SoCoordinate3;
    coords->point.setValues(0,nbNodesInFace, vertices);
    EdgeRoot->addChild(coords);

    // define the indexed face set
    Gui::SoFCSelection* h = new Gui::SoFCSelection();
    SbString name("Edge");
    name += SbString(i);
    h->featureName = pcFeature->getName();
    h->documentName = pcFeature->getDocument().getName();
    h->subElementName = name;

    SoLineSet * lineset = new SoLineSet;
    h->addChild(lineset);
    EdgeRoot->addChild(h);

  }

  return true;
}


Standard_Boolean ViewProviderPart::computeVertices(SoSeparator* VertexRoot, const TopoDS_Shape &myShape)
{
/*
  TopExp_Explorer ex;


  for (ex.Init(myShape, TopAbs_VERTEX); ex.More(); ex.Next()) {

    // get the shape
		const TopoDS_Vertex& aVertex = TopoDS::Vertex(ex.Current());

  }
*/
  return true;
}




Standard_Boolean ViewProviderPart::computeFaces(SoSeparator* FaceRoot, const TopoDS_Shape &myShape)
{
  TopExp_Explorer ex;

  FaceRoot->addChild(pcSolidMaterial);

//  BRepMesh::Mesh(myShape,1.0);
//	BRepMesh_Discret MESH(1.0,myShape,20.0);
	BRepMesh_IncrementalMesh MESH(myShape,fMeshDeviation);

  // counting faces and start sequencer
  int l = 1;
  for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next(),l++) {}
  Base::Sequencer().start("creating view representation", l);

  int i = 0;
  for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next(),i++) {

    // get the shape and mesh it
		const TopoDS_Face& aFace = TopoDS::Face(ex.Current());


    // this block mesh the face and transfers it in a C array of vertices and face indexes
		Standard_Integer nbNodesInFace,nbTriInFace;
		SbVec3f* vertices=0;
		SbVec3f* vertexnormals=0;
		long* cons=0;
    
    transferToArray(aFace,&vertices,&vertexnormals,&cons,nbNodesInFace,nbTriInFace);

    if(!vertices) break;

    if(!bNoPerVertexNormals)
    {
      // define normals (this is optional)
      SoNormal * norm = new SoNormal;
      norm->vector.setValues(0, nbNodesInFace, vertexnormals);
      FaceRoot->addChild(norm);

      // bind one normal per face
      SoNormalBinding * normb = new SoNormalBinding;
      normb->value = SoNormalBinding::PER_VERTEX_INDEXED;
      FaceRoot->addChild(normb);
    }

	  // define vertices
	  SoCoordinate3 * coords = new SoCoordinate3;
	  coords->point.setValues(0,nbNodesInFace, vertices);
	  FaceRoot->addChild(coords);

   // Turns on backface culling
//    SoShapeHints * hints = new SoShapeHints;
//    hints->vertexOrdering = SoShapeHints::CLOCKWISE ;
//    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE ;
//    hints->shapeType = SoShapeHints::SOLID;
//    hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
//    root->addChild(hints);

    //SoDrawStyle *Stype = new SoDrawStyle();
    //Stype->pointSize.setValue(3.0);
    //Stype->style.setValue( SoDrawStyle::POINTS );

    //SoPointSet *PtSet = new SoPointSet;
    //root->addChild(PtSet);

	  // define the indexed face set
		Gui::SoFCSelection* h = new Gui::SoFCSelection();
    SbString name("Face");
    name += SbString(i);
    h->featureName = pcFeature->getName();
    h->documentName = pcFeature->getDocument().getName();
    h->subElementName = name;

    SoIndexedFaceSet * faceset = new SoIndexedFaceSet;
		faceset->coordIndex.setValues(0,4*nbTriInFace,(const int*) cons);
		h->addChild(faceset);
		FaceRoot->addChild(h);

    
//    Base::Console().Log("Inventor tree:\n%s",buffer_writeaction(root).c_str());

		delete [] vertexnormals;
		delete [] vertices;
		delete [] cons;

    Base::Sequencer().next();

  } // end of face loop

  Base::Sequencer().stop();

  return true;
}

void ViewProviderPart::transferToArray(const TopoDS_Face& aFace,SbVec3f** vertices,SbVec3f** vertexnormals, long** cons,int &nbNodesInFace,int &nbTriInFace )
{
	TopLoc_Location aLoc;

  // doing the meshing and checking the result
	//BRepMesh_IncrementalMesh MESH(aFace,fDeflection);
	Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
  if(aPoly.IsNull()) throw Base::Exception("Empty face trianglutaion\n");
    
  // geting the transformation of the shape/face
	gp_Trsf myTransf;
	Standard_Boolean identity = true;
	if(!aLoc.IsIdentity())  {
	  identity = false;
		myTransf = aLoc.Transformation();
  }


  Standard_Integer i;
  // geting size and create the array
	nbNodesInFace = aPoly->NbNodes();
	nbTriInFace = aPoly->NbTriangles();
	*vertices = new SbVec3f[nbNodesInFace];
  *vertexnormals = new SbVec3f[nbNodesInFace];
  for(i=0;i < nbNodesInFace;i++) {
    (*vertexnormals)[i]= SbVec3f(0.0,0.0,0.0);
  }

	*cons = new long[4*(nbTriInFace)];

  // check orientation
  TopAbs_Orientation orient = aFace.Orientation();

  // cycling through the poly mesh
	const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
	const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
  for(i=1;i<=nbTriInFace;i++) {
  // Get the triangle

    Standard_Integer N1,N2,N3;
    Triangles(i).Get(N1,N2,N3);
    
    // change orientation of the triangles
    if ( orient != TopAbs_FORWARD )
    {
      Standard_Integer tmp = N1;
      N1 = N2;
      N2 = tmp;
    }

    gp_Pnt V1 = Nodes(N1);
    gp_Pnt V2 = Nodes(N2);
    gp_Pnt V3 = Nodes(N3);

    // transform the vertices to the place of the face
    if(!identity) {
      V1.Transform(myTransf);
      V2.Transform(myTransf);
      V3.Transform(myTransf);
    }

    if(!bNoPerVertexNormals)
    {
      // Calculate triangle normal
      gp_Vec v1(V1.X(),V1.Y(),V1.Z()),v2(V2.X(),V2.Y(),V2.Z()),v3(V3.X(),V3.Y(),V3.Z());
      gp_Vec Normal = (v2-v1)^(v3-v1); 

      //Standard_Real Area = 0.5 * Normal.Magnitude();

      // add the triangle normal to the vertex normal for all points of this triangle
      (*vertexnormals)[N1-1] += SbVec3f(Normal.X(),Normal.Y(),Normal.Z());
      (*vertexnormals)[N2-1] += SbVec3f(Normal.X(),Normal.Y(),Normal.Z());
      (*vertexnormals)[N3-1] += SbVec3f(Normal.X(),Normal.Y(),Normal.Z());
    }

    (*vertices)[N1-1].setValue((float)(V1.X()),(float)(V1.Y()),(float)(V1.Z()));
    (*vertices)[N2-1].setValue((float)(V2.X()),(float)(V2.Y()),(float)(V2.Z()));
    (*vertices)[N3-1].setValue((float)(V3.X()),(float)(V3.Y()),(float)(V3.Z()));

    int j = i - 1;
    N1--; N2--; N3--;
    (*cons)[4*j] = N1; (*cons)[4*j+1] = N2; (*cons)[4*j+2] = N3; (*cons)[4*j+3] = SO_END_FACE_INDEX;
  }

  // normalize all vertex normals
  for(i=0;i < nbNodesInFace;i++) {

    if(bQualityNormals)
    {
      gp_Dir clNormal;

      try {
        Handle_Geom_Surface Surface = BRep_Tool::Surface(aFace);

        gp_Pnt vertex((*vertices)[i][0], (*vertices)[i][1], (*vertices)[i][2]);
        GeomAPI_ProjectPointOnSurf ProPntSrf(vertex, Surface);
        Standard_Real fU, fV; ProPntSrf.Parameters(1, fU, fV);

        GeomLProp_SLProps clPropOfFace(Surface, fU, fV, 2, gp::Resolution());

        clNormal = clPropOfFace.Normal();
        SbVec3f temp = SbVec3f(clNormal.X(),clNormal.Y(),clNormal.Z());
        //Base::Console().Log("unterschied:%.2f",temp.dot((*vertexnormals)[i]));
        if ( temp.dot((*vertexnormals)[i]) < 0 )
          temp = -temp;
        (*vertexnormals)[i] = temp;

      }catch(...){}
    }else{
      (*vertexnormals)[i].normalize();
    }
  }
}

