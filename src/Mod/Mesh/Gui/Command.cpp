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
# include <qinputdialog.h>
# include <qmessagebox.h>
# include <qstringlist.h>
# include <gts.h>
# include <map>
#endif

#ifndef __InventorAll__
# include <Gui/InventorAll.h>
#endif

#include "../App/MeshFeature.h"
#include "../App/MeshAlgos.h"

#include <Base/Console.h>
#include <Base/Exception.h>
#include <App/Document.h>
#include <App/DocumentObjectGroup.h>
#include <App/Feature.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/MainWindow.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/FileDialog.h>
#include <Gui/Selection.h>
#include <Gui/ViewProvider.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include "DlgEvaluateMeshImp.h"
#include "DlgRegularSolidImp.h"
#include "ViewProviderMeshFaceSet.h"

using namespace Mesh;


DEF_STD_CMD_A(CmdMeshTransform);

CmdMeshTransform::CmdMeshTransform()
  :Command("Mesh_Transform")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Transform mesh");
  sToolTipText  = QT_TR_NOOP("Rotate or move a mesh");
  sWhatsThis    = QT_TR_NOOP("Rotate or move a mesh");
  sStatusTip    = QT_TR_NOOP("Rotate or move a mesh");
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
  doCommand(Doc,"App.activeDocument().addObject(\"Mesh::Transform\",\"%s\")",fName.c_str());
  doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),cSel[0].FeatName);
  doCommand(Gui,"Gui.hide(\"%s\")",cSel[0].FeatName);
  commitCommand(); 
 
  updateActive();
}

bool CmdMeshTransform::isActive(void)
{
  //return true;
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshDemolding);

CmdMeshDemolding::CmdMeshDemolding()
  :Command("Mesh_Demolding")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
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
  doCommand(Doc,"App.activeDocument().addObject(\"Mesh::TransformDemolding\",\"%s\")",fName.c_str());
  doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),cSel[0].FeatName);
  doCommand(Gui,"Gui.hide(\"%s\")",cSel[0].FeatName);
  commitCommand(); 
 
  updateActive();
}

bool CmdMeshDemolding::isActive(void)
{
  //return true;
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

//--------------------------------------------------------------------------------------

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
    "mb = Mesh.mesh()\n"
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
    "App.activeDocument().addObject(\"Mesh::Feature\",\"MeshBox\")\n"
    "App.activeDocument().MeshBox.Mesh=mb\n"
    "App.activeDocument().recompute()" );

  doCommand(Gui,"Gui.activeDocument().activeView().fitAll()");
  commitCommand();

  updateActive();
}

bool CmdMeshExMakeMesh::isActive(void)
{
  return hasActiveDocument() && !hasObject("MeshBox");
}


//--------------------------------------------------------------------------------------

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
    "mt = Mesh.mesh()\n"
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
    "App.activeDocument().addObject(\"Mesh::Feature\",\"MeshTool\")\n"
    "App.activeDocument().MeshTool.Mesh=mt\n");

  commitCommand();
 
  updateActive();
}

bool CmdMeshExMakeTool::isActive(void)
{
  return hasObject("MeshBox") && !hasObject("MeshTool");
}

//--------------------------------------------------------------------------------------

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
    "m1 = App.activeDocument().MeshBox.Mesh\n"
    "m2 = App.activeDocument().MeshTool.Mesh\n"
    "m3 = m1.copy()\n"
    "m3.unite(m2)\n"
    "App.activeDocument().addObject(\"Mesh::Feature\",\"MeshUnion\")\n"
    "App.activeDocument().MeshUnion.Mesh=m3\n");
 
  updateActive();

  doCommand(Gui,"Gui.hide(\"MeshBox\")");
  doCommand(Gui,"Gui.hide(\"MeshTool\")");

  commitCommand();
}

bool CmdMeshExMakeUnion::isActive(void)
{
  return hasObject("MeshTool") && !hasObject("MeshUnion");
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshUnion);

CmdMeshUnion::CmdMeshUnion()
  :Command("Mesh_Union")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Union");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  iAccel        = 0;
}

