/***************************************************************************
                          DlgToolbarsImp.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize toolbars
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




#ifndef DLGTOOLBARS_IMP_H
#define DLGTOOLBARS_IMP_H

#include "DlgToolbars.h"
#include "DlgCustomizeImp.h"

class FCDlgCustomToolbarsImp : public FCDlgCustomToolbars, public FCPropertyPage
{ 
  Q_OBJECT

  public:
    FCDlgCustomToolbarsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCDlgCustomToolbarsImp();

  protected:
    void apply();
    void cancel();

  protected slots:
    /// shows all buttons of the toolbar
    void slotToolBarSelected(const QString &);
    /// creates new toolbar
    void slotCreateToolBar();
    /// deletes toolbar
    void slotDeleteToolBar();
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

  protected:
    // groups of commands
    std::map<std::string, std::vector<FCCommand*> > m_alCmdGroups;
    std::vector<FCToolBar*>                         m_aclToolbars;
};

#endif
