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
#include <algorithm>

#include <Base/Exception.h>
#include <Base/Interpreter.h>
#include <Base/Sequencer.h>
#include <App/Document.h>
#include <App/DocumentObjectGroup.h>
#include <App/Feature.h>

#include "Action.h"
#include "Application.h"
#include "Document.h"
#include "Command.h"
#include "FileDialog.h"
#include "MainWindow.h"
#include "BitmapFactory.h"
#include "Selection.h"
#include "DlgProjectInformationImp.h"

using Base::Sequencer;
using namespace Gui;

//#define TANGO_ICONS

//===========================================================================
// Std_Open
//===========================================================================

DEF_STD_CMD(StdCmdOpen);

StdCmdOpen::StdCmdOpen()
  : Command("Std_Open")
{
    // seting the
    sGroup        = QT_TR_NOOP("File");
    sMenuText     = QT_TR_NOOP("&Open...");
    sToolTipText  = QT_TR_NOOP("Open a document or import files");
    sWhatsThis    = "Std_Open";
    sStatusTip    = QT_TR_NOOP("Open a document or import files");
#ifndef TANGO_ICONS
    sPixmap       = "Open";
#else
    sPixmap       = "document-open";
#endif
    iAccel        = Qt::CTRL+Qt::Key_O;
}

void StdCmdOpen::activated(int iMsg)
{
    // fill the list of registered endings
    QString formatList;
    const char* supported = QT_TR_NOOP("Supported formats");
    const char* allFiles = QT_TR_NOOP("All files (*.*)");
    formatList = QObject::tr(supported);
    formatList += QLatin1String(" (");

    std::map<std::string,std::string> EndingMap = App::GetApplication().getImportTypes();
    std::map<std::string,std::string>::const_iterator It;
    for (It=EndingMap.begin();It != EndingMap.end();++It) {
        formatList += QLatin1String(" *.");
        formatList += QLatin1String(It->first.c_str());
    }

    formatList += QLatin1String(");;");

    std::vector<std::string> FilterList = App::GetApplication().getImportFilters();
    std::vector<std::string>::const_iterator Jt;
    for (Jt=FilterList.begin();Jt != FilterList.end();++Jt) {
        formatList += QLatin1String((*Jt).c_str());
        formatList += QLatin1String(";;");
    }
    formatList += QObject::tr(allFiles);

    QStringList FileList = FileDialog::getOpenFileNames(getMainWindow(),
        QObject::tr("Open document"), QString(), formatList);
    for (QStringList::Iterator it = FileList.begin(); it != FileList.end(); ++it) {
        getGuiApplication()->open((*it).toUtf8());
    }
}

//===========================================================================
// Std_Import
//===========================================================================

DEF_STD_CMD_A(StdCmdImport);

StdCmdImport::StdCmdImport()
  : Command("Std_Import")
{
    // seting the
    sGroup        = QT_TR_NOOP("File");
    sMenuText     = QT_TR_NOOP("&Import...");
    sToolTipText  = QT_TR_NOOP("Import a file in the active document");
    sWhatsThis    = "Std_Import";
    sStatusTip    = QT_TR_NOOP("Import a file in the active document");
    //sPixmap       = "Open";
    iAccel        = Qt::CTRL+Qt::Key_I;
}