void CmdMeshUnion::activated(int iMsg)
{
  std::vector<App::DocumentObject*> obj = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  std::string name1 = obj.front()->getNameInDocument();
  std::string name2 = obj.back()->getNameInDocument();
  std::string name3 = getUniqueObjectName("Union");
  openCommand("Mesh Union");
  doCommand(Doc,
    "import Mesh,MeshGui\n"
    "mesh = App.activeDocument().%s.Mesh\n"
    "mesh.union(App.activeDocument().%s.Mesh)\n"
    "App.activeDocument().addObject(\"Mesh::Feature\",\"%s\")\n"
    "App.activeDocument().%s.Mesh = mesh\n", name1.c_str(), name2.c_str(), name3.c_str(), name3.c_str());
 
  updateActive();
  commitCommand();
}

bool CmdMeshUnion::isActive(void)
{
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 2;
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshDifference);

CmdMeshDifference::CmdMeshDifference()
  :Command("Mesh_Difference")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Difference");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  iAccel        = 0;
}

void CmdMeshDifference::activated(int iMsg)
{
  std::vector<App::DocumentObject*> obj = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  std::string name1 = obj.front()->getNameInDocument();
  std::string name2 = obj.back()->getNameInDocument();
  std::string name3 = getUniqueObjectName("Difference");
  openCommand("Mesh Union");
  doCommand(Doc,
    "import Mesh,MeshGui\n"
    "mesh = App.activeDocument().%s.Mesh\n"
    "mesh.diff(App.activeDocument().%s.Mesh)\n"
    "App.activeDocument().addObject(\"Mesh::Feature\",\"%s\")\n"
    "App.activeDocument().%s.Mesh = mesh\n", name1.c_str(), name2.c_str(), name3.c_str(), name3.c_str());
 
  updateActive();
  commitCommand();
}

bool CmdMeshDifference::isActive(void)
{
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 2;
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshIntersection);

CmdMeshIntersection::CmdMeshIntersection()
  :Command("Mesh_Intersection")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Intersection");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  iAccel        = 0;
}

void CmdMeshIntersection::activated(int iMsg)
{
  std::vector<App::DocumentObject*> obj = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  std::string name1 = obj.front()->getNameInDocument();
  std::string name2 = obj.back()->getNameInDocument();
  std::string name3 = getUniqueObjectName("Intersection");
  openCommand("Mesh Union");
  doCommand(Doc,
    "import Mesh,MeshGui\n"
    "mesh = App.activeDocument().%s.Mesh\n"
    "mesh.intersect(App.activeDocument().%s.Mesh)\n"
    "App.activeDocument().addObject(\"Mesh::Feature\",\"%s\")\n"
    "App.activeDocument().%s.Mesh = mesh\n", name1.c_str(), name2.c_str(), name3.c_str(), name3.c_str());
 
  updateActive();
  commitCommand();
}

bool CmdMeshIntersection::isActive(void)
{
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 2;
}

