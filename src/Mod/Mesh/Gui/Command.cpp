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
DEF_STD_CMD_A(CmdMeshExMakeMesh);

CmdMeshExMakeMesh::CmdMeshExMakeMesh()
  :CppCommand("Mesh_ExMakeMesh")
{
  sAppModule    = QT_TR_NOOP("Mesh");
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
    "App.DocGet().AddFeature(\"Mesh\",\"MeshBox\")\n"
    "App.DocGet().MeshBox.setMesh(mb)\n"
    "App.DocGet().MeshBox.showMode = \"FlatWire\"\n" 
    "App.DocGet().Update()" );

  doCommand(Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFit\")");
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
  :CppCommand("Mesh_ExMakeTool")
{
  sAppModule    = QT_TR_NOOP("Mesh");
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
    "App.DocGet().AddFeature(\"Mesh\",\"MeshTool\")\n"
    "App.DocGet().MeshTool.setMesh(mt)\n"
    "App.DocGet().MeshTool.shadedMaterial.diffuseColor = (0.5,0.2,0.2)\n"
    "App.DocGet().MeshTool.showMode = \"FlatWire\"" );

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
  :CppCommand("Mesh_ExMakeUnion")
{
  sAppModule    = QT_TR_NOOP("Mesh");
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
    "m1 = App.DocGet().MeshBox.getMesh()\n"
    "m2 = App.DocGet().MeshTool.getMesh()\n"
    "m3 = m1.copy()\n"
    "m3.Union(m2)\n"
    "App.DocGet().AddFeature(\"Mesh\",\"MeshUnion\")\n"
    "App.DocGet().MeshUnion.setMesh(m3)\n"
    "App.DocGet().MeshUnion.shadedMaterial.ambientColor = (0.1,1,0)\n"
    "App.DocGet().MeshUnion.shadedMaterial.transparency = 0.5\n"
    "App.DocGet().MeshUnion.lineMaterial.ambientColor = (0.1,0.11,0.1)\n"
    "App.DocGet().MeshUnion.lineSize = 2\n"
    "App.DocGet().MeshUnion.showMode = \"FlatWire\"" );

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
  :CppCommand("Mesh_Import")
{
  sAppModule    = QT_TR_NOOP("Mesh");
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
  QString fn = Gui::FileDialog::getOpenFileName( QString::null, filter, Gui::ApplicationWindow::Instance );
  if (! fn.isEmpty() )
  {
    openCommand("Mesh ImportSTL Create");
    doCommand(Doc,"f = App.DocGet().AddFeature(\"MeshImport\",\"MeshImport\")");
    doCommand(Doc,"f.FileName = \"%s\"",fn.ascii());
    commitCommand();
 
    updateActive();
  }
}

bool CmdMeshImport::isActive(void)
{
  if( getActiveDocument() )
    return true;
  else
    return false;
}

DEF_STD_CMD_A(CmdMeshVertexCurvature);

CmdMeshVertexCurvature::CmdMeshVertexCurvature()
  :CppCommand("Mesh_VertexCurvature")
{
  sAppModule    = QT_TR_NOOP("Mesh");
  sGroup        = QT_TR_NOOP("Mesh");
  sMenuText     = QT_TR_NOOP("Curvature per vertex");
  sToolTipText  = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sWhatsThis    = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sStatusTip    = QT_TR_NOOP("Calculates the curvature of the vertices of a mesh");
  sPixmap       = "curv_info";
}

