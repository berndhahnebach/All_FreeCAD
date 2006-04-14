/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qdir.h>
# include <qfileinfo.h>
# include <qmessagebox.h>
# include <gts.h>
# include <map>
#endif

#include "../App/MeshFeature.h"
#include "../App/MeshAlgos.h"

#include <Base/Console.h>
#include <Base/Exception.h>
#include <App/Document.h>
#include <App/Feature.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/FileDialog.h>
#include <Gui/Selection.h>
#include <Gui/ViewProvider.h>

#include "DlgEvaluateMeshImp.h"

using namespace Mesh;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Mesh transform
//===========================================================================
DEF_STD_CMD_A(CmdMeshTransform);

CmdMeshTransform::CmdMeshTransform()
  :Command("Mesh_Transform")
{
  sAppModule    = "Mesh";
  sGroup        = "Mesh";
  sMenuText     = QT_TR_NOOP("Transformation (moving) of a mesh");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void CmdMeshTransform::activated(int iMsg)
{
  unsigned int n = getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( n!=1 ) return;

  std::string fName = getUniqueObjectName("Move");
  std::vector<Gui::SelectionSingleton::SelObj> cSel = getSelection().getSelection();

  openCommand("Mesh Mesh Create");
  doCommand(Doc,"App.document().addObject(\"Mesh::Transform\",\"%s\")",fName.c_str());
  doCommand(Doc,"App.document().%s.Source = App.document().%s",fName.c_str(),cSel[0].FeatName);
  doCommand(Doc,"App.document().%s.showMode=\"%s\"",fName.c_str(), "Transform");
  doCommand(Gui,"Gui.hide(\"%s\")",cSel[0].FeatName);
  commitCommand(); 
 
  updateActive();
}

bool CmdMeshTransform::isActive(void)
{
  //return true;
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

//===========================================================================
// Mesh demolding
//===========================================================================
DEF_STD_CMD_A(CmdMeshDemolding);

CmdMeshDemolding::CmdMeshDemolding()
  :Command("Mesh_Demolding")
{
  sAppModule    = "Mesh";
  sGroup        = "Mesh";
  sMenuText     = QT_TR_NOOP("Interactive demolding direction");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void CmdMeshDemolding::activated(int iMsg)
{
  unsigned int n = getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( n!=1 ) return;

  std::string fName = getUniqueObjectName("Demolding");
  std::vector<Gui::SelectionSingleton::SelObj> cSel = getSelection().getSelection();

  openCommand("Mesh Mesh Create");
  doCommand(Doc,"App.document().addObject(\"Mesh::TransformDemolding\",\"%s\")",fName.c_str());
  doCommand(Doc,"App.document().%s.Source = App.document().%s",fName.c_str(),cSel[0].FeatName);
  doCommand(Doc,"App.document().%s.showMode=\"%s\"",fName.c_str(), "Demold");
  doCommand(Gui,"Gui.hide(\"%s\")",cSel[0].FeatName);
  commitCommand(); 
 
  updateActive();
}

bool CmdMeshDemolding::isActive(void)
{
  //return true;
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}
//===========================================================================
// Example MakeMesh
//===========================================================================
DEF_STD_CMD_A(CmdMeshExMakeMesh);

CmdMeshExMakeMesh::CmdMeshExMakeMesh()
  :Command("Mesh_ExMakeMesh")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Make a Box");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void CmdMeshExMakeMesh::activated(int iMsg)
{
  openCommand("Mesh Mesh Create");
  doCommand(Doc,
    "import Mesh\n"
    "mb = Mesh.newMesh()\n"
    "mb.addFacet(0.0,0.0,0.0, 0.0,0.0,1.0, 0.0,1.0,1.0)\n"
    "mb.addFacet(0.0,0.0,0.0, 0.0,1.0,1.0, 0.0,1.0,0.0)\n"
    "mb.addFacet(0.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,1.0)\n"
    "mb.addFacet(0.0,0.0,0.0, 1.0,0.0,1.0, 0.0,0.0,1.0)\n"
    "mb.addFacet(0.0,0.0,0.0, 0.0,1.0,0.0, 1.0,1.0,0.0)\n"
    "mb.addFacet(0.0,0.0,0.0, 1.0,1.0,0.0, 1.0,0.0,0.0)\n"
    "mb.addFacet(0.0,1.0,0.0, 0.0,1.0,1.0, 1.0,1.0,1.0)\n"
    "mb.addFacet(0.0,1.0,0.0, 1.0,1.0,1.0, 1.0,1.0,0.0)\n"
    "mb.addFacet(0.0,1.0,1.0, 0.0,0.0,1.0, 1.0,0.0,1.0)\n"
    "mb.addFacet(0.0,1.0,1.0, 1.0,0.0,1.0, 1.0,1.0,1.0)\n"
    "mb.addFacet(1.0,1.0,0.0, 1.0,1.0,1.0, 1.0,0.0,1.0)\n"
    "mb.addFacet(1.0,1.0,0.0, 1.0,0.0,1.0, 1.0,0.0,0.0)\n"
    "mb.scale(100.0)\n"
    "App.document().addObject(\"Mesh::Feature\",\"MeshBox\")\n"
    "App.document().MeshBox.setMesh(mb)\n"
    "App.document().MeshBox.showMode = \"FlatWire\"\n" 
    "App.document().recompute()" );

  doCommand(Gui,"Gui.SendMsgToActiveView(\"ViewFit\")");
  commitCommand();

  updateActive();
}

bool CmdMeshExMakeMesh::isActive(void)
{
  return hasActiveDocument() && !hasObject("MeshBox");
}

//===========================================================================
// Example MakeTool
//===========================================================================
DEF_STD_CMD_A(CmdMeshExMakeTool);

CmdMeshExMakeTool::CmdMeshExMakeTool()
  :Command("Mesh_ExMakeTool")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Make a Tool");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool2";
  iAccel        = 0;
}

void CmdMeshExMakeTool::activated(int iMsg)
{
  openCommand("Mesh Mesh Create");
  doCommand(Doc,
    "import Mesh,MeshGui\n"
    "mt = Mesh.newMesh()\n"
    "mt.addFacet(0.0,0.0,0.0, 0.0,0.0,1.0, 0.0,1.0,1.0)\n"
    "mt.addFacet(0.0,0.0,0.0, 0.0,1.0,1.0, 0.0,1.0,0.0)\n"
    "mt.addFacet(0.0,0.0,0.0, 1.0,0.0,0.0, 1.0,0.0,1.0)\n"
    "mt.addFacet(0.0,0.0,0.0, 1.0,0.0,1.0, 0.0,0.0,1.0)\n"
    "mt.addFacet(0.0,0.0,0.0, 0.0,1.0,0.0, 1.0,1.0,0.0)\n"
    "mt.addFacet(0.0,0.0,0.0, 1.0,1.0,0.0, 1.0,0.0,0.0)\n"
    "mt.addFacet(0.0,1.0,0.0, 0.0,1.0,1.0, 1.0,1.0,1.0)\n"
    "mt.addFacet(0.0,1.0,0.0, 1.0,1.0,1.0, 1.0,1.0,0.0)\n"
    "mt.addFacet(0.0,1.0,1.0, 0.0,0.0,1.0, 1.0,0.0,1.0)\n"
    "mt.addFacet(0.0,1.0,1.0, 1.0,0.0,1.0, 1.0,1.0,1.0)\n"
    "mt.addFacet(1.0,1.0,0.0, 1.0,1.0,1.0, 1.0,0.0,1.0)\n"
    "mt.addFacet(1.0,1.0,0.0, 1.0,0.0,1.0, 1.0,0.0,0.0)\n"
    "mt.scale(100.0)\n"
    "mt.translate(50.0,50.0,50.0)\n"
    "App.document().addObject(\"Mesh::Feature\",\"MeshTool\")\n"
    "App.document().MeshTool.setMesh(mt)\n"
    "App.document().MeshTool.solidMaterial.diffuseColor = (0.5,0.2,0.2)\n"
    "App.document().MeshTool.showMode = \"FlatWire\"" );

  commitCommand();
 
  updateActive();
}

bool CmdMeshExMakeTool::isActive(void)
{
  return hasObject("MeshBox") && !hasObject("MeshTool");
}


//===========================================================================
// Example MakeUnion
//===========================================================================
DEF_STD_CMD_A(CmdMeshExMakeUnion);

CmdMeshExMakeUnion::CmdMeshExMakeUnion()
  :Command("Mesh_ExMakeUnion")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Make Union");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool3";
  iAccel        = 0;
}

void CmdMeshExMakeUnion::activated(int iMsg)
{
  openCommand("Mesh Mesh Create");
  doCommand(Doc,
    "import Mesh,MeshGui\n"
    "m1 = App.document().MeshBox.getMesh()\n"
    "m2 = App.document().MeshTool.getMesh()\n"
    "m3 = m1.copy()\n"
    "m3.unite(m2)\n"
    "App.document().addObject(\"Mesh::Feature\",\"MeshUnion\")\n"
    "App.document().MeshUnion.setMesh(m3)\n"
    "App.document().MeshUnion.solidMaterial.ambientColor = (0.1,1,0)\n"
    "App.document().MeshUnion.solidMaterial.transparency = 0.5\n"
    "App.document().MeshUnion.lineMaterial.ambientColor = (0.1,0.11,0.1)\n"
    "App.document().MeshUnion.lineSize = 2\n"
    "App.document().MeshUnion.showMode = \"FlatWire\"" );
 
  updateActive();

  doCommand(Gui,"Gui.hide(\"MeshBox\")");
  doCommand(Gui,"Gui.hide(\"MeshTool\")");

  commitCommand();
}

bool CmdMeshExMakeUnion::isActive(void)
{
  return hasObject("MeshTool") && !hasObject("MeshUnion");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdMeshImport
//===========================================================================
DEF_STD_CMD_A(CmdMeshImport);

CmdMeshImport::CmdMeshImport()
  :Command("Mesh_Import")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Import mesh...");
  sToolTipText  = QT_TR_NOOP("Imports a mesh from file");
  sWhatsThis    = QT_TR_NOOP("Imports a mesh from file");
  sStatusTip    = QT_TR_NOOP("Imports a mesh from file");
  sPixmap       = "import_mesh";
  iAccel        = 0;
}

void CmdMeshImport::activated(int iMsg)
{
  // use current path as default
  std::string path = QDir::currentDirPath().latin1();
  FCHandle<ParameterGrp> hPath = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Preferences")->GetGroup("General");
  path = hPath->GetASCII("FileOpenSavePath", path.c_str());
  QString dir = path.c_str();

  QString filter = "All Mesh Files (*.stl *.ast *.bms);;Binary STL (*.stl);;ASCII STL (*.ast);;Binary Mesh (*.bms);;All Files (*.*)";
  QString fn = Gui::FileDialog::getOpenFileName( dir, filter, Gui::getMainWindow() );
  if (! fn.isEmpty() )
  {
    QFileInfo fi;
    fi.setFile(fn);

    openCommand("Mesh ImportSTL Create");
    doCommand(Doc,"f = App.document().addObject(\"Mesh::Import\",\"%s\")", fi.baseName().latin1());
    doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    commitCommand();
    updateActive();

    hPath->SetASCII("FileOpenSavePath", fi.dirPath(true).latin1());
  }
}

bool CmdMeshImport::isActive(void)
{
  if( getActiveGuiDocument() )
    return true;
  else
    return false;
}

//===========================================================================
// CmdMeshExport
//===========================================================================
DEF_STD_CMD_A(CmdMeshExport);

CmdMeshExport::CmdMeshExport()
  :Command("Mesh_Export")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Export mesh...");
  sToolTipText  = QT_TR_NOOP("Exports a mesh to file");
  sWhatsThis    = QT_TR_NOOP("Exports a mesh to file");
  sStatusTip    = QT_TR_NOOP("Exports a mesh to file");
  sPixmap       = "export_mesh";
  iAccel        = 0;
}

