/***************************************************************************
                          DlgCommandsImp.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Shows all available commands in a view
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




#ifndef DLGCOMMANDS_IMP_H
#define DLGCOMMANDS_IMP_H

#include "DlgCommands.h"
#include "PropertyPage.h"

class FCCommand;

namespace Gui {
namespace Dialog {

/**
 * Shows an overview of all available commands of all groups and modules.
 */
class DlgCustomCommandsImp : public FCDlgCustomCommands, public Gui::Dialog::PropertyPage
{ 
  Q_OBJECT

public:
	DlgCustomCommandsImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~DlgCustomCommandsImp();

protected:
	void apply();
	void cancel();

protected slots:
	/** Shows the description for the corresponding command */
	void onDescription(QString);
	/** Shows all commands of this category */
	void onGroupSelected(const QString &);

protected:
	// groups of commands
	std::map<std::string, std::vector<FCCommand*> > m_alCmdGroups;
};

} // namespace Dialog
} // namespace Gui

#endif
