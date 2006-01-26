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

using namespace Mesh;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// Example MakeMesh
//===========================================================================
DEF_STD_CMD_A(CmdMeshDemolding);

CmdMeshDemolding::CmdMeshDemolding()
  :Command("Mesh_Demolding")
{
  sAppModule    = "Mesh";
  sGroup        = "Mesh";
  sMenuText     = QT_TR_NOOP("Interactive dmolding direction");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void CmdMeshDemolding::activated(int iMsg)
{
  unsigned int n = getSelection().getNbrOfType("Mesh");
  if ( n!=1 ) return;

  std::string fName = getUniqueFeatureName("Demolding");
  std::vector<Gui::SelectionSingelton::SelObj> cSel = getSelection().getSelection();

  openCommand("Mesh Mesh Create");
  doCommand(Doc,"App.document().AddFeature(\"MeshTransformDemolding\",\"%s\")",fName.c_str());
  doCommand(Doc,"App.document().%s.Source = App.document().%s",fName.c_str(),cSel[0].FeatName);
  doCommand(Doc,"App.document().%s.showMode=\"%s\"",fName.c_str(), "Demold");
  doCommand(Gui,"Gui.hide(\"%s\")",cSel[0].FeatName);
  commitCommand(); 
 
  updateActive();
}

bool CmdMeshDemolding::isActive(void)
{
  //return true;
  return getSelection().getNbrOfType("Mesh") == 1;
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
    "App.document().AddFeature(\"Mesh\",\"MeshBox\")\n"
    "App.document().MeshBox.setMesh(mb)\n"
    "App.document().MeshBox.showMode = \"FlatWire\"\n" 
    "App.document().Recompute()" );

  doCommand(Gui,"Gui.SendMsgToActiveView(\"ViewFit\")");
  commitCommand();

  updateActive();
}

bool CmdMeshExMakeMesh::isActive(void)
{
  return hasActiveDocument() && !hasFeature("MeshBox");
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
    "App.document().AddFeature(\"Mesh\",\"MeshTool\")\n"
    "App.document().MeshTool.setMesh(mt)\n"
    "App.document().MeshTool.solidMaterial.diffuseColor = (0.5,0.2,0.2)\n"
    "App.document().MeshTool.showMode = \"FlatWire\"" );

  commitCommand();
 
  updateActive();
}

bool CmdMeshExMakeTool::isActive(void)
{
  return hasFeature("MeshBox") && !hasFeature("MeshTool");
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
    "m3.Union(m2)\n"
    "App.document().AddFeature(\"Mesh\",\"MeshUnion\")\n"
    "App.document().MeshUnion.setMesh(m3)\n"
    "App.document().MeshUnion.solidMaterial.ambientColor = (0.1,1,0)\n"
    "App.document().MeshUnion.solidMaterial.transparency = 0.5\n"
    "App.document().MeshUnion.lineMaterial.ambientColor = (0.1,0.11,0.1)\n"
    "App.document().MeshUnion.lineSize = 2\n"
    "App.document().MeshUnion.showMode = \"FlatWire\"" );

  doCommand(Gui,"Gui.hide(\"MeshBox\")");
  doCommand(Gui,"Gui.hide(\"MeshTool\")");

  commitCommand();
 
  updateActive();
}

bool CmdMeshExMakeUnion::isActive(void)
{
  return hasFeature("MeshTool") && !hasFeature("MeshUnion");
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
  sMenuText     = QT_TR_NOOP("Import Mesh");
  sToolTipText  = QT_TR_NOOP("Imports a mesh from file");
  sWhatsThis    = QT_TR_NOOP("Imports a mesh from file");
  sStatusTip    = QT_TR_NOOP("Imports a mesh from file");
  sPixmap       = "import_mesh";
  iAccel        = 0;
}

