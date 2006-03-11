/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <qapplication.h>
# include <qcursor.h>
# include <qdatetime.h>
# include <qdesktopwidget.h>
# include <qfileinfo.h>
# include <qimage.h>
# include <qmessagebox.h>
# include <qtextedit.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoPerspectiveCamera.h>
#endif


#include "Command.h"
#include "Action.h"
#include "Application.h"
#include "FileDialog.h"
#include "MainWindow.h"
#include "View.h"
#include "Document.h"
#include "Macro.h"
#include "DlgDisplayPropertiesImp.h"
#include "DlgSettingsImageImp.h"
#include "Selection.h"
#include "SoFCOffscreenRenderer.h"
#include "View3DInventor.h"
#include "View3DInventorViewer.h"

#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <App/Document.h>
#include <App/Feature.h>

using namespace Gui;
using Gui::Dialog::DlgSettingsImageImp;

void writeJPEGComment(const char* InFile, const char* OutFile, const char* Comment);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//===========================================================================
// Std_OrthographicCamera
//===========================================================================
DEF_STD_CMD_A(StdCmdOrthographicCamera);

StdCmdOrthographicCamera::StdCmdOrthographicCamera()
  :Command("Std_OrthographicCamera")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Orthographic view");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void StdCmdOrthographicCamera::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"OrthographicCamera\")");
}

bool StdCmdOrthographicCamera::isActive(void)
{
  return true;
}
//===========================================================================
// Std_PerspectiveCamera
//===========================================================================
DEF_STD_CMD_A(StdCmdPerspectiveCamera);

StdCmdPerspectiveCamera::StdCmdPerspectiveCamera()
  :Command("Std_PerspectiveCamera")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Perspective view");
  sToolTipText  = sMenuText;
  sWhatsThis    = sMenuText;
  sStatusTip    = sMenuText;
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void StdCmdPerspectiveCamera::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"PerspectiveCamera\")");
}

bool StdCmdPerspectiveCamera::isActive(void)
{
  return true;
}

//===========================================================================
// Std_CameraType
//===========================================================================
DEF_STD_CMD_GROUP_A(StdCameraType);

StdCameraType::StdCameraType()
 : CommandGroup("Std_CameraType")
{
  sGroup        = QT_TR_NOOP("Standard-View");

  const char* sMenuText1  = QT_TR_NOOP("Orthographic view");
  const char* sMenuText2  = QT_TR_NOOP("Perspective view");
  
  _aCommands.push_back( new CommandItem(this, sMenuText1,sMenuText1,sMenuText1,sMenuText1/*,"ortho_xpm"*/,      0,Qt::Key_O) );
  _aCommands.push_back( new CommandItem(this, sMenuText2,sMenuText2,sMenuText2,sMenuText2/*,"perspective_xpm"*/,0,Qt::Key_P) );
}

void StdCameraType::activated(int iMsg)
{
  if (iMsg == 0)
    doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"OrthographicCamera\")");
  else if (iMsg == 1)
    doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"PerspectiveCamera\")");
}

bool StdCameraType::isActive(void)
{
  View3DInventor* view = dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if ( view )
  {
    // update the action group if needed
    ActionGroup* pActGrp = reinterpret_cast<ActionGroup*>(_pcAction->qt_cast("Gui::ActionGroup"));
    if ( pActGrp )
    {
      int index = pActGrp->currentActive();
      int type = (view->getViewer()->getCameraType() == SoOrthographicCamera::getClassTypeId() ? 0 : 1);
      if ( type != index )
        pActGrp->setCurrentActive( type );
    }

    return true;
  }

  return false;
}

//===========================================================================
// Std_ToggleVisibility
//===========================================================================
DEF_STD_CMD_A(StdCmdToggleVisibility);

StdCmdToggleVisibility::StdCmdToggleVisibility()
  :Command("Std_ToggleVisibility")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Toggle visibility");
  sToolTipText  = QT_TR_NOOP("Toggles visibility");
  sWhatsThis    = QT_TR_NOOP("Toggles visibility");
  sStatusTip    = QT_TR_NOOP("Toggles visibility");
  sPixmap       = "Std_Tool1";
  iAccel        = Qt::Key_Space;
}

void StdCmdToggleVisibility::activated(int iMsg)
{
  // go through all documents
  const std::vector<App::Document*> docs = App::GetApplication().getDocuments();
  for ( std::vector<App::Document*>::const_iterator it = docs.begin(); it != docs.end(); ++it )
  {
    Document *pcDoc = Application::Instance->getDocument(*it);
    const std::vector<App::AbstractFeature*> sel = Selection().getFeaturesOfType(App::AbstractFeature::getClassTypeId(), (*it)->getName());
    for(std::vector<App::AbstractFeature*>::const_iterator ft=sel.begin();ft!=sel.end();ft++)
    {
      if ( pcDoc && pcDoc->isShow((*ft)->name.getValue()) )
        doCommand(Gui,"Gui.getDocument(\"%s\").hide(\"%s\")", (*it)->getName(), (*ft)->name.getValue());
      else
        doCommand(Gui,"Gui.getDocument(\"%s\").show(\"%s\")", (*it)->getName(), (*ft)->name.getValue());
    }
  }
}

