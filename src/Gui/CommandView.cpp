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
# include <qmessagebox.h>
# include <qstringlist.h>
# include <qtextedit.h>
# include <Inventor/SbBox.h>
# include <Inventor/SbViewportRegion.h>
# include <Inventor/nodes/SoOrthographicCamera.h>
# include <Inventor/nodes/SoPerspectiveCamera.h>
# include <Inventor/events/SoMouseButtonEvent.h>
#endif


#include "Command.h"
#include "Action.h"
#include "Application.h"
#include "BitmapFactory.h"
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
#include "WaitCursor.h"

#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Reader.h>
#include <App/Document.h>
#include <App/Feature.h>
#define new DEBUG_CLIENTBLOCK
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().setCameraType(\"Orthographic\")");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().setCameraType(\"Perspective\")");
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
    doCommand(Command::Gui,"Gui.activeDocument().activeView().setCameraType(\"Orthographic\")");
  else if (iMsg == 1)
    doCommand(Command::Gui,"Gui.activeDocument().activeView().setCameraType(\"Perspective\")");
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
      // mark the appropriate menu item but do not invoke the command 
      if ( type != index ) {
        pActGrp->blockSignals(true);
        pActGrp->setCurrentActive( type );
        pActGrp->blockSignals(false);
      }
    }

    return true;
  }

  return false;
}

//===========================================================================
// Std_FreezeViews
//===========================================================================
DEF_STD_CMD_AC(StdCmdFreezeViews);

StdCmdFreezeViews::StdCmdFreezeViews()
  :Command("Std_FreezeViews")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Freeze display");
  sToolTipText  = QT_TR_NOOP("Freezes the current view position");
  sWhatsThis    = QT_TR_NOOP("Freezes the current view position");
  sStatusTip    = QT_TR_NOOP("Freezes the current view position");
  iAccel        = Qt::SHIFT+Qt::Key_F;
}

QAction * StdCmdFreezeViews::createAction(void)
{
  ListActionGroup* pcAction = new ListActionGroup( this, getMainWindow(),sName, false );
  pcAction->setUsesDropDown( true );
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setMenuText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIconSet(Gui::BitmapFactory().pixmap(sPixmap));

  pcAction->addActionData( QT_TR_NOOP("Save views") );
  pcAction->addActionData( QT_TR_NOOP("Restore views") );
  pcAction->addSeparator();
  pcAction->addActionData( QT_TR_NOOP("Freeze view") );
  QAction* action = pcAction->getAction(2);
  if (action)
    action->setAccel(iAccel);
  pcAction->addSeparator();

  return pcAction;
}