void StdCmdImport::activated(int iMsg)
{
    // fill the list of registered endings
    QString formatList;
    const char* supported = QT_TR_NOOP("Supported formats");
    const char* allFiles = QT_TR_NOOP("All files (*.*)");
    formatList = QObject::tr(supported);
    formatList += QLatin1String(" (");

    std::map<std::string,std::string> EndingMap = App::GetApplication().getImportTypes();
    EndingMap.erase("FCStd"); // ignore the project file format
    std::map<std::string,std::string>::const_iterator It;
    for (It=EndingMap.begin();It != EndingMap.end();++It) {
        formatList += QLatin1String(" *.");
        formatList += QLatin1String(It->first.c_str());
    }

    formatList += QLatin1String(");;");

    std::vector<std::string> FilterList = App::GetApplication().getImportFilters();
    std::vector<std::string>::const_iterator Jt;
    for (Jt=FilterList.begin();Jt != FilterList.end();++Jt) {
        // ignore the project file format
        if (Jt->find("(*.FCStd)") == std::string::npos) {
            formatList += QLatin1String(Jt->c_str());
            formatList += QLatin1String(";;");
        }
    }
    formatList += QObject::tr(allFiles);

    QStringList FileList = FileDialog::getOpenFileNames(getMainWindow(),
        QObject::tr("Import file"), QString(), formatList);
    for (QStringList::Iterator it = FileList.begin(); it != FileList.end(); ++it) {
        getGuiApplication()->importFrom((*it).toUtf8(),getActiveGuiDocument()->getDocument()->getName());
    }
}

bool StdCmdImport::isActive(void)
{
  return ( getActiveGuiDocument() ? true : false );
}


//===========================================================================
// Std_Export
//===========================================================================

DEF_STD_CMD_A(StdCmdExport);

StdCmdExport::StdCmdExport()
  : Command("Std_Export")
{
    // seting the
    sGroup        = QT_TR_NOOP("File");
    sMenuText     = QT_TR_NOOP("&Export...");
    sToolTipText  = QT_TR_NOOP("Export an object in the active document");
    sWhatsThis    = "Std_Export";
    sStatusTip    = QT_TR_NOOP("Export an object in the active document");
    //sPixmap       = "Open";
    iAccel        = Qt::CTRL+Qt::Key_E;
}

void StdCmdExport::activated(int iMsg)
{
    // fill the list of registered endings
    QString formatList;
    const char* supported = QT_TR_NOOP("Supported formats");
    formatList = QObject::tr(supported);
    formatList += QLatin1String(" (");

    std::map<std::string,std::string> EndingMap = App::GetApplication().getExportTypes();
    EndingMap.erase("FCStd"); // ignore the project file format
    std::map<std::string,std::string>::const_iterator It;
    for (It=EndingMap.begin();It != EndingMap.end();++It) {
        formatList += QLatin1String(" *.");
        formatList += QLatin1String(It->first.c_str());
    }

    formatList += QLatin1String(");;");

    std::vector<std::string> FilterList = App::GetApplication().getExportFilters();
    std::vector<std::string>::const_iterator Jt;
    for (Jt=FilterList.begin();Jt != FilterList.end();++Jt) {
        // ignore the project file format
        if (Jt->find("(*.FCStd)") == std::string::npos) {
            formatList += QLatin1String(Jt->c_str());
            formatList += QLatin1String(";;");
        }
    }

    QString fileName = FileDialog::getSaveFileName(getMainWindow(),
        QObject::tr("Export file"), QString(), formatList);
    if (!fileName.isEmpty())
        getGuiApplication()->exportTo(fileName.toUtf8(),getActiveGuiDocument()
        ->getDocument()->getName());
}

bool StdCmdExport::isActive(void)
{
    return (getActiveGuiDocument() ? true : false);
}

//===========================================================================
// Std_New
//===========================================================================

DEF_STD_CMD(StdCmdNew);

StdCmdNew::StdCmdNew()
  :Command("Std_New")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("&New");
  sToolTipText  = QT_TR_NOOP("Create a new empty Document");
  sWhatsThis    = "Std_New";
  sStatusTip    = QT_TR_NOOP("Create a new empty Document");
#ifndef TANGO_ICONS
  sPixmap       = "New";
#else
  sPixmap       = "document-new";
#endif
  iAccel        = Qt::CTRL+Qt::Key_N;
}

void StdCmdNew::activated(int iMsg)
{
  doCommand(Command::Doc,"App.newDocument()");
}

//===========================================================================
// Std_Save
//===========================================================================
DEF_STD_CMD_A(StdCmdSave);

