/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
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
# include <qaction.h>
# include <AIS_InteractiveContext.hxx>
# include <BRep_Tool.hxx>
# include <BRepMesh_IncrementalMesh.hxx>
# include <BRepPrimAPI_MakeBox.hxx>
# include <GeomAPI_ProjectPointOnSurf.hxx>
# include <GeomLProp_SLProps.hxx>
# include <gp_Pnt.hxx>
# include <Poly_Triangulation.hxx>
# include <TNaming_Builder.hxx>
# include <TopExp_Explorer.hxx>
# include <TopoDS.hxx>
# include <TopoDS_Shape.hxx>
#endif

#include <Base/Exception.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Command.h>
#include <Gui/FileDialog.h>
#include <Gui/View.h>
#include <Gui/ProgressBar.h>


#include <Inventor/SoInput.h>
//#include <Inventor/SoCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>


#include <Mod/Part/App/PartFeature.h>
  
#include "FreeCADpov.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdRaytracingWriteCamera
//===========================================================================
DEF_STD_CMD_A(CmdRaytracingWriteCamera);

CmdRaytracingWriteCamera::CmdRaytracingWriteCamera()
  :CppCommand("Raytracing_WriteCamera")
{
  sAppModule    = "Raytracing";
  sGroup        = "Raytracing";
  sMenuText     = "Write camera position";
  sToolTipText  = "Write the camera positon of the active 3D view in PovRay format to a file";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Test1";
  iAccel        = 0;
}

void CmdRaytracingWriteCamera::activated(int iMsg)
{
  const char* ppReturn=0;

  getAppWnd()->sendMsgToActiveView("GetCamera",&ppReturn);

  Base::Console().Log("GetCamera MSG send:\n%s",ppReturn);

  SoInput in;
  in.setBuffer((void*)ppReturn,strlen(ppReturn));

  //if (!in.openFile(filename)) { exit(1); }

  SoCamera * Cam;
  SoDB::read(&in,(SoNode*&)Cam);

  if (!Cam)
    throw Base::Exception("CmdRaytracingWriteCamera::activated(): Could not read Camera information from ASCII stream....\n");

  // root-node returned from SoDB::readAll() has initial zero
  // ref-count, so reference it before we start using it to
  // avoid premature destruction.
  Cam->ref();

  SbRotation camrot = Cam->orientation.getValue();

  SbVec3f upvec(0, 1, 0); // init to default up vector
  camrot.multVec(upvec, upvec);

  SbVec3f lookat(0, 0, -1); // init to default view direction vector
  camrot.multVec(lookat, lookat);

  SbVec3f pos = Cam->position.getValue();

  float Dist = Cam->focalDistance.getValue();


  std::stringstream out;
  out << "// declares positon and view direction\n"
      << "#declare  CamPos = <" << pos.getValue()[0]    <<"," << pos.getValue()[2]    <<"," << pos.getValue()[1]    <<">;\n" 
      << "#declare  CamDir = <" << lookat.getValue()[0] <<"," << lookat.getValue()[2] <<"," << lookat.getValue()[1] <<">;\n" ;
  lookat *= Dist;
  lookat += pos;
  out << "#declare  LookAt = <" << lookat.getValue()[0] <<"," << lookat.getValue()[2] <<"," << lookat.getValue()[1] <<">;\n" 
      << "#declare  Up     = <" << upvec.getValue()[0]  <<"," << upvec.getValue()[2]  <<"," << upvec.getValue()[1]  <<">;\n" ;

  //Base::Console().Log("Pov Camera out:\n%s",out.str().c_str());

  FCParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Raytracing");

  std::string cDir             = hGrp->GetASCII("ProjectPath", "");
  std::string cCameraName      = hGrp->GetASCII("CameraName", "TempCamera.inc");
  std::string cPartName        = hGrp->GetASCII("PartName", "TempPart.inc");
  bool bNotWriteVertexNormals  = hGrp->GetBool("NotWriteVertexNormals",false);
  float fMeshDeviation         = hGrp->GetFloat("MeshDeviation",0.1);

  if(cDir!="" && cDir[cDir.size()-1] != PATHSEP)
    cDir += PATHSEP;

  std::string cFullName = cDir+cCameraName;
  Base::Console().Log("Using file name:%s",cFullName.c_str());

  // open the file and write
  std::ofstream fout(cFullName.c_str());
  fout <<  out.str() << endl;
  fout.close();


  // Bring ref-count of root-node back to zero to cause the
  // destruction of the scene.
  Cam->unref();
  // (Upon self-destructing, the root-node will also de-reference
  // it's children nodes, so they also self-destruct, and so on
  // recursively down the scenegraph hierarchy until the complete
  // scenegraph has self-destructed and thereby returned all
  // memory resources it was using.)



/*
  QString fn = Gui::FileDialog::getSaveFileName( QString::null, "POV include (*.inc);;All Files (*.*)", 
                                                 Gui::ApplicationWindow::Instance );
	if (! fn.isEmpty() )
	{
    Base::Console().Log("File name: %s",fn.latin1());
	}
*/
}

