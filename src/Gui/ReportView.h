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


#ifndef __FC_REPORT_VIEW_H__
#define __FC_REPORT_VIEW_H__

#ifndef _PreComp_
# include <qsyntaxhighlighter.h>
# include <qtextedit.h>
#endif

#include "Window.h"
#include "../Base/Console.h"

class PythonConsole;
class QTabWidget;

namespace Gui {
namespace DockWnd {

class ReportOutput;
class ReportHighlighter;

/** Report view containing an output window and a very simple Python console.
 * @see ReportOutput
 * @see PythonConsole
 * \author Werner Mayer
 */
class ReportView : public FCDockWindow
{ 
  Q_OBJECT

public:
  ReportView( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~ReportView();

private:
  QTabWidget* tab;
  ReportOutput* mle; /**< Output window */
  PythonConsole* pyc; /**< Python console */
};

/** Syntax highlighter to write log or normal messages, warnings and errors in different colors.
 * \author Werner Mayer
 */
class GuiExport ReportHighlighter : public QSyntaxHighlighter
{
public: 
  enum Paragraph { 
      Message  = 0, /**< normal text */
      Warning  = 1, /**< Warning */
      Error    = 2, /**< Error text */
      LogText  = 3  /**< Log text */
  };

public:
  ReportHighlighter(QTextEdit* );
  ~ReportHighlighter();

  /** Parses the given text and highlight it in the right colors. */
  int highlightParagraph ( const QString & text, int endStateOfLastPara );
  /** 
   * Sets the current paragraph type used in ReportOutput
   * @see ReportOutput::Message
   * @see ReportOutput::Warning
   * @see ReportOutput::Error
   */
  void setParagraphType(Paragraph);

private:
  /** @name for internal use only */
  //@{
  Paragraph type;
  int lastPos;
  int lastPar;
  //@}
};

/** Output window to show messages.
 * @see Base::ConsoleObserver
 * @see QTextEdit
 * \author Werner Mayer
 */
class GuiExport ReportOutput : public QTextEdit, public Base::ConsoleObserver
{
  Q_OBJECT

public:
  ReportOutput(QWidget* parent=0, const char* name=0);
  virtual ~ReportOutput();

  /** Writes warnings */
  void Warning(const char * s);
  /** Writes normal text */
  void Message(const char * s);
  /** Writes errors */
  void Error  (const char * s);
  /** Does not do anything */
  void Log (const char * s);

  /** Restore the default font settings */
  void restoreFont ();

protected:
  /** For internal use only */
  bool event( QEvent* ev );
  /** Pops up the context menu with some extensions */
  QPopupMenu * createPopupMenu ( const QPoint & pos );

public slots:
  /** Save the report messages into a file. */
  void onSaveAs();

private:
  ReportHighlighter* reportHl; /**< Syntax highlighter */
};

} // namespace DockWnd
} // namespace Gui

#endif //__FC_REPORT_VIEW_H__
