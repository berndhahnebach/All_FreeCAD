/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           * 
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef UNDOREDODLG_H
#define UNDOREDODLG_H

#ifndef _PreComp_
# include <qlistbox.h>
# include <qframe.h>
# include <qtoolbutton.h>
#endif

// forward declarations
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListBox;
class QListBoxItem;

namespace Gui {
namespace Dialog {

/** This class implements an extension of QListBox.
 * If you select an item all items above are selected automatically.
 * This behaviour is known of the Undo/Redo dialog of a lot of applications.
 * @see UndoRedoDlg
 * \author Werner Mayer.
 */
class UndoRedoList : public QListBox
{
public:
  UndoRedoList( QWidget * parent=0, const char * name=0, WFlags f=0);
  virtual ~UndoRedoList();

  void mouseMoveEvent ( QMouseEvent * e );
  void mousePressEvent (QMouseEvent* e);
};

// ------------------------------------------------------------

/** This class implements the undo/redo dialog.
 * \author Werner Mayer
 */
class UndoRedoDialog : public QFrame
{ 
    Q_OBJECT

public:
  enum TMode 
  {
    Undo, /**< Undo mode */ 
    Redo  /**< Redo mode */
  };

 UndoRedoDialog( QWidget* parent = 0, const char* name = 0, TMode tMode = Undo );
 virtual ~UndoRedoDialog();

  void setMode(TMode tMode);
  TMode getMode() const;
  void updateUndoRedoList();

signals:
  /** This signal is emitted by the @ref onSelected() slot */
  void clickedListBox();

protected:
  void fetchUndoRedoInfo();

protected slots:
  void onSelChangeUndoRedoList();
  void onSelected();

protected:
  QLabel*          pTextLabel; /**< Text label to show the number of onSelected items. */
  UndoRedoList*    pListBox; /**< List containing the undo/redo information. */
  TMode            tMode; /**< Either it's a undo or a redo list. */
};

// ------------------------------------------------------------

class ToolButtonDropDownPrivate;

/** Drop down button
 * This class implements the behaviour of buttons with an additional smaller button
 * (drop-down button). Such buttons are usually used as undo/redo-buttons in a lot
 * of Windows programs.
 * \author Werner Mayer
 */
class ToolButtonDropDown : public QToolButton
{
  Q_OBJECT

public:
  ToolButtonDropDown(QWidget * parent, const QPixmap& rclPixmap, QWidget* pWidget=0, const char * name = 0);
  ~ToolButtonDropDown ();

  QSize sizeHint() const;
  void setWidget(QWidget* pWidget);
  QWidget* getWidget();

signals
  /** This signal is emitted when @ref onPopupWidget() is called */:
  void updateWidgetSignal();

protected slots:
  void onPopupWidget();

protected:
  /** @name overwritten methods from base class
   */
  //@{
  void enterEvent         ( QEvent      * e );
  void leaveEvent         ( QEvent      * e );
  void mousePressEvent    ( QMouseEvent * e );
  void mouseReleaseEvent  ( QMouseEvent * e );
  void drawButton         ( QPainter    * p );
  void drawButtonLabel    ( QPainter    * p );
  void paintEvent         ( QPaintEvent * e ); 
  //@}

  QWidget* pWidget; /**< This widget arises when clicking on drop-down button */

private:
  /** Draws the arrow for the drop down button */ 
  void drawArrow( QPainter *, bool, int, int, int, int, const QColorGroup &, bool, const QBrush* = NULL);
  ToolButtonDropDownPrivate* d; /**< for internal use only */
};

} // namespace Dialog
} // namespace Gui

#endif
