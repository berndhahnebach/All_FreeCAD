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
#include "PropertyPage.h"

class FCCommand;
class FCToolBar;

namespace Gui {
namespace Dialog {

/**
 * This class implements the creation of user defined toolbars.
 */
class DlgCustomToolbarsBase : public FCDlgCustomToolbars, public Gui::Dialog::PropertyPage
{ 
  Q_OBJECT

  protected:
    DlgCustomToolbarsBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    virtual ~DlgCustomToolbarsBase();

  protected:
    virtual void apply();
    virtual void cancel();
		virtual void onUpdate();

    /** Shows all buttons of the toolbar */
    void onItemActivated(const QString &);
    /** Adds a new action by double click */
    void onDoubleClickedAction(QListViewItem*);
    /** Adds a new action */
    void onAddAction();
    /** Removes an action */
    void onRemoveAction();
    /** Noves up an action */
    void onMoveUpAction();
    /** Moves down an action */
    void onMoveDownAction();
    /** Enables/disables buttons for change */
    void onNewActionChanged( QListViewItem *i );
    /** Enables/disables buttons for change */
    void onAllActionsChanged( QListViewItem *i );

  protected:
    // groups of commands
    std::map<std::string, std::vector<FCCommand*> > m_alCmdGroups;
    std::vector<FCToolBar*>                         m_aclToolbars;
};

/**
 * This class implements the creation of user defined command bars.
 * @see DlgCustomToolbarsBase
 * @see DlgCustomCmdbarsImp
 */
class DlgCustomToolbarsImp : public DlgCustomToolbarsBase
{ 
  Q_OBJECT

public:
	DlgCustomToolbarsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~DlgCustomToolbarsImp();

protected:
	/** Adds created or removes deleted toolbars */
	void apply();
	/** Discards all changes */
	void cancel();
	/** Shows all actions from the last specified command bar */
	void onUpdate();

	/** Creates new toolbar */
	void onCreateToolbar();
	/** Deletes a toolbar */
	void onDeleteToolbar();
};

} // namespace Dialog
} // namespace Gui

#endif
