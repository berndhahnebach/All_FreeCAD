/*************************************************************
**
** OPENCASCADE AND COIN3D VIEWER 1.0
** 
** SoBrepShape class
**
** http://www.opencascade.org   http://www.coin3d.org
**
** c-attanasio@opencascade.com
**
*************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <qfont.h>
# include <Standard.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <BRep_Builder.hxx>
# include <BRep_Tool.hxx>
# include <BRepTools.hxx>
# include <Bnd_Box.hxx>
# include <BRepBndLib.hxx>
# include <BRepBuilderAPI_NurbsConvert.hxx>
# include <BRepMesh_IncrementalMesh.hxx>
# include <Geom_BSplineSurface.hxx>
# include <Geom_Surface.hxx>
# include <TopExp_Explorer.hxx>
# include <TColStd_ListOfReal.hxx>
# include <TColStd_ListIteratorOfListOfReal.hxx>
# include <TColStd_Array1OfReal.hxx>
# include <TColgp_Array1OfPnt2d.hxx>
# include <TColgp_Array1OfPnt.hxx>
# include <TColgp_Array2OfPnt.hxx>
# include <Poly_Triangulation.hxx>
# include <gp_Pnt.hxx>
# include <gp_Pnt2d.hxx>
# include <Inventor/nodes/SoBaseColor.h>
# include <Inventor/nodes/SoComplexity.h>
# include <Inventor/nodes/SoCoordinate3.h>
# include <Inventor/nodes/SoCoordinate4.h>
# include <Inventor/nodes/SoIndexedFaceSet.h>
# include <Inventor/nodes/SoIndexedTriangleStripSet.h>
# include <Inventor/nodes/SoLocateHighlight.h>
# include <Inventor/nodes/SoMaterial.h>
# include <Inventor/nodes/SoMaterialBinding.h>
# include <Inventor/nodes/SoNurbsSurface.h>
# include <Inventor/nodes/SoSelection.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/nodes/SoShape.h>
# include <Inventor/nodes/SoShapeHints.h>
# include <Inventor/SoInput.h>
#endif

#include "SoBrepShape.h"

//OpenCascade 4.0 headers

#include "../Base/Console.h"


#define MAX2(X, Y)	(  Abs(X) > Abs(Y)? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)	( MAX2 ( MAX2(X,Y) , Z) )

/*******************************************
 *  Compute normal of vertex on a surface
 *******************************************/

void ComputeNormal(const BRepAdaptor_Surface& SF,
				   gp_Pnt& V1,
				   gp_Pnt2d& V1_2d,
				   gp_Vec Normal)
{
	gp_Vec T1,T2;
	// Compute normal
	SF.D1(V1_2d.X(),V1_2d.Y(),V1,T1,T2);
	Normal = T1.Crossed(T2);
	Standard_Real x = Normal.Magnitude();
	if (x > 1.e-10)
		Normal.Multiply(1/x);
	else {
		Normal.SetCoord(1/2.,0,0);
		cout << "Null normal"<< endl;
	}
}

/*******************************************
 *  Add a vertex with normal for strip algo
 *******************************************/

void AddVertex(int N1,
			   const BRepAdaptor_Surface& SF,
			   const TColgp_Array1OfPnt& Nodes,
			   const TColgp_Array1OfPnt2d& UVNodes,
			   Standard_Boolean identity,
			   gp_Trsf myTransf)
{
	gp_Pnt V1 = Nodes(N1);
	if(!identity) V1.Transform(myTransf);
	gp_Vec Normal(0,0,0);
	gp_Pnt2d P = UVNodes(N1);
	ComputeNormal(SF,V1,P,Normal);

	meshVec3 coord;
	meshVec3 norm;

	coord[0]=V1.X();
	coord[1]=V1.Y();
	coord[2]=V1.Z();

	norm[0]=Normal.X();
	norm[1]=Normal.Y();
	norm[2]=Normal.Z();

	int c_index = 0;
	meshVec4 c;
	int t_index = 0;
	meshVec3 t;

	meshAddVertex(N1,coord,norm,c_index,c,t_index,t);
}

/*******************************************  
 *  Constructor
 *******************************************/

SoBrepShape::SoBrepShape()
{
  filename = new char[500];
  nurbs_enable = 0;
  strip_enable = 1;
  strip_color = 0;
  nbstrips = 0;
  deflection = 0;
  selection = 1;
}

/*******************************************  
 *  Destructor
 *******************************************/

SoBrepShape::~SoBrepShape()
{
	delete [] filename;
}

/*******************************************  
 *  Set the filname to read
 *******************************************/