void StdCmdFreezeViews::activated(int iMsg)
{
  ListActionGroup* pcAction = dynamic_cast<ListActionGroup*>(_pcAction);
  
  if ( iMsg == 0 ) {
    // Save the views to an XML file
    QString dir = FileDialog::getWorkingDirectory();
    QString file = FileDialog::getSaveFileName(dir, "Frozen views (*.cam)", getMainWindow(),0,QT_TR_NOOP("Save frozen views"));
    std::ofstream str( file.latin1(), std::ios::out );
    if ( str && str.is_open() )
    {
      int ct = pcAction->countActions()-3;
      str << "<?xml version='1.0' encoding='utf-8'?>" << std::endl
          << "<FrozenViews SchemaVersion=\"1\">" << std::endl;
      str << "  <Views Count=\"" << ct <<"\">" << std::endl;
      for ( int i=0; i<ct; i++ )
      {
        QString data = pcAction->getData(i+3);

        // remove the first line because it's a comment like '#Inventor V2.1 ascii'
        QString viewPos="";
        if ( !data.isEmpty() ) {
          QStringList lines = QStringList::split("\n", data);
          if ( lines.size() > 1 ) {
            lines.pop_front();
            viewPos = lines.join(" ");
          }
        }

        str << "    <Camera settings=\"" << viewPos.latin1() << "\"/>" << std::endl;
      }
      str << "  </Views>" << std::endl;
      str << "</FrozenViews>" << std::endl;
    }
  } else if ( iMsg == 1 ) {
    // Restore the views from an XML file
    QString dir = FileDialog::getWorkingDirectory();
    QString file = FileDialog::getOpenFileName(dir, "Frozen views (*.cam)", getMainWindow(),0,QT_TR_NOOP("Save frozen views"));
    std::ifstream str( file.latin1(), std::ios::in | std::ios::binary );
    Base::XMLReader xmlReader(file.latin1(), str);
    xmlReader.readElement("FrozenViews");
    long scheme = xmlReader.getAttributeAsInteger("SchemaVersion");
    int ctViews = pcAction->countActions()-3+1;

    // SchemeVersion "1"
    if ( scheme == 1 ) {
      // read the views itself
      xmlReader.readElement("Views");
      int ct = xmlReader.getAttributeAsInteger("Count");
      for(int i=0; i<ct; i++)
      {
        xmlReader.readElement("Camera");
        const char* camera = xmlReader.getAttribute("settings");
        QString viewnr = QString(QT_TR_NOOP("Restore view &%1")).arg(ctViews+i);
        pcAction->addActionData( viewnr, camera );
        if ( ctViews+i < 10 ) {
          int accel = Qt::CTRL+Qt::Key_1;
          QAction* action = pcAction->getAction(ctViews+2+i);
          if (action)
            action->setAccel(accel+ctViews+i-1);
        }
      }

      xmlReader.readEndElement("Views");
    }
 
    xmlReader.readEndElement("FrozenViews");
  }
  else if ( iMsg == 3 ) {
    // Create a new view
    const char* ppReturn=0;
    getGuiApplication()->sendMsgToActiveView("GetCamera",&ppReturn);
    int ctViews = pcAction->countActions()-3+1;
    QString viewnr = QString(QT_TR_NOOP("Restore view &%1")).arg(ctViews);
    pcAction->addActionData( viewnr, ppReturn );
    if ( ctViews < 10 ) {
      int accel = Qt::CTRL+Qt::Key_1;
      QAction* action = pcAction->getAction(ctViews+2);
      if (action)
        action->setAccel(accel+ctViews-1);
    }
  } else {
    // Activate a view
    QString data = pcAction->getData(iMsg-2);
    QString send = QString("SetCamera %1").arg(data);
    getGuiApplication()->sendMsgToActiveView(send.latin1());
  }
}

bool StdCmdFreezeViews::isActive(void)
{
  return (dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow()));
}

//===========================================================================
// Std_ToggleClipPlane
//===========================================================================
DEF_STD_CMD_TOGGLE_A(StdCmdToggleClipPlane);

StdCmdToggleClipPlane::StdCmdToggleClipPlane()
  :ToggleCommand("Std_ToggleClipPlane")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Clipping plane");
  sToolTipText  = QT_TR_NOOP("Toggles clipping plane for active view");
  sWhatsThis    = QT_TR_NOOP("Toggles clipping plane for active view");
  sStatusTip    = QT_TR_NOOP("Toggles clipping plane for active view");
}

void StdCmdToggleClipPlane::activated(int iMsg)
{
  View3DInventor* view = dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if (view)
  {
    view->toggleClippingPlane();
  }
}

bool StdCmdToggleClipPlane::isActive(void)
{
  View3DInventor* view = dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if (view){
    if (_pcAction->isOn() != view->hasClippingPlane()){
      _pcAction->blockSignals(true);
      _pcAction->setOn(view->hasClippingPlane());
      _pcAction->blockSignals(false);
    }
    return true;
  } else {
    if (_pcAction->isOn()){
      _pcAction->blockSignals(true);
      _pcAction->setOn(false);
      _pcAction->blockSignals(false);
    }
    return false;
  }
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
    const std::vector<App::DocumentObject*> sel = Selection().getObjectsOfType(App::DocumentObject::getClassTypeId(), (*it)->getName());
    for(std::vector<App::DocumentObject*>::const_iterator ft=sel.begin();ft!=sel.end();ft++)
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
  sMenuText     = QT_TR_NOOP("Display...");
  sToolTipText  = QT_TR_NOOP("Sets the display properties of the selected object");
  sWhatsThis    = QT_TR_NOOP("Sets the display properties of the selected object");
  sStatusTip    = QT_TR_NOOP("Sets the display properties of the selected object");
  sPixmap       = "Std_Tool1";
  iAccel        = Qt::CTRL+Qt::Key_D;
}

