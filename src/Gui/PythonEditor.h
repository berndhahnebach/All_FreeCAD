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


#ifndef __PYTHON_EDITOR_H__
#define __PYTHON_EDITOR_H__

#include "View.h"
#include "Window.h"

#ifndef _PreComp_
# include <qsyntaxhighlighter.h>
#endif

#include <TextEdit.h>

class QListBox;
class QListBoxItem;
class QPrinter;

namespace Gui {

class PythonSyntaxHighlighter;
class PythonSyntaxHighlighterP;

/**
 * Basic stuff for a Python text editor with syntax highlighting..
 * \author Werner Mayer
 */
class GuiExport PythonWindow : public TextEdit, public WindowParameter
{
  Q_OBJECT

protected:
  PythonWindow(QWidget *parent = 0,const char *name = 0);
  virtual ~PythonWindow();

  virtual void OnChange( FCSubject<const char*> &rCaller,const char* rcReason );

protected:
  virtual void keyPressEvent(QKeyEvent * e);

protected:
  PythonSyntaxHighlighter* pythonSyntax;

private:
  int nInsertTabs;
};

/**
 * Python text editor with syntax highlighting..
 * \author Werner Mayer
 */
class GuiExport PythonEditor : public PythonWindow
{
public:
  PythonEditor(QWidget *parent = 0,const char *name = 0);
  ~PythonEditor();

  void OnChange( FCSubject<const char*> &rCaller,const char* rcReason );
};

/**
 * Python text console with syntax highlighting..
 * \author Werner Mayer
 */
class GuiExport PythonConsole : public PythonWindow
{
public:
  PythonConsole(QWidget *parent = 0,const char *name = 0);
  ~PythonConsole();

protected:
  void keyPressEvent(QKeyEvent * e);

private:
  int lastPara;
};

/**
 * Syntax highlighter for Python.
 * \author Werner Mayer
 */
class GuiExport PythonSyntaxHighlighter : public QSyntaxHighlighter
{
protected:
  enum Paragraph {
    Normal       = 0,
    Comment      = 1,
    Blockcomment = 2,
    Literal      = 3,
    Number       = 4,
    Operator     = 5,
    Keywords     = 6
  };

public:
  PythonSyntaxHighlighter(QTextEdit* );
  ~PythonSyntaxHighlighter();

  int highlightParagraph ( const QString & text, int endStateOfLastPara );
  void highlightError (bool b);
  void setColor( Paragraph type, const QColor& col );
  void setColor( const QString& type, const QColor& col );

private:
  int highlightLiteral( const QString&, int& from, int endStateOfLastPara );
  int highlightBlockComment( const QString&, int& from, int endStateOfLastPara );
  int highlightComment( const QString&, int& from, int endStateOfLastPara );
  int highlightOperator( const QString&, int& from, int endStateOfLastPara );
  int highlightNumber( const QString&, int& from, int endStateOfLastPara );
  int highlightNormalText( const QString&, int& from, int endStateOfLastPara );
  int highlightKeyword( const QString&, int& from, int endStateOfLastPara );

  PythonSyntaxHighlighterP* d;
};

/**
 * A special view class which sends the messages from the application to
 * the Python editor and embeds the editor in a window.
 * \author Werner Mayer
 */
class GuiExport PythonEditView : public MDIView
{
public:
  PythonEditView( QWidget* parent, const char* name);
  ~PythonEditView();

  QTextEdit* editor() const { return textEdit; }

  const char *getName(void){return "PythonEditView";}
  void onUpdate(void){};

  bool onMsg(const char* pMsg,const char** ppReturn);
  bool onHasMsg(const char* pMsg);

  bool canClose(void);
  void print( QPrinter* printer );
  void openFile (const QString& fileName);

  /** @name Standard actions of the editor */
  //@{
  bool save   (void);
  bool saveAs (void);
  bool open   (void);
  void cut    (void);
  void copy   (void);
  void paste  (void);
  void undo   (void);
  void redo   (void);
  //@}

  bool isSavedOnce();

  QStringList undoActions() const;
  QStringList redoActions() const;

private:
  void saveFile();

private:
  QTextEdit* textEdit;
  QString _fileName;
};

} // namespace Gui

#endif // __PYTHON_EDITOR_H__