void SoBrepShape::SetFile(const char* aFileName)
{
	strcpy(filename,aFileName);
}

void SoBrepShape::SetShape(TopoDS_Shape cShape)
{
	myShape = cShape;
}

/*******************************************  
 *  Set Rendering options
 *******************************************/

void SoBrepShape::SetRenderOptions(int Strips,int Strip_Color,int Nurbs,int select)
{
  nurbs_enable = Nurbs;
  strip_enable = Strips;
  strip_color = Strip_Color;
  selection = select;
}	

/*******************************************  
 *  Reads the OpenCascade file
 *******************************************/

Standard_Boolean SoBrepShape::ReadShape()
{
	BRep_Builder aBuilder;
	return(BRepTools::Read(myShape,filename,aBuilder));
}

/*******************************************  
 *  Set deflection for mesher
 *******************************************/

void SoBrepShape::SetDeflection(float defle)
{
	deflection=defle;
}

/*******************************************  
 *  Return nb of strips
 *******************************************/

int SoBrepShape::GetNbStrips()
{
	return nbstrips;
}

/*******************************************  
 *  Compute2
 *******************************************/

Standard_Boolean SoBrepShape::Compute2(SoSeparator* root)
{
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

/*******************************************  
 *  Compute
 *******************************************/

Standard_Boolean SoBrepShape::Compute(SoSeparator* root)
{
//	if(ReadShape()) {
		if(deflection==0) {
			Bnd_Box B;
			BRepBndLib::Add(myShape, B);
			Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
			B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
			deflection = MAX3( aXmax-aXmin , aYmax-aYmin , aZmax-aZmin) * 0.001 *4;
		}

		if(nurbs_enable) {

			//-----------------------
			// Use OpenGL NURBS
			//-----------------------

			TopExp_Explorer ex;
			for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next()) {

				float* mySurfaceVKnots;
				float* mySurfaceUKnots;
				int myNbVKnots;
				int myNbUKnots;

				const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
				Handle(Geom_Surface) aGeomSurface = BRep_Tool::Surface(aFace);
				Handle(Geom_BSplineSurface) aBSplineSurface; 
				Standard_Boolean ok = false;

				if(aGeomSurface->DynamicType() != STANDARD_TYPE(Geom_BSplineSurface)) {
					// try to convert into nurbs
					BRepBuilderAPI_NurbsConvert nbscv;
					nbscv.Perform(aFace);
					if (nbscv.IsDone()){
						aGeomSurface = BRep_Tool::Surface(TopoDS::Face(nbscv.Shape()));
						ok = true;
					}
				}
				else ok = true;


				if(ok) {
					aBSplineSurface = Handle(Geom_BSplineSurface)::DownCast(aGeomSurface);
	
					int myNbUPoles = aBSplineSurface->NbUPoles();
					int myNbVPoles = aBSplineSurface->NbVPoles();

					TColgp_Array2OfPnt aPoles(1, myNbVPoles, 1, myNbUPoles);
	
					TColStd_Array1OfReal aVKnots(1, aBSplineSurface->NbVKnots());
					TColStd_Array1OfReal aUKnots(1, aBSplineSurface->NbUKnots());
	
					aBSplineSurface->Poles(aPoles);
					aBSplineSurface->UKnots(aUKnots);
					aBSplineSurface->VKnots(aVKnots);
	
					//Push the nurbs in Coin3d
	
					// Control Point
					Standard_Integer i, j, aCounter;
					int aLength = myNbVPoles*myNbUPoles; //Create array of control points and their weights
	
					SbVec4f* mySurfaceCtrlPoints = new SbVec4f[aLength];
					if(mySurfaceCtrlPoints == NULL) return Standard_False;
	
					aCounter = -1;
	
					for(j = 1; j <= myNbVPoles; j++) {
						for(i = 1; i <= myNbUPoles; i++) {
							const gp_Pnt& aPoint = aBSplineSurface->Pole(i, j); //Control point (U,V)
							mySurfaceCtrlPoints[++aCounter].setValue((float)(aPoint.X()),(float)(aPoint.Y()),(float)(aPoint.Z()),(float)(aBSplineSurface->Weight(i, j)));
						}
					}
	
	
					TColStd_ListOfReal aListOfKnot;
					TColStd_ListIteratorOfListOfReal aListOfKnotIterator;
	
					try {
						//Fill the knot`s array taking into account multiplicities
						// VKnots
						for(i = aVKnots.Lower(); i<=aVKnots.Upper(); i++) {
							for(j = 1; j<= aBSplineSurface->VMultiplicity(i); j++)
							aListOfKnot.Append(aVKnots(i));
						}
	  
						mySurfaceVKnots = new float[aListOfKnot.Extent()]; 
						if(mySurfaceVKnots == NULL) return Standard_False;
	  
						aCounter = -1;    
						for(aListOfKnotIterator.Initialize(aListOfKnot); aListOfKnotIterator.More(); aListOfKnotIterator.Next()) {
							mySurfaceVKnots[++aCounter] = (float)(aListOfKnotIterator.Value());
						}
						myNbVKnots = aCounter+1;
	  
						// UKnots
						aListOfKnot.Clear();
						for(i = aUKnots.Lower(); i<=aUKnots.Upper(); i++) {
							for(j = 1; j<= aBSplineSurface->UMultiplicity(i); j++)
							aListOfKnot.Append(aUKnots(i));
						}
	  
						mySurfaceUKnots =  new float[aListOfKnot.Extent()];
						if(mySurfaceUKnots == NULL) return Standard_False;
	  
						aCounter = -1;
						for(aListOfKnotIterator.Initialize(aListOfKnot); aListOfKnotIterator.More(); aListOfKnotIterator.Next()) {
							mySurfaceUKnots[++aCounter] = (float)(aListOfKnotIterator.Value());
						}	
						myNbUKnots = aCounter+1;
					}
					catch(Standard_Failure) {
						continue;
					}
	
					//Nurbs definition
					SoNurbsSurface* surface = new SoNurbsSurface;
					SoCoordinate4 *controlPts = new SoCoordinate4;
					SoComplexity *comp = new SoComplexity;
					comp->type.setValue(SoComplexity::OBJECT_SPACE);
					comp->value = 0.5;
					controlPts->point.setValues(0,aLength,mySurfaceCtrlPoints);
					surface->numUControlPoints.setValue(myNbUPoles);
					surface->numVControlPoints.setValue(myNbVPoles);
					surface->uKnotVector.setValues(0, myNbUKnots, mySurfaceUKnots);
					surface->vKnotVector.setValues(0, myNbVKnots, mySurfaceVKnots);
					root->addChild(comp);
					root->addChild(controlPts);
					root->addChild(surface);

					delete [] mySurfaceVKnots;
					delete [] mySurfaceUKnots;

				}      
			}
		}
		else {

			//----------------------------------
			// Use BrepMesh to mesh the surface
			//----------------------------------
    
			int error=0;
			int errortri=0;
			int error_dupl=0;
			BRepMesh_IncrementalMesh MESH(myShape,deflection);
			TopExp_Explorer ex;
			/*int nbFaces = 0;
			for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next()) {
				nbFaces++;
			}
			ProgressBar1->setTotalSteps(nbFaces);
			nbFaces=0;*/

			if(strip_enable) {
				//------------------------------
				// Use stripifieur
				//------------------------------

				for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next()) {
					const TopoDS_Face& aFace = TopoDS::Face(ex.Current());
					BRepAdaptor_Surface SF(aFace);
					TopLoc_Location aLoc;
					Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);

					gp_Trsf myTransf;
					Standard_Boolean identity = true;
					if(!aLoc.IsIdentity())  {
						identity = false;
						myTransf = aLoc.Transformation();
					}

					SoSeparator* sep = new SoSeparator();
					if(selection) {
						SoLocateHighlight* h = new SoLocateHighlight();
						sep = h;
					}
					root->addChild(sep);

					if(aPoly.IsNull()) error++;
					else {
      
						Standard_Integer nbNodesInFace = aPoly->NbNodes();
						Standard_Integer nbTriInFace = aPoly->NbTriangles();

						const Poly_Array1OfTriangle& Triangles = aPoly->Triangles();
						const TColgp_Array1OfPnt& Nodes = aPoly->Nodes();
						const TColgp_Array1OfPnt2d& UVNodes = aPoly->UVNodes();
						TColStd_SequenceOfInteger theStrips;

						Standard_Integer i;
						meshReset();

						//coin3d vertices
						SbVec3f* vertices = new SbVec3f[nbNodesInFace];

						//-------------------------
						// Build the strips
						//-------------------------
						
						Standard_Integer pN1,pN2,pN3;
						pN1 = 0; pN2 = 0; pN3 = 0;

						for(i=1;i<=nbTriInFace;i++) {
	
							Standard_Integer N1,N2,N3;
							Triangles(i).Get(N1,N2,N3);	
							
							AddVertex(N1,SF,Nodes,UVNodes,identity,myTransf);
							AddVertex(N2,SF,Nodes,UVNodes,identity,myTransf);
							AddVertex(N3,SF,Nodes,UVNodes,identity,myTransf);
	
							if(N1!=N2 && N1!=N3 && N2!=N3) {
								gp_Pnt P1 = Nodes(N1);
								gp_Pnt P2 = Nodes(N2);
								gp_Pnt P3 = Nodes(N3);

								if(!identity) {
									P1.Transform(myTransf);
									P2.Transform(myTransf);
									P3.Transform(myTransf);
								}

								vertices[N1-1].setValue((float)(P1.X()),(float)(P1.Y()),(float)(P1.Z()));
								vertices[N2-1].setValue((float)(P2.X()),(float)(P2.Y()),(float)(P2.Z()));
								vertices[N3-1].setValue((float)(P3.X()),(float)(P3.Y()),(float)(P3.Z()));

								meshAddTriangle(N1,N2,N3);
							}
							else errortri++;
							pN1=N1; pN2=N2; pN3=N3;

						}
						meshSetFlag(MESH_PRINTSTRIPCONTENTS,1);
						meshCreateStrips(TRUE,theStrips);

						// Send strip to coin3d
						//---------------------

						if(strip_color) {
							//--------------------------------
							// Paint each strip with a color
							//--------------------------------

							// define vertices
							SoCoordinate3 * coords = new SoCoordinate3;
							coords->point.setValues(0,nbNodesInFace, vertices);
							sep->addChild(coords);
				
							Standard_Integer length = theStrips.Length();
							TColStd_SequenceOfInteger aStrip;

							for(i=1;i<=length;i++) {
								Standard_Integer vertexId = theStrips(i);
								if(vertexId!=0) aStrip.Append(vertexId);
								else { 
									nbstrips++;
									Standard_Integer LStrip = aStrip.Length();
									int32_t* cons = new int32_t[LStrip+1];

									for(int j=1;j<=LStrip;j++) {
										Standard_Integer vertexId2 = aStrip(j);
										cons[j-1]=vertexId2-1;
									}
									cons[LStrip]=SO_END_FACE_INDEX;

									// define the indexed strip set
									SoSeparator* asepStrip = new SoSeparator();

									SoBaseColor* aRandomColor = new SoBaseColor();
									(aRandomColor->rgb).setValue(((float)rand())/RAND_MAX,((float)rand())/RAND_MAX,((float)rand())/RAND_MAX);
									asepStrip->addChild(aRandomColor);

									SoIndexedTriangleStripSet* coinStrips = new SoIndexedTriangleStripSet();
									coinStrips->coordIndex.setValues(0,LStrip+1,cons);
									asepStrip->addChild(coinStrips);

									sep->addChild(asepStrip);
									aStrip.Clear();
									delete [] cons;
								}
							}
							delete [] vertices;
						}
						else {
							//------------------------------------
							// All strip have the same properties
							//------------------------------------

							Standard_Integer length = theStrips.Length();
							int32_t* cons = new int32_t[length];

							for(i=1;i<=length;i++) {
								Standard_Integer vertexId = theStrips(i);
								if(vertexId!=0) cons[i-1]=vertexId-1;
								else { cons[i-1]=SO_END_FACE_INDEX; nbstrips++;}
							}

							// define vertices
							SoCoordinate3 * coords = new SoCoordinate3;
							coords->point.setValues(0,nbNodesInFace, vertices);
							sep->addChild(coords);
      
							// define the indexed strip set
							SoIndexedTriangleStripSet* coinStrips = new SoIndexedTriangleStripSet();
							coinStrips->coordIndex.setValues(0,length, cons);
							sep->addChild(coinStrips);
      
							delete [] vertices;
							delete [] cons;
						}

					}
				}
			}
			else {
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
				QString Qnberrors;
				Qnberrors.setNum(error,10);
				QMessageBox::information(0, "Warning bad quality:",Qnberrors + " faces cannot be meshed !");
			}
			if(errortri) {
				QString Qnberrors;
				Qnberrors.setNum(errortri,10);
				QMessageBox::information(0, "Warning bad quality:",Qnberrors + " triangles degenerated !");
			}
			if(error_dupl) {
				QString Qnberrors;
				Qnberrors.setNum(error_dupl,10);
				QMessageBox::information(0, "Warning bad quality:",Qnberrors + " triangles duplicated !");
			}

			
		}
