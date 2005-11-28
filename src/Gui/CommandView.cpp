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
# include <qdesktopwidget.h>
# include <qimage.h>
# include <qmessagebox.h>
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
#include <App/Document.h>
#include <App/Feature.h>

using namespace Gui;
using Gui::Dialog::DlgSettingsImageImp;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//===========================================================================
// Std_OrthographicCamera
//===========================================================================
DEF_STD_CMD_A(StdCmdOrthographicCamera);

StdCmdOrthographicCamera::StdCmdOrthographicCamera()
  :CppCommand("Std_OrthographicCamera")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"OrthographicCamera\")");
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
  :CppCommand("Std_PerspectiveCamera")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"PerspectiveCamera\")");
}

bool StdCmdPerspectiveCamera::isActive(void)
{
  return true;
}
//===========================================================================
// Std_ToggleVisibility
//===========================================================================
DEF_STD_CMD_A(StdCmdToggleVisibility);

StdCmdToggleVisibility::StdCmdToggleVisibility()
  :CppCommand("Std_ToggleVisibility")
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
  if( getActiveGuiDocument() )
  {
    std::vector<App::Feature*> sel = Gui::Selection().getSelectedFeatures();
    for(std::vector<App::Feature*>::const_iterator It=sel.begin();It!=sel.end();It++)
    {
      if(getActiveGuiDocument()->isShow(*It))
        doCommand(Gui,"Gui.hide(\"%s\")", (*It)->getName());
//        getActiveGuiDocument()->setHide(*It);
      else
        doCommand(Gui,"Gui.show(\"%s\")", (*It)->getName());
//        getActiveGuiDocument()->setShow(*It);

    }

    //Gui::Selection().clearSelection();
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
  :CppCommand("Std_SetMaterial")
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
  :CppCommand("Std_ViewBottom")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewBottom\")");
}

//===========================================================================
// Std_ViewFront
//===========================================================================
DEF_3DV_CMD(StdCmdViewFront);

StdCmdViewFront::StdCmdViewFront()
  :CppCommand("Std_ViewFront")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFront\")");
}

//===========================================================================
// Std_ViewLeft
//===========================================================================
DEF_3DV_CMD(StdCmdViewLeft);

StdCmdViewLeft::StdCmdViewLeft()
  :CppCommand("Std_ViewLeft")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewLeft\")");
}

//===========================================================================
// Std_ViewRear
//===========================================================================
DEF_3DV_CMD(StdCmdViewRear);

StdCmdViewRear::StdCmdViewRear()
  :CppCommand("Std_ViewRear")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewRear\")");
}

//===========================================================================
// Std_ViewRight
//===========================================================================
DEF_3DV_CMD(StdCmdViewRight);

StdCmdViewRight::StdCmdViewRight()
  :CppCommand("Std_ViewRight")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewRight\")");
}

//===========================================================================
// Std_ViewTop
//===========================================================================
DEF_3DV_CMD(StdCmdViewTop);

StdCmdViewTop::StdCmdViewTop()
  :CppCommand("Std_ViewTop")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewTop\")");
}

//===========================================================================
// Std_ViewAxo
//===========================================================================
DEF_3DV_CMD(StdCmdViewAxo);

StdCmdViewAxo::StdCmdViewAxo()
  :CppCommand("Std_ViewAxo")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewAxo\")");
}

//===========================================================================
// Std_ViewFitAll
//===========================================================================
DEF_STD_CMD_A(StdCmdViewFitAll);

StdCmdViewFitAll::StdCmdViewFitAll()
  :CppCommand("Std_ViewFitAll")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"ViewFit\")");
}

bool StdCmdViewFitAll::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("ViewFit");
}


//===========================================================================
// Std_ViewFullScreen
//===========================================================================
DEF_STD_CMD_A(StdViewFullScreen);

StdViewFullScreen::StdViewFullScreen()
  :CppCommand("Std_ViewFullScreen")
{
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Fullscreen");
  sToolTipText= QT_TR_NOOP("Display the active view in fullscreen");
  sWhatsThis  = QT_TR_NOOP("Display the active view in fullscreen");
  sStatusTip  = QT_TR_NOOP("Display the active view in fullscreen");
//  sPixmap     = "view_fitall";
  iAccel      = Qt::Key_F;
}

void StdViewFullScreen::activated(int iMsg)
{
  MDIView* view = getMainWindow()->activeWindow();
  if ( !view ) return; // no active view
  view->setCurrentViewMode( MDIView::FullScreen );
}

bool StdViewFullScreen::isActive(void)
{
  return (getMainWindow()->activeWindow() != 0);
}
//===========================================================================
// Std_ViewTopLevel
//===========================================================================
DEF_STD_CMD_A(StdViewTopLevel);

StdViewTopLevel::StdViewTopLevel()
  :CppCommand("Std_ViewTopLevel")
{
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Undock view");
  sToolTipText= QT_TR_NOOP("Display the active view in toplevel mode");
  sWhatsThis  = QT_TR_NOOP("Display the active view in toplevel mode");
  sStatusTip  = QT_TR_NOOP("Display the active view in toplevel mode");
}

