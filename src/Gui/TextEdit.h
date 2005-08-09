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


#ifndef TEXT_EDIT_H_
#define TEXT_EDIT_H_

#include "View.h"
#include "Window.h"

#ifndef _PreComp_
# include <qlistbox.h>
# include <qtextedit.h>
# include <private/qrichtext_p.h>
#endif

class QListBox;
class QListBoxItem;

namespace Gui {
class CompletionBox;

/**
 * Completion is a means by which an editor automatically completes words that the user is typing. 
 * For example, in a code editor, a programmer might type "sur", then Tab, and the editor will complete 
 * the word the programmer was typing so that "sur" is replaced by "surnameLineEdit". This is very 
 * useful for text that contains long words or variable names. The completion mechanism usually works 
 * by looking at the existing text to see if any words begin with what the user has typed, and in most 
 * editors completion is invoked by a special key sequence.
 *
 * TextEdit can detect a special key sequence to invoke the completion mechanism, and can handle three 
 * different situations:
 * \li There are no possible completions.
 * \li There is a single possible completion.
 * \li There are two or more possible completions.
 * 
 * \remark The original sources are taken from Qt Quarterly (Customizing for Completion).
 * \author Werner Mayer
 */
class GuiExport TextEdit : public QTextEdit
{
  Q_OBJECT

public:
  TextEdit(QWidget *parent = 0,const char *name = 0);
  virtual ~TextEdit();

  QTextDocument *document() const { return QTextEdit::document(); }

private slots:
  void complete();
  void itemChosen( QListBoxItem *item );

private:
  void createListBox();
  void adjustListBoxSize(int maxHeight = 32767, int maxWidth = 32767);
  QPoint textCursorPoint() const;

private:
  QString wordPrefix;
  CompletionBox *listBox;
};

/**
 * The CompletionBox class provides a list box that pops up in a text edit if the user has pressed
 * an accelerator to complete the current word he is typing in..
 * @author Werner Mayer
 */
class CompletionBox : public QListBox
{
public:
  /// Construction
  CompletionBox( QWidget* parent = 0, const char*  name=0 );
  /// Destruction
  ~CompletionBox();

  /// Opens the list box at position \a pos.
  void popup( const QPoint& pos );

protected:
  void mousePressEvent( QMouseEvent * );
    
private:
  QWidget* popupParent;
};

} // namespace Gui

#endif // TEXT_EDIT_H_