/*	}
	else {
		// BRep file cannot be read !!
	    QMessageBox::information(0, "Error:","Input file cannot be read, rendering abort !");
	    return 0;
	}*/
	return 1;
}



/*

This is a simple stripifier using basic SGI style algorithms appropriate for OpenGL (as opposed to
IRISGL) on the grounds that the complexity of the materials, smoothing groups etc liable to be found
in real data can make sophisticated algorithms little more effective than basic ones.

The source is a modified version of code written by Brad Grantham of SGI.

Changes for this version were made by Neal Tringham, at VX.

The original files are marked "Copyright 1995, 1996 Brad Grantham.
All rights reserved."  I have the right to distribute my changed version 
of the source, but anyone who wants to add further changes and distribute
the result should contact Brad Grantham directly.  (I waive any rights I
may have to my changes, though it would be nice if anyone who used this 
version gave me a mention in the credits somewhere:-))

Brad Grantham's email address is "grantham@hadron.org" (current as of 
February 2000) and his "Meshifier" web pages are at: 
http://tofu.alt.net/~grantham/meshifier/show.html

The core of the stripification algorithm is:

		1) Pick a triangle which is one of the least connected in the merge group.

		2) Walk along a strip formed by picking the next connected triangle (this is done using
			a database of shared edges) until you reach a triangle with no forwards connections.

		3) Reverse the current strip.

		4) Start sgain at the new end (the original start) and walk in the other direction until 
			you again reach an unconnected triangle.

Note that to support single sided rendering all output strips have an anti-clockwise winding (i.e. the first 3 vertices form
an anti-clockwise triangle and later vertices produce triangles of alternate winding).  
In order to support this behaviour, the winding is determined for each triangle
in the strips obtained using the algorithm above, and if the final start triangle (end of the first walk)
is clockwise, the algorithm either "backs up one" to invert the winding or (if there was
only one triangle in the strip) abandons the current strip and produces a
one triangle strip with guaranteed anti-clockwise winding.

*/

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cmath>



