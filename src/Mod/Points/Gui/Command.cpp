/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
#endif

#include <Base/Exception.h>
#include <Base/Matrix.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/MainWindow.h>
#include <Gui/Command.h>
#include <Gui/FileDialog.h>
#include <Gui/Selection.h>
#include <Gui/ViewProvider.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include "../App/PointsFeature.h"
#include "DlgPointsReadImp.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//===========================================================================
// CmdPointsImport
//===========================================================================
DEF_STD_CMD_A(CmdPointsImport);

CmdPointsImport::CmdPointsImport()
  :Command("Points_Import")
{
  sAppModule    = "Points";
  sGroup        = QT_TR_NOOP("Points");
  sMenuText     = QT_TR_NOOP("Import points...");
  sToolTipText  = QT_TR_NOOP("Imports a point cloud");
  sWhatsThis    = QT_TR_NOOP("Imports a point cloud");
  sStatusTip    = QT_TR_NOOP("Imports a point cloud");
  sPixmap       = "Test1";
}

void CmdPointsImport::activated(int iMsg)
{
  QString dir = Gui::FileDialog::getWorkingDirectory();
  QString fn = Gui::FileDialog::getOpenFileName(Gui::getMainWindow(), QString::null, dir, "Ascii Points (*.asc);;All Files (*.*)");
  if ( fn.isEmpty() )
    return;

  if (! fn.isEmpty() )
  {
    QFileInfo fi;
    fi.setFile(fn);

    openCommand("Points Import Create");
    doCommand(Doc,"f = App.document().addObject(\"Points::ImportAscii\",\"%s\")", (const char*)fi.baseName().toAscii());
    doCommand(Doc,"f.FileName = \"%s\"",(const char*)fn.toAscii());
    commitCommand();
 
    updateActive();
    Gui::FileDialog::setWorkingDirectory(fn);
  }
}

bool CmdPointsImport::isActive(void)
{
  if( getActiveGuiDocument() )
    return true;
  else
    return false;
}

DEF_STD_CMD_A(CmdPointsExport);

CmdPointsExport::CmdPointsExport()
  :Command("Points_Export")
{
  sAppModule    = "Points";
  sGroup        = QT_TR_NOOP("Points");
  sMenuText     = QT_TR_NOOP("Export points...");
  sToolTipText  = QT_TR_NOOP("Exports a point cloud");
  sWhatsThis    = QT_TR_NOOP("Exports a point cloud");
  sStatusTip    = QT_TR_NOOP("Exports a point cloud");
  sPixmap       = "Test2";
}

void CmdPointsExport::activated(int iMsg)
{
  QString dir = Gui::FileDialog::getWorkingDirectory();
  QString fn = Gui::FileDialog::getSaveFileName(Gui::getMainWindow(), QString::null, dir, "Ascii Points (*.asc);;All Files (*.*)");
  if ( fn.isEmpty() )
    return;

  if (! fn.isEmpty() )
  {
    QFileInfo fi;
    fi.setFile(fn);
  
    openCommand("Export Points");
    std::vector<App::DocumentObject*> points = getSelection().getObjectsOfType(Points::Feature::getClassTypeId());
    doCommand(Doc,"f = App.document().addObject(\"Points::Export\",\"%s\")", (const char*)fi.baseName().toAscii());
    doCommand(Doc,"f.FileName = \"%s\"",(const char*)fn.toAscii());
    doCommand(Doc,"l=list()");
    
    for ( std::vector<App::DocumentObject*>::const_iterator it = points.begin(); it != points.end(); ++it )
    {
      doCommand(Doc,"l.append(App.document().getObject(\"%s\"))",(*it)->name.getValue());
    }

    doCommand(Doc,"f.Sources = l");
    commitCommand();
    updateActive();
    Gui::FileDialog::setWorkingDirectory(fn);
  }
}

bool CmdPointsExport::isActive(void)
{
  return getSelection().countObjectsOfType(Points::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdPointsTransform);

CmdPointsTransform::CmdPointsTransform()
  :Command("Points_Transform")
{
  sAppModule    = "Points";
  sGroup        = QT_TR_NOOP("Points");
  sMenuText     = QT_TR_NOOP("Transform Points");
  sToolTipText  = QT_TR_NOOP("Test to transform a point cloud");
  sWhatsThis    = QT_TR_NOOP("Test to transform a point cloud");
  sStatusTip    = QT_TR_NOOP("Test to transform a point cloud");
  sPixmap       = "Test1";
}

void CmdPointsTransform::activated(int iMsg)
{
  // This is a test command to transform a point cloud directly written in C++ (not Python)
  Base::Matrix4D trans( Base::Vector3f(0.0f, 0.0f, 0.0f), Base::Vector3f(0.0f, 0.0f, 1.0f), 1.570796f  );

  App::Document* pDoc = App::GetApplication().getActiveDocument();
  Gui::Document* pGui = Gui::Application::Instance->activeDocument();

  std::vector<App::DocumentObject*> points = getSelection().getObjectsOfType(Points::Feature::getClassTypeId());
  for ( std::vector<App::DocumentObject*>::const_iterator it = points.begin(); it != points.end(); ++it )
  {
    if ( (*it)->getTypeId().isDerivedFrom(Points::Transform::getClassTypeId()) )
    {
      Points::Transform* f = (Points::Transform*)(*it);
      f->Trnsfrm.setValue( trans*f->Trnsfrm.getValue() );
      pDoc->recompute();
    }
    else
    {
      Points::Transform* f = (Points::Transform*)pDoc->addObject(Points::Transform::getClassTypeId().getName(),"Transform");
      pGui->setHide( (*it)->name.getValue() );
      f->Source.setValue(dynamic_cast<App::AbstractFeature*>(*it));
      pDoc->recompute();
    }
  }
}

bool CmdPointsTransform::isActive(void)
{
  return getSelection().countObjectsOfType(Points::Feature::getClassTypeId()) > 0;
}

DEF_STD_CMD_A(CmdPointsPolyCut);

CmdPointsPolyCut::CmdPointsPolyCut()
  :Command("Points_PolyCut")
{
  sAppModule    = "Points";
  sGroup        = QT_TR_NOOP("Points");
  sMenuText     = QT_TR_NOOP("Cut point cloud");
  sToolTipText  = QT_TR_NOOP("Cuts a point cloud with a picked polygon");
  sWhatsThis    = QT_TR_NOOP("Cuts a point cloud with a picked polygon");
  sStatusTip    = QT_TR_NOOP("Cuts a point cloud with a picked polygon");
  sPixmap       = "PolygonPick";
}

void CmdPointsPolyCut::activated(int iMsg)
{
  std::vector<App::DocumentObject*> docObj = Gui::Selection().getObjectsOfType(Points::Feature::getClassTypeId());
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

bool CmdPointsPolyCut::isActive(void)
{
  // Check for the selected mesh feature (all Mesh types)
  return getSelection().countObjectsOfType(Points::Feature::getClassTypeId()) > 0;
}

void CreatePointsCommands(void)
{
  Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();
  rcCmdMgr.addCommand(new CmdPointsImport());
  rcCmdMgr.addCommand(new CmdPointsExport());
  rcCmdMgr.addCommand(new CmdPointsTransform());
  rcCmdMgr.addCommand(new CmdPointsPolyCut());
}
