/***************************************************************************
                          FCUndoRedoDlg.h  -  description
                             -------------------
    begin                : Die Apr 23 21:02:14 CEST 2002
    copyright            : (C) 2002 by Werner Mayer
    email                : 
 ***************************************************************************/

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


class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListBox;
class QListBoxItem;

/** Special Undo/Redo list box
 * Diese Klasse stellt eine Erweiterung der Basisklasse QListBox dar.
 * Normalerweise werden bei Selektion eines Elements in einem Undo/Redo-Dialog
 * alle darüberliegenden Elemente ebenfalls selektiert.
 * Diese Klasse steuert gerade dieses Verhalten.
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
 * Diese Klasse stellt eine erste Möglchkeit zur Erzeugung eines 
 * Undo/Redo-Dialogs dar. Diese Klasse erbt von QFrame; integriert
 * werden dann einfach ein FCUndoRedoList-Objekt und ein Label.
 * Die Positionierung muss vom Benutzer slebst vorgenommen werden.
 */

class FCUndoRedoDlg : public QFrame
{ 
    Q_OBJECT
  public:
    enum TMode {Undo, Redo};

    FCUndoRedoDlg( QWidget* parent = 0, const char* name = 0, TMode tMode = Undo );
    virtual ~FCUndoRedoDlg();

    void setMode(TMode tMode);
    TMode getMode() const;
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
 * Das ist eine zweite Möglichkeit, einen Undo/Redo-Dialog zu erzeugen.
 * Als ersten Menüeintrag wählt man hier eine Listbox, als zweiten ein
 * Textlabel. Die Vorteile hierbei sind die, dass man das Verschieben des
 * Menüs nicht selbst vornehmen muss und dass das Zeichnen des Frames vom
 * Popup-Menü selbst gemacht wird.
 * Ein Objekt kann man mit der Methode setPopup() der QToolButton auf 
 * einfache Weise einbinden.
 * Ein Problem stellt hierbei noch folgende Tatsache dar:
 * öffnet man das Menü öfters, so wird es immer schmäler.
 */

class FCUndoRedoPopup : public QPopupMenu
{
    Q_OBJECT
  public:
    FCUndoRedoPopup( QWidget* parent = 0, const char* name = 0, bool bUndo = true );
    virtual ~FCUndoRedoPopup();

  protected:
	  void init();

  protected slots:
	  void selChangeUndoRedoList();

  protected:
    QLabel*    _pTextLabel;
    QListBox*  _pListBox;
    bool       _bUndo;
};



/** Drop Down Toolbar button
 * Description:
 * ============
 * This class implements the behaviour of buttons with an additional smaller button
 * (drop-down button). Such buttons are usually used as undo/redo-buttons in a lot
 * of windows programs.
 *
 * Todo:
 * =====
 * When you put the toolbar having one of this kind of button as child to the left or to the
 * right border of your main window then the drop-down button will be arranged below your
 * button and not right besides.
 * This looks a little bit strange.
 */

class QToolButtonDropDown : public QToolButton
{
  Q_OBJECT
  public:
    // constructor/destructor
    QToolButtonDropDown(QWidget * parent, const QPixmap& rclPixmap, QWidget* pWidget=0, const char * name = 0);
    virtual ~QToolButtonDropDown ();

    void setWidget(QWidget* pWidget);
    QWidget* getWidget();
    void setButtonEnabled(bool bEnable);
    bool isButtonEnabled();

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
    QToolButtonDropDown(QWidget* parent, const char * name);

    // this is the drop-down button right besides the actual button
    // on the button there will be drawn a small arrow
    QToolButtonDropDown* _pDropDown;

  protected:
    // show this widget when clicking on drop-down button
    QWidget* _pWidget;
};




#endif