StdCmdSave::StdCmdSave()
  :Command("Std_Save")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("&Save");
  sToolTipText  = QT_TR_NOOP("Save the active document");
  sWhatsThis    = "Std_Save";
  sStatusTip    = QT_TR_NOOP("Save the active document");
#ifndef TANGO_ICONS
  sPixmap       = "Save";
#else
  sPixmap       = "document-save";
#endif
  iAccel        = Qt::CTRL+Qt::Key_S;
}

void StdCmdSave::activated(int iMsg)
{
#if 0
  Gui::Document* pActiveDoc = getActiveGuiDocument();
  if ( pActiveDoc )
    pActiveDoc->save();
  else
#endif
    doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"Save\")");
}

bool StdCmdSave::isActive(void)
{
#if 0
  if( getActiveGuiDocument() )
    return true;
  else
#endif
    return getGuiApplication()->sendHasMsgToActiveView("Save");
}

//===========================================================================
// Std_SaveAs
//===========================================================================
DEF_STD_CMD_A(StdCmdSaveAs);

StdCmdSaveAs::StdCmdSaveAs()
  :Command("Std_SaveAs")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("Save &As...");
  sToolTipText  = QT_TR_NOOP("Save the active document under a new file name");
  sWhatsThis    = "Std_SaveAs";
  sStatusTip    = QT_TR_NOOP("Save the active document under a new file name");
#ifdef TANGO_ICONS
  sPixmap       = "document-save-as";
#endif
  iAccel        = 0;
}

void StdCmdSaveAs::activated(int iMsg)
{
#if 0
  Gui::Document* pActiveDoc = getActiveGuiDocument();
  if ( pActiveDoc )
    pActiveDoc->saveAs();
  else
#endif
    doCommand(Command::Gui,"Gui.SendMsgToActiveView(\"SaveAs\")");
}

bool StdCmdSaveAs::isActive(void)
{
#if 0
  if( getActiveGuiDocument() )
    return true;
  else
#endif
    return getGuiApplication()->sendHasMsgToActiveView("SaveAs");
}

//===========================================================================
// Std_ProjectInfo
//===========================================================================

DEF_STD_CMD_A(StdCmdProjectInfo);

StdCmdProjectInfo::StdCmdProjectInfo()
  :Command("Std_ProjectInfo")
{
  // seting the 
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("Project i&nformation...");
  sToolTipText  = QT_TR_NOOP("Show details of the currently active project");
  sWhatsThis    = "Std_ProjectInfo";
  sStatusTip    = QT_TR_NOOP("Show details of the currently active project");
#ifdef TANGO_ICONS
  sPixmap       = "document-properties";
#endif
}

void StdCmdProjectInfo::activated(int iMsg)
{
  Gui::Dialog::DlgProjectInformationImp dlg(getActiveGuiDocument()->getDocument(), getMainWindow());
  dlg.exec();
}

bool StdCmdProjectInfo::isActive(void)
{
  return ( getActiveGuiDocument() ? true : false );
}

//===========================================================================
// Std_Print
//===========================================================================
DEF_STD_CMD_A(StdCmdPrint );

StdCmdPrint::StdCmdPrint()
  :Command("Std_Print")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("&Print...");
  sToolTipText  = QT_TR_NOOP("Print the document");
  sWhatsThis    = "Std_Print";
  sStatusTip    = QT_TR_NOOP("Print the document");
#ifndef TANGO_ICONS
  sPixmap       = "Print";
#else
  sPixmap       = "document-print";
#endif
  iAccel        = Qt::CTRL+Qt::Key_P;;
}

void StdCmdPrint::activated(int iMsg)
{
  if ( getMainWindow()->activeWindow() )
  {
      getMainWindow()->statusBar()->showMessage(QObject::tr("Printing..."));
    getMainWindow()->activeWindow()->print();
  }
}

bool StdCmdPrint::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Print");
}

//===========================================================================
// Std_PrintPdf
//===========================================================================
DEF_STD_CMD_A(StdCmdPrintPdf);