void StdCmdSetMaterial::activated(int iMsg)
{
  Gui::Dialog::DlgDisplayPropertiesImp dlg(getMainWindow()->activeWindow(), "Display", true );
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().viewBottom()");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().viewFront()");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().viewLeft()");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().viewRear()");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().viewRight()");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().viewTop()");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().viewAxometric()");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().fitAll()");
}

bool StdCmdViewFitAll::isActive(void)
{
  return isViewOfType(Gui::View3DInventor::getClassTypeId());
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
        pActGrp->blockSignals(true);
        pActGrp->setCurrentActive( mode );
        pActGrp->blockSignals(false);
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
      SoFCOffscreenRenderer& rd = SoFCOffscreenRenderer::instance(vp);
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

    fd.setOptionsWidget(FileOptionsDialog::Right, opt);
    opt->onSelectedFilter(fd.selectedFilter());
    QObject::connect(&fd, SIGNAL(filterSelected ( const QString & )), opt, SLOT(onSelectedFilter ( const QString & )));
    if ( fd.exec() == QDialog::Accepted )
    {
      selFilter = fd.selectedFilter();
      QString fn = fd.selectedFile();
      Gui::WaitCursor wc;

      // get the defined values
      int w = opt->imageWidth();
      int h = opt->imageHeight();

      // search for the matching format
      QString format = formats.front(); // take the first as default
      for ( QStringList::Iterator it = formats.begin(); it != formats.end(); ++it )
      {
        if ( selFilter.startsWith( *it ) )
        {
          format = *it;
          break;
        }
      }

      // which background chosen
      const char* background;
      switch(opt->backgroundType()){
        case 0:  background="Current"; break;
        case 1:  background="White"; break;
        case 2:  background="Black"; break;
        case 3:  background="Transparent"; break;
        default: background="Current"; break;
      }

      QString comment = opt->comment();
      if ( !comment.isEmpty())
      {
        // Replace newline escape sequence trough '\\n' string to build one big string, otherwise Python would interpret it as an invalid command. 
        // Python does the decoding for us.
        QStringList lines = QStringList::split("\n", comment, true );
        QString text = lines.join("\\n");
        doCommand(Gui,"Gui.document().view().saveImage('%s',%d,%d,'%s','%s')",fn.latin1(),w,h,background,text.latin1());
      }else{
        doCommand(Gui,"Gui.document().view().saveImage('%s',%d,%d,'%s')",fn.latin1(),w,h,background);
      }
    }
  }
}

bool StdViewScreenShot::isActive(void)
{
  return isViewOfType(Gui::View3DInventor::getClassTypeId());
}


//===========================================================================
// Std_ViewCreate
//===========================================================================
DEF_STD_CMD_A(StdCmdViewCreate);

StdCmdViewCreate::StdCmdViewCreate()
  :Command("Std_ViewCreate")
{
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Create new view");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().setStereoType(\"None\")");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().setStereoType(\"Anaglyph\")");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().setStereoType(\"QuadBuffer\")");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().setStereoType(\"InterleavedRows\")");
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
  doCommand(Command::Gui,"Gui.activeDocument().activeView().setStereoType(\"InterleavedColumns\")");
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
  sToolTipText  = QT_TR_NOOP("Issue the camera position to the console and to a macro, to easily recall this position");
  sWhatsThis    = QT_TR_NOOP("Issue the camera position to the console and to a macro, to easily recall this position");
  sStatusTip    = QT_TR_NOOP("Issue the camera position to the console and to a macro, to easily recall this position");
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

  getGuiApplication()->macroManager()->addLine(MacroManager::Gui,"FreeCAD.document().recompute()");
  getGuiApplication()->macroManager()->addLine(MacroManager::Gui,Temp.c_str());
  //doCommand(Command::Gui,Temp.c_str());
}

bool StdCmdViewIvIssueCamPos::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("SetStereoOn");
}


//===========================================================================
// Std_ViewZoomIn
//===========================================================================
DEF_STD_CMD_A(StdViewZoomIn);