/* Vertices */
static int		vertcnt;
static meshifierVertex	verts[MAX_TRIS * 3];


/* Triangles */
static int		tricnt;
static meshifierTriangle	ts[MAX_TRIS];


/* Edges */
static int		edgecnt;
static meshifierEdge		edges[MAX_TRIS * 3];

static int 		doDrawStripOutlines = 0;
static int 		doDrawConnectivity = 0;
static int 		doPrintStripContents = 0;
static int 		doPrintAddTriangles = 0;


static void nullText(REAL v[3], char *text)
{
}


static void nullLine(REAL v1[3], REAL v2[3])
{
}


static void nullVertPair(meshifierVertex *v1, meshifierVertex *v2)
{
}


static void nullContStrip(meshifierVertex *v)
{
}


static void nullEndStrip(void)
{
}


static meshDrawTextProc		drawText =		nullText;
static meshDrawLineProc		drawLine =		nullLine;
static meshDrawEdgeProc		drawEdge =		nullVertPair;
static meshBeginStripProc	drawBeginStrip =	nullVertPair;
static meshContStripProc	drawContStrip =		nullContStrip;
static meshEndStripProc		drawEndStrip =		nullEndStrip;


void meshSetProcs(meshDrawTextProc dt, meshDrawLineProc dl, meshDrawEdgeProc de,
    meshBeginStripProc bs, meshContStripProc cs, meshEndStripProc es)
{
    drawText = (dt == NULL) ? nullText : dt;
    drawLine = (dl == NULL) ? nullLine : dl;
    drawEdge = (de == NULL) ? nullVertPair : de;
    drawBeginStrip = (bs == NULL) ? nullVertPair : bs;
    drawContStrip = (cs == NULL) ? nullContStrip : cs;
    drawEndStrip = (es == NULL) ? nullEndStrip : es;
}


