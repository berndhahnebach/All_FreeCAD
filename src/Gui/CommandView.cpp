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
# include <sstream>
#endif

#include "Command.h"
#include "Action.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "FileDialog.h"
#include "MainWindow.h"
#include "Tree.h"
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
#include "ViewProviderMeasureDistance.h"
#include "SceneInspector.h"

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Reader.h>
#include <App/Document.h>
#include <App/GeoFeature.h>
#include <App/MeasureDistance.h>
#include <App/Feature.h>

#include <QDomDocument>
#include <QDomElement>

using namespace Gui;
using Gui::Dialog::DlgSettingsImageImp;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

DEF_STD_CMD_AC(StdOrthographicCamera);

StdOrthographicCamera::StdOrthographicCamera()
  : Command("Std_OrthographicCamera")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Orthographic view");
  sToolTipText  = QT_TR_NOOP("Switches to orthographic view mode");
  sWhatsThis    = "Std_OrthographicCamera";
  sStatusTip    = QT_TR_NOOP("Switches to orthographic view mode");
  iAccel        = Qt::Key_O;
}

void StdOrthographicCamera::activated(int iMsg)
{
    if (iMsg == 1) {
        View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
        if (view->getViewer()->getCameraType() != SoOrthographicCamera::getClassTypeId())
            doCommand(Command::Gui,"Gui.activeDocument().activeView().setCameraType(\"Orthographic\")");
    }
}

bool StdOrthographicCamera::isActive(void)
{
  View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if ( view )
  {
    // update the action group if needed
    bool check = _pcAction->isChecked();
    bool mode = view->getViewer()->getCameraType() == SoOrthographicCamera::getClassTypeId();
    if (mode != check)
        _pcAction->setChecked(mode);

    return true;
  }

  return false;
}

Action * StdOrthographicCamera::createAction(void)
{
  Action *pcAction = Command::createAction();
  pcAction->setCheckable( true );
  return pcAction;
}

DEF_STD_CMD_AC(StdPerspectiveCamera);

StdPerspectiveCamera::StdPerspectiveCamera()
  : Command("Std_PerspectiveCamera")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Perspective view");
  sToolTipText  = QT_TR_NOOP("Switches to perspective view mode");
  sWhatsThis    = "Std_PerspectiveCamera";
  sStatusTip    = QT_TR_NOOP("Switches to perspective view mode");
  iAccel        = Qt::Key_P;
}

void StdPerspectiveCamera::activated(int iMsg)
{
    if (iMsg == 1) {
        View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
        if (view->getViewer()->getCameraType() != SoPerspectiveCamera::getClassTypeId())
            doCommand(Command::Gui,"Gui.activeDocument().activeView().setCameraType(\"Perspective\")");
    }
}

bool StdPerspectiveCamera::isActive(void)
{
  View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if ( view )
  {
    // update the action group if needed
    bool check = _pcAction->isChecked();
    bool mode = view->getViewer()->getCameraType() == SoPerspectiveCamera::getClassTypeId();
    if (mode != check)
        _pcAction->setChecked(mode);

    return true;
  }

  return false;
}

Action * StdPerspectiveCamera::createAction(void)
{
  Action *pcAction = Command::createAction();
  pcAction->setCheckable( true );
  return pcAction;
}

//===========================================================================
// Std_FreezeViews
//===========================================================================
class StdCmdFreezeViews : public Gui::Command
{
public:
    StdCmdFreezeViews();
    virtual ~StdCmdFreezeViews(){}

protected: 
    virtual void activated(int iMsg);
    virtual bool isActive(void);
    virtual Action * createAction(void);
    virtual void languageChange();

private:
    void onSaveViews();
    void onRestoreViews();

private:
    const int maxViews;
    int savedViews;
    int offset;
    QAction* saveView;
    QAction* freezeView;
    QAction* clearView;
    QAction* separator;
};

StdCmdFreezeViews::StdCmdFreezeViews()
  : Command("Std_FreezeViews"), maxViews(50), savedViews(0)
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Freeze display");
  sToolTipText  = QT_TR_NOOP("Freezes the current view position");
  sWhatsThis    = "Std_FreezeViews";
  sStatusTip    = QT_TR_NOOP("Freezes the current view position");
  iAccel        = Qt::SHIFT+Qt::Key_F;
}

