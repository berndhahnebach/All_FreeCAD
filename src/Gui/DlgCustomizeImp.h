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

class QTabWidget;

namespace Gui {
namespace Dialog {

/**
 * Dialog which contains all available commands and separate
 * them in several categories (file, edit, view,...).
 * You can extend the existing toolbars or buttongroups with 
 * several commands just by drag and drop.
 * Furthermore it is possible to create new commands from macros.
 * @see DlgCustomCommandsImp
 * @see DlgCustomToolbarsImp
 * @see DlgCustomCmdbarsImp
 * @see DlgCustomActionsImp
 */
class DlgCustomizeImp : public QDialog
{ 
    Q_OBJECT

public:
	DlgCustomizeImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
	~DlgCustomizeImp();

	/** Inserts a new tab page with its caption */
	void insertTab (QWidget* w, QString name);

private slots:
	/** Click the OK button to save changes and to exit */
	void onOK();
	/** Click the Apply button to save all changes */
	void onApply();
	/** Click the Cancel button to discard all changes */
	void onCancel();

private:
	/** @name for internal use only */
	//@{
	QPushButton* buttonHelp; /**< the help button */
	QPushButton* buttonApply; /**< the apply button */
	QPushButton* buttonOk; /**< the Ok button */
	QPushButton* buttonCancel; /**< the cancel button */
	QTabWidget* tabWidget; /**< tab wigdets containing all pages */ 
	QGridLayout* FCDlgCustomizeBaseLayout; /**< layout */
	QHBoxLayout* Layout; /** horizontal layout */
	std::vector<QWidget*> tabPages; /**< tab pages */
	//@}
};

} // namespace Dialog
} // namespace Gui

#endif // DLGCUSTOMIZE_H
