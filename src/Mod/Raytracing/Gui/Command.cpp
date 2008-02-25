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
# include <BRep_Tool.hxx>
# include <GeomAPI_ProjectPointOnSurf.hxx>
# include <GeomLProp_SLProps.hxx>
# include <Poly_Triangulation.hxx>
# include <TopoDS_Face.hxx>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Command.h>
#include <Gui/FileDialog.h>
#include <Gui/View.h>

#include <Mod/Part/App/PartFeature.h>
  
#include "FreeCADpov.h"


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//===========================================================================
// CmdRaytracingWriteCamera
//===========================================================================
DEF_STD_CMD_A(CmdRaytracingWriteCamera);

CmdRaytracingWriteCamera::CmdRaytracingWriteCamera()
  :Command("Raytracing_WriteCamera")
{
  sAppModule    = "Raytracing";
  sGroup        = QT_TR_NOOP("Raytracing");
  sMenuText     = QT_TR_NOOP("Write camera position");
  sToolTipText  = QT_TR_NOOP("Write the camera positon of the active 3D view in PovRay format to a file");
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Test1";
  iAccel        = 0;
}

void CmdRaytracingWriteCamera::activated(int iMsg)
{
  const char* ppReturn=0;

  getGuiApplication()->sendMsgToActiveView("GetCamera",&ppReturn);

  Base::Console().Log("GetCamera MSG send:\n%s",ppReturn);

  SoInput in;
  in.setBuffer((void*)ppReturn,std::strlen(ppReturn));

  //if (!in.openFile(filename)) { exit(1); }

  SoNode* rootNode;
  SoDB::read(&in,rootNode);

  if (!rootNode || !rootNode->getTypeId().isDerivedFrom(SoCamera::getClassTypeId()))
    throw Base::Exception("CmdRaytracingWriteCamera::activated(): Could not read Camera information from ASCII stream....\n");

  // root-node returned from SoDB::readAll() has initial zero
  // ref-count, so reference it before we start using it to
  // avoid premature destruction.
  SoCamera * Cam = static_cast<SoCamera*>(rootNode);
  Cam->ref();

  SbRotation camrot = Cam->orientation.getValue();

  SbVec3f upvec(0, 1, 0); // init to default up vector
  camrot.multVec(upvec, upvec);

  SbVec3f lookat(0, 0, -1); // init to default view direction vector
  camrot.multVec(lookat, lookat);

  SbVec3f pos = Cam->position.getValue();
  float Dist = Cam->focalDistance.getValue();

  // getting standard parameter
  ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Raytracing");
  std::string cDir             = hGrp->GetASCII("ProjectPath", "");
  std::string cCameraName      = hGrp->GetASCII("CameraName", "TempCamera.inc");

  if(cDir!="" && cDir[cDir.size()-1] != PATHSEP)
    cDir += PATHSEP;
  std::string cFullName = cDir+cCameraName;

  // building up the python string
  std::stringstream out;
  out << "Raytracing.writeCameraFile(\"" << strToPython(cFullName) << "\"," 
      << "(" << pos.getValue()[0]    <<"," << pos.getValue()[1]    <<"," << pos.getValue()[2]    <<")," 
      << "(" << lookat.getValue()[0] <<"," << lookat.getValue()[1] <<"," << lookat.getValue()[2] <<")," ;
  lookat *= Dist;
  lookat += pos;
  out << "(" << lookat.getValue()[0] <<"," << lookat.getValue()[1] <<"," << lookat.getValue()[2] <<")," 
      << "(" << upvec.getValue()[0]  <<"," << upvec.getValue()[1]  <<"," << upvec.getValue()[2]  <<") )" ;

  doCommand(Doc,"import Raytracing");
 	doCommand(Gui,out.str().c_str());


  // Bring ref-count of root-node back to zero to cause the
  // destruction of the camera.
  Cam->unref();
}

bool CmdRaytracingWriteCamera::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("GetCamera");
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

  // getting the transformation of the shape/face
	gp_Trsf myTransf;
	Standard_Boolean identity = true;
	if(!aLoc.IsIdentity())  {
	  identity = false;
		myTransf = aLoc.Transformation();
  }

  Standard_Integer i;
  // getting size and create the array
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
  :Command("Raytracing_WritePart")
{
  sAppModule    = "Raytracing";
  sGroup        = QT_TR_NOOP("Raytracing");
  sMenuText     = QT_TR_NOOP("Write the part");
  sToolTipText  = QT_TR_NOOP("Write the Part (object) of the active 3D view in PovRay format to a file");
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Test1";
  iAccel        = 0;
}