void meshSetFlag(int flag, int value)
{
    switch(flag)
    {
        case MESH_DRAWSTRIPOUTLINES:
	    doDrawStripOutlines = value; break;

        case MESH_DRAWCONNECTIVITY:
	    doDrawConnectivity = value; break;

        case MESH_PRINTSTRIPCONTENTS:
	    doPrintStripContents = value; break;

        case MESH_PRINTADDTRIANGLES:
	    doPrintAddTriangles = value; break;
    }
}


static int notSharedVertex(int	t1, int	t2)
{
    if((ts[t1].v[0] != ts[t2].v[0])  &&
        (ts[t1].v[0] != ts[t2].v[1])   &&
        (ts[t1].v[0] != ts[t2].v[2]))
    {
        return(ts[t1].v[0]);
    }

    if((ts[t1].v[1] != ts[t2].v[0])  &&
        (ts[t1].v[1] != ts[t2].v[1])   &&
        (ts[t1].v[1] != ts[t2].v[2]))
    {
        return(ts[t1].v[1]);
    }

    /*
     * Well, must be the last one; if they shared all three, then they 
     *  would be the same triangle.
     */

    return(ts[t1].v[2]);
}


static int firstSharedVertex(int t1, int t2)
{
    if((ts[t1].v[0] == ts[t2].v[0])  ||
        (ts[t1].v[0] == ts[t2].v[1]) ||
        (ts[t1].v[0] == ts[t2].v[2]))
    {
        return(ts[t1].v[0]);
    }

    if((ts[t1].v[1] == ts[t2].v[0]) ||
        (ts[t1].v[1] == ts[t2].v[1]) ||
        (ts[t1].v[1] == ts[t2].v[2]))
    {
        return(ts[t1].v[1]);
    }

    /*
     * Well, can't be either; if this was the first shared, it's the ONLY
     * one shared.
     */

	return MESHIFIER_ERROR;
}