void CmdMeshVertexCurvature::activated(int iMsg)
{
//  openCommand("Mesh VertexCurvature Create");
//  doCommand(Doc,"f = App.DocGet().GetActiveFeature().getMesh().calcVertexCurvature()");
//  commitCommand();
//  updateActive();

  MeshFeature* mesh;
  const std::set<App::Feature*>& sel = Gui::Selection().Selection();
  for ( std::set<App::Feature*>::iterator it = sel.begin(); it != sel.end(); ++it )
  {
    if ( strcmp( (*it)->type(), "MeshImport") == 0 )
    {
      mesh = dynamic_cast<MeshFeature*>(*it);
      break;
    }
  }

//  App::Feature* fea = getActiveDocument()->getDocument()->GetActiveFeature();
//  MeshFeature* mesh = dynamic_cast<MeshFeature*>(fea);
  MeshWithProperty& rMesh = mesh->getMesh();
  MeshAlgos::calcVertexCurvature( &rMesh );

  MeshPropertyCurvature *prop = dynamic_cast<MeshPropertyCurvature*>(rMesh.Get("VertexCurvature") );
  if( prop && prop->isValid() )
  {
    std::map<MeshPropertyCurvature::Mode, std::string> modi;
    modi[MeshPropertyCurvature::MaxCurvature] = "Max. curvature";
    modi[MeshPropertyCurvature::MinCurvature] = "Min. curvature";
    modi[MeshPropertyCurvature::AvgCurvature] = "Avg. curvature";
    modi[MeshPropertyCurvature::GaussCurvature] = "Gaussian curvature";


    for ( std::map<MeshPropertyCurvature::Mode, std::string>::iterator mI = modi.begin(); mI != modi.end(); ++mI)
    {
      const char* mode = mI->second.c_str();
      printf("Mode: %s\n", mode);
      MeshPropertyColor* color = dynamic_cast<MeshPropertyColor*>(rMesh.Get( mode ) );
      if ( !color )
      {
        color = new MeshPropertyColor();
        rMesh.Add( color, mode );
      }

      std::vector<float> aValues = prop->getCurvature( mI->first );
      if ( aValues.empty() ) return; // no values inside
      float fMin = *std::min_element( aValues.begin(), aValues.end() );/*-1.0f*/;
      float fMax = *std::max_element( aValues.begin(), aValues.end() );/* 1.0f*/;

      // histogram over all values
      std::map<int, int> aHistogram;
      for ( std::vector<float>::iterator it2 = aValues.begin(); it2 != aValues.end(); ++it2 )
      {
        int grp = (int)(10.0f*( *it2 - fMin )/( fMax - fMin ));
        aHistogram[grp]++;
      }

      float fRMin;
      for ( std::map<int, int>::iterator mIt = aHistogram.begin(); mIt != aHistogram.end(); ++mIt )
      {
        if ( (float)mIt->second / (float)aValues.size() > 0.05f )
        {
          fRMin = mIt->first * ( fMax - fMin )/10.0f + fMin;
          break;
        }
      }

      float fRMax;
      for ( std::map<int, int>::reverse_iterator rIt = aHistogram.rbegin(); rIt != aHistogram.rend(); ++rIt )
      {
        if ( (float)rIt->second / (float)aValues.size() > 0.05f )
        {
          fRMax = rIt->first * ( fMax - fMin )/10.0f + fMin;
          break;
        }
      }

      float fAbs = std::max<float>(fabs(fRMin), fabs(fRMax));
      fRMin = -fAbs;
      fRMax =  fAbs;
      Base::Console().Message("Type: %s: Min: %.2f, Max: %.2f, Range:[%.2f, %.2f]\n", mode, fMin, fMax, fRMin, fRMax);
      fMin = fRMin; fMax = fRMax;

      color->Color.clear();
      for ( std::vector<float>::iterator it = aValues.begin(); it != aValues.end(); ++it )
      {
        MeshPropertyColor::fColor fCol;
        float x = *it;

        // Lookup table (linear interpolating)
        if ( x < fMin )
        { fCol.r = 0.0f; fCol.g = 0.0f; fCol.b = 1.0f; }
        else if ( x < 0.75f*fMin+0.25f*fMax )
        { float s = 4.0f*(x-fMin)/(fMax-fMin);
          fCol.r = 0.0f; fCol.g = s; fCol.b = 1.0f; }
        else if ( x < 0.5f*fMin+0.5f*fMax )
        { float s = (4.0f*x-3.0f*fMin-fMax)/(fMax-fMin);
          fCol.r = 0.0f; fCol.g = 1.0f; fCol.b = 1.0f-s; }
        else if ( x < 0.25f*fMin+0.75f*fMax )
        { float s = (4.0f*x-2.0f*(fMin+fMax))/(fMax-fMin);
          fCol.r = s; fCol.g = 1.0f; fCol.b = 0.0f; }
        else if ( x < fMax )
        { float s = (4.0f*x-fMin-3.0f*fMax)/(fMax-fMin);
          fCol.r = 1.0f; fCol.g = 1.0f-s; fCol.b = 0.0f; }
        else
        { fCol.r = 1.0f; fCol.g = 0.0f; fCol.b = 0.0f; }
        color->Color.push_back( fCol );
      }
    }

    Gui::ViewProviderInventor *pcProv = getActiveDocument()->getViewProvider( mesh );
    if ( pcProv )
    {
      pcProv->setMode("Max. curvature");
      getActiveDocument()->onUpdate();
    }
  }
  else if ( prop )
  {
    Base::Console().Warning("Invalid property 'VertexCurvature' found.\n");
  }
  else
  {
    Base::Console().Warning("Property 'VertexCurvature' not found.\n");
  }
}

bool CmdMeshVertexCurvature::isActive(void)
{
  // Check for the selected mesh feature
  const std::set<App::Feature*>& sel = Gui::Selection().Selection();
  for ( std::set<App::Feature*>::iterator it = sel.begin(); it != sel.end(); ++it )
  {
    if ( strcmp( (*it)->type(), "MeshImport") == 0 )
  	  return true; // MeshFeature found
  }

  return false;
//  if( getActiveDocument() ){
//    App::Feature* fea = getActiveDocument()->getDocument()->GetActiveFeature();
//    if ( fea && strcmp(fea->Type(), "MeshImport")==0)
//      return true;
//  }
//  return false;
}

void CreateMeshCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::ApplicationWindow::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdMeshImport());
  rcCmdMgr.addCommand(new CmdMeshVertexCurvature());
  rcCmdMgr.addCommand(new CmdMeshExMakeMesh());
  rcCmdMgr.addCommand(new CmdMeshExMakeTool());
  rcCmdMgr.addCommand(new CmdMeshExMakeUnion());
}