//--------------------------------------------------------------------------------------

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
  QStringList filter;
  filter << "All Mesh Files (*.stl *.ast *.bms *.obj)";
  filter << "Binary STL (*.stl)";
  filter << "ASCII STL (*.ast)";
  filter << "Binary Mesh (*.bms)";
  filter << "Alias Mesh (*.obj)";
  filter << "Inventor V2.1 ascii (*.iv)";
  //filter << "Nastran (*.nas *.bdf)";
  filter << "All Files (*.*)";

  // Allow multi selection
  QStringList fn = Gui::FileDialog::getOpenFileNames(Gui::getMainWindow(), QObject::tr("Import mesh"), QString(), filter.join(";;"));
  for ( QStringList::Iterator it = fn.begin(); it != fn.end(); ++it )
  {
    QFileInfo fi;
    fi.setFile(*it);

    openCommand("Import Mesh");
    doCommand(Doc,"f = App.activeDocument().addObject(\"Mesh::Import\",\"%s\")", (const char*)fi.baseName().toAscii());
    doCommand(Doc,"f.FileName = \"%s\"",(const char*)(*it).toUtf8());
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


//--------------------------------------------------------------------------------------

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
  std::vector<App::DocumentObject*> docObjs = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  if ( docObjs.size() != 1 )
    return;

  App::DocumentObject* docObj = docObjs.front();

  QString dir = docObj->getNameInDocument();
  QString filter = "Binary STL (*.stl);;ASCII STL (*.stl);;ASCII STL (*.ast);;Binary Mesh (*.bms);;Alias Mesh (*.obj);;"
                   "Inventor V2.1 ascii (*.iv);;VRML V2.0 (*.wrl *.vrml);;Compressed VRML 2.0 (*.wrz);;"
                   "Nastran (*.nas *.bdf);;Python module def (*.py);;All Files (*.*)";

  QString format;
  QString fn = Gui::FileDialog::getSaveFileName(Gui::getMainWindow(), QObject::tr("Export mesh"), dir, filter, &format);
  if (!fn.isEmpty())
  {
    QFileInfo fi(fn);
    if (format == QString("ASCII STL (*.stl)"))
        format = "ast";
    else
        format = fi.suffix();
    openCommand("Export Mesh");
    doCommand(Doc,"f = App.activeDocument().addObject(\"Mesh::Export\",\"%s\")", (const char*)fi.baseName().toAscii());
    doCommand(Doc,"f.FileName = \"%s\"",(const char*)fn.toUtf8());
    doCommand(Doc,"f.Format = \"%s\"",(const char*)format.toAscii());
    doCommand(Doc,"f.Source = App.activeDocument().%s",docObj->getNameInDocument());
    commitCommand();
    updateActive();
  }
}

bool CmdMeshExport::isActive(void)
{
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshVertexCurvature);

CmdMeshVertexCurvature::CmdMeshVertexCurvature()
  :Command("Mesh_VertexCurvature")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Curvature plot");
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
    std::string fName = (*it)->getNameInDocument();
    fName += "_Curvature";
    fName = getUniqueObjectName(fName.c_str());

    openCommand("Mesh VertexCurvature");
    App::DocumentObjectGroup* grp = App::DocumentObjectGroup::getGroupOfObject( *it );
    if (grp)
      doCommand(Doc,"App.activeDocument().getObject(\"%s\").addObject(\"Mesh::Curvature\",\"%s\")",grp->getNameInDocument(), fName.c_str());
    else
      doCommand(Doc,"App.activeDocument().addObject(\"Mesh::Curvature\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),(*it)->getNameInDocument());
  }

  commitCommand();
  updateActive();
}

bool CmdMeshVertexCurvature::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

//--------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------

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
  sPixmap       = "mesh_cut";
}

void CmdMeshPolyCut::activated(int iMsg)
{
  std::vector<App::DocumentObject*> docObj = Gui::Selection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::iterator it = docObj.begin(); it != docObj.end(); ++it )
  {
    if ( it == docObj.begin() ) {
      Gui::Document* doc = getActiveGuiDocument();
      Gui::MDIView* view = doc->getActiveView();
      if ( view->getTypeId().isDerivedFrom(Gui::View3DInventor::getClassTypeId()) ) {
        Gui::View3DInventorViewer* viewer = ((Gui::View3DInventor*)view)->getViewer();
        viewer->startPicking( Gui::View3DInventorViewer::Lasso );
      } else {
        return;
      }
    }

    Gui::ViewProvider* pVP = getActiveGuiDocument()->getViewProvider( *it );
    pVP->setEdit();
  }
}

bool CmdMeshPolyCut::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

//--------------------------------------------------------------------------------------

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
      "App.activeDocument().addObject(\"Mesh::SegmentByMesh\",\"%s\")\n"
      "App.activeDocument().%s.Source = App.activeDocument().%s\n"
      "App.activeDocument().%s.Tool = App.activeDocument().%s\n",
      fName.c_str(), fName.c_str(),  mesh->getNameInDocument(), fName.c_str(), tool->getNameInDocument() );

    commitCommand();
    updateActive();

    App::Document* pDoc = getDocument();
    App::DocumentObject * pObj = pDoc->getObject( fName.c_str() );

    if ( pObj )
    {
      doCommand(Gui,"Gui.hide(\"%s\")", mesh->getNameInDocument());
      doCommand(Gui,"Gui.hide(\"%s\")", tool->getNameInDocument());
      getSelection().clearSelection();
    }
  }
}

