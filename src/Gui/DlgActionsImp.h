/***************************************************************************
                          DlgActionsImp.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Customize actions
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




#ifndef DLGACTIONS_IMP_H
#define DLGACTIONS_IMP_H

#include "DlgActions.h"
#include "DlgCustomizeImp.h"

/**
 * This class implements the creation of user defined commands executing a recorded macro.
 */
class FCDlgCustomActionsImp : public FCDlgCustomActions, public FCPropertyPage
{ 
  Q_OBJECT

  public:
    /// construction
    FCDlgCustomActionsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    /// destruction
    ~FCDlgCustomActionsImp();

    /// shows the page
    void show();

  protected:
    //@{
    /// for internal use only
    void init();
    void apply();
    void cancel();
    QString m_sPixmap;
    std::vector<FCCommand*> _aclCurMacros;
    std::vector<FCCommand*> _aclNewMacros;
    std::vector<FCCommand*> _aclDelMacros;
    int iCtMacros;

  protected slots:
    /// enables/disables buttons for change
    void slotCustomActionsChanged( QListViewItem *i );
    void slotCustomActionsDoubleClicked( QListViewItem *i );
    /// adds a custom action
    void slotAddCustomAction();
    /// deletes a custom action
    void slotDelCustomAction();
    /// select a pixmap
    void slotCustomActionPixmap();
    //@}
};

#endif