bool StdCmdToggleVisibility::isActive(void)
{
  return ( Gui::Selection().size() != 0 );
}

//===========================================================================
// Std_SetMaterial
//===========================================================================
DEF_STD_CMD_A(StdCmdSetMaterial);

StdCmdSetMaterial::StdCmdSetMaterial()
  :Command("Std_SetMaterial")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Material");
  sToolTipText  = QT_TR_NOOP("Set the material properties of the selcted feature");
  sWhatsThis    = QT_TR_NOOP("Set the material properties of the selcted feature");
  sStatusTip    = QT_TR_NOOP("Set the material properties of the selcted feature");
  sPixmap       = "Std_Tool1";
  iAccel        = Qt::CTRL+Qt::Key_D;
}

void StdCmdSetMaterial::activated(int iMsg)
{
  Gui::Dialog::DlgDisplayPropertiesImp dlg(this, getMainWindow()->activeWindow(), "Display", true );
  dlg.exec();
}

bool StdCmdSetMaterial::isActive(void)
{
  return Gui::Selection().size() != 0;
}

//===========================================================================
// Std_ViewBottom
//===========================================================================
DEF_3DV_CMD(StdCmdViewBottom)

StdCmdViewBottom::StdCmdViewBottom()
  :Command("Std_ViewBottom")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Bottom");
  sToolTipText  = QT_TR_NOOP("Set to bottom view");
  sWhatsThis    = QT_TR_NOOP("Set to bottom view");
  sStatusTip    = QT_TR_NOOP("Set to bottom view");
  sPixmap       = "view_bottom";
  iAccel        = Qt::Key_5;
}

void StdCmdViewBottom::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ViewBottom\")");
}

//===========================================================================
// Std_ViewFront
//===========================================================================
DEF_3DV_CMD(StdCmdViewFront);

StdCmdViewFront::StdCmdViewFront()
  :Command("Std_ViewFront")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Front");
  sToolTipText  = QT_TR_NOOP("Set to front view");
  sWhatsThis    = QT_TR_NOOP("Set to front view");
  sStatusTip    = QT_TR_NOOP("Set to front view");
  sPixmap       = "view_front";
  iAccel        = Qt::Key_1;
}

void StdCmdViewFront::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ViewFront\")");
}

//===========================================================================
// Std_ViewLeft
//===========================================================================
DEF_3DV_CMD(StdCmdViewLeft);

StdCmdViewLeft::StdCmdViewLeft()
  :Command("Std_ViewLeft")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Left");
  sToolTipText  = QT_TR_NOOP("Set to left view");
  sWhatsThis    = QT_TR_NOOP("Set to left view");
  sStatusTip    = QT_TR_NOOP("Set to left view");
  sPixmap       = "view_left";
  iAccel        = Qt::Key_6;
}

void StdCmdViewLeft::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ViewLeft\")");
}

//===========================================================================
// Std_ViewRear
//===========================================================================
DEF_3DV_CMD(StdCmdViewRear);

StdCmdViewRear::StdCmdViewRear()
  :Command("Std_ViewRear")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Rear");
  sToolTipText  = QT_TR_NOOP("Set to rear view");
  sWhatsThis    = QT_TR_NOOP("Set to rear view");
  sStatusTip    = QT_TR_NOOP("Set to rear view");
  sPixmap       = "view_back";
  iAccel        = Qt::Key_4;
}

void StdCmdViewRear::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ViewRear\")");
}

//===========================================================================
// Std_ViewRight
//===========================================================================
DEF_3DV_CMD(StdCmdViewRight);

StdCmdViewRight::StdCmdViewRight()
  :Command("Std_ViewRight")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Right");
  sToolTipText  = QT_TR_NOOP("Set to right view");
  sWhatsThis    = QT_TR_NOOP("Set to right view");
  sStatusTip    = QT_TR_NOOP("Set to right view");
  sPixmap       = "view_right";
  iAccel        = Qt::Key_3;
}

void StdCmdViewRight::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ViewRight\")");
}

//===========================================================================
// Std_ViewTop
//===========================================================================
DEF_3DV_CMD(StdCmdViewTop);

StdCmdViewTop::StdCmdViewTop()
  :Command("Std_ViewTop")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Top");
  sToolTipText  = QT_TR_NOOP("Set to top view");
  sWhatsThis    = QT_TR_NOOP("Set to top view");
  sStatusTip    = QT_TR_NOOP("Set to top view");
  sPixmap       = "view_top";
  iAccel        = Qt::Key_2;
}

void StdCmdViewTop::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ViewTop\")");
}

//===========================================================================
// Std_ViewAxo
//===========================================================================
DEF_3DV_CMD(StdCmdViewAxo);