StdCmdPrintPdf::StdCmdPrintPdf()
  :Command("Std_PrintPdf")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("&Export PDF...");
  sToolTipText  = QT_TR_NOOP("Export the document as PDF");
  sWhatsThis    = "Std_PrintPdf";
  sStatusTip    = QT_TR_NOOP("Export the document as PDF");
}

void StdCmdPrintPdf::activated(int iMsg)
{
  if ( getMainWindow()->activeWindow() )
  {
    getMainWindow()->statusBar()->showMessage(QLatin1String("Exporting PDF..."));
    getMainWindow()->activeWindow()->printPdf();
  }
}

bool StdCmdPrintPdf::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("PrintPdf");
}

//===========================================================================
// Std_Quit
//===========================================================================

DEF_STD_CMD(StdCmdQuit );

StdCmdQuit::StdCmdQuit()
  :Command("Std_Quit")
{
  sGroup        = QT_TR_NOOP("File");
  sMenuText     = QT_TR_NOOP("E&xit");
  sToolTipText  = QT_TR_NOOP("Quits the application");
  sWhatsThis    = "Std_Quit";
  sStatusTip    = QT_TR_NOOP("Quits the application");
#ifdef TANGO_ICONS
  sPixmap       = "system-log-out";
#endif
  iAccel        = Qt::ALT+Qt::Key_F4;
}

void StdCmdQuit::activated(int iMsg)
{
  // close the main window and exit the event loop
  if (getMainWindow()->close())
    qApp->quit();
}

//===========================================================================
// Std_Undo
//===========================================================================

DEF_STD_CMD_AC(StdCmdUndo);

StdCmdUndo::StdCmdUndo()
  :Command("Std_Undo")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("&Undo");
  sToolTipText  = QT_TR_NOOP("Undo exactly one action");
  sWhatsThis    = "Std_Undo";
  sStatusTip    = QT_TR_NOOP("Undo exactly one action");
#ifndef TANGO_ICONS
  sPixmap       = "Undo";
#else
  sPixmap       = "edit-undo";
#endif
  iAccel        = Qt::CTRL+Qt::Key_Z;
}

void StdCmdUndo::activated(int iMsg)
{
//  Application::Instance->slotUndo();
  getGuiApplication()->sendMsgToActiveView("Undo");
}

bool StdCmdUndo::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Undo");
}

Action * StdCmdUndo::createAction(void)
{
  Action *pcAction;

  pcAction = new UndoAction(this,getMainWindow());
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIcon(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setShortcut(iAccel);

  return pcAction;
}

//===========================================================================
// Std_Redo
//===========================================================================

DEF_STD_CMD_AC(StdCmdRedo );

StdCmdRedo::StdCmdRedo()
  :Command("Std_Redo")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("&Redo");
  sToolTipText  = QT_TR_NOOP("Redoes a previously undone action");
  sWhatsThis    = "Std_Redo";
  sStatusTip    = QT_TR_NOOP("Redoes a previously undone action");
#ifndef TANGO_ICONS
  sPixmap       = "Redo";
#else
  sPixmap       = "edit-redo";
#endif
  iAccel        = Qt::CTRL+Qt::Key_Y;
}

void StdCmdRedo::activated(int iMsg)
{
//  Application::Instance->slotRedo();
  getGuiApplication()->sendMsgToActiveView("Redo");
}

bool StdCmdRedo::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Redo");
}

Action * StdCmdRedo::createAction(void)
{
  Action *pcAction;

  pcAction = new RedoAction(this,getMainWindow());
  pcAction->setText(QObject::tr(sMenuText));
  pcAction->setToolTip(QObject::tr(sToolTipText));
  pcAction->setStatusTip(QObject::tr(sStatusTip));
  pcAction->setWhatsThis(QObject::tr(sWhatsThis));
  if(sPixmap)
    pcAction->setIcon(Gui::BitmapFactory().pixmap(sPixmap));
  pcAction->setShortcut(iAccel);

  return pcAction;
}

//===========================================================================
// Std_Cut
//===========================================================================
DEF_STD_CMD_A(StdCmdCut);