bool CmdRaytracingWriteCamera::isActive(void)
{
  return getAppWnd()->sendHasMsgToActiveView("GetCamera");
}

//===========================================================================
// Helper funciton
//===========================================================================


void transferToArray(const TopoDS_Face& aFace,SbVec3f** vertices,SbVec3f** vertexnormals, long** cons,int &nbNodesInFace,int &nbTriInFace )
{
	TopLoc_Location aLoc;

  // doing the meshing and checking the result
	//BRepMesh_IncrementalMesh MESH(aFace,fDeflection);
	Handle(Poly_Triangulation) aPoly = BRep_Tool::Triangulation(aFace,aLoc);
  if(aPoly.IsNull()){ 
    Base::Console().Log("Empty face trianglutaion\n");
    nbNodesInFace =0;
    nbTriInFace = 0;
    vertices = 0l;
    cons = 0l;
    return;
  }

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

	*cons = new long[3*(nbTriInFace)+1];

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

    // Calculate triangle normal
    gp_Vec v1(V1.X(),V1.Y(),V1.Z()),v2(V2.X(),V2.Y(),V2.Z()),v3(V3.X(),V3.Y(),V3.Z());
    gp_Vec Normal = (v2-v1)^(v3-v1); 

    //Standard_Real Area = 0.5 * Normal.Magnitude();

    // add the triangle normal to the vertex normal for all points of this triangle
    (*vertexnormals)[N1-1] += SbVec3f(Normal.X(),Normal.Y(),Normal.Z());
    (*vertexnormals)[N2-1] += SbVec3f(Normal.X(),Normal.Y(),Normal.Z());
    (*vertexnormals)[N3-1] += SbVec3f(Normal.X(),Normal.Y(),Normal.Z());

    (*vertices)[N1-1].setValue((float)(V1.X()),(float)(V1.Y()),(float)(V1.Z()));
    (*vertices)[N2-1].setValue((float)(V2.X()),(float)(V2.Y()),(float)(V2.Z()));
    (*vertices)[N3-1].setValue((float)(V3.X()),(float)(V3.Y()),(float)(V3.Z()));

    int j = i - 1;
    N1--; N2--; N3--;
    (*cons)[3*j] = N1; (*cons)[3*j+1] = N2; (*cons)[3*j+2] = N3;
  }

  // normalize all vertex normals
  for(i=0;i < nbNodesInFace;i++) {

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


    (*vertexnormals)[i].normalize();

  }

}

//===========================================================================
// CmdRaytracingWritePart
//===========================================================================
DEF_STD_CMD_A(CmdRaytracingWritePart);

CmdRaytracingWritePart::CmdRaytracingWritePart()
  :CppCommand("Raytracing_WritePart")
{
  sAppModule    = "Raytracing";
  sGroup        = "Raytracing";
  sMenuText     = "Write the part";
  sToolTipText  = "Write the Part (object) of the active 3D view in PovRay format to a file";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Test1";
  iAccel        = 0;
}