static int secondSharedVertex(int t1, int t2)
{
    if((ts[t1].v[2] == ts[t2].v[0])  ||
        (ts[t1].v[2] == ts[t2].v[1]) ||
        (ts[t1].v[2] == ts[t2].v[2]))
    {
        return(ts[t1].v[2]);
    }

    if((ts[t1].v[1] == ts[t2].v[0]) ||
        (ts[t1].v[1] == ts[t2].v[1]) ||
        (ts[t1].v[1] == ts[t2].v[2]))
    {
        return(ts[t1].v[1]);
    }

    /*
     * Well, can't be either; if we think the first vertex was the
     * SECOND shared, something is way out of whack.
     */
	return MESHIFIER_ERROR;
}



static int triangleUsesVertex(int t, int v)
{
    if((v == ts[t].v[0]) || (v == ts[t].v[1]) || (v == ts[t].v[2]))
        return(1);
    return(0);
}


static int followStrip(int strip[], int *count, int last, int* final)
{
    int	i;
    int	next;

	// winding = 0 is clockwise, winding = 1 is anti-clockwise
	int winding;

    while(*count < MAX_STRIP){

        /* find next triangle to add */
        next = -1;
        if(*count == 0){
                /* for 2nd tri, just pick one. */
            for(i = 0; i < ts[last].tcnt; i++)
                if(!ts[ts[last].t[i]].done){
                    next = ts[last].t[i];
                    break;
                }
            if(next != -1) {

                strip[0] = notSharedVertex(last, next);
                strip[1] = firstSharedVertex(last, next);

				if (strip[1] == MESHIFIER_ERROR)
					{
					return strip[1];
					}

                strip[2] = secondSharedVertex(last, next);

				if (strip[2] == MESHIFIER_ERROR)
					{
					return strip[2];
					}

				// decode initial winding
				if ((strip[0] == ts[last].v[0]) &&
					(strip[1] == ts[last].v[1]) &&
					(strip[2] == ts[last].v[2]))
					{
					winding = 1; // anti-clock
					}
				else if ((strip[0] == ts[last].v[1]) &&
						 (strip[1] == ts[last].v[0]) &&
						 (strip[2] == ts[last].v[2]))
					{
					winding = 0; // clock
					}		
				else if ((strip[0] == ts[last].v[2]) &&
						 (strip[1] == ts[last].v[0]) &&
						 (strip[2] == ts[last].v[1]))
					{
					winding = 1; // anti-clock
					}		
				else
					{
//					printf("winding analysis failure\n");
					exit(1);
					}

                *count = 3;
            }else{
                strip[0] = ts[last].v[0];
                strip[1] = ts[last].v[1];
                strip[2] = ts[last].v[2];

				winding = 1; // anti-clock
                *count = 3;
            }
        }else{
            /* third and later tris must share prev two verts */
            for(i = 0; i < ts[last].tcnt; i++)
                if(!ts[ts[last].t[i]].done &&
                    triangleUsesVertex(ts[last].t[i], strip[*count - 2]) &&
                    triangleUsesVertex(ts[last].t[i], strip[*count - 1]))
                {
                    next = ts[last].t[i];
                    break;
                }
        }

        if(next == -1)
            break;

//	if(doPrintAddTriangles)
//	    fprintf(stdout, "adding %d to strip.\n", next);

        strip[*count] = notSharedVertex(next, last);

		// alternate winding
		winding ^= 1;

        *count = *count + 1;

        ts[next].done = 1;

        last = next;
    }

	*final = last;

	return winding; // no error
}


