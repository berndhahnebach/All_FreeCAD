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
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "../../../Base/Console.h"

#include "ViewProvider.h"

#include "../App/PartFeature.h"
//#include "Tree.h"




using namespace PartGui;


//**************************************************************************
// Construction/Destruction

       
ViewProviderInventorPart::ViewProviderInventorPart()
{

}

ViewProviderInventorPart::~ViewProviderInventorPart()
{

}

SoNode* ViewProviderInventorPart::create(App::Feature *pcFeature)
{ 

  SoSeparator * SepShapeRoot=new SoSeparator();
  if(ComputeFaces(SepShapeRoot,dynamic_cast<Part::PartFeature*>(pcFeature)->GetShape()),0.1)
    return SepShapeRoot;
  else
    Base::Console().Error("View3DInventorEx::Update() Cannot compute Inventor representation for the actual shape");

  return 0l;

}


#define MAX2(X, Y)	(  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)	( MAX2 ( MAX2(X,Y) , Z) )


Standard_Boolean ViewProviderInventorPart::ComputeFaces(SoSeparator* root, const TopoDS_Shape &myShape, float deflection)
{
  const bool selection=true;


		if(deflection==0) {
			Bnd_Box B;
			BRepBndLib::Add(myShape, B);
			Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
			B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
			deflection = MAX3( aXmax-aXmin , aYmax-aYmin , aZmax-aZmin) * 0.001 *4;
		}

    {

			//----------------------------------
			// Use BrepMesh to mesh the surface
			//----------------------------------
    
			int error=0;
			int errortri=0;
			int error_dupl=0;
			BRepMesh_IncrementalMesh MESH(myShape,deflection);
			TopExp_Explorer ex;
      {
				for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next()) {
					const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
					TopLoc_Location aLoc;
					Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
					if(aPoly.IsNull()) error++;
					else {
      
						gp_Trsf myTransf;
						Standard_Boolean identity = true;
						if(!aLoc.IsIdentity())  {
							identity = false;
							myTransf = aLoc.Transformation();
						}

						Standard_Integer nbNodesInFace = aPoly->NbNodes();
						Standard_Integer nbTriInFace = aPoly->NbTriangles();
		
						SbVec3f* vertices = new SbVec3f[nbNodesInFace];
						int32_t* cons = new int32_t[4*(nbTriInFace)];
      
						const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
						const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
      
						Standard_Integer i;
						for(i=1;i<=nbTriInFace;i++) {
						// Get the triangle
	
							Standard_Integer N1,N2,N3;
							Triangles(i).Get(N1,N2,N3);
	
							gp_Pnt V1 = Nodes(N1);
							gp_Pnt V2 = Nodes(N2);
							gp_Pnt V3 = Nodes(N3);

							if(!identity) {
								V1.Transform(myTransf);
								V2.Transform(myTransf);
								V3.Transform(myTransf);
							}

							vertices[N1-1].setValue((float)(V1.X()),(float)(V1.Y()),(float)(V1.Z()));
							vertices[N2-1].setValue((float)(V2.X()),(float)(V2.Y()),(float)(V2.Z()));
							vertices[N3-1].setValue((float)(V3.X()),(float)(V3.Y()),(float)(V3.Z()));
	
							int j = i - 1;
							N1--; N2--; N3--;
							cons[4*j] = N1; cons[4*j+1] = N2; cons[4*j+2] = N3; cons[4*j+3] = SO_END_FACE_INDEX;
						}
      
						// define vertices
						SoCoordinate3 * coords = new SoCoordinate3;
						coords->point.setValues(0,nbNodesInFace, vertices);
						root->addChild(coords);
      
						// define the indexed face set

						if(selection) {
							SoLocateHighlight* h = new SoLocateHighlight();
              h->color.setValue((float)0.2,(float)0.2,(float)1);
							SoIndexedFaceSet * faceset = new SoIndexedFaceSet;
							faceset->coordIndex.setValues(0,4*nbTriInFace, cons);
							h->addChild(faceset);
							root->addChild(h);
						}
						else {
							SoIndexedFaceSet * faceset = new SoIndexedFaceSet;
							faceset->coordIndex.setValues(0,4*nbTriInFace, cons);
							root->addChild(faceset);
						}
      
						delete [] vertices;
						delete [] cons;
					}
//					nbFaces++;
//					ProgressBar1->setProgress(nbFaces);

				}
			}

			if(error) {
        Base::Console().Warning("Warning bad quality: %d faces cannot be meshed !\n",error);
			}
			if(errortri) {
        Base::Console().Warning("Warning bad quality: %d triangles degenerated !\n",errortri);
			}
			if(error_dupl) {
        Base::Console().Warning("Warning bad quality: %d ttriangles duplicated !\n",error_dupl);
			}
		
		}
	return 1;
}