bool CmdMeshToolMesh::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 2;
}

//--------------------------------------------------------------------------------------

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
}

void CmdMeshEvaluation::activated(int iMsg)
{
  MeshGui::DlgEvaluateMeshImp* dlg = MeshGui::DockEvaluateMeshImp::instance();
  dlg->setAttribute(Qt::WA_DeleteOnClose);
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    dlg->setMesh((Mesh::Feature*)(*it));
    break;
  }

  dlg->show();
}

bool CmdMeshEvaluation::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return (!MeshGui::DockEvaluateMeshImp::hasInstance()) && (getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0);
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshEvaluateFacet);

CmdMeshEvaluateFacet::CmdMeshEvaluateFacet()
  :Command("Mesh_EvaluateFacet")
{
    sAppModule    = "Mesh";
    sGroup        = QT_TR_NOOP("Mesh");
    sMenuText     = QT_TR_NOOP("Face info");
    sToolTipText  = QT_TR_NOOP("Information about face");
    sWhatsThis    = QT_TR_NOOP("Information about face");
    sStatusTip    = QT_TR_NOOP("Information about face");
    sPixmap       = "mesh_pipette";
}

void CmdMeshEvaluateFacet::activated(int iMsg)
{
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    Gui::View3DInventor* view = static_cast<Gui::View3DInventor*>(doc->getActiveView());
    if (view) {
        Gui::View3DInventorViewer* viewer = view->getViewer();
        viewer->setEditing(true);
        viewer->getWidget()->setCursor(QCursor(Gui::BitmapFactory().pixmap("mesh_pipette"),4,29));
        viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), MeshGui::ViewProviderMeshFaceSet::faceInfoCallback);
     }
}

bool CmdMeshEvaluateFacet::isActive(void)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc || doc->countObjectsOfType(Mesh::Feature::getClassTypeId()) == 0)
        return false;

    Gui::MDIView* view = Gui::getMainWindow()->activeWindow();
    if (view && view->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
        return !viewer->isEditing();
    }

    return false;
}

//--------------------------------------------------------------------------------------

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
    QString msg = QString("The mesh '%1' is ").arg(mesh->getNameInDocument());
    if ( mesh->Mesh.getValue().HasOpenEdges() )
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

//--------------------------------------------------------------------------------------

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
    std::string fName = (*it)->getNameInDocument();
    fName += "_Harmonize";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::HarmonizeNormals\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),(*it)->getNameInDocument());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->getNameInDocument());
  }
}

bool CmdMeshHarmonizeNormals::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

//--------------------------------------------------------------------------------------

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
    std::string fName = (*it)->getNameInDocument();
    fName += "_Flip";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Flip Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FlipNormals\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),(*it)->getNameInDocument());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->getNameInDocument());
  }
}

bool CmdMeshFlipNormals::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

//--------------------------------------------------------------------------------------

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
    std::string fName = (*it)->getNameInDocument();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixDegenerations\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),(*it)->getNameInDocument());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->getNameInDocument());
  }
}

bool CmdMeshFixDegenerations::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

//--------------------------------------------------------------------------------------

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
    std::string fName = (*it)->getNameInDocument();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixDuplicatedFaces\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),(*it)->getNameInDocument());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->getNameInDocument());
  }
}

bool CmdMeshFixDuplicateFaces::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

//--------------------------------------------------------------------------------------

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
    std::string fName = (*it)->getNameInDocument();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Remove duplicated points");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixDuplicatedPoints\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),(*it)->getNameInDocument());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->getNameInDocument());
  }
}

bool CmdMeshFixDuplicatePoints::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

//--------------------------------------------------------------------------------------

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
    std::string fName = (*it)->getNameInDocument();
    fName += "_Fixed";
    fName = getUniqueObjectName(fName.c_str());
    openCommand("Mesh Harmonize Normals");
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FixIndices\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),(*it)->getNameInDocument());
    commitCommand();
    updateActive();
    doCommand(Gui,"Gui.hide(\"%s\")",(*it)->getNameInDocument());
  }
}