void CmdMeshImport::activated(int iMsg)
{
  QString filter = "All STL Files (*.stl *.ast);;Binary STL (*.stl);;ASCII STL (*.ast);;All Files (*.*)";
  QString fn = Gui::FileDialog::getOpenFileName( QString::null, filter, Gui::getMainWindow() );
  if (! fn.isEmpty() )
  {
    openCommand("Mesh ImportSTL Create");
    doCommand(Doc,"f = App.document().AddFeature(\"MeshImport\",\"MeshImport\")");
    doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    commitCommand();

    updateActive();
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
  sMenuText     = QT_TR_NOOP("Export Mesh");
  sToolTipText  = QT_TR_NOOP("Exports a mesh to file");
  sWhatsThis    = QT_TR_NOOP("Exports a mesh to file");
  sStatusTip    = QT_TR_NOOP("Exports a mesh to file");
  sPixmap       = "export_mesh";
  iAccel        = 0;
}

void CmdMeshExport::activated(int iMsg)
{
  QString filter = "Binary STL (*.stl);;ASCII STL (*.stl);;ASCII STL (*.ast);;All Files (*.*)";
  QString format;
  QString fn = Gui::FileDialog::getSaveFileName( QString::null, filter, Gui::getMainWindow(), 0,
                                                 QObject::tr("Export mesh"), &format, true, QObject::tr("Export") );
  std::vector<App::Feature*> fea = Gui::Selection().getSelectedFeatures("Mesh");

  if (! fn.isEmpty() )
  {
    if ( format.startsWith("Binary STL") )
      format = "Binary STL";
    else if ( format.startsWith("ASCII STL") )
      format = "ASCII STL";
    openCommand("Mesh ExportSTL Create");
    doCommand(Doc,"f = App.document().AddFeature(\"MeshExport\",\"MeshExport\")");
    doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    doCommand(Doc,"f.Format = \"%s\"",format.ascii());
    doCommand(Doc,"f.Source = App.document().%s",fea.front()->getName());
    commitCommand();

    updateActive();
  }
}

bool CmdMeshExport::isActive(void)
{
  return getSelection().getNbrOfType("Mesh") == 1;
}

DEF_STD_CMD_A(CmdMeshVertexCurvature);

CmdMeshVertexCurvature::CmdMeshVertexCurvature()
  :Command("Mesh_VertexCurvature")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Curvature per vertex");
  sToolTipText  = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sWhatsThis    = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sStatusTip    = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sPixmap       = "curv_info";
}

void CmdMeshVertexCurvature::activated(int iMsg)
{
  unsigned int n = getSelection().getNbrOfType("Mesh");
  if ( n!=1 ) return;

  std::string fName = getUniqueFeatureName("Vertex_Curvature");
  std::vector<Gui::SelectionSingelton::SelObj> cSel = getSelection().getSelection();

  openCommand("Mesh VertexCurvature");
  doCommand(Doc,"App.document().AddFeature(\"MeshCurvature\",\"%s\")",fName.c_str());
  doCommand(Doc,"App.document().%s.Source = App.document().%s",fName.c_str(),cSel[0].FeatName);
  doCommand(Doc,"App.document().%s.showMode=\"%s\"",fName.c_str(), "Absolute curvature");
  commitCommand();
  updateActive();
  doCommand(Gui,"Gui.hide(\"%s\")",cSel[0].FeatName);
  getSelection().clearSelection();
}

bool CmdMeshVertexCurvature::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return (getSelection().getNbrOfType("Mesh")==1);
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
  std::vector<App::Feature*> fea = Gui::Selection().getSelectedFeatures("Mesh");
  if ( fea.size() == 1 )
  {
    Gui::ViewProvider* pVP = getActiveGuiDocument()->getViewProvider(fea.front());
    pVP->setEdit();
  }
}

bool CmdMeshPolyPick::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return (getSelection().getNbrOfType("Mesh")==1);
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
  std::vector<App::Feature*> fea = Gui::Selection().getSelectedFeatures("Mesh");
  if ( fea.size() == 2 )
  {
    std::string fName = getUniqueFeatureName("MeshSegment");
    App::Feature* mesh = fea.front();
    App::Feature* tool = fea.back();

    openCommand("Segment by tool mesh");
    doCommand(Doc, "import Mesh");
    doCommand(Gui, "import MeshGui");
    doCommand(Doc,
      "App.document().AddFeature(\"MeshSegmentByMesh\",\"%s\")\n"
      "App.document().%s.Source = App.document().%s\n"
      "App.document().%s.Tool = App.document().%s\n",
      fName.c_str(), fName.c_str(),  mesh->getName(), fName.c_str(), tool->getName() );

    commitCommand();
    updateActive();

    App::Document* pDoc = getDocument();
    App::Feature * pFea = pDoc->getFeature( fName.c_str() );

    if ( pFea && pFea->isValid() )
    {
      doCommand(Gui,"Gui.hide(\"%s\")", mesh->getName());
      doCommand(Gui,"Gui.hide(\"%s\")", tool->getName());
      getSelection().clearSelection();
    }
  }
}

bool CmdMeshToolMesh::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return (getSelection().getNbrOfType("Mesh")==2);
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
  rcCmdMgr.addCommand(new CmdMeshToolMesh());
}