void StdViewTopLevel::activated(int iMsg)
{
  MDIView* view = getMainWindow()->activeWindow();
  if ( !view ) return; // no active view

  view->setCurrentViewMode( MDIView::TopLevel );
}

bool StdViewTopLevel::isActive(void)
{
  return (getMainWindow()->activeWindow() != 0);
}
//===========================================================================
// Std_ViewDock
//===========================================================================
DEF_STD_CMD_A(StdViewDock);

StdViewDock::StdViewDock()
  :CppCommand("Std_ViewDock")
{
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Dock view");
  sToolTipText= QT_TR_NOOP("Display the active view in fullscreen");
  sWhatsThis  = QT_TR_NOOP("Display the active view in fullscreen");
  sStatusTip  = QT_TR_NOOP("Display the active view in fullscreen");
}

void StdViewDock::activated(int iMsg)
{
  MDIView* view = getMainWindow()->activeWindow();
  if ( !view ) return; // no active view

  view->setCurrentViewMode( MDIView::Normal );
}

bool StdViewDock::isActive(void)
{
  return (getMainWindow()->activeWindow() != 0);
}

//===========================================================================
// Std_ViewDockUndockFullscreen
//===========================================================================
// Test class for command groups
class StdViewDockUndockFullscreen : public CommandGroup
{
public:
  StdViewDockUndockFullscreen()
    : CommandGroup("Std_ViewDockUndockFullscreen", true)
  {
    sGroup      = QT_TR_NOOP("Standard-View");
    sMenuText   = QT_TR_NOOP("Display mode");
    sToolTipText= QT_TR_NOOP("Display the active view either in fullscreen, in undocked or docked mode");
    sWhatsThis  = QT_TR_NOOP("Display the active view either in fullscreen, in undocked or docked mode");
    sStatusTip  = QT_TR_NOOP("Display the active view either in fullscreen, in undocked or docked mode");
    
    _aCommands.push_back( new CommandBase("Docked",0,0,0,0,Qt::Key_D));
    _aCommands.push_back( new CommandBase("Undocked",0,0,0,0,Qt::Key_U));
    _aCommands.push_back( new CommandBase("Fullscreen",0,0,0,0,Qt::Key_F));
  }

  void activated(int iMsg)
  {
    MDIView* view = getMainWindow()->activeWindow();
    if ( !view ) return; // no active view

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

  bool isActive(void)
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
        if ( index != -1 && index != mode )
        {
          // active window has changed with another view mode
          pActGrp->setCurrentActive( mode );
        }
      }

      return true;
    }

    return false;
  }
};

//===========================================================================
// Std_ViewScreenShot
//===========================================================================
DEF_STD_CMD_A(StdViewScreenShot);

StdViewScreenShot::StdViewScreenShot()
  :CppCommand("Std_ViewScreenShot")
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
    SbViewportRegion vp(view->getViewer()->getViewportRegion());
    SoFCOffscreenRenderer rd(vp);
    QStringList formats = rd.getWriteImageFiletypeInfo();

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

    fd.setOptionsWidget(FileOptionsDialog::Right, opt);
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

      ok = view->getViewer()->makeScreenShot( fn.latin1(), format.latin1(), w, h, r, (int)opt->imageFormat(), opt->imageBackgroundColor() );
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
  :CppCommand("Std_ViewCreate")
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
  :CppCommand("Std_ViewExample1")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Example1\")");
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
  :CppCommand("Std_ViewExample2")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Example2\")");
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
  :CppCommand("Std_ViewExample3")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"Example3\")");
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
  :CppCommand("Std_ViewIvStereoOff")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoOff\")");
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
  :CppCommand("Std_ViewIvStereoRedGreen")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoRedGreen\")");
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
  :CppCommand("Std_ViewIvStereoQuadBuff")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoQuadBuff\")");
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
  :CppCommand("Std_ViewIvStereoInterleavedRows")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoInterleavedRows\")");
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
  :CppCommand("Std_ViewIvStereoInterleavedColumns")
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
  doCommand(Command::Gui,"FreeCADGui.SendMsgToActiveView(\"SetStereoInterleavedColumns\")");
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
  :CppCommand("Std_ViewIvIssueCamPos")
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
  Temp += "FreeCADGui.SendMsgToActiveView(\"SetCamera ";
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

  rcCmdMgr.addCommand(new StdViewFullScreen());
  rcCmdMgr.addCommand(new StdViewTopLevel());
  rcCmdMgr.addCommand(new StdViewDock());
  rcCmdMgr.addCommand(new StdViewDockUndockFullscreen());
  rcCmdMgr.addCommand(new StdCmdSetMaterial());
  rcCmdMgr.addCommand(new StdCmdToggleVisibility());
  rcCmdMgr.addCommand(new StdCmdPerspectiveCamera());
  rcCmdMgr.addCommand(new StdCmdOrthographicCamera());
}

} // namespace Gui