StdCmdViewAxo::StdCmdViewAxo()
  :Command("Std_ViewAxo")
{
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Axometric");
  sToolTipText= QT_TR_NOOP("Set to axometric view");
  sWhatsThis  = QT_TR_NOOP("Set to axometric view");
  sStatusTip  = QT_TR_NOOP("Set to axometric view");
  sPixmap     = "view_axo";
  iAccel      = Qt::Key_0;
}

void StdCmdViewAxo::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ViewAxo\")");
}

//===========================================================================
// Std_ViewFitAll
//===========================================================================
DEF_STD_CMD_A(StdCmdViewFitAll);

StdCmdViewFitAll::StdCmdViewFitAll()
  :Command("Std_ViewFitAll")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Fit all");
  sToolTipText  = QT_TR_NOOP("Fits the whole content on the screen");
  sWhatsThis    = QT_TR_NOOP("Fits the whole content on the screen");
  sStatusTip    = QT_TR_NOOP("Fits the whole content on the screen");
  sPixmap       = "view_fitall";
  iAccel        = 0;
}

void StdCmdViewFitAll::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ViewFit\")");
}

bool StdCmdViewFitAll::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("ViewFit");
}

//===========================================================================
// Std_ViewDockUndockFullscreen
//===========================================================================
DEF_STD_CMD_GROUP_A(StdViewDockUndockFullscreen);

StdViewDockUndockFullscreen::StdViewDockUndockFullscreen()
    : CommandGroup("Std_ViewDockUndockFullscreen", true, true)
{
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Display mode");
  sToolTipText= QT_TR_NOOP("Display the active view either in fullscreen, in undocked or docked mode");
  sWhatsThis  = QT_TR_NOOP("Display the active view either in fullscreen, in undocked or docked mode");
  sStatusTip  = QT_TR_NOOP("Display the active view either in fullscreen, in undocked or docked mode");
  
  _aCommands.push_back( new CommandItem(this, "Docked",    0,0,0,0,Qt::Key_D) );
  _aCommands.push_back( new CommandItem(this, "Undocked",  0,0,0,0,Qt::Key_U) );
  _aCommands.push_back( new CommandItem(this, "Fullscreen",0,0,0,0,Qt::Key_F) );
}

void StdViewDockUndockFullscreen::activated(int iMsg)
{
  MDIView* view = getMainWindow()->activeWindow();
  if ( !view ) return; // no active view
  if ( iMsg == (int)(view->currentViewMode()) ) 
    return; // nothing to do

  if (iMsg==0)
  {
    view->setCurrentViewMode( MDIView::Normal );
  }
  else if (iMsg==1)
  {
    view->setCurrentViewMode( MDIView::TopLevel );
  }
  else if (iMsg==2)
  {
    view->setCurrentViewMode( MDIView::FullScreen );
  }
}

bool StdViewDockUndockFullscreen::isActive(void)
{
  MDIView* view = getMainWindow()->activeWindow();
  if ( view )
  {
    // update the action group if needed
    ActionGroup* pActGrp = reinterpret_cast<ActionGroup*>(_pcAction->qt_cast("Gui::ActionGroup"));
    if ( pActGrp )
    {
      int index = pActGrp->currentActive();
      int mode = (int)(view->currentViewMode());
      if ( index != mode )
      {
        // active window has changed with another view mode
        pActGrp->setCurrentActive( mode );
      }
    }

    return true;
  }

  return false;
}

//===========================================================================
// Std_ViewScreenShot
//===========================================================================
DEF_STD_CMD_A(StdViewScreenShot);

StdViewScreenShot::StdViewScreenShot()
  :Command("Std_ViewScreenShot")
{
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Save picture...");
  sToolTipText= QT_TR_NOOP("Creates a screenshot of the active view");
  sWhatsThis  = QT_TR_NOOP("Creates a screenshot of the active view");
  sStatusTip  = QT_TR_NOOP("Creates a screenshot of the active view");
  iAccel      = 0;
}

