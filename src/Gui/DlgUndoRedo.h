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

#include <qpopupmenu.h>
#include <qlistbox.h>
#include <qtoolbutton.h>
#include <qhbox.h>


// forward declarations
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListBox;
class QListBoxItem;

/**
 * This class implements an extension of 'QListBox'.
 * If you select an item all items above will be selected automatically.
 */
class FCUndoRedoList : public QListBox
{
  public:
    FCUndoRedoList( QWidget * parent=0, const char * name=0, WFlags f=0);
    virtual ~FCUndoRedoList(){};

    void mouseMoveEvent ( QMouseEvent * e );
    void mousePressEvent (QMouseEvent* e);
};

/**
 * This class implements the undo/redo dialog.
 */
class FCUndoRedoDlg : public QFrame
{ 
    Q_OBJECT
  public:
    enum TMode {Undo, Redo};

    FCUndoRedoDlg( QWidget* parent = 0, const char* name = 0, TMode tMode = Undo );
    virtual ~FCUndoRedoDlg();

    /// sets the mode (Undo, Redo)
    void setMode(TMode tMode);
    /// returns the mode
    TMode getMode() const;
    /// updates the undo/redo list
    void updateUndoRedoList();

  signals:
    void clickedListBox();

  protected:
	  void init();

  protected slots:
	  void selChangeUndoRedoList();
    void selected();

  protected:
    QLabel*          _pTextLabel;
    FCUndoRedoList*  _pListBox;
    TMode            _tMode;
};

/** 
 * Drop Down Toolbar button
 * This class implements the behaviour of buttons with an additional smaller button
 * (drop-down button). Such buttons are usually used as undo/redo-buttons in a lot
 * of Windows programs.
 */
class FCToolButtonDropDown : public QToolButton
{
  Q_OBJECT
  public:
    // constructor/destructor
    FCToolButtonDropDown(QWidget * parent, const QPixmap& rclPixmap, QWidget* pWidget=0, const char * name = 0);
    virtual ~FCToolButtonDropDown ();

    /// sets a widget, this widget will be shown if click this button
    void setWidget(QWidget* pWidget);
    /// returns the widget
    QWidget* getWidget();
    /// enables this button if bEnable is true, otherwise disables it
    void setButtonEnabled(bool bEnable);
    /// returns true if this button is enabled otherwise false
    bool isButtonEnabled();
    void setAutoRaiseEx (bool bEnable);
    bool autoRaiseEx () const;

  signals:
    // signals when entering or leaving the button with the mouse
    void enterEventSignal(QEvent* e);
    void leaveEventSignal(QEvent* e);
    void updateWidgetSignal();

  protected:
    // overwrite methods from base class
    virtual void enterEvent(QEvent* e);
    virtual void leaveEvent(QEvent* e);

  protected slots:
    // popup the window
	  void popupWidget();
    // process the events 
    void enterEventSlot(QEvent* e);
    void leaveEventSlot(QEvent* e);

  private:
    // this constructor will be used to create an instance
    // of this object for '_pDropDown'
    FCToolButtonDropDown(QWidget* parent, const char * name);

    // this is the drop-down button right besides the actual button
    // on the button there will be drawn a small arrow
    FCToolButtonDropDown* _pDropDown;

  protected:
    // show this widget when clicking on drop-down button
    QWidget* _pWidget;
};




#endif