Action * StdCmdFreezeViews::createAction(void)
{
  ActionGroup* pcAction = new ActionGroup(this, getMainWindow());
  pcAction->setDropDownMenu(true);
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  
  // add the action items
  saveView = pcAction->addAction(QObject::tr("Save views..."));
  pcAction->addAction(QObject::tr("Load views..."));
  pcAction->addAction(QString::fromAscii(""))->setSeparator(true);
  freezeView = pcAction->addAction(QObject::tr("Freeze view"));
  freezeView->setShortcut(iAccel);
  clearView = pcAction->addAction(QObject::tr("Clear views"));
  separator = pcAction->addAction(QString::fromAscii(""));
  separator->setSeparator(true);
  offset = pcAction->actions().count();

  // allow up to 50 views
  for (int i=0; i<maxViews; i++)
      pcAction->addAction(QString::fromAscii(""))->setVisible(false);

  return pcAction;
}

void StdCmdFreezeViews::activated(int iMsg)
{
  ActionGroup* pcAction = qobject_cast<ActionGroup*>(_pcAction);
  
  if ( iMsg == 0 ) {
      onSaveViews();
  } else if ( iMsg == 1 ) {
      onRestoreViews();
  } else if ( iMsg == 3 ) {
    // Create a new view
    const char* ppReturn=0;
    getGuiApplication()->sendMsgToActiveView("GetCamera",&ppReturn);

    QList<QAction*> acts = pcAction->actions();
    int index = 0;
    for (QList<QAction*>::ConstIterator it = acts.begin()+offset; it != acts.end(); ++it, index++) {
      if ( !(*it)->isVisible() ) {
        savedViews++;
        QString viewnr = QString(QObject::tr("Restore view &%1")).arg(index+1);
        (*it)->setText(viewnr);
        (*it)->setToolTip(QString::fromAscii(ppReturn));
        (*it)->setVisible(true);
        if ( index < 9 ) {
          int accel = Qt::CTRL+Qt::Key_1;
          (*it)->setShortcut(accel+index);
        }
        break;
      }
    }
  } else if ( iMsg == 4 ) {
    savedViews = 0;
    QList<QAction*> acts = pcAction->actions();   
    for (QList<QAction*>::ConstIterator it = acts.begin()+offset; it != acts.end(); ++it)
      (*it)->setVisible(false);
  } else if ( iMsg >= offset ) {
    // Activate a view
    QList<QAction*> acts = pcAction->actions();
    QString data = acts[iMsg]->toolTip();
    QString send = QString::fromAscii("SetCamera %1").arg(data);
    getGuiApplication()->sendMsgToActiveView(send.toAscii());
  }
}

void StdCmdFreezeViews::onSaveViews()
{
    // Save the views to an XML file
    QString fn = FileDialog::getSaveFileName(getMainWindow(), QObject::tr("Save frozen views"),
                                             QString(), QObject::tr("Frozen views (*.cam)"));
    if (fn.isEmpty())
        return;
    QFile file(fn);
    if (file.open(QFile::WriteOnly))
    {
        QTextStream str(&file);
        ActionGroup* pcAction = qobject_cast<ActionGroup*>(_pcAction);
        QList<QAction*> acts = pcAction->actions();      
        str << "<?xml version='1.0' encoding='utf-8'?>" << endl
            << "<FrozenViews SchemaVersion=\"1\">" << endl;
        str << "  <Views Count=\"" << savedViews <<"\">" << endl;

        for (QList<QAction*>::ConstIterator it = acts.begin()+offset; it != acts.end(); ++it) {
            if ( !(*it)->isVisible() )
                break;
            QString data = (*it)->toolTip();

            // remove the first line because it's a comment like '#Inventor V2.1 ascii'
            QString viewPos;
            if ( !data.isEmpty() ) {
                QStringList lines = data.split(QString::fromAscii("\n"));
                if ( lines.size() > 1 ) {
                    lines.pop_front();
                    viewPos = lines.join(QString::fromAscii(" "));
                }
            }

            str << "    <Camera settings=\"" << viewPos.toAscii().constData() << "\"/>" << endl;
        }

        str << "  </Views>" << endl;
        str << "</FrozenViews>" << endl;
    }
}