void StdViewScreenShot::activated(int iMsg)
{
  View3DInventor* view = dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if ( view )
  {
    QStringList formats;
    SbViewportRegion vp(view->getViewer()->getViewportRegion());
    {
      SoFCOffscreenRenderer rd(vp);
      formats = rd.getWriteImageFiletypeInfo();
    }

    QString filter, selFilter;
    for( QStringList::Iterator it = formats.begin(); it != formats.end(); ++it )
    {
      filter += QString("%1 %2 (*.%3);;").arg( *it ).arg( QObject::tr("files") ).arg( (*it).lower() );
    }

    FileOptionsDialog fd( QString::null, filter, getMainWindow(), 0, true );
    fd.setMode( QFileDialog::AnyFile );
    fd.setCaption( QObject::tr("Save picture") );
    fd.setFilter( selFilter );

    // create the image options widget
    DlgSettingsImageImp* opt = new DlgSettingsImageImp(&fd);
    SbVec2s sz = vp.getWindowSize();
    opt->setImageSize((int)sz[0], (int)sz[1]);
    opt->setPixelsPerInch( vp.getPixelsPerInch() );
    opt->setImageFormat( SoFCOffscreenRenderer::RGB );
    opt->setMatrix(view->getViewer()->getCamera()->getViewVolume().getMatrix());

    fd.setOptionsWidget(FileOptionsDialog::Right, opt);
    opt->onSelectedFilter(fd.selectedFilter());
    QObject::connect(&fd, SIGNAL(filterSelected ( const QString & )), opt, SLOT(onSelectedFilter ( const QString & )));
    if ( fd.exec() == QDialog::Accepted )
    {
      selFilter = fd.selectedFilter();
      QString fn = fd.selectedFile();
      QApplication::setOverrideCursor( Qt::WaitCursor );

      // get the defined values
      int w = opt->imageWidth();
      int h = opt->imageHeight();
      float r = opt->pixelsPerInch();

      // search for the matching format
      bool ok = false;
      QString format = formats.front(); // take the first as default
      for ( QStringList::Iterator it = formats.begin(); it != formats.end(); ++it )
      {
        if ( selFilter.startsWith( *it ) )
        {
          format = *it;
          break;
        }
      }

      if ( !opt->textEditComment->text().isEmpty() && (format == "JPG" || format == "JPEG") )
      {
        std::string tempFileName = Base::FileInfo::getTempFileName();
        ok = view->getViewer()->makeScreenShot( tempFileName.c_str(), format.latin1(), w, h, r, (int)opt->imageFormat(), opt->imageBackgroundColor() );
        writeJPEGComment(tempFileName.c_str(),fn.latin1() ,opt->textEditComment->text());   
      }else{
        ok = view->getViewer()->makeScreenShot( fn.latin1(), format.latin1(), w, h, r, (int)opt->imageFormat(), opt->imageBackgroundColor() );
        // Reopen PNG images and embed information into
        if ( ok && format == "PNG" )
        {
          QFileInfo fi(fn);
          QImage img;
          img.load( fn, "PNG" );
          img.setText("Title", 0, fi.baseName() );
          img.setText("Author", 0, "FreeCAD (http://free-cad.sourceforge.net)");
          if ( opt->textEditComment->text().isEmpty() )
            img.setText("Description", 0, "Screenshot created by FreeCAD");
          else
            img.setText("Description", 0, opt->textEditComment->text() );
          img.setText("Creation Time", 0, QDateTime::currentDateTime().toString());
          img.setText("Software", 0, App::Application::Config()["ExeName"].c_str());
          img.save( fn, "PNG" );
        }
      }

      QApplication::restoreOverrideCursor();

      if ( !ok )
        QMessageBox::warning(getMainWindow(), QObject::tr("Save picture"), QObject::tr("Couldn't save the screenshot '%1'").arg(fn));
    }
  }
}

bool StdViewScreenShot::isActive(void)
{
  return (dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow()) != 0);
}


//===========================================================================
// Std_ViewCreate
//===========================================================================
DEF_STD_CMD_A(StdCmdViewCreate);

StdCmdViewCreate::StdCmdViewCreate()
  :Command("Std_ViewCreate")
{
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Create new  View");
  sToolTipText= QT_TR_NOOP("Creates a new  view window for the active document");
  sWhatsThis  = QT_TR_NOOP("Creates a new  view window for the active document");
  sStatusTip  = QT_TR_NOOP("Creates a new  view window for the active document");
  sPixmap     = "view_fitall";
  iAccel      = 0;
}

void StdCmdViewCreate::activated(int iMsg)
{
  getActiveGuiDocument()->createView("View3DIv");
}

bool StdCmdViewCreate::isActive(void)
{
  return (getActiveGuiDocument()!=NULL);
}

//===========================================================================
// Std_ViewExample1
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample1);

StdCmdViewExample1::StdCmdViewExample1()
  :Command("Std_ViewExample1")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Inventor example #1");
  sToolTipText  = QT_TR_NOOP("Shows a 3D texture with manipulator");
  sWhatsThis    = QT_TR_NOOP("Shows a 3D texture with manipulator");
  sStatusTip    = QT_TR_NOOP("Shows a 3D texture with manipulator");
  sPixmap       = "Std_Tool1";
  iAccel        = 0;
}

void StdCmdViewExample1::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"Example1\")");
}

bool StdCmdViewExample1::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Example1");
}
//===========================================================================
// Std_ViewExample2
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample2);

StdCmdViewExample2::StdCmdViewExample2()
  :Command("Std_ViewExample2")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Inventor example #2");
  sToolTipText  = QT_TR_NOOP("Shows spheres an dragglights");
  sWhatsThis    = QT_TR_NOOP("Shows spheres an dragglights");
  sStatusTip    = QT_TR_NOOP("Shows spheres an dragglights");
  sPixmap       = "Std_Tool2";
  iAccel        = 0;
}

void StdCmdViewExample2::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"Example2\")");
}

bool StdCmdViewExample2::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Example2");
}

//===========================================================================
// Std_ViewExample3
//===========================================================================
DEF_STD_CMD_A(StdCmdViewExample3);

