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
#include "PropertyPage.h"

class FCCommand;

namespace Gui {
namespace Dialog {

/**
 * This class implements the creation of user defined actions executing a recorded macro.
 * @see FCAction
 * @see FCScriptCommand
 * @see FCCommand
 */
class DlgCustomActionsImp : public DlgCustomActionsBase, public Gui::Dialog::PropertyPage
{ 
Q_OBJECT

public:
	/** construction */
	DlgCustomActionsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	/** destruction */
	~DlgCustomActionsImp();

	/** 
	 * Displays this page. If no macros were found a message box
	 * appears.
	 */
	void show();

private slots:
	/** Enables/disables buttons for deletion */
	void onCustomActionsCanDelete( QListViewItem *i );
	/** Shows the setup of the action */
	void onCustomActionsDoubleClicked( QListViewItem *i );
	/** Adds a custom action */
	void onAddCustomAction();
	/** Deletes a custom action */
	void onDelCustomAction();
	/** Opens a file dialog to select a pixmap */
	void onCustomActionPixmap();

private:
	/** Shows the pixmaps of macros if available */
	void showPixmaps();
	/** 
	 * Add all temporary created commands to the FCCommandManager.
	 * @see FCCommandManager
	 */
	void apply();
	/** Discards all changes */
	void cancel();
	 /** Name for the new created action */
	void newActionName();

private:
	bool bShown; /**< For internal use only*/
	QString m_sPixmap; /**< Path of the specified pixmap */
	std::vector<FCCommand*> _aclNewMacros; /**< All temporary created actions */
	std::vector<FCCommand*> _aclDelMacros; /**< All temporary deleted actions */
};

} // namespace Dialog
} // namespace Gui

#endif