StdViewZoomIn::StdViewZoomIn()
  :Command("Std_ViewZoomIn")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Zoom In");
  sToolTipText  = QT_TR_NOOP("Zoom In");
  sWhatsThis    = QT_TR_NOOP("Zoom In");
  sStatusTip    = QT_TR_NOOP("Zoom In");
  iAccel        = Qt::Key_Plus;
}

void StdViewZoomIn::activated(int iMsg)
{
  View3DInventor* view = dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if ( view ) {
    View3DInventorViewer* viewer = view->getViewer();
#if 0
    SoCamera* cam = viewer->getCamera();
    // for orthographic cameras this does exactly the same as View3DInventorViewer::zoom()
    float val = float(exp(-0.05f));
    cam->scaleHeight(val);
#else // send an event to the GL widget to use the internal View3DInventorViewer::zoom() method
    // do only one step to zoom in 
//    QWheelEvent we(QPoint(0,0), -1/*QApplication::wheelScrollLines ()*/, Qt::NoButton );
//    QApplication::sendEvent(viewer->getGLWidget(), &we);
    SoMouseButtonEvent e;
    e.setButton(SoMouseButtonEvent::BUTTON5);
    e.setState(SoMouseButtonEvent::DOWN);
    viewer->sendSoEvent(&e);
#endif
  }
}

bool StdViewZoomIn::isActive(void)
{
  return (dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow()));
}

//===========================================================================
// Std_ViewZoomOut
//===========================================================================
DEF_STD_CMD_A(StdViewZoomOut);

StdViewZoomOut::StdViewZoomOut()
  :Command("Std_ViewZoomOut")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Zoom Out");
  sToolTipText  = QT_TR_NOOP("Zoom Out");
  sWhatsThis    = QT_TR_NOOP("Zoom Out");
  sStatusTip    = QT_TR_NOOP("Zoom Out");
  iAccel        = Qt::Key_Minus;
}

void StdViewZoomOut::activated(int iMsg)
{
  View3DInventor* view = dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if ( view ) {
    View3DInventorViewer* viewer = view->getViewer();
#if 0
    SoCamera* cam = viewer->getCamera();
    float val = float(exp(0.05f));
    cam->scaleHeight(val);
#else // send an event to the GL widget to use the internal View3DInventorViewer::zoom() method
    // do only one step to zoom out 
    SoMouseButtonEvent e;
    e.setButton(SoMouseButtonEvent::BUTTON4);
    e.setState(SoMouseButtonEvent::DOWN);
    viewer->sendSoEvent(&e);
#endif
  }
}

bool StdViewZoomOut::isActive(void)
{
  return (dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow()));
}

//===========================================================================
// Std_ViewBoxZoom
//===========================================================================
DEF_STD_CMD_A(StdViewBoxZoom);

StdViewBoxZoom::StdViewBoxZoom()
  :Command("Std_ViewBoxZoom")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Box zoom");
  sToolTipText  = QT_TR_NOOP("Box zoom");
  sWhatsThis    = QT_TR_NOOP("Box zoom");
  sStatusTip    = QT_TR_NOOP("Box zoom");
  iAccel        = Qt::CTRL+Qt::Key_B;
}

void StdViewBoxZoom::activated(int iMsg)
{
  View3DInventor* view = dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if ( view ) {
    View3DInventorViewer* viewer = view->getViewer();

    // This is just for test purposes. We create two poinmts that define a bounding box to which we want to zoom.
    // FIXME: We should start a rubber band to select the bounding box by the user
    SbVec2f p1(0.5f, 0.5f), p2(1.0f, 1.0f);
    SbBox2f box(p1, p2);
    viewer->boxZoom(box);
  }
}

bool StdViewBoxZoom::isActive(void)
{
  return false; // tmp. deactivated
  //return (dynamic_cast<View3DInventor*>(getMainWindow()->activeWindow()));
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
  rcCmdMgr.addCommand(new StdCmdToggleClipPlane());
  rcCmdMgr.addCommand(new StdCmdFreezeViews());
  rcCmdMgr.addCommand(new StdViewZoomIn());
  rcCmdMgr.addCommand(new StdViewZoomOut());
  rcCmdMgr.addCommand(new StdViewBoxZoom());
}

} // namespace Gui

