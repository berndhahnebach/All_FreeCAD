/***************************************************************************
                          ReportView.h  -  description
                             -------------------
    begin                : 2003/08/16 10:47:44
    copyright            : (C) 2003 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file ReportView.h
 *  \brief The output window of FreeCAD
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
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
 

#ifndef __FC_REPORT_VIEW_H__
#define __FC_REPORT_VIEW_H__

#include "Window.h"

class PythonConsole;

namespace Gui {
namespace DockWnd {

class ReportOutput;
class ReportHighlighter;

/** 
 * Report view containing an output window and a very simple Python console.
 * @see ReportOutput
 * @see PythonConsole
 */
class CReportView : public FCDockWindow
{ 
    Q_OBJECT

  public:
    CReportView( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~CReportView();

  private:
    QTabWidget* tab;
    ReportOutput* mle; /**< Output window */
		PythonConsole* pyc; /**< Python console */
};

/**
 * Syntax highlighter to write log or normal messages, warnings and errors in different colors.
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

/**
 * Output window to show messages.
 * @see Base::ConsoleObserver
 * @see QTextEdit
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
