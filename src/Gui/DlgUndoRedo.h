/***************************************************************************
                          DlgUndoRedo.h  -  description
                             -------------------
    begin                : 2002/04/23 21:02:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file DlgUndoRedo.h
 *  \brief The undo/redo dialog module
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef UNDOREDODLG_H
#define UNDOREDODLG_H

#include <qlistbox.h>

// forward declarations
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListBox;
class QListBoxItem;

namespace Gui {
namespace Dialog {

/**
 * This class implements an extension of @ref QListBox.
 * If you select an item all items above will be selected automatically.
 */
class CUndoRedoList : public QListBox
{
public:
	CUndoRedoList( QWidget * parent=0, const char * name=0, WFlags f=0);
	virtual ~CUndoRedoList(){};

	/** 
	 * If the left mouse button is pressed and depending on the moving direction
	 * the items are selected or deselected.
	 */
	void mouseMoveEvent ( QMouseEvent * e );
	/** Select all items above the selected one */
	void mousePressEvent (QMouseEvent* e);
};

// ------------------------------------------------------------

/**
 * This class implements the undo/redo dialog.
 */
class CUndoRedoDlg : public QFrame
{ 
    Q_OBJECT
public:
	enum TMode {Undo, Redo};

	CUndoRedoDlg( QWidget* parent = 0, const char* name = 0, TMode tMode = Undo );
	virtual ~CUndoRedoDlg();

	/** Sets the mode (Undo, Redo) */
	void setMode(TMode tMode);
	/** Returns the mode */
	TMode getMode() const;
	/** Updates the undo/redo list */
	void updateUndoRedoList();

signals:
	/** This signal is emitted by the @ref selected() slot */
	void clickedListBox();

protected:
	/** 
	 *  This method fetches the undo / redo information from the 
	 *  active document and shows it in the undo / redo dialog.
	 */
	void fetchUndoRedoInfo();

protected slots:
	/** Sets the number of actons to undo/redo */
	void selChangeUndoRedoList();
	/** Closes the dialog and emits the @ref clickedListBox() signal */
	void selected();

protected:
	QLabel*          _pTextLabel; /**< Text label to show the number of selected items */
	CUndoRedoList*   _pListBox; /**< List containing the undo/redo info */
	TMode            _tMode; /**< Either it's a undo or a redo list */
};

// ------------------------------------------------------------

/** 
 * Drop Down Toolbar button
 * This class implements the behaviour of buttons with an additional smaller button
 * (drop-down button). Such buttons are usually used as undo/redo-buttons in a lot
 * of Windows programs.
 */
class CToolButtonDropDownPrivate;
class CToolButtonDropDown : public QToolButton
{
  Q_OBJECT

public:
	// constructor/destructor
	CToolButtonDropDown(QWidget * parent, const QPixmap& rclPixmap, QWidget* pWidget=0, const char * name = 0);
	~CToolButtonDropDown ();

	/** Returns the size hint of a normal button plus extra space for the drop down button */
	QSize sizeHint() const;

	/** Sets a widget, this widget will be shown if click this button */
	void setWidget(QWidget* pWidget);
	/** Returns the current widget */
	QWidget* getWidget();

signals
	/** This signal is emitted when @ref popupWidget() is called */:
	void updateWidgetSignal();

protected slots:
	/** Pops up the window */
	void popupWidget();

protected:
	/** @name overwritten methods from base class
 	 */
	//@{
	void enterEvent(QEvent* e);
	void leaveEvent(QEvent* e);
	void mousePressEvent  ( QMouseEvent *e );
	void mouseReleaseEvent( QMouseEvent *e );
	/** Draws the button */
	void drawButton     ( QPainter * p );
	/** Draws the button label */
	void drawButtonLabel( QPainter * p );
	void paintEvent     ( QPaintEvent *e ); 
	//@}

	QWidget* _pWidget; /**< Shows this widget when clicking on drop-down button */

private:
	/** Draws the arrow for the drop down button */
	void drawArrow( QPainter *, bool, int, int, int, int, const QColorGroup &, bool, const QBrush* = NULL);
	CToolButtonDropDownPrivate* d; /**< for internal use only */
};

} // namespace Dialog
} // namespace Gui

#endif