StdCmdViewExample3::StdCmdViewExample3()
  :Command("Std_ViewExample3")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Inventor example #3");
  sToolTipText  = QT_TR_NOOP("Shows a animated texture");
  sWhatsThis    = QT_TR_NOOP("Shows a animated texture");
  sStatusTip    = QT_TR_NOOP("Shows a animated texture");
  sPixmap       = "Std_Tool3";
  iAccel        = 0;
}

void StdCmdViewExample3::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"Example3\")");
}

bool StdCmdViewExample3::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Example3");
}


//===========================================================================
// Std_ViewIvStereoOff
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoOff);

StdCmdViewIvStereoOff::StdCmdViewIvStereoOff()
  :Command("Std_ViewIvStereoOff")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo Off");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing off");
  sWhatsThis    = QT_TR_NOOP("Switch stereo viewing off");
  sStatusTip    = QT_TR_NOOP("Switch stereo viewing off");
  sPixmap       = "Std_Tool6";
  iAccel        = 0;
}

void StdCmdViewIvStereoOff::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"SetStereoOff\")");
}

bool StdCmdViewIvStereoOff::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("SetStereoOff");
}


//===========================================================================
// Std_ViewIvStereoRedGreen
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoRedGreen);

StdCmdViewIvStereoRedGreen::StdCmdViewIvStereoRedGreen()
  :Command("Std_ViewIvStereoRedGreen")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo red/green");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing to red/green");
  sWhatsThis    = QT_TR_NOOP("Switch stereo viewing to red/green");
  sStatusTip    = QT_TR_NOOP("Switch stereo viewing to red/green");
  sPixmap       = "Std_Tool7";
  iAccel        = 0;
}

void StdCmdViewIvStereoRedGreen::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"SetStereoRedGreen\")");
}

bool StdCmdViewIvStereoRedGreen::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("SetStereoRedGreen");
}

//===========================================================================
// Std_ViewIvStereoQuadBuff
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoQuadBuff);

StdCmdViewIvStereoQuadBuff::StdCmdViewIvStereoQuadBuff()
  :Command("Std_ViewIvStereoQuadBuff")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo quad buffer");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing to quad buffer");
  sWhatsThis    = QT_TR_NOOP("Switch stereo viewing to quad buffer");
  sStatusTip    = QT_TR_NOOP("Switch stereo viewing to quad buffer");
  sPixmap       = "Std_Tool7";
  iAccel        = 0;
}

void StdCmdViewIvStereoQuadBuff::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"SetStereoQuadBuff\")");
}

bool StdCmdViewIvStereoQuadBuff::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("SetStereoQuadBuff");
}

//===========================================================================
// Std_ViewIvStereoInterleavedRows
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoInterleavedRows);

StdCmdViewIvStereoInterleavedRows::StdCmdViewIvStereoInterleavedRows()
  :Command("Std_ViewIvStereoInterleavedRows")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo Interleaved Rows");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing to Interleaved Rows");
  sWhatsThis    = QT_TR_NOOP("Switch stereo viewing to Interleaved Rows");
  sStatusTip    = QT_TR_NOOP("Switch stereo viewing to Interleaved Rows");
  sPixmap       = "Std_Tool7";
  iAccel        = 0;
}

void StdCmdViewIvStereoInterleavedRows::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"SetStereoInterleavedRows\")");
}

bool StdCmdViewIvStereoInterleavedRows::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("SetStereoInterleavedRows");
}

//===========================================================================
// Std_ViewIvStereoInterleavedColumns
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvStereoInterleavedColumns);

StdCmdViewIvStereoInterleavedColumns::StdCmdViewIvStereoInterleavedColumns()
  :Command("Std_ViewIvStereoInterleavedColumns")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo Interleaved Columns");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing to Interleaved Columns");
  sWhatsThis    = QT_TR_NOOP("Switch stereo viewing to Interleaved Columns");
  sStatusTip    = QT_TR_NOOP("Switch stereo viewing to Interleaved Columns");
  sPixmap       = "Std_Tool7";
  iAccel        = 0;
}

void StdCmdViewIvStereoInterleavedColumns::activated(int iMsg)
{
  doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"SetStereoInterleavedColumns\")");
}

bool StdCmdViewIvStereoInterleavedColumns::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("SetStereoInterleavedColumns");
}


//===========================================================================
// Std_ViewIvIssueCamPos
//===========================================================================
DEF_STD_CMD_A(StdCmdViewIvIssueCamPos);

StdCmdViewIvIssueCamPos::StdCmdViewIvIssueCamPos()
  :Command("Std_ViewIvIssueCamPos")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Issue camera position");
  sToolTipText  = QT_TR_NOOP("Issue the camera posiotion to the console and to a macro, to easily recall this position");
  sWhatsThis    = QT_TR_NOOP("Issue the camera posiotion to the console and to a macro, to easily recall this position");
  sStatusTip    = QT_TR_NOOP("Issue the camera posiotion to the console and to a macro, to easily recall this position");
  sPixmap       = "Std_Tool8";
  iAccel        = 0;
}