void CmdRaytracingWritePart::activated(int iMsg)
{
  TopoDS_Shape myShape;

  App::Feature *pcActFeature = getActiveDocument()->getDocument()->GetActiveFeature();

  if(pcActFeature)
  {
    Part::PartFeature *pcPart = dynamic_cast<Part::PartFeature*> (pcActFeature);
    myShape = pcPart->GetShape();
  }
  
  // get the preferences
  FCParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Raytracing");
  std::string cDir             = hGrp->GetASCII("ProjectPath", "");
  std::string cCameraName      = hGrp->GetASCII("CameraName", "TempCamera.inc");
  std::string cPartName        = hGrp->GetASCII("PartName", "TempPart.inc");
  bool bNotWriteVertexNormals  = hGrp->GetBool("NotWriteVertexNormals",false);
  float fMeshDeviation         = hGrp->GetFloat("MeshDeviation",0.1);


  // name of the objects in the pov file
  std::string Name = "Part";

  Base::Sequencer().start("Meshing Shape", 0);
  Base::Console().Log("Meshing with Deviation: %f\n",fMeshDeviation);

  TopExp_Explorer ex;
	BRepMesh_IncrementalMesh MESH(myShape,fMeshDeviation);

  Base::Sequencer().stop();


  if(cDir!="" && cDir[cDir.size()-1] != PATHSEP)
    cDir += PATHSEP;

  std::string cFullName = cDir+cPartName;
  Base::Console().Log("Using file name:%s",cFullName.c_str());

  // open the file and write
  std::ofstream fout(cFullName.c_str());

  // counting faces and start sequencer
  int l = 1;
  for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next(),l++) {}
  Base::Sequencer().start("Writing file", l);


  // write the file
  fout <<  "// Written by FreeCAD http://free-cad.sf.net/" << endl;
  l = 1;
  for (ex.Init(myShape, TopAbs_FACE); ex.More(); ex.Next(),l++) {

    // get the shape and mesh it
		const TopoDS_Face& aFace = TopoDS::Face(ex.Current());


    // this block mesh the face and transfers it in a C array of vertices and face indexes
		Standard_Integer nbNodesInFace,nbTriInFace;
		SbVec3f* vertices=0;
		SbVec3f* vertexnormals=0;
		long* cons=0;
    
    transferToArray(aFace,&vertices,&vertexnormals,&cons,nbNodesInFace,nbTriInFace);

    if(!vertices) break;
    // writing per face header 
    fout << "// face number" << l << " +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl
         << "#declare " << Name << l << " = mesh2{" << endl
         << "  vertex_vectors {" << endl
         << "    " << nbNodesInFace << "," << endl;
    // writing vertices
    for(int i=0;i < nbNodesInFace;i++) {
      fout << "    <" << vertices[i][0] << ","
                      << vertices[i][2] << ","
                      << vertices[i][1] << ">," 
           << endl;
    }
    fout << "  }" << endl
    // writing per vertex normals
         << "  normal_vectors {" << endl
         << "    " << nbNodesInFace << "," << endl;
    for(int j=0;j < nbNodesInFace;j++) {
      fout << "    <" << vertexnormals[j][0] << ","
                      << vertexnormals[j][2] << ","
                      << vertexnormals[j][1] << ">,"
           << endl;
    }

    fout << "  }" << endl
    // writing triangle indices
         << "  face_indices {" << endl
         << "    " << nbTriInFace << "," << endl;
    for(int k=0;k < nbTriInFace;k++) {
      fout << "    <" << cons[3*k] << ","<< cons[3*k+2] << ","<< cons[3*k+1] << ">," << endl;
    }
    // end of face
    fout << "  }" << endl
         << "} // end of Face"<< l << endl << endl;

		delete [] vertexnormals;
		delete [] vertices;
		delete [] cons;

    Base::Sequencer().next();

  } // end of face loop


  fout << endl << endl << "// Declare all together +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl
       << "#declare " << Name << " = union {" << endl;
  for(int i=1;i < l;i++) {
    fout << "mesh2{ " << Name << i << "}" << endl;
  }
  fout << "}" << endl << endl;

  Base::Sequencer().stop();


  fout.close();


}

bool CmdRaytracingWritePart::isActive(void)
{
	if( getActiveDocument() )
		return true;
	else
		return false;
}


//===========================================================================
// CmdRaytracingNewProject
//===========================================================================
DEF_STD_CMD_A(CmdRaytracingNewProject);

CmdRaytracingNewProject::CmdRaytracingNewProject()
  :CppCommand("Raytracing_NewProject")
{
  sAppModule    = "Raytracing";
  sGroup        = "Raytracing";
  sMenuText     = "New render project";
  sToolTipText  = "Wirte the initial povray file to render a part";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Test1";
  iAccel        = 0;
}

void CmdRaytracingNewProject::activated(int iMsg)
{
  // get the preferences
  FCParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Raytracing");
  std::string cDir             = hGrp->GetASCII("ProjectPath", "");

  if(cDir!="" && cDir[cDir.size()-1] != PATHSEP)
    cDir += PATHSEP;

  std::string cFullName = cDir+"FreeCAD.pov";
  Base::Console().Log("Using file name:%s",cFullName.c_str());

  // open the file and write
  std::ofstream fout(cFullName.c_str());
	fout << FreeCAD ;
	fout.close();


}

bool CmdRaytracingNewProject::isActive(void)
{
	//if( getActiveDocument() )
		return true;
	//else
	//	return false;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void CreateCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::ApplicationWindow::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdRaytracingWriteCamera());
  rcCmdMgr.addCommand(new CmdRaytracingWritePart());
  rcCmdMgr.addCommand(new CmdRaytracingNewProject());
}