void StdCmdFreezeViews::onRestoreViews()
{
    // Should we clear the already saved views
    if ( savedViews > 0 ) {
        int ret = QMessageBox::question(getMainWindow(), QObject::tr("Restore views"), 
            QObject::tr("Importing the restored views would clear the already stored views.\n"
                        "Do you want to continue?"), QMessageBox::Yes|QMessageBox::Default,
                                                     QMessageBox::No|QMessageBox::Escape); 
        if (ret!=QMessageBox::Yes)
            return;
    }

    // Restore the views from an XML file
    QString fn = FileDialog::getOpenFileName(getMainWindow(), QObject::tr("Restore frozen views"),
                                             QString(), QObject::tr("Frozen views (*.cam)"));
    if (fn.isEmpty())
        return;
    QFile file(fn);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::critical(getMainWindow(), QObject::tr("Restore views"),
            QObject::tr("Cannot open file '%1'.").arg(fn));
        return;
    }

    QDomDocument xmlDocument;
    QString errorStr;
    int errorLine;
    int errorColumn;

    // evaluate the XML content
    if (!xmlDocument.setContent(&file, true, &errorStr, &errorLine, &errorColumn)) {
        std::cerr << "Parse error in XML content at line " << errorLine
                  << ", column " << errorColumn << ": "
                  << (const char*)errorStr.toAscii() << std::endl;
        return;
    }

    // get the root element
    QDomElement root = xmlDocument.documentElement();
    if (root.tagName() != QLatin1String("FrozenViews")) {
        std::cerr << "Unexpected XML structure" << std::endl;
        return;
    }

    bool ok;
    int scheme = root.attribute(QString::fromAscii("SchemaVersion")).toInt(&ok);
    if (!ok) return;
    // SchemeVersion "1"
    if (scheme == 1) {
        // read the views, ignore the attribute 'Count'
        QDomElement child = root.firstChildElement(QString::fromAscii("Views"));
        QDomElement views = child.firstChildElement(QString::fromAscii("Camera"));
        QStringList cameras;
        while (!views.isNull()) {
            QString setting = views.attribute(QString::fromAscii("settings"));
            cameras << setting;
            views = views.nextSiblingElement(QString::fromAscii("Camera"));
        }

        // use this rather than the attribute 'Count' because it could be
        // changed from outside
        int ct = cameras.count();
        ActionGroup* pcAction = qobject_cast<ActionGroup*>(_pcAction);
        QList<QAction*> acts = pcAction->actions();      

        int numRestoredViews = std::min<int>(ct, acts.size()-offset);
        savedViews = numRestoredViews;

        if (numRestoredViews > 0)
            separator->setVisible(true);
        for(int i=0; i<numRestoredViews; i++) {
            QString setting = cameras[i];
            QString viewnr = QString(QObject::tr("Restore view &%1")).arg(i+1);
            acts[i+offset]->setText(viewnr);
            acts[i+offset]->setToolTip(setting);
            acts[i+offset]->setVisible(true);
            if ( i < 9 ) {
                int accel = Qt::CTRL+Qt::Key_1;
                acts[i+offset]->setShortcut(accel+i);
            }
        }

        // if less views than actions
        for (int index = numRestoredViews+offset; index < acts.count(); index++)
            acts[index]->setVisible(false);
    }
}

bool StdCmdFreezeViews::isActive(void)
{
  View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if (view) {
    saveView->setEnabled(savedViews > 0);
    freezeView->setEnabled(savedViews < maxViews);
    clearView->setEnabled(savedViews > 0);
    separator->setVisible(savedViews > 0);
    return true;
  } else {
    separator->setVisible(savedViews > 0);
  }

  return false;
}

void StdCmdFreezeViews::languageChange()
{
  Command::languageChange();

  if (!_pcAction)
    return;
  ActionGroup* pcAction = qobject_cast<ActionGroup*>(_pcAction);
  QList<QAction*> acts = pcAction->actions();
  acts[0]->setText(QObject::tr("Save views..."));
  acts[1]->setText(QObject::tr("Load views..."));
  acts[3]->setText(QObject::tr("Freeze view"));
  acts[4]->setText(QObject::tr("Clear views"));
  int index=1;
  for (QList<QAction*>::ConstIterator it = acts.begin()+5; it != acts.end(); ++it, index++) {
    if ( (*it)->isVisible() ) {
      QString viewnr = QString(QObject::tr("Restore view &%1")).arg(index);
      (*it)->setText(viewnr);
    }
  }
}

//===========================================================================
// Std_ToggleClipPlane
//===========================================================================

DEF_STD_CMD_AC(StdCmdToggleClipPlane);

StdCmdToggleClipPlane::StdCmdToggleClipPlane()
  : Command("Std_ToggleClipPlane")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Clipping plane");
  sToolTipText  = QT_TR_NOOP("Toggles clipping plane for active view");
  sWhatsThis    = "Std_ToggleClipPlane";
  sStatusTip    = QT_TR_NOOP("Toggles clipping plane for active view");
}

Action * StdCmdToggleClipPlane::createAction(void)
{
  Action *pcAction = (Action*)Command::createAction();
  pcAction->setCheckable( true );
  return pcAction;
}

void StdCmdToggleClipPlane::activated(int iMsg)
{
  View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if (view) {
    if (iMsg > 0 && !view->hasClippingPlane())
      view->toggleClippingPlane();
    else if (iMsg == 0 && view->hasClippingPlane())
      view->toggleClippingPlane();
  }
}