void StdCmdViewIvIssueCamPos::activated(int iMsg)
{
  std::string Temp,Temp2;
  unsigned int pos;

  const char* ppReturn=0;
  getGuiApplication()->sendMsgToActiveView("GetCamera",&ppReturn);

  // remove the #inventor line...
  Temp2 = ppReturn;
  pos = Temp2.find_first_of("\n");
  Temp2.erase(0,pos);

  // remove all returns
  while((pos=Temp2.find('\n')) != std::string::npos)
    Temp2.replace(pos,1," ");

  // build up the command string
  Temp += "Gui.SendMsgToActiveView(\"SetCamera ";
  Temp += Temp2;
  Temp += "\")";

  Base::Console().Message("%s",Temp.c_str());

  getGuiApplication()->macroManager()->addLine(MacroManager::Gui,"FreeCAD.document().Recompute()");
  getGuiApplication()->macroManager()->addLine(MacroManager::Gui,Temp.c_str());
  //doCommand(Command::Gui,Temp.c_str());
}

bool StdCmdViewIvIssueCamPos::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("SetStereoOn");
}




//===========================================================================
// Instanciation
//===========================================================================


namespace Gui {

void CreateViewStdCommands(void)
{
  CommandManager &rcCmdMgr = Application::Instance->commandManager();

  // views
  rcCmdMgr.addCommand(new StdCmdViewBottom());
  rcCmdMgr.addCommand(new StdCmdViewFront());
  rcCmdMgr.addCommand(new StdCmdViewLeft());
  rcCmdMgr.addCommand(new StdCmdViewRear());
  rcCmdMgr.addCommand(new StdCmdViewRight());
  rcCmdMgr.addCommand(new StdCmdViewTop());
  rcCmdMgr.addCommand(new StdCmdViewAxo());
  rcCmdMgr.addCommand(new StdCmdViewFitAll());

  rcCmdMgr.addCommand(new StdCmdViewExample1());
  rcCmdMgr.addCommand(new StdCmdViewExample2());
  rcCmdMgr.addCommand(new StdCmdViewExample3());

  rcCmdMgr.addCommand(new StdCmdViewIvStereoQuadBuff());
  rcCmdMgr.addCommand(new StdCmdViewIvStereoRedGreen());
  rcCmdMgr.addCommand(new StdCmdViewIvStereoInterleavedColumns());
  rcCmdMgr.addCommand(new StdCmdViewIvStereoInterleavedRows());
  rcCmdMgr.addCommand(new StdCmdViewIvStereoOff());

  rcCmdMgr.addCommand(new StdCmdViewIvIssueCamPos());

  rcCmdMgr.addCommand(new StdCmdViewCreate());
  rcCmdMgr.addCommand(new StdViewScreenShot());

  rcCmdMgr.addCommand(new StdViewDockUndockFullscreen());
  rcCmdMgr.addCommand(new StdCmdSetMaterial());
  rcCmdMgr.addCommand(new StdCmdToggleVisibility());
  rcCmdMgr.addCommand(new StdCmdPerspectiveCamera());
  rcCmdMgr.addCommand(new StdCmdOrthographicCamera());
  rcCmdMgr.addCommand(new StdCameraType());
}

} // namespace Gui


//===========================================================================
// helper from wrjpgcom.c
//===========================================================================

/*
 * wrjpgcom.c
 *
 * Copyright (C) 1994, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains a very simple stand-alone application that inserts
 * user-supplied text as a COM (comment) marker in a JFIF file.
 * This may be useful as an example of the minimum logic needed to parse
 * JPEG markers.
 */

//#define JPEG_CJPEG_DJPEG	/* to get the command-line config symbols */
//#include "jinclude.h"		/* get auto-config symbols, <stdio.h> */

#ifndef HAVE_STDLIB_H		/* <stdlib.h> should declare malloc() */
extern void * malloc ();
#endif
#include <ctype.h>		/* to declare isupper(), tolower() */
#ifdef USE_SETMODE
#include <fcntl.h>		/* to declare setmode()'s parameter macros */
/* If you have setmode() but not <io.h>, just delete this line: */
#include <io.h>			/* to declare setmode() */
#endif

#ifdef USE_CCOMMAND		/* command-line reader for Macintosh */
#ifdef __MWERKS__
#include <SIOUX.h>              /* Metrowerks declares it here */
#endif
#ifdef THINK_C
#include <console.h>		/* Think declares it here */
#endif
#endif

#ifdef DONT_USE_B_MODE		/* define mode parameters for fopen() */
#define READ_BINARY	"r"
#define WRITE_BINARY	"w"
#else
#define READ_BINARY	"rb"
#define WRITE_BINARY	"wb"
#endif

#ifndef EXIT_FAILURE		/* define exit() codes if not provided */
#define EXIT_FAILURE  1
#endif
#ifndef EXIT_SUCCESS
#ifdef VMS
#define EXIT_SUCCESS  1		/* VMS is very nonstandard */
#else
#define EXIT_SUCCESS  0
#endif
#endif