StdCmdCut::StdCmdCut()
  :Command("Std_Cut")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("&Cut");
  sToolTipText  = QT_TR_NOOP("Cut out");
  sWhatsThis    = "Std_Cut";
  sStatusTip    = QT_TR_NOOP("Cut out");
#ifndef TANGO_ICONS
  sPixmap       = "Cut";
#else
  sPixmap       = "edit-cut";
#endif
  iAccel        = Qt::CTRL+Qt::Key_X;
}

void StdCmdCut::activated(int iMsg)
{
  getGuiApplication()->sendMsgToActiveView("Cut");
}

bool StdCmdCut::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Cut");
}

//===========================================================================
// Std_Copy
//===========================================================================
DEF_STD_CMD_A(StdCmdCopy);

StdCmdCopy::StdCmdCopy()
  :Command("Std_Copy")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("C&opy");
  sToolTipText  = QT_TR_NOOP("Copy operation");
  sWhatsThis    = "Std_Copy";
  sStatusTip    = QT_TR_NOOP("Copy operation");
#ifndef TANGO_ICONS
  sPixmap       = "Copy";
#else
  sPixmap       = "edit-copy";
#endif
  iAccel        = Qt::CTRL+Qt::Key_C;
}

void StdCmdCopy::activated(int iMsg)
{
  getGuiApplication()->sendMsgToActiveView("Copy");
}

bool StdCmdCopy::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Copy");
}

//===========================================================================
// Std_Paste
//===========================================================================
DEF_STD_CMD_A(StdCmdPaste);

StdCmdPaste::StdCmdPaste()
  :Command("Std_Paste")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("&Paste");
  sToolTipText  = QT_TR_NOOP("Paste operation");
  sWhatsThis    = "Std_Paste";
  sStatusTip    = QT_TR_NOOP("Paste operation");
#ifndef TANGO_ICONS
  sPixmap       = "Paste";
#else
  sPixmap       = "edit-paste";
#endif
  iAccel        = Qt::CTRL+Qt::Key_V;
}

void StdCmdPaste::activated(int iMsg)
{
  getGuiApplication()->sendMsgToActiveView("Paste");
}

bool StdCmdPaste::isActive(void)
{
  return getGuiApplication()->sendHasMsgToActiveView("Paste");
}

//===========================================================================
// Std_SelectAll
//===========================================================================

DEF_STD_CMD_A(StdCmdSelectAll);

StdCmdSelectAll::StdCmdSelectAll()
  : Command("Std_SelectAll")
{
    sGroup        = QT_TR_NOOP("Edit");
    sMenuText     = QT_TR_NOOP("Select &All");
    sToolTipText  = QT_TR_NOOP("Select all");
    sWhatsThis    = "Std_SelectAll";
    sStatusTip    = QT_TR_NOOP("Select all");
#ifdef TANGO_ICONS
    sPixmap       = "edit-select-all";
#endif
    iAccel        = Qt::CTRL+Qt::Key_A;
}

void StdCmdSelectAll::activated(int iMsg)
{
    SelectionSingleton& rSel = Selection();
    App::Document* doc = App::GetApplication().getActiveDocument();
    std::vector<App::DocumentObject*> objs = doc->getObjectsOfType(App::DocumentObject::getClassTypeId());
    for(std::vector<App::DocumentObject*>::const_iterator it=objs.begin();it!=objs.end();++it) {
        if (!rSel.isSelected(doc->getName(), (*it)->getNameInDocument()))
            rSel.addSelection(doc->getName(), (*it)->getNameInDocument());
    }
}

bool StdCmdSelectAll::isActive(void)
{
    return App::GetApplication().getActiveDocument() != 0;
}

//===========================================================================
// Std_Delete
//===========================================================================
DEF_STD_CMD_A(StdCmdDelete);

StdCmdDelete::StdCmdDelete()
  :Command("Std_Delete")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("&Delete");
  sToolTipText  = QT_TR_NOOP("Deletes the selected objects");
  sWhatsThis    = "Std_Delete";
  sStatusTip    = QT_TR_NOOP("Deletes the selected objects");