bool CmdMeshFixIndices::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshBoundingBox);

CmdMeshBoundingBox::CmdMeshBoundingBox()
  :Command("Mesh_BoundingBox")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Boundings info...");
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
    const MeshCore::MeshKernel& rMesh = ((Mesh::Feature*)(*it))->Mesh.getValue();
    const Base::BoundBox3f& box = rMesh.GetBoundBox();

    Base::Console().Message("Boundings: Min=<%f,%f,%f>, Max=<%f,%f,%f>\n",
                            box.MinX,box.MinY,box.MinZ,box.MaxX,box.MaxY,box.MaxZ);

    QString bound = QString("Min=<%1,%2,%3>\n\nMax=<%4,%5,%6>").arg(box.MinX).arg(box.MinY).arg(box.MinZ)
                                                               .arg(box.MaxX).arg(box.MaxY).arg(box.MaxZ);
    QMessageBox::information(Gui::getMainWindow(), QObject::tr("Boundings"), bound);
    break;
  }
}

bool CmdMeshBoundingBox::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) == 1;
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshBuildRegularSolid);

CmdMeshBuildRegularSolid::CmdMeshBuildRegularSolid()
  :Command("Mesh_BuildRegularSolid")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Regular solid...");
  sToolTipText  = QT_TR_NOOP("Builds a regular solid");
  sWhatsThis    = QT_TR_NOOP("Builds a regular solid");
  sStatusTip    = QT_TR_NOOP("Builds a regular solid");
  sPixmap       = "solid_mesh";
}

void CmdMeshBuildRegularSolid::activated(int iMsg)
{
  MeshGui::SingleDlgRegularSolidImp::instance()->show();
}

bool CmdMeshBuildRegularSolid::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return (!MeshGui::SingleDlgRegularSolidImp::hasInstance())&&hasActiveDocument();
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshFillupHoles);

CmdMeshFillupHoles::CmdMeshFillupHoles()
  :Command("Mesh_FillupHoles")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Fill holes...");
  sToolTipText  = QT_TR_NOOP("Fill holes of the mesh");
  sWhatsThis    = QT_TR_NOOP("Fill holes of the mesh");
  sStatusTip    = QT_TR_NOOP("Fill holes of the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshFillupHoles::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  bool ok;
  int FillupHolesOfLength = QInputDialog::getInteger( Gui::getMainWindow(), QObject::tr("Fill holes"), QObject::tr("Fill holes with maximum number of edges:"), 
                                                      3, 3, 10000, 1, &ok  );
  if (!ok) return;

  openCommand("Fill up holes");
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->getNameInDocument();
    fName += "_fill";
    fName = getUniqueObjectName(fName.c_str());
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::FillHoles\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),(*it)->getNameInDocument());
    doCommand(Doc,"App.activeDocument().%s.FillupHolesOfLength = %d",fName.c_str(), FillupHolesOfLength);
  }
  commitCommand();
  updateActive();
}

bool CmdMeshFillupHoles::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshRemoveComponents);

CmdMeshRemoveComponents::CmdMeshRemoveComponents()
  :Command("Mesh_RemoveComponents")
{
  sAppModule    = "Mesh";
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Remove components...");
  sToolTipText  = QT_TR_NOOP("Remove topologic independant components from the mesh");
  sWhatsThis    = QT_TR_NOOP("Remove topologic independant components from the mesh");
  sStatusTip    = QT_TR_NOOP("Remove topologic independant components from the mesh");
//  sPixmap       = "curv_info";
}