bool StdCmdToggleClipPlane::isActive(void)
{
  View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if (view){
    Action* action = qobject_cast<Action*>(_pcAction);
    if (action->isChecked() != view->hasClippingPlane())
      action->setChecked(view->hasClippingPlane());
    return true;
  } else {
    Action* action = qobject_cast<Action*>(_pcAction);
    if (action->isChecked())
      action->setChecked(false);
    return false;
  }
}

//===========================================================================
// Std_ToggleVisibility
//===========================================================================
DEF_STD_CMD_A(StdCmdToggleVisibility);

StdCmdToggleVisibility::StdCmdToggleVisibility()
  : Command("Std_ToggleVisibility")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Toggle visibility");
  sToolTipText  = QT_TR_NOOP("Toggles visibility");
  sWhatsThis    = "Std_ToggleVisibility";
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
      if ( pcDoc && pcDoc->isShow((*ft)->getNameInDocument()) )
        doCommand(Gui,"Gui.getDocument(\"%s\").getObject(\"%s\").Visibility=False", (*it)->getName(), (*ft)->getNameInDocument());
      else
        doCommand(Gui,"Gui.getDocument(\"%s\").getObject(\"%s\").Visibility=True", (*it)->getName(), (*ft)->getNameInDocument());
    }
  }
}

bool StdCmdToggleVisibility::isActive(void)
{
  return ( Gui::Selection().size() != 0 );
}

//===========================================================================
// Std_SetAppearance
//===========================================================================
DEF_STD_CMD_A(StdCmdSetAppearance);

StdCmdSetAppearance::StdCmdSetAppearance()
  : Command("Std_SetAppearance")
{
    sGroup        = QT_TR_NOOP("Standard-View");
    sMenuText     = QT_TR_NOOP("Appearance...");
    sToolTipText  = QT_TR_NOOP("Sets the display properties of the selected object");
    sWhatsThis    = "Std_SetAppearance";
    sStatusTip    = QT_TR_NOOP("Sets the display properties of the selected object");
    sPixmap       = "Std_Tool1";
    iAccel        = Qt::CTRL+Qt::Key_D;
}

void StdCmdSetAppearance::activated(int iMsg)
{
    static QPointer<QDialog> dlg = 0;
    if (!dlg)
        dlg = new Gui::Dialog::DlgDisplayPropertiesImp(getMainWindow());
    dlg->setModal(false);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

bool StdCmdSetAppearance::isActive(void)
{
    return Gui::Selection().size() != 0;
}

//===========================================================================
// Std_ViewBottom
//===========================================================================
DEF_3DV_CMD(StdCmdViewBottom)

StdCmdViewBottom::StdCmdViewBottom()
  : Command("Std_ViewBottom")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Bottom");
  sToolTipText  = QT_TR_NOOP("Set to bottom view");
  sWhatsThis    = "Std_ViewXX";
  sStatusTip    = QT_TR_NOOP("Set to bottom view");
  sPixmap       = "view-bottom";
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
  : Command("Std_ViewFront")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Front");
  sToolTipText  = QT_TR_NOOP("Set to front view");
  sWhatsThis    = "Std_ViewXX";
  sStatusTip    = QT_TR_NOOP("Set to front view");
  sPixmap       = "view-front";
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
  : Command("Std_ViewLeft")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Left");
  sToolTipText  = QT_TR_NOOP("Set to left view");
  sWhatsThis    = "Std_ViewXX";
  sStatusTip    = QT_TR_NOOP("Set to left view");
  sPixmap       = "view-left";
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
  : Command("Std_ViewRear")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Rear");
  sToolTipText  = QT_TR_NOOP("Set to rear view");
  sWhatsThis    = "Std_ViewXX";
  sStatusTip    = QT_TR_NOOP("Set to rear view");
  sPixmap       = "view-rear";
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
  : Command("Std_ViewRight")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Right");
  sToolTipText  = QT_TR_NOOP("Set to right view");
  sWhatsThis    = "Std_ViewXX";
  sStatusTip    = QT_TR_NOOP("Set to right view");
  sPixmap       = "view-right";
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
  : Command("Std_ViewTop")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Top");
  sToolTipText  = QT_TR_NOOP("Set to top view");
  sWhatsThis    = "Std_ViewXX";
  sStatusTip    = QT_TR_NOOP("Set to top view");
  sPixmap       = "view-top";
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
  : Command("Std_ViewAxo")
{
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Axometric");
  sToolTipText= QT_TR_NOOP("Set to axometric view");
  sWhatsThis  = "Std_ViewXX";
  sStatusTip  = QT_TR_NOOP("Set to axometric view");
  sPixmap     = "view-axometric";
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
  : Command("Std_ViewFitAll")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Fit all");
  sToolTipText  = QT_TR_NOOP("Fits the whole content on the screen");
  sWhatsThis    = "Std_ViewFitAll";
  sStatusTip    = QT_TR_NOOP("Fits the whole content on the screen");
  sPixmap       = "view-zoom-all";
  iAccel        = 0;
}

void StdCmdViewFitAll::activated(int iMsg)
{
  //doCommand(Command::Gui,"Gui.activeDocument().activeView().fitAll()");
   doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ViewFit\")");
}