#ifdef TANGO_ICONS
  sPixmap       = "edit-delete";
#endif
  iAccel        = Qt::Key_Delete;
}

void StdCmdDelete::activated(int iMsg)
{
    // go through all documents
    const SelectionSingleton& rSel = Selection();
    const std::vector<App::Document*> docs = App::GetApplication().getDocuments();
    for ( std::vector<App::Document*>::const_iterator it = docs.begin(); it != docs.end(); ++it ) {
        const std::vector<App::DocumentObject*> sel = rSel.getObjectsOfType(App::DocumentObject::getClassTypeId(), (*it)->getName());
        if (!sel.empty()) {
            (*it)->openTransaction("Delete");
            for(std::vector<App::DocumentObject*>::const_iterator ft=sel.begin();ft!=sel.end();ft++) {
                if ((*ft)->getTypeId().isDerivedFrom(App::DocumentObjectGroup::getClassTypeId()))
                doCommand(Doc,"App.getDocument(\"%s\").getObject(\"%s\").removeObjectsFromDocument()"
                             ,(*it)->getName(), (*ft)->getNameInDocument());
                doCommand(Doc,"App.getDocument(\"%s\").removeObject(\"%s\")"
                             ,(*it)->getName(), (*ft)->getNameInDocument());
            }
            (*it)->commitTransaction();
        }
    }
}

bool StdCmdDelete::isActive(void)
{
  return Selection().getCompleteSelection().size() > 0;
}

//===========================================================================
// Std_Refresh
//===========================================================================
DEF_STD_CMD_A(StdCmdRefresh);

StdCmdRefresh::StdCmdRefresh()
  :Command("Std_Refresh")
{
  sGroup        = QT_TR_NOOP("Edit");
  sMenuText     = QT_TR_NOOP("&Refresh");
  sToolTipText  = QT_TR_NOOP("Recomputes the current active document");
  sWhatsThis    = "Std_Refresh";
  sStatusTip    = QT_TR_NOOP("Recomputes the current active document");
#ifndef TANGO_ICONS
  sPixmap       = "reload";
#else
  sPixmap       = "view-refresh";
#endif
  iAccel        = Qt::Key_F5;
}

void StdCmdRefresh::activated(int iMsg)
{
  if ( getActiveGuiDocument() )
  {
    openCommand("Refresh active document");
    doCommand(Doc,"App.activeDocument().recompute()");
    commitCommand(); 
  }
}

bool StdCmdRefresh::isActive(void)
{
  return this->getDocument() && this->getDocument()->isTouched();
}


namespace Gui {

void CreateDocCommands(void)
{
    CommandManager &rcCmdMgr = Application::Instance->commandManager();

    rcCmdMgr.addCommand(new StdCmdNew());
    rcCmdMgr.addCommand(new StdCmdOpen());
    rcCmdMgr.addCommand(new StdCmdImport());
    rcCmdMgr.addCommand(new StdCmdExport());

    rcCmdMgr.addCommand(new StdCmdSave());
    rcCmdMgr.addCommand(new StdCmdSaveAs());
    rcCmdMgr.addCommand(new StdCmdProjectInfo());
    rcCmdMgr.addCommand(new StdCmdUndo());
    rcCmdMgr.addCommand(new StdCmdRedo());
    rcCmdMgr.addCommand(new StdCmdPrint());
    rcCmdMgr.addCommand(new StdCmdPrintPdf());
    rcCmdMgr.addCommand(new StdCmdQuit());
    rcCmdMgr.addCommand(new StdCmdCut());
    rcCmdMgr.addCommand(new StdCmdCopy());
    rcCmdMgr.addCommand(new StdCmdPaste());
    rcCmdMgr.addCommand(new StdCmdSelectAll());
    rcCmdMgr.addCommand(new StdCmdDelete());
    rcCmdMgr.addCommand(new StdCmdRefresh());
}

} // namespace Gui