void CmdMeshExport::activated(int iMsg)
{
  // use current path as default
  std::string path = QDir::currentDirPath().latin1();
  FCHandle<ParameterGrp> hPath = App::GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Preferences")->GetGroup("General");
  path = hPath->GetASCII("FileOpenSavePath", path.c_str());
  QString dir = path.c_str();

  QString filter = "Binary STL (*.stl);;ASCII STL (*.stl);;ASCII STL (*.ast);;Binary Mesh (*.bms);;All Files (*.*)";
  QString format;
  QString fn = Gui::FileDialog::getSaveFileName( dir, filter, Gui::getMainWindow(), 0,
                                                 QObject::tr("Export mesh"), &format, true, QObject::tr("Export") );
  std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());

  if (! fn.isEmpty() )
  {
    if ( format.startsWith("Binary STL") )
      format = "Binary STL";
    else if ( format.startsWith("ASCII STL") )
      format = "ASCII STL";

    QFileInfo fi; fi.setFile(fn);
    openCommand("Mesh ExportSTL Create");
    doCommand(Doc,"f = App.document().addObject(\"Mesh::Export\",\"%s\")", fi.baseName().ascii());
    doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    doCommand(Doc,"f.Format = \"%s\"",format.ascii());
    doCommand(Doc,"f.Source = App.document().%s",fea.front()->name.getValue());
    commitCommand();
    updateActive();

    hPath->SetASCII("FileOpenSavePath", fi.dirPath(true).latin1());
  }
}

