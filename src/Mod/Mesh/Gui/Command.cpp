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
#endif

#include "../App/MeshFeature.h"

#include <Base/Console.h>
#include <Base/Exception.h>
#include <App/Document.h>
#include <App/Feature.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/FileDialog.h>

using namespace Mesh;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdMeshImport THIS IS JUST A TEST COMMAND
//===========================================================================
DEF_STD_CMD_A(CmdMeshImport);

CmdMeshImport::CmdMeshImport()
  :CppCommand("Mesh_Test")
{
  sAppModule    = "Mesh";
  sGroup        = "Mesh";
  sMenuText     = "Import Mesh";
//  sToolTipText  = "Create or change an Import Mesh feature";
  sToolTipText  = "Open STL files from File|Open, please!";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Open";
  iAccel        = Qt::CTRL+Qt::Key_T;
}

void CmdMeshImport::activated(int iMsg)
{
  QString fn = Gui::FileDialog::getOpenFileName( QString::null, "STL (*.stl *.ast);;All Files (*.*)", 
                                                 Gui::ApplicationWindow::Instance );
  if (! fn.isEmpty() )
  {
    openCommand("Mesh ImportSTL Create");
    doCommand(Doc,"f = App.DocGet().AddFeature(\"MeshImport\")");
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
  sAppModule    = "Mesh";
  sGroup        = "Mesh";
  sMenuText     = "Curvature per vertex";
  sToolTipText  = "Calculates the curvature of the vetices of a mesh";
  sWhatsThis    = sToolTipText;
  sStatusTip    = sToolTipText;
  sPixmap       = "Std_Tool1";
}

void CmdMeshVertexCurvature::activated(int iMsg)
{
  openCommand("Mesh VertexCurvature Create");
  doCommand(Doc,"f = App.DocGet().GetActiveFeature().getMesh().calcVertexCurvature()");
  commitCommand();
  updateActive();

  App::Feature* fea = getActiveDocument()->getDocument()->GetActiveFeature();
  MeshFeature* mesh = dynamic_cast<MeshFeature*>(fea);
  MeshWithProperty& rMesh = mesh->getMesh();
  MeshPropertyCurvature *prop = dynamic_cast<MeshPropertyCurvature*>(rMesh.Get("VertexCurvature") );
  if( prop && prop->isValid() )
  {
    MeshPropertyColor* color = dynamic_cast<MeshPropertyColor*>(rMesh.Get("VertexColor") );
    if ( !color )
    {
      color = new MeshPropertyColor();
      rMesh.Add(color,"VertexCurvature");  
    }

    std::vector<float> aValues = prop->getCurvature( MeshPropertyCurvature::MaxCurvature );
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
    Base::Console().Message("Min: %.2f, Max: %.2f, Range:[%.2f, %.2f]\n", fMin, fMax, fRMin, fRMax);
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
}

bool CmdMeshVertexCurvature::isActive(void)
{
  if( getActiveDocument() ){
    App::Feature* fea = getActiveDocument()->getDocument()->GetActiveFeature();
    if ( fea && strcmp(fea->Type(), "MeshImport")==0)
      return true;
  }
  return false;
}

void CreateCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::ApplicationWindow::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdMeshImport());
  rcCmdMgr.addCommand(new CmdMeshVertexCurvature());
}
