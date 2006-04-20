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

#include <qsyntaxhighlighter.h>
#include <qtextedit.h>

#include <Base/Console.h>
#include "DockWindow.h"
#include "Window.h"

class QTabWidget;

namespace Gui {
class PythonConsole;
namespace DockWnd {

class ReportOutput;
class ReportHighlighter;

/** Report view containing an output window and a very simple Python console.
 * @see ReportOutput
 * @see PythonConsole
 * \author Werner Mayer
 */
class ReportView : public Gui::DockWindow
{ 
  Q_OBJECT

public:
  ReportView( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~ReportView();

protected:
  void languageChange();

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

  /**
   * Sets the text color to  \a col.
   */
  void setTextColor( const QColor& col );

  /**
   * Sets the color for log messages to  \a col.
   */
  void setLogColor( const QColor& col );

  /**
   * Sets the color for warnings to  \a col.
   */
  void setWarningColor( const QColor& col );

  /**
   * Sets the color for error messages to  \a col.
   */
  void setErrorColor( const QColor& col );

private:
  /** @name for internal use only */
  //@{
  Paragraph type;
  int lastPos;
  int lastPar;
  QColor txtCol, logCol, warnCol, errCol;
  //@}
};

/** Output window to show messages.
 * @see Base::ConsoleObserver
 * @see QTextEdit
 * \author Werner Mayer
 */
class GuiExport ReportOutput : public QTextEdit, public WindowParameter, public Base::ConsoleObserver
{
  Q_OBJECT

public:
  ReportOutput(QWidget* parent=0, const char* name=0);
  virtual ~ReportOutput();

  /** Observes its parameter group. */
  void OnChange(Base::Subject<const char*> &rCaller, const char * sReason);

  /** Writes warnings */
  void Warning(const char * s);
  /** Writes normal text */
  void Message(const char * s);
  /** Writes errors */
  void Error  (const char * s);
  /** Does not do anything */
  void Log (const char * s);

  /** 
   * Appends new text \a text and scrolls to bottom if the widget itself is not visible.
   * \Renark Normally a scrollToBottom() call in the constructor should be sufficient
   * but works with WindowsXP style only but not for all other styles, at least on 
   * WindowsXP.
   */
  virtual void append ( const QString & text );

  /// returns the name for observer handling
  const char* Name(void){return "ReportOutput";}

  /** Restore the default font settings. */
  void restoreFont ();

  /** Returns true whether errors are reported. */ 
  bool isError() const;
  /** Returns true whether warnings are reported. */ 
  bool isWarning() const;
  /** Returns true whether log messages are reported. */ 
  bool isLogging() const;

protected:
  /** For internal use only */
  bool event( QEvent* ev );
  /** Pops up the context menu with some extensions */
  QPopupMenu * createPopupMenu ( const QPoint & pos );

public slots:
  /** Save the report messages into a file. */
  void onSaveAs();
  /** Toggles the report of errors. */
  void onToggleError();
  /** Toggles the report of warnings. */
  void onToggleWarning();
  /** Toggles the report of log messages. */
  void onToggleLogging();

private:
  ReportHighlighter* reportHl; /**< Syntax highlighter */
};

} // namespace DockWnd
} // namespace Gui

#endif //__FC_REPORT_VIEW_H__