bool CmdMeshExport::isActive(void)
{
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

DEF_STD_CMD_A(CmdMeshVertexCurvature);

CmdMeshVertexCurvature::CmdMeshVertexCurvature()
  :Command("Mesh_VertexCurvature")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Curvature info");
  sToolTipText  = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sWhatsThis    = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sStatusTip    = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sPixmap       = "curv_info";
}

void CmdMeshVertexCurvature::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Curvature";
    fName = getUniqueObjectName(fName.c_str());

    openCommand("Mesh VertexCurvature");
    doCommand(Doc,"App.document().addObject(\"Mesh::Curvature\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.document().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    doCommand(Doc,"App.document().%s.showMode=\"%s\"",fName.c_str(), "Absolute curvature");
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }

  getSelection().clearSelection();
}

bool CmdMeshVertexCurvature::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshPolyPick);

CmdMeshPolyPick::CmdMeshPolyPick()
  :Command("Mesh_PolyPick")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Surface info");
  sToolTipText  = QT_TR_NOOP("Creates a segment from a picked polygon");
  sWhatsThis    = QT_TR_NOOP("Creates a segment from a picked polygon");
  sStatusTip    = QT_TR_NOOP("Creates a segment from a picked polygon");
  sPixmap       = "PolygonPick";
}