static int getNextStrip(int start, int *cnt, int strip[], bool MakeStrips)
{
    int	i;
    int	istrip[MAX_STRIP];
	int final;
    int 	min, mincnt;

	if (MakeStrips)
	{
    /* do slightly more wise strips; pick one of least connected
        triangles */

    mincnt = 4;
    min = -1;	/* so compiler doesn't complain */
    for(i = 0; i < tricnt; i++)
        if((! ts[i].done) && (ts[i].tcnt < mincnt))
	{
	    min = i;
	    mincnt = ts[i].tcnt;
        }

    if(min != -1)
    {
        start = min;
    }
    else
    {
		return MESHIFIER_ERROR;
    }

    ts[start].done = 1;
    *cnt = 0;

	int winding;

    winding = followStrip(istrip, cnt, start, &final);

	if (winding == MESHIFIER_ERROR)
		{
		return winding;
		}

    for(i = 0; i < *cnt; i++)
        strip[i] = istrip[*cnt - i - 1];

	// winding should be inverted by strip reversal
	winding ^= 1;

	// the value of winding is the value of the winding order
	// for the final triangle in the strip, which is the start
	// after the reversal of the pass 1 data

	// if winding is clockwise, back up one and undo "done" marker on last triangle...
	// backing up should flip winding automatically
	if (winding == 0)
		{
		// test for infinite looping...
		if (final != start)
			{
			// recopy istrip into strip, starting at i=1 to allow
			// for skipping old end (new start)
		    for(i = 1; i < *cnt; i++)
				{
    		    strip[i-1] = istrip[*cnt - i - 1];
				}

			// back up count
			*cnt -= 1;

			// mark triangle corresponding to backed up vertex as not done
			ts[final].done = 0;

			// pass 2 - follow reversed strip out in the other direction
		    winding = followStrip(strip, cnt, start, &final);

			if (winding == MESHIFIER_ERROR)
				{
				return winding;
				}
			}
		else
			{
			// rebuild single triangle strip in anti-clockwise order
			strip[0] = ts[start].v[0];
			strip[1] = ts[start].v[1];
			strip[2] = ts[start].v[2];

			}
		}
	else
		{
		// pass 2 - follow reversed strip out in the other direction
		winding = followStrip(strip, cnt, start, &final);

		if (winding == MESHIFIER_ERROR)
			{
			return winding;
			}
		}
	}
	else
	{

    *cnt = 3;
    strip[0] = ts[start].v[0];
    strip[1] = ts[start].v[1];
    strip[2] = ts[start].v[2];
    ts[start].done = 1;

	}

	return 0; // no error
}


int meshCreateStrips(bool MakeStrips,TColStd_SequenceOfInteger& Strips)
{
    int		done;
    int		first;
    int		sl, sv[MAX_STRIP*3];
    int		i, j;
    int		meshcnt;

    if(doDrawConnectivity)
    {
	meshVec3	v1, v2;
	char		str[512];
	int		t;



	for(i = 0; i < tricnt; i++){
	    for(j = 0; j < ts[i].tcnt; j++){

		t = ts[i].t[j];

		v1[Xi] = (verts[ts[i].v[0]].v[Xi] + verts[ts[i].v[1]].v[Xi] +
		    verts[ts[i].v[2]].v[Xi]) / 3;
		v1[Yi] = (verts[ts[i].v[0]].v[Yi] + verts[ts[i].v[1]].v[Yi] +
		    verts[ts[i].v[2]].v[Yi]) / 3;
		v1[Zi] = (verts[ts[i].v[0]].v[Zi] + verts[ts[i].v[1]].v[Zi] +
		    verts[ts[i].v[2]].v[Zi]) / 3;

		v2[Xi] = (verts[ts[t].v[0]].v[Xi] + verts[ts[t].v[1]].v[Xi] +
		    verts[ts[t].v[2]].v[Xi]) / 3;
		v2[Yi] = (verts[ts[t].v[0]].v[Yi] + verts[ts[t].v[1]].v[Yi] +
		    verts[ts[t].v[2]].v[Yi]) / 3;
		v2[Zi] = (verts[ts[t].v[0]].v[Zi] + verts[ts[t].v[1]].v[Zi] +
		    verts[ts[t].v[2]].v[Zi]) / 3;

//		sprintf(str, "%d", t);

		drawText(v1, str);
		drawLine(v1, v2);
	    }
	}
    }

    done = 0;

    meshcnt = 0;
	//FILE* fd = fopen("stip.data","w");

    do{
        /* Find out if we are done (no more triangles left) and */
        /*  if so, use that first triangle as a starting suggestion */
        for(first = 0; ts[first].done && first < tricnt; first++);

        if(first == tricnt){
            break;	/* all done; no more triangles */
        }

        /* extract the next strip out of the data set */
        int ret = getNextStrip(first, &sl, sv, MakeStrips);

		if (ret == MESHIFIER_ERROR)
			{
			return ret;
			}

        meshcnt ++;

	if(doPrintStripContents)
	{

		//fprintf(fd, "Strip:");

	    for(i = 0; i < sl; i++){
        Strips.Append(sv[i]);
		//fprintf(fd, " %d", sv[i]);
	    }
        Strips.Append(0);
	    //fprintf(fd, "\n");
	}

        /* draw the triangles in each strip */
	drawBeginStrip(&verts[sv[0]], &verts[sv[1]]);

        for(i = 2; i < sl; i++)
	    drawContStrip(&verts[sv[i]]);

	drawEndStrip();

	if(doDrawStripOutlines)
	{
	    /* draw the outline of each strip */
	    drawEdge(&verts[sv[0]], &verts[sv[1]]);
	    for(i = 0; i < sl - 2; i++){
		drawEdge(&verts[sv[i]], &verts[sv[i + 2]]);
		if(i < sl - 3)
		    drawEdge(&verts[sv[i + 1]], &verts[sv[i + 3]]);
	    }
	    drawEdge(&verts[sv[sl - 2]], &verts[sv[sl - 1]]);
	}

    }while(!done);

	//fclose(fd);
    return(meshcnt);
}