/* Reduce this value if your malloc() can't allocate blocks up to 64K.
 * On DOS, compiling in large model is usually a better solution.
 */

#ifndef MAX_COM_LENGTH
#define MAX_COM_LENGTH 65000	/* must be < 65534 in any case */
#endif


/*
 * These macros are used to read the input file and write the output file.
 * To reuse this code in another application, you might need to change these.
 */

static FILE * infile;		/* input JPEG file */

/* Return next input byte, or EOF if no more */
#define NEXTBYTE()  getc(infile)

static FILE * outfile;		/* output JPEG file */

/* Emit an output byte */
#define PUTBYTE(x)  putc((x), outfile)


/* Error exit handler */
#define ERREXIT(msg)  (throw Base::Exception(msg))


/* Read one byte, testing for EOF */
static int
read_1_byte (void)
{
  int c;

  c = NEXTBYTE();
  if (c == EOF)
    ERREXIT("Premature EOF in JPEG file");
  return c;
}

/* Read 2 bytes, convert to unsigned int */
/* All 2-byte quantities in JPEG markers are MSB first */
static unsigned int
read_2_bytes (void)
{
  int c1, c2;

  c1 = NEXTBYTE();
  if (c1 == EOF)
    ERREXIT("Premature EOF in JPEG file");
  c2 = NEXTBYTE();
  if (c2 == EOF)
    ERREXIT("Premature EOF in JPEG file");
  return (((unsigned int) c1) << 8) + ((unsigned int) c2);
}


/* Routines to write data to output file */

static void
write_1_byte (int c)
{
  PUTBYTE(c);
}

static void
write_2_bytes (unsigned int val)
{
  PUTBYTE((val >> 8) & 0xFF);
  PUTBYTE(val & 0xFF);
}

static void
write_marker (int marker)
{
  PUTBYTE(0xFF);
  PUTBYTE(marker);
}

static void
copy_rest_of_file (void)
{
  int c;

  while ((c = NEXTBYTE()) != EOF)
    PUTBYTE(c);
}


/*
 * JPEG markers consist of one or more 0xFF bytes, followed by a marker
 * code byte (which is not an FF).  Here are the marker codes of interest
 * in this program.  (See jdmarker.c for a more complete list.)
 */

#define M_SOF0  0xC0		/* Start Of Frame N */
#define M_SOF1  0xC1		/* N indicates which compression process */
#define M_SOF2  0xC2		/* Only SOF0 and SOF1 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8		/* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9		/* End Of Image (end of datastream) */
#define M_SOS   0xDA		/* Start Of Scan (begins compressed data) */
#define M_COM   0xFE		/* COMment */


/*
 * Find the next JPEG marker and return its marker code.
 * We expect at least one FF byte, possibly more if the compressor used FFs
 * to pad the file.  (Padding FFs will NOT be replicated in the output file.)
 * There could also be non-FF garbage between markers.  The treatment of such
 * garbage is unspecified; we choose to skip over it but emit a warning msg.
 * NB: this routine must not be used after seeing SOS marker, since it will
 * not deal correctly with FF/00 sequences in the compressed image data...
 */

static int
next_marker (void)
{
  int c;
  int discarded_bytes = 0;

  /* Find 0xFF byte; count and skip any non-FFs. */
  c = read_1_byte();
  while (c != 0xFF) {
    discarded_bytes++;
    c = read_1_byte();
  }
  /* Get marker code byte, swallowing any duplicate FF bytes.  Extra FFs
   * are legal as pad bytes, so don't count them in discarded_bytes.
   */
  do {
    c = read_1_byte();
  } while (c == 0xFF);

  if (discarded_bytes != 0) {
    fprintf(stderr, "Warning: garbage data found in JPEG file\n");
  }

  return c;
}


/*
 * Read the initial marker, which should be SOI.
 * For a JFIF file, the first two bytes of the file should be literally
 * 0xFF M_SOI.  To be more general, we could use next_marker, but if the
 * input file weren't actually JPEG at all, next_marker might read the whole
 * file and then return a misleading error message...
 */

static int
first_marker (void)
{
  int c1, c2;

  c1 = NEXTBYTE();
  c2 = NEXTBYTE();
  if (c1 != 0xFF || c2 != M_SOI)
    ERREXIT("Not a JPEG file");
  return c2;
}


/*
 * Most types of marker are followed by a variable-length parameter segment.
 * This routine skips over the parameters for any marker we don't otherwise
 * want to process.
 * Note that we MUST skip the parameter segment explicitly in order not to
 * be fooled by 0xFF bytes that might appear within the parameter segment;
 * such bytes do NOT introduce new markers.
 */

static void
copy_variable (void)
/* Copy an unknown or uninteresting variable-length marker */
{
  unsigned int length;

  /* Get the marker parameter length count */
  length = read_2_bytes();
  write_2_bytes(length);
  /* Length includes itself, so must be at least 2 */
  if (length < 2)
    ERREXIT("Erroneous JPEG marker length");
  length -= 2;
  /* Skip over the remaining bytes */
  while (length > 0) {
    write_1_byte(read_1_byte());
    length--;
  }
}