void CmdMeshRemoveComponents::activated(int iMsg)
{
  std::vector<App::DocumentObject*> meshes = getSelection().getObjectsOfType(Mesh::Feature::getClassTypeId());
  bool ok;
  int RemoveCompOfSize = QInputDialog::getInteger( Gui::getMainWindow(), QObject::tr("Remove components"), QObject::tr("Removes components up to a maximum number of triangles:"), 
                                                      3, 1, 10000000, 1, &ok );
  if (!ok) return;

  openCommand("Remove components");
  for ( std::vector<App::DocumentObject*>::const_iterator it = meshes.begin(); it != meshes.end(); ++it )
  {
    std::string fName = (*it)->getNameInDocument();
    fName += "_rem_comps";
    fName = getUniqueObjectName(fName.c_str());
    doCommand(Doc,"App.activeDocument().addObject(\"Mesh::RemoveComponents\",\"%s\")",fName.c_str());
    doCommand(Doc,"App.activeDocument().%s.Source = App.activeDocument().%s",fName.c_str(),(*it)->getNameInDocument());
    doCommand(Doc,"App.activeDocument().%s.RemoveCompOfSize = %d",fName.c_str(), RemoveCompOfSize);
  }
  commitCommand();
  updateActive();
}

bool CmdMeshRemoveComponents::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Mesh::Feature::getClassTypeId()) > 0;
}

//--------------------------------------------------------------------------------------

DEF_STD_CMD_A(CmdMeshFillInteractiveHole);

CmdMeshFillInteractiveHole::CmdMeshFillInteractiveHole()
  :Command("Mesh_FillInteractiveHole")
{
    sAppModule    = "Mesh";
    sGroup        = QT_TR_NOOP("Mesh");
    sMenuText     = QT_TR_NOOP("Close hole");
    sToolTipText  = QT_TR_NOOP("Close holes interactively");
    sWhatsThis    = QT_TR_NOOP("Close holes interactively");
    sStatusTip    = QT_TR_NOOP("Close holes interactively");
    sPixmap       = "mesh_boundary";
}

void CmdMeshFillInteractiveHole::activated(int iMsg)
{
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    Gui::View3DInventor* view = static_cast<Gui::View3DInventor*>(doc->getActiveView());
    if (view) {
        Gui::View3DInventorViewer* viewer = view->getViewer();
        viewer->setEditing(true);
        viewer->getWidget()->setCursor(QCursor(Gui::BitmapFactory().pixmap("mesh_fillhole"),5,5));
        viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), MeshGui::ViewProviderMeshFaceSet::fillHoleCallback);
     }
}

bool CmdMeshFillInteractiveHole::isActive(void)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc || doc->countObjectsOfType(Mesh::Feature::getClassTypeId()) == 0)
        return false;

    Gui::MDIView* view = Gui::getMainWindow()->activeWindow();
    if (view && view->isDerivedFrom(Gui::View3DInventor::getClassTypeId())) {
        Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(view)->getViewer();
        return !viewer->isEditing();
    }

    return false;
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
  rcCmdMgr.addCommand(new CmdMeshUnion());
  rcCmdMgr.addCommand(new CmdMeshDifference());
  rcCmdMgr.addCommand(new CmdMeshIntersection());
  rcCmdMgr.addCommand(new CmdMeshDemolding());
  rcCmdMgr.addCommand(new CmdMeshPolyPick());
  rcCmdMgr.addCommand(new CmdMeshPolyCut());
  rcCmdMgr.addCommand(new CmdMeshToolMesh());
  rcCmdMgr.addCommand(new CmdMeshTransform());
  rcCmdMgr.addCommand(new CmdMeshEvaluation());
  rcCmdMgr.addCommand(new CmdMeshEvaluateFacet());
  rcCmdMgr.addCommand(new CmdMeshEvaluateSolid());
  rcCmdMgr.addCommand(new CmdMeshHarmonizeNormals());
  rcCmdMgr.addCommand(new CmdMeshFlipNormals());
  rcCmdMgr.addCommand(new CmdMeshFixDegenerations());
  rcCmdMgr.addCommand(new CmdMeshFixDuplicateFaces());
  rcCmdMgr.addCommand(new CmdMeshFixDuplicatePoints());
  rcCmdMgr.addCommand(new CmdMeshFixIndices());
  rcCmdMgr.addCommand(new CmdMeshBoundingBox());
  rcCmdMgr.addCommand(new CmdMeshBuildRegularSolid());
  rcCmdMgr.addCommand(new CmdMeshFillupHoles());
  rcCmdMgr.addCommand(new CmdMeshRemoveComponents());
  rcCmdMgr.addCommand(new CmdMeshFillInteractiveHole());
}