void CmdMeshPolyPick::activated(int iMsg)
{
  std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( fea.size() == 1 )
  {
    Gui::ViewProvider* pVP = getActiveGuiDocument()->getViewProvider(fea.front());
    pVP->setEdit();
  }
}

bool CmdMeshPolyPick::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

DEF_STD_CMD_A(CmdMeshPolyCut);

CmdMeshPolyCut::CmdMeshPolyCut()
  :Command("Mesh_PolyCut")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Cut mesh");
  sToolTipText  = QT_TR_NOOP("Cuts a mesh with a picked polygon");
  sWhatsThis    = QT_TR_NOOP("Cuts a mesh with a picked polygon");
  sStatusTip    = QT_TR_NOOP("Cuts a mesh with a picked polygon");
  sPixmap       = "PolygonPick";
}

void CmdMeshPolyCut::activated(int iMsg)
{
  std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( fea.size() == 1 )
  {
    Gui::ViewProvider* pVP = getActiveGuiDocument()->getViewProvider(fea.front());
    pVP->setEdit();
  }
}

bool CmdMeshPolyCut::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

DEF_STD_CMD_A(CmdMeshToolMesh);

CmdMeshToolMesh::CmdMeshToolMesh()
  :Command("Mesh_ToolMesh")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Segment by tool mesh");
  sToolTipText  = QT_TR_NOOP("Creates a segment from a given tool mesh");
  sWhatsThis    = QT_TR_NOOP("Creates a segment from a given tool mesh");
  sStatusTip    = QT_TR_NOOP("Creates a segment from a given tool mesh");
  sPixmap       = "Std_Tool4";
}

void CmdMeshToolMesh::activated(int iMsg)
{
  std::vector<App::DocumentObject*> fea = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( fea.size() == 2 )
  {
    std::string fName = getUniqueObjectName("MeshSegment");
    App::DocumentObject* mesh = fea.front();
    App::DocumentObject* tool = fea.back();

    openCommand("Segment by tool mesh");
    doCommand(Doc, "import Mesh");
    doCommand(Gui, "import MeshGui");
    doCommand(Doc,
      "App.document().addObject(\"Mesh::SegmentByMesh\",\"%s\")\n"
      "App.document().%s.Source = App.document().%s\n"
      "App.document().%s.Tool = App.document().%s\n",
      fName.c_str(), fName.c_str(),  mesh->name.getValue(), fName.c_str(), tool->name.getValue() );

    commitCommand();
    updateActive();

    App::Document* pDoc = getDocument();
    App::DocumentObject * pObj = pDoc->getObject( fName.c_str() );

    if ( pObj )
    {
      doCommand(Gui,"Gui.hide(\"%s\")", mesh->name.getValue());
      doCommand(Gui,"Gui.hide(\"%s\")", tool->name.getValue());
      getSelection().clearSelection();
    }
  }
}

