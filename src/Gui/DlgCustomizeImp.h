/***************************************************************************
                          DlgCustomizeImp.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize toolbars and button groups
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/




#ifndef DLGCUSTOMIZE_H
#define DLGCUSTOMIZE_H

#include "DlgToolbars.h"
#include "DlgCommands.h"
#include "DlgActions.h"
#include "Window.h"

class FCCommand;
class QTabWidget;

class FCDlgCustomToolbarsImp : public FCDlgCustomToolbars
{ 
  Q_OBJECT

  public:
    FCDlgCustomToolbarsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCDlgCustomToolbarsImp();

  protected slots:
    /// shows all buttons of the toolbar
    void slotToolBarSelected(const QString &);
    /// creates new toolbar
    void slotCreateToolBar();
    /// adds a new action by double click
    void slotDblClickAddAction(QListViewItem*);
    /// adds a new action
    void slotAddAction();
    /// removes an action
    void slotRemoveAction();
    /// moves up an action
    void slotMoveUpAction();
    /// moves down an action
    void slotMoveDownAction();
    /// enables/disables buttons for change
    void slotCurrentActionsChanged( QListViewItem *i );
    /// enables/disables buttons for change
    void slotAvailableActionsChanged( QListViewItem *i );
    // applies changes
    void apply();

  protected:
    // groups of commands
    std::map<std::string, std::vector<FCCommand*> > m_alCmdGroups;
    std::vector<QToolBar*>                          m_aclToolbars;
    bool bChanged;
};

class FCDlgCustomCommandsImp : public FCDlgCustomCommands
{ 
  Q_OBJECT

  public:
    FCDlgCustomCommandsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCDlgCustomCommandsImp();

  protected slots:
    /// shows the description for the corresponding command
    void slotDescription(QString);
    /// show all commands of this category
    void slotGroupSelected(const QString &);
    // applies changes
    void apply();

  protected:
    // groups of commands
    std::map<std::string, std::vector<FCCommand*> > m_alCmdGroups;
    bool bChanged;
};

class FCDlgCustomActionsImp : public FCDlgCustomActions
{ 
  Q_OBJECT

  public:
    FCDlgCustomActionsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCDlgCustomActionsImp();

  protected slots:
    /// enables/disables buttons for change
    void slotCustomActionsChanged( QListViewItem *i );
    /// adds a custom action
    void slotAddCustomAction();
    /// deletes a custom action
    void slotDelCustomAction();
    /// select a pixmap
    void slotCustomActionPixmap();
    // applies changes
    void apply();

  protected:
    bool bChanged;
};


/**
 * Dialog which contains all available commands and separate
 * them in several categories (file, edit, view,...).
 * You can extend the existing toolbars or buttongroups with 
 * several commands just by drag and drop.
 */
class FCDlgCustomize : public QDialog
{ 
    Q_OBJECT

  public:
    FCDlgCustomize( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FCDlgCustomize();

    QPushButton* buttonHelp;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QTabWidget* tabWidget;

  protected slots:
    /// click the OK button
    void slotOK();
    /// click the Apply button
    void slotApply();

  protected:
    QGridLayout* FCDlgCustomizeBaseLayout;
    QHBoxLayout* Layout;
    std::vector<QWidget*> tabPages;
};

#endif // DLGCUSTOMIZE_H