static void addEdge(int tnum, int vnum1, int vnum2)
{
    meshifierTriangle		*t, *t2;
    meshifierVertex		*v1, *v2;
    int			e;	/* index of edge record */

    t = &ts[tnum];
    v1 = &verts[vnum1];
    v2 = &verts[vnum2];

    /* set up edge between v1 and v2 unless one exists */

    for(e = 0; (e < v1->conncnt) && (v1->conn[e][0] != vnum2); e++);

    if(e < v1->conncnt){
            /* found existing edge */
        e = v1->conn[e][1];
        edges[e].t[1] = tnum;
        t2 = &ts[edges[e].t[0]];
        t->t[t->tcnt++] = edges[e].t[0];
        t2->t[t2->tcnt++] = tnum;

    }else{
            /* have to make new edge */

        e = edgecnt++;
        v1->conn[v1->conncnt][0] = vnum2;
        v1->conn[v1->conncnt++][1] = e;
        v2->conn[v2->conncnt][0] = vnum1;
        v2->conn[v2->conncnt++][1] = e;
        edges[e].t[0] = tnum;
    }
}


static void prepareTriangle(int t)
{
    /* set up edge connectivity */

    addEdge(t, ts[t].v[0], ts[t].v[1]);
    addEdge(t, ts[t].v[1], ts[t].v[2]);
	addEdge(t, ts[t].v[2], ts[t].v[0]);
}


void meshReset(void)
{
	// code does not reset properly; emulate start conditions for
	// program run (bss segment cleared)
	// no doubt this could be done rather more elegantly and faster,
	// but it is a tool and this will work...
	memset(&verts[0], 0, vertcnt * sizeof(meshifierVertex));
	memset(&ts[0], 0, tricnt * sizeof(meshifierTriangle));
	memset(&edges[0], 0, edgecnt * sizeof(meshifierEdge));

    vertcnt = 0;
    edgecnt = 0;
    tricnt = 0;
}


void meshAddVertex(int v_index, meshVec3 v, meshVec3 n, int c_index, 
					meshVec4 c, int t_index, meshVec3 t)
{
	// pass through to make Neal's stuff easier...
	verts[vertcnt].v_index = v_index;
	verts[vertcnt].c_index = c_index;
	verts[vertcnt].t_index = t_index;

    verts[vertcnt].v[Xi] = v[Xi];
    verts[vertcnt].v[Yi] = v[Yi];
    verts[vertcnt].v[Zi] = v[Zi];

    verts[vertcnt].n[Xi] = n[Xi];
    verts[vertcnt].n[Yi] = n[Yi];
    verts[vertcnt].n[Zi] = n[Zi];

    verts[vertcnt].c[Ri] = c[Ri];
    verts[vertcnt].c[Gi] = c[Gi];
    verts[vertcnt].c[Bi] = c[Bi];
    verts[vertcnt].c[Ai] = c[Ai];

    verts[vertcnt].t[Ri] = t[Ri];
    verts[vertcnt].t[Si] = t[Si];
    verts[vertcnt].t[Ti] = t[Ti];

    verts[vertcnt].conncnt = 0;

    vertcnt ++;
}


void meshAddTriangle(int v1, int v2, int v3)
{
    ts[tricnt].v[0] = v1;
    ts[tricnt].v[1] = v2;
    ts[tricnt].v[2] = v3;
    ts[tricnt].done = 0;

    prepareTriangle(tricnt);

    tricnt ++;
}