bool CmdMeshToolMesh::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 2;
}

DEF_STD_CMD_A(CmdMeshEvaluation);

CmdMeshEvaluation::CmdMeshEvaluation()
  :Command("Mesh_Evaluation")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Evaluate mesh...");
  sToolTipText  = QT_TR_NOOP("Opens a dialog to analyze and repair a mesh");
  sWhatsThis    = QT_TR_NOOP("Opens a dialog to analyze and repair a mesh");
  sStatusTip    = QT_TR_NOOP("Opens a dialog to analyze and repair a mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshEvaluation::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    MeshGui::DockEvaluateMeshImp::instance()->setMesh( (Mesh::Feature*)(*it) );
    break;
  }
}

bool CmdMeshEvaluation::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return (!MeshGui::DockEvaluateMeshImp::hasInstance()) && (getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1);
}

DEF_STD_CMD_A(CmdMeshEvaluateSolid);

CmdMeshEvaluateSolid::CmdMeshEvaluateSolid()
  :Command("Mesh_EvaluateSolid")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Check solid mesh");
  sToolTipText  = QT_TR_NOOP("Checks whether the mesh is a solid");
  sWhatsThis    = QT_TR_NOOP("Checks whether the mesh is a solid");
  sStatusTip    = QT_TR_NOOP("Checks whether the mesh is a solid");
//  sPixmap       = "curv_info";
}

void CmdMeshEvaluateSolid::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    Mesh::Feature* mesh = (Mesh::Feature*)(*it);
    QString msg = QString("The mesh '%1' is ").arg(mesh->name.getValue());
    if ( mesh->getMesh().HasOpenEdges() )
      msg += "not a solid.";
    else
      msg += "a solid.";
    QMessageBox::information(Gui::getMainWindow(), QObject::tr("Solid Mesh"), msg);
  }
}

bool CmdMeshEvaluateSolid::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

DEF_STD_CMD_A(CmdMeshHarmonizeNormals);

CmdMeshHarmonizeNormals::CmdMeshHarmonizeNormals()
  :Command("Mesh_HarmonizeNormals")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Harmoinze normals");
  sToolTipText  = QT_TR_NOOP("Hormonizes the normals of the mesh");
  sWhatsThis    = QT_TR_NOOP("Hormonizes the normals of the mesh");
  sStatusTip    = QT_TR_NOOP("Hormonizes the normals of the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshHarmonizeNormals::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Harmonize";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::HarmonizeNormals\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    doCommand(Doc,"App.activeDocument().%s.showMode=App.activeDocument().%s.showMode",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshHarmonizeNormals::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshFlipNormals);

CmdMeshFlipNormals::CmdMeshFlipNormals()
  :Command("Mesh_FlipNormals")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Flip normals");
  sToolTipText  = QT_TR_NOOP("Flips the normals of the mesh");
  sWhatsThis    = QT_TR_NOOP("Flips the normals of the mesh");
  sStatusTip    = QT_TR_NOOP("Flips the normals of the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshFlipNormals::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Flip";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Flip Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FlipNormals\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    doCommand(Doc,"App.activeDocument().%s.showMode=App.activeDocument().%s.showMode",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshFlipNormals::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshFixDegenerations);

CmdMeshFixDegenerations::CmdMeshFixDegenerations()
  :Command("Mesh_FixDegenerations")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Remove degenerated faces");
  sToolTipText  = QT_TR_NOOP("Remove degenerated faces from the mesh");
  sWhatsThis    = QT_TR_NOOP("Remove degenerated faces from the mesh");
  sStatusTip    = QT_TR_NOOP("Remove degenerated faces from the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshFixDegenerations::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixDegenerations\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    doCommand(Doc,"App.activeDocument().%s.showMode=App.activeDocument().%s.showMode",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshFixDegenerations::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshFixDuplicateFaces);

CmdMeshFixDuplicateFaces::CmdMeshFixDuplicateFaces()
  :Command("Mesh_FixDuplicateFaces")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Remove duplicated faces");
  sToolTipText  = QT_TR_NOOP("Remove duplicated faces from the mesh");
  sWhatsThis    = QT_TR_NOOP("Remove duplicated faces from the mesh");
  sStatusTip    = QT_TR_NOOP("Remove duplicated faces from the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshFixDuplicateFaces::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixDuplicatedFaces\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    doCommand(Doc,"App.activeDocument().%s.showMode=App.activeDocument().%s.showMode",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshFixDuplicateFaces::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshFixDuplicatePoints);

CmdMeshFixDuplicatePoints::CmdMeshFixDuplicatePoints()
  :Command("Mesh_FixDuplicatePoints")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Remove duplicated points");
  sToolTipText  = QT_TR_NOOP("Remove duplicated points from the mesh");
  sWhatsThis    = QT_TR_NOOP("Remove duplicated points from the mesh");
  sStatusTip    = QT_TR_NOOP("Remove duplicated points from the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshFixDuplicatePoints::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixDuplicatedFaces\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    doCommand(Doc,"App.activeDocument().%s.showMode=App.activeDocument().%s.showMode",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshFixDuplicatePoints::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshFixIndices);

CmdMeshFixIndices::CmdMeshFixIndices()
  :Command("Mesh_FixIndices")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Fix indices");
  sToolTipText  = QT_TR_NOOP("Fixes invalid indices in the mesh structure");
  sWhatsThis    = QT_TR_NOOP("Fixes invalid indices in the mesh structure");
  sStatusTip    = QT_TR_NOOP("Fixes invalid indices in the mesh structure");
//  sPixmap       = "curv_info";
}

void CmdMeshFixIndices::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->name.getValue();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixIndices\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.document().%s",fName.c_str(),(*it)->name.getValue());
    doCommand(Doc,"App.activeDocument().%s.showMode=App.activeDocument().%s.showMode",fName.c_str(),(*it)->name.getValue());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->name.getValue());
  }
}

bool CmdMeshFixIndices::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdMeshBoundingBox);