bool StdCmdViewFitAll::isActive(void)
{
  //return isViewOfType(Gui::View3DInventor::getClassTypeId());
  return getGuiApplication()->sendHasMsgToActiveView("ViewFit");
}

//===========================================================================
// Std_ViewFitSelection
//===========================================================================
DEF_STD_CMD_A(StdCmdViewFitSelection);

StdCmdViewFitSelection::StdCmdViewFitSelection()
  : Command("Std_ViewFitSelection")
{
    sGroup        = QT_TR_NOOP("Standard-View");
    sMenuText     = QT_TR_NOOP("Fit selection");
    sToolTipText  = QT_TR_NOOP("Fits the selected content on the screen");
    sWhatsThis    = "Std_ViewFitSelection";
    sStatusTip    = QT_TR_NOOP("Fits the selected content on the screen");
#if QT_VERSION >= 0x040200
    sPixmap       = "view-zoom-selection";
#endif
    iAccel        = 0;
}

void StdCmdViewFitSelection::activated(int iMsg)
{
    //doCommand(Command::Gui,"Gui.activeDocument().activeView().fitAll()");
    doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"ViewSelection\")");
}

bool StdCmdViewFitSelection::isActive(void)
{
  //return isViewOfType(Gui::View3DInventor::getClassTypeId());
  return getGuiApplication()->sendHasMsgToActiveView("ViewSelection");
}

//===========================================================================
// Std_ViewDockUndockFullscreen
//===========================================================================
DEF_STD_CMD_AC(StdViewDockUndockFullscreen);

StdViewDockUndockFullscreen::StdViewDockUndockFullscreen()
  : Command("Std_ViewDockUndockFullscreen")
{
  sGroup      = QT_TR_NOOP("Standard-View");
  sMenuText   = QT_TR_NOOP("Display mode");
  sToolTipText= QT_TR_NOOP("Display the active view either in fullscreen, in undocked or docked mode");
  sWhatsThis  = "Std_ViewDockUndockFullscreen";
  sStatusTip  = QT_TR_NOOP("Display the active view either in fullscreen, in undocked or docked mode");
}

Action * StdViewDockUndockFullscreen::createAction(void)
{
  ActionGroup* pcAction = new ActionGroup(this, getMainWindow());
  pcAction->setDropDownMenu(true);
  pcAction->setText(QObject::tr(sMenuText));

  QAction* docked = pcAction->addAction(QObject::tr(QT_TR_NOOP("Docked")));
  docked->setToolTip(QObject::tr(sToolTipText));
  docked->setStatusTip(QObject::tr(sStatusTip));
  docked->setWhatsThis(QObject::tr(sWhatsThis));
  docked->setShortcut(Qt::Key_D);
  docked->setCheckable(true);

  QAction* undocked = pcAction->addAction(QObject::tr(QT_TR_NOOP("Undocked")));
  undocked->setToolTip(QObject::tr(sToolTipText));
  undocked->setStatusTip(QObject::tr(sStatusTip));
  undocked->setWhatsThis(QObject::tr(sWhatsThis));
  undocked->setShortcut(Qt::Key_U);
  undocked->setCheckable(true);

  QAction* fullscr = pcAction->addAction(QObject::tr(QT_TR_NOOP("Fullscreen")));
  fullscr->setToolTip(QObject::tr(sToolTipText));
  fullscr->setStatusTip(QObject::tr(sStatusTip));
  fullscr->setWhatsThis(QObject::tr(sWhatsThis));
  fullscr->setShortcut(Qt::Key_F11);
  fullscr->setCheckable(true);
  fullscr->setIcon(Gui::BitmapFactory().pixmap("view-fullscreen"));

  return pcAction;
}

