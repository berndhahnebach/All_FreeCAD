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


#ifndef GUI_PYTHONEDITOR_H
#define GUI_PYTHONEDITOR_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include "Window.h"
#include "TextEdit.h"

namespace Gui {

class PythonSyntaxHighlighter;
class PythonSyntaxHighlighterP;

/**
 * Python text editor with syntax highlighting..
 * \author Werner Mayer
 */
class GuiExport PythonEditor : public TextEdit, public WindowParameter
{
  Q_OBJECT

public:
  PythonEditor(QWidget *parent = 0);
  ~PythonEditor();

  void OnChange( Base::Subject<const char*> &rCaller,const char* rcReason );

public Q_SLOTS:
  /** Inserts a '#' at the beginning of each selected line or the current line if 
   * nothing is selected
   */
  void onComment();
  /**
   * Removes the leading '#' from each selected line or the current line if
   * nothing is selected. In case a line hasn't a leading '#' then
   * this line is skipped.
   */
  void onUncomment();

private Q_SLOTS:
    void onCursorPositionChanged();

protected:
  void keyPressEvent ( QKeyEvent * e );
  /** Pops up the context menu with some extensions */
  void contextMenuEvent ( QContextMenuEvent* e );
  /** Draw a beam in the line where the cursor is. */
  void paintEvent ( QPaintEvent * e );

private:
  PythonSyntaxHighlighter* pythonSyntax;
  struct PythonEditorP* d;
};

/**
 * Syntax highlighter for Python.
 * \author Werner Mayer
 */
class GuiExport PythonSyntaxHighlighter : public QSyntaxHighlighter
{
public:
  PythonSyntaxHighlighter(QTextEdit* );
  virtual ~PythonSyntaxHighlighter();

  void highlightBlock ( const QString & text );
  int maximumUserState() const;
  
  void setColor( const QString& type, const QColor& col );
  QColor color( const QString& type );

protected:
  virtual void colorChanged( const QString& type, const QColor& col );

private:
  PythonSyntaxHighlighterP* d;
};

} // namespace Gui

#endif // GUI_PYTHONEDITOR_H
