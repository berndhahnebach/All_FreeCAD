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

#include "DlgCustomize.h"
#include "Window.h"

class FCCommand;

/**
 * Dialog which contains all available commands and separate
 * them in several categories (file, edit, view,...).
 * You can extend the existing toolbars or buttongroups with 
 * several commands just by drag and drop.
 */
class FCDlgCustomize : public FCDlgCustomizeBase
{ 
  Q_OBJECT

  public:
    FCDlgCustomize( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~FCDlgCustomize();

  protected slots:
    /// shows the description for the corresponding command
    void slotDescription(QString);
    /// show all commands of this category
    void slotGroupSelected(const QString &);
    /// shows all buttons of the toolbar
    void slotToolBarSelected(const QString &);
    /// creates new toolbar
    void slotCreateToolBar();
    /// enables/disables buttons for change
    void slotCurrentActionsChanged( QListViewItem *i );
    /// enables/disables buttons for change
    void slotAvailableActionsChanged( QListViewItem *i );
    /// adds a new action
    void slotAddAction();
    /// adds a new action by double click
    void slotDblClickAddAction(QListViewItem*);
    /// removes an action
    void slotRemoveAction();
    /// moves up an action
    void slotMoveUpAction();
    /// moves down an action
    void slotMoveDownAction();
    /// click the OK button
    void slotOK();
    /// click the Apply button
    void slotApply();

  protected:
    void init();
    // groups of commands
    std::map<std::string, std::vector<FCCommand*> > m_alCmdGroups;
    std::vector<QToolBar*>                          m_aclToolbars;
    bool bChanged;
};

#endif // DLGCUSTOMIZE_H
