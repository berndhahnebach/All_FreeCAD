/***************************************************************************
                          DlgCmdbarsImp.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize command bars
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




#ifndef DLGCMDBARS_IMP_H
#define DLGCMDBARS_IMP_H

#include "DlgCmdbars.h"
#include "DlgCustomizeImp.h"

class FCDlgCustomCmdbarsImp : public FCDlgCustomCmdbars, public FCPropertyPage
{ 
  Q_OBJECT

  public:
    FCDlgCustomCmdbarsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~FCDlgCustomCmdbarsImp();

  protected:
    void apply();
    void cancel();

  protected slots:
    /// shows all buttons of the cmdbar
    void slotCmdBarSelected(const QString &);
    /// creates new cmdbar
    void slotCreateCmdBar();
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
    std::vector<FCToolBar*>                         m_aclCmdbars;
};

#endif