static void
skip_variable (void)
/* Skip over an unknown or uninteresting variable-length marker */
{
  unsigned int length;

  /* Get the marker parameter length count */
  length = read_2_bytes();
  /* Length includes itself, so must be at least 2 */
  if (length < 2)
    ERREXIT("Erroneous JPEG marker length");
  length -= 2;
  /* Skip over the remaining bytes */
  while (length > 0) {
    (void) read_1_byte();
    length--;
  }
}


/*
 * Parse the marker stream until SOFn or EOI is seen;
 * copy data to output, but discard COM markers unless keep_COM is true.
 */

static int
scan_JPEG_header (int keep_COM)
{
  int marker;

  /* Expect SOI at start of file */
  if (first_marker() != M_SOI)
    ERREXIT("Expected SOI marker first");
  write_marker(M_SOI);

  /* Scan miscellaneous markers until we reach SOFn. */
  for (;;) {
    marker = next_marker();
    switch (marker) {
    case M_SOF0:		/* Baseline */
    case M_SOF1:		/* Extended sequential, Huffman */
    case M_SOF2:		/* Progressive, Huffman */
    case M_SOF3:		/* Lossless, Huffman */
    case M_SOF5:		/* Differential sequential, Huffman */
    case M_SOF6:		/* Differential progressive, Huffman */
    case M_SOF7:		/* Differential lossless, Huffman */
    case M_SOF9:		/* Extended sequential, arithmetic */
    case M_SOF10:		/* Progressive, arithmetic */
    case M_SOF11:		/* Lossless, arithmetic */
    case M_SOF13:		/* Differential sequential, arithmetic */
    case M_SOF14:		/* Differential progressive, arithmetic */
    case M_SOF15:		/* Differential lossless, arithmetic */
      return marker;

    case M_SOS:			/* should not see compressed data before SOF */
      ERREXIT("SOS without prior SOFn");
      break;

    case M_EOI:			/* in case it's a tables-only JPEG stream */
      return marker;

    case M_COM:			/* Existing COM: conditionally discard */
      if (keep_COM) {
	write_marker(marker);
	copy_variable();
      } else {
	skip_variable();
      }
      break;

    default:			/* Anything else just gets copied */
      write_marker(marker);
      copy_variable();		/* we assume it has a parameter count... */
      break;
    }
  } /* end loop */
}


#ifdef _MSC_VER
/* Command line parsing code */
static const char * progname;	/* program name for error messages */

static int
keymatch (char * arg, const char * keyword, int minchars)
/* Case-insensitive matching of (possibly abbreviated) keyword switches. */
/* keyword is the constant keyword (must be lower case already), */
/* minchars is length of minimum legal abbreviation. */
{
  register int ca, ck;
  register int nmatched = 0;

  while ((ca = *arg++) != '\0') {
    if ((ck = *keyword++) == '\0')
      return 0;			/* arg longer than keyword, no good */
    if (isupper(ca))		/* force arg to lcase (assume ck is already) */
      ca = tolower(ca);
    if (ca != ck)
      return 0;			/* no good */
    nmatched++;			/* count matched characters */
  }
  /* reached end of argument; fail if it's too short for unique abbrev */
  if (nmatched < minchars)
    return 0;
  return 1;			/* A-OK */
}
#endif

void writeJPEGComment(const char* InFile, const char* OutFile, const char* Comment)
{
  char * comment_arg;
  unsigned int comment_length;
  int marker;

  comment_arg = (char *) malloc((size_t) strlen(Comment)+2);
	strcpy(comment_arg, Comment);
  comment_length = strlen(comment_arg);


  if ((infile = fopen(InFile, READ_BINARY)) == NULL) {
    //fprintf(stderr, "%s: can't open %s\n", progname, argv[argn]);
    return;
  }

   
  if ((outfile = fopen(OutFile, WRITE_BINARY)) == NULL) {
    //fprintf(stderr, "%s: can't open %s\n", progname, argv[argn+1]);
    return;
  }

  /* Copy JPEG headers until SOFn marker;
  * we will insert the new comment marker just before SOFn.
  * This (a) causes the new comment to appear after, rather than before,
  * existing comments; and (b) ensures that comments come after any JFIF
  * or JFXX markers, as required by the JFIF specification.
  */
  marker = scan_JPEG_header(0);
  /* Insert the new COM marker, but only if nonempty text has been supplied */
  if (comment_length > 0) {
    write_marker(M_COM);
    write_2_bytes(comment_length + 2);
    while (comment_length > 0) {
      write_1_byte(*comment_arg++);
      comment_length--;
    }
  }
  /* Duplicate the remainder of the source file.
   * Note that any COM markers occuring after SOF will not be touched.
   */
  write_marker(marker);
  copy_rest_of_file();


  fclose(infile);
  fclose(outfile);

}