CmdMeshBoundingBox::CmdMeshBoundingBox()
  :Command("Mesh_BoundingBox")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Boundings info");
  sToolTipText  = QT_TR_NOOP("Shows the boundings of the selected mesh");
  sWhatsThis    = QT_TR_NOOP("Shows the boundings of the selected mesh");
  sStatusTip    = QT_TR_NOOP("Shows the boundings of the selected mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshBoundingBox::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    const MeshCore::MeshKernel& rMesh = ((Mesh::Feature*)(*it))->getMesh();
    const Base::BoundBox3f& box = rMesh.GetBoundBox();
    QString minX, maxX, minY, maxY, minZ, maxZ;
    minX.sprintf("Min X=%f", box.MinX);
    maxX.sprintf("Max X=%f", box.MaxX);
    minY.sprintf("Min Y=%f", box.MinY);
    maxY.sprintf("Min Y=%f", box.MaxY);
    minZ.sprintf("Min Z=%f", box.MinZ);
    maxZ.sprintf("Max Z=%f", box.MaxZ);

    QString msg = QString("%1\t%2\t%3\n\n%4\t%5\t%6\n\n").arg(minX).arg(minY).arg(minZ).arg(maxX).arg(maxY).arg(maxZ);
    QMessageBox::information(Gui::getMainWindow(), QObject::tr("Boundings"), msg);
    break;
  }
}

bool CmdMeshBoundingBox::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

void CreateMeshCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdMeshImport());
  rcCmdMgr.addCommand(new CmdMeshExport());
  rcCmdMgr.addCommand(new CmdMeshVertexCurvature());
  rcCmdMgr.addCommand(new CmdMeshExMakeMesh());
  rcCmdMgr.addCommand(new CmdMeshExMakeTool());
  rcCmdMgr.addCommand(new CmdMeshExMakeUnion());
  rcCmdMgr.addCommand(new CmdMeshDemolding());
  rcCmdMgr.addCommand(new CmdMeshPolyPick());
  rcCmdMgr.addCommand(new CmdMeshPolyCut());
  rcCmdMgr.addCommand(new CmdMeshToolMesh());
  rcCmdMgr.addCommand(new CmdMeshTransform());
  rcCmdMgr.addCommand(new CmdMeshEvaluation());
  rcCmdMgr.addCommand(new CmdMeshEvaluateSolid());
  rcCmdMgr.addCommand(new CmdMeshHarmonizeNormals());
  rcCmdMgr.addCommand(new CmdMeshFlipNormals());
  rcCmdMgr.addCommand(new CmdMeshFixDegenerations());
  rcCmdMgr.addCommand(new CmdMeshFixDuplicateFaces());
  rcCmdMgr.addCommand(new CmdMeshFixDuplicatePoints());
  rcCmdMgr.addCommand(new CmdMeshFixIndices());
  rcCmdMgr.addCommand(new CmdMeshBoundingBox());
}