void StdViewDockUndockFullscreen::activated(int iMsg)
{
  MDIView* view = getMainWindow()->activeWindow();
  if ( !view ) return; // no active view
  if ( iMsg == (int)(view->currentViewMode()) ) 
    return; // nothing to do

  if (iMsg==0)
  {
    view->setCurrentViewMode( MDIView::Child );
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
    ActionGroup* pActGrp = qobject_cast<ActionGroup*>(_pcAction);
    if ( pActGrp )
    {
      int index = pActGrp->checkedAction();
      int mode = (int)(view->currentViewMode());
      if ( index != mode )
      {
        // active window has changed with another view mode
        pActGrp->setCheckedAction( mode );
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
  : Command("Std_ViewScreenShot")
{
    sGroup      = QT_TR_NOOP("Standard-View");
    sMenuText   = QT_TR_NOOP("Save picture...");
    sToolTipText= QT_TR_NOOP("Creates a screenshot of the active view");
    sWhatsThis  = "Std_ViewScreenShot";
    sStatusTip  = QT_TR_NOOP("Creates a screenshot of the active view");
    iAccel      = 0;
    sPixmap     = "camera-photo";
}

void StdViewScreenShot::activated(int iMsg)
{
  View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
  if ( view )
  {
    QStringList formats;
    SbViewportRegion vp(view->getViewer()->getViewportRegion());
    {
      SoFCOffscreenRenderer rd(vp);
      formats = rd.getWriteImageFiletypeInfo();
    }

    QStringList filter;
    QString selFilter;
    for (QStringList::Iterator it = formats.begin(); it != formats.end(); ++it)
    {
        filter << QString::fromAscii("%1 %2 (*.%3)").arg((*it).toUpper()).
          arg(QObject::tr("files")).arg((*it).toLower());
    }

    FileOptionsDialog fd(getMainWindow(), 0);
    fd.setFileMode(QFileDialog::AnyFile);
    fd.setAcceptMode(QFileDialog::AcceptSave);
    fd.setWindowTitle(QObject::tr("Save picture"));
    fd.setFilters(filter);

    // create the image options widget
    DlgSettingsImageImp* opt = new DlgSettingsImageImp(&fd);
    SbVec2s sz = vp.getWindowSize();
    opt->setImageSize((int)sz[0], (int)sz[1]);

    fd.setOptionsWidget(FileOptionsDialog::ExtensionRight, opt);
    fd.setConfirmOverwrite(true);
    opt->onSelectedFilter(fd.selectedFilter());
    QObject::connect(&fd, SIGNAL(filterSelected(const QString&)),
                     opt, SLOT(onSelectedFilter(const QString&)));

    if (fd.exec() == QDialog::Accepted) {
      selFilter = fd.selectedFilter();
      QString fn = fd.selectedFiles().front();
      // We must convert '\' path separators to '/' before otherwise
      // Python would interpret them as escape sequences.
      fn.replace(QLatin1Char('\\'), QLatin1Char('/'));

      Gui::WaitCursor wc;

      // get the defined values
      int w = opt->imageWidth();
      int h = opt->imageHeight();

      // search for the matching format
      QString format = formats.front(); // take the first as default
      for (QStringList::Iterator it = formats.begin(); it != formats.end(); ++it)
      {
        if (selFilter.startsWith((*it).toUpper()))
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
        QStringList lines = comment.split(QLatin1String("\n"), QString::KeepEmptyParts );
        comment = lines.join(QLatin1String("\\n"));
        doCommand(Gui,"Gui.activeDocument().activeView().saveImage('%s',%d,%d,'%s','%s')",
            fn.toUtf8().constData(),w,h,background,comment.toUtf8().constData());
      }else{
        doCommand(Gui,"Gui.activeDocument().activeView().saveImage('%s',%d,%d,'%s')",
            fn.toUtf8().constData(),w,h,background);
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
  : Command("Std_ViewCreate")
{
    sGroup      = QT_TR_NOOP("Standard-View");
    sMenuText   = QT_TR_NOOP("Create new view");
    sToolTipText= QT_TR_NOOP("Creates a new  view window for the active document");
    sWhatsThis  = "Std_ViewCreate";
    sStatusTip  = QT_TR_NOOP("Creates a new  view window for the active document");
    sPixmap     = "window-new";
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
  : Command("Std_ViewExample1")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Inventor example #1");
  sToolTipText  = QT_TR_NOOP("Shows a 3D texture with manipulator");
  sWhatsThis    = "Std_ViewExamples";
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
  : Command("Std_ViewExample2")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Inventor example #2");
  sToolTipText  = QT_TR_NOOP("Shows spheres an dragglights");
  sWhatsThis    = "Std_ViewExamples";
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
  : Command("Std_ViewExample3")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Inventor example #3");
  sToolTipText  = QT_TR_NOOP("Shows a animated texture");
  sWhatsThis    = "Std_ViewExamples";
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
  : Command("Std_ViewIvStereoOff")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo Off");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing off");
  sWhatsThis    = "Std_ViewIvStereo";
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
  : Command("Std_ViewIvStereoRedGreen")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo red/green");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing to red/green");
  sWhatsThis    = "Std_ViewIvStereo";
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
  : Command("Std_ViewIvStereoQuadBuff")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo quad buffer");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing to quad buffer");
  sWhatsThis    = "Std_ViewIvStereo";
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
  : Command("Std_ViewIvStereoInterleavedRows")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo Interleaved Rows");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing to Interleaved Rows");
  sWhatsThis    = "Std_ViewIvStereo";
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
  : Command("Std_ViewIvStereoInterleavedColumns")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Stereo Interleaved Columns");
  sToolTipText  = QT_TR_NOOP("Switch stereo viewing to Interleaved Columns");
  sWhatsThis    = "Std_ViewIvStereo";
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
  : Command("Std_ViewIvIssueCamPos")
{
  sGroup        = QT_TR_NOOP("Standard-View");
  sMenuText     = QT_TR_NOOP("Issue camera position");
  sToolTipText  = QT_TR_NOOP("Issue the camera position to the console and to a macro, to easily recall this position");
  sWhatsThis    = "Std_ViewIvIssueCamPos";
  sStatusTip    = QT_TR_NOOP("Issue the camera position to the console and to a macro, to easily recall this position");
  sPixmap       = "Std_Tool8";
  iAccel        = 0;
}

void StdCmdViewIvIssueCamPos::activated(int iMsg)
{
  std::string Temp,Temp2;
  std::string::size_type pos;

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

  Base::Console().Message("%s\n",Temp2.c_str());
  getGuiApplication()->macroManager()->addLine(MacroManager::Gui,Temp.c_str());
}

bool StdCmdViewIvIssueCamPos::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("GetCamera");
}


//===========================================================================
// Std_ViewZoomIn
//===========================================================================
DEF_STD_CMD_A(StdViewZoomIn);

StdViewZoomIn::StdViewZoomIn()
  : Command("Std_ViewZoomIn")
{
    sGroup        = QT_TR_NOOP("Standard-View");
    sMenuText     = QT_TR_NOOP("Zoom In");
    sToolTipText  = QT_TR_NOOP("Zoom In");
    sWhatsThis    = "Std_ViewZoom";
    sStatusTip    = QT_TR_NOOP("Zoom In");
#if QT_VERSION >= 0x040200
    sPixmap       = "view-zoom-in";
#endif
    iAccel        = Qt::Key_Plus;
}

void StdViewZoomIn::activated(int iMsg)
{
    View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
    if ( view ) {
        View3DInventorViewer* viewer = view->getViewer();

        // send an event to the GL widget to use the internal View3DInventorViewer::zoom() method
        // do only one step to zoom in 
        SoMouseButtonEvent e;
        e.setButton(SoMouseButtonEvent::BUTTON5);
        e.setState(SoMouseButtonEvent::DOWN);
        viewer->sendSoEvent(&e);
    }
}

bool StdViewZoomIn::isActive(void)
{
    return (qobject_cast<View3DInventor*>(getMainWindow()->activeWindow()));
}

//===========================================================================
// Std_ViewZoomOut
//===========================================================================
DEF_STD_CMD_A(StdViewZoomOut);

StdViewZoomOut::StdViewZoomOut()
  : Command("Std_ViewZoomOut")
{
    sGroup        = QT_TR_NOOP("Standard-View");
    sMenuText     = QT_TR_NOOP("Zoom Out");
    sToolTipText  = QT_TR_NOOP("Zoom Out");
    sWhatsThis    = "Std_ViewZoom";
    sStatusTip    = QT_TR_NOOP("Zoom Out");
#if QT_VERSION >= 0x040200
    sPixmap       = "view-zoom-out";
#endif
    iAccel        = Qt::Key_Minus;
}

void StdViewZoomOut::activated(int iMsg)
{
    View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
    if ( view ) {
        View3DInventorViewer* viewer = view->getViewer();
        // send an event to the GL widget to use the internal View3DInventorViewer::zoom() method
        // do only one step to zoom out 
        SoMouseButtonEvent e;
        e.setButton(SoMouseButtonEvent::BUTTON4);
        e.setState(SoMouseButtonEvent::DOWN);
        viewer->sendSoEvent(&e);
    }
}

bool StdViewZoomOut::isActive(void)
{
    return (qobject_cast<View3DInventor*>(getMainWindow()->activeWindow()));
}

//===========================================================================
// Std_ViewBoxZoom
//===========================================================================
DEF_3DV_CMD(StdViewBoxZoom);

StdViewBoxZoom::StdViewBoxZoom()
  : Command("Std_ViewBoxZoom")
{
    sGroup        = QT_TR_NOOP("Standard-View");
    sMenuText     = QT_TR_NOOP("Box zoom");
    sToolTipText  = QT_TR_NOOP("Box zoom");
    sWhatsThis    = "Std_ViewBoxZoom";
    sStatusTip    = QT_TR_NOOP("Box zoom");
#if QT_VERSION >= 0x040200
    sPixmap       = "view-zoom-border";
#endif
    iAccel        = Qt::CTRL+Qt::Key_B;
}

void StdViewBoxZoom::activated(int iMsg)
{
    View3DInventor* view = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
    if ( view ) {
        View3DInventorViewer* viewer = view->getViewer();
        if (!viewer->isPicking())
            viewer->startPicking(View3DInventorViewer::BoxZoom);
    }
}

//===========================================================================
// Std_TreeSelection
//===========================================================================

DEF_STD_CMD(StdCmdTreeSelection);

StdCmdTreeSelection::StdCmdTreeSelection()
  : Command("Std_TreeSelection")
{
    sGroup        = QT_TR_NOOP("View");
    sMenuText     = QT_TR_NOOP("Go to selection");
    sToolTipText  = QT_TR_NOOP("Scroll to first selected item");
    sWhatsThis    = "Std_TreeSelection";
    sStatusTip    = QT_TR_NOOP("Scroll to first selected item");
    iAccel        = 0;
}

void StdCmdTreeSelection::activated(int iMsg)
{
    TreeDockWidget* tree = Gui::getMainWindow()->findChild<TreeDockWidget*>();
    if (tree) {
        Gui::Document* doc = Gui::Application::Instance->activeDocument();
        tree->scrollItemToTop(doc);
    }
}

//===========================================================================
// Std_MeasureDistance
//===========================================================================

DEF_STD_CMD_A(StdCmdMeasureDistance);

StdCmdMeasureDistance::StdCmdMeasureDistance()
  : Command("Std_MeasureDistance")
{
    sGroup        = QT_TR_NOOP("View");
    sMenuText     = QT_TR_NOOP("Measure distance");
    sToolTipText  = QT_TR_NOOP("Measure distance");
    sWhatsThis    = "Std_MeasureDistance";
    sStatusTip    = QT_TR_NOOP("Measure distance");
    iAccel        = 0;
}

void StdCmdMeasureDistance::activated(int iMsg)
{
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    Gui::View3DInventor* view = static_cast<Gui::View3DInventor*>(doc->getActiveView());
    if (view) {
        App::DocumentObject* obj = doc->getDocument()->addObject(App::MeasureDistance::getClassTypeId().getName(),"Distance");
        Gui::View3DInventorViewer* viewer = view->getViewer();
        viewer->addEventCallback(SoMouseButtonEvent::getClassTypeId(), ViewProviderMeasureDistance::measureDistanceCallback, obj);
     }
}

bool StdCmdMeasureDistance::isActive(void)
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (doc && doc->countObjectsOfType(App::GeoFeature::getClassTypeId()) > 0)
        return true;
    return false;
}

//===========================================================================
// Std_SceneInspector
//===========================================================================

DEF_3DV_CMD(StdCmdSceneInspector);

StdCmdSceneInspector::StdCmdSceneInspector()
  : Command("Std_SceneInspector")
{
    // seting the
    sGroup        = QT_TR_NOOP("Tools");
    sMenuText     = QT_TR_NOOP("Scene inspector...");
    sToolTipText  = QT_TR_NOOP("Scene inspector");
    sWhatsThis    = "Std_SceneInspector";
    sStatusTip    = QT_TR_NOOP("Scene inspector");
}

void StdCmdSceneInspector::activated(int iMsg)
{
    View3DInventor* child = qobject_cast<View3DInventor*>(getMainWindow()->activeWindow());
    if (child) {
        View3DInventorViewer* viewer = child->getViewer();
        Gui::Dialog::DlgInspector dlg(getMainWindow());
        dlg.setNode(viewer->getSceneGraph());
        dlg.exec();
    }
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
    rcCmdMgr.addCommand(new StdCmdViewFitSelection());

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
    rcCmdMgr.addCommand(new StdCmdSetAppearance());
    rcCmdMgr.addCommand(new StdCmdToggleVisibility());
    rcCmdMgr.addCommand(new StdOrthographicCamera());
    rcCmdMgr.addCommand(new StdPerspectiveCamera());
    rcCmdMgr.addCommand(new StdCmdToggleClipPlane());
    rcCmdMgr.addCommand(new StdCmdFreezeViews());
    rcCmdMgr.addCommand(new StdViewZoomIn());
    rcCmdMgr.addCommand(new StdViewZoomOut());
    rcCmdMgr.addCommand(new StdViewBoxZoom());
    rcCmdMgr.addCommand(new StdCmdTreeSelection());
    rcCmdMgr.addCommand(new StdCmdMeasureDistance());
    rcCmdMgr.addCommand(new StdCmdSceneInspector());
}

} // namespace Gui