void CmdRaytracingWritePart::activated(int iMsg)
{
  
  // get the preferences
  ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Raytracing");
  std::string cDir             = hGrp->GetASCII("ProjectPath", "");
  std::string cPartName        = hGrp->GetASCII("PartName", "TempPart.inc");
  //bool bNotWriteVertexNormals  = hGrp->GetBool("NotWriteVertexNormals",false);
  //float fMeshDeviation         = hGrp->GetFloat("MeshDeviation",0.1);

  // name of the objects in the pov file
  std::string Name = "Part";

  if(cDir!="" && cDir[cDir.size()-1] != PATHSEP)
    cDir += PATHSEP;

  std::string cFullName = cDir+cPartName;

  std::stringstream out;
  //Raytracing.writePartFile(App.document().GetActiveFeature().getShape())
  out << "Raytracing.writePartFile(\"" << strToPython(cFullName) << "\",\"" << Name << "\",App.document().activeObject().getShape())";

  doCommand(Doc,"import Raytracing");
 	doCommand(Doc,out.str().c_str());
}

bool CmdRaytracingWritePart::isActive(void)
{
	if( getActiveGuiDocument() )
  {
    App::DocumentObject* obj = getActiveGuiDocument()->getDocument()->getActiveObject();
    if ( obj && obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId()) )
  		return true;
  }

	return false;
}


//===========================================================================
// CmdRaytracingNewProject
//===========================================================================
DEF_STD_CMD_A(CmdRaytracingNewProject);

CmdRaytracingNewProject::CmdRaytracingNewProject()
  :Command("Raytracing_NewProject")
{
  sAppModule    = "Raytracing";
  sGroup        = QT_TR_NOOP("Raytracing");
  sMenuText     = QT_TR_NOOP("New project");
  sToolTipText  = QT_TR_NOOP("Write the initial povray file to render a part");
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Test1";
  iAccel        = 0;
}

void CmdRaytracingNewProject::activated(int iMsg)
{
  // getting standard parameter
  ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Raytracing");
  std::string cDir             = hGrp->GetASCII("ProjectPath", "");
  // xorx: The following has to be implemented as a setting
  std::string cPovRayName      = hGrp->GetASCII("SceneFilename", "PovrayScene.pov");
  // HACK: This is the workaround
  //std::string cPovRayName="PovrayScene.pov";

  if(cDir!="" && cDir[cDir.size()-1] != PATHSEP)
    cDir += PATHSEP;
  std::string cFullName = cDir+cPovRayName;

  // Open RayTracing module
  doCommand(Doc,"import Raytracing");
  // Get the default scene file and write it to the Project directory
  doCommand(Doc,"Raytracing.copyResource(\"FCSimple.pov\",\"%s\")",strToPython(cFullName).c_str());
}

bool CmdRaytracingNewProject::isActive(void)
{
	//if( getActiveDocument() )
		return true;
	//else
	//	return false;
}

//===========================================================================
// CmdRaytracingQuickRender
//===========================================================================
DEF_STD_CMD_A(CmdRaytracingQuickRender);

CmdRaytracingQuickRender::CmdRaytracingQuickRender()
  :Command("Raytracing_QuickRender")
{
  sAppModule    = "Raytracing";
  sGroup        = QT_TR_NOOP("Raytracing");
  sMenuText     = QT_TR_NOOP("Render");
  sToolTipText  = QT_TR_NOOP("Renders the actual view");
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Test1";
  iAccel        = 0;
}

void CmdRaytracingQuickRender::activated(int iMsg)
{
  // get the preferences
  ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Raytracing");
  std::string cDir             = hGrp->GetASCII("ProjectPath", "");

  //cDir = Gui::FileDialog::getExistingDirectory(cDir.c_str()).latin1();
  
  if(cDir!="" && cDir[cDir.size()-1] != PATHSEP)
    cDir += PATHSEP;

  std::string cFullName = cDir+"FreeCAD.pov";
  Base::Console().Log("Using file name:%s",cFullName.c_str());

  // open the file and write
  std::ofstream fout(cFullName.c_str());
	fout << FreeCAD ;
	fout.close();


}

bool CmdRaytracingQuickRender::isActive(void)
{
	//if( getActiveDocument() )
		return true;
	//else
	//	return false;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void CreateRaytracingCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdRaytracingWriteCamera());
  rcCmdMgr.addCommand(new CmdRaytracingWritePart());
  rcCmdMgr.addCommand(new CmdRaytracingNewProject());
}
