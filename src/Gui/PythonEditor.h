/** \file PythonEditor.h
 *  \brief A simple Python editor with syntax highlighting
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Werner Mayer (werner.wm.mayer@gmx.de) 2000 - 2003                 *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Werner Mayer 2002                                                     *
 ***************************************************************************/


#ifndef __PYTHON_EDITOR_H__
#define __PYTHON_EDITOR_H__

#include "View.h"

#ifndef _PreComp_
#	include <qtextedit.h>
#	include <qsyntaxhighlighter.h>
#endif

class PythonSyntaxHighlighter;
class PythonSyntaxHighlighterP;

class GuiExport PythonWindow : public QTextEdit
{
	protected:
		PythonWindow(QWidget *parent = 0,const char *name = 0);
		virtual ~PythonWindow();

	protected:
		virtual void keyPressEvent(QKeyEvent * e);
		int nInsertTabs;

	protected:
		PythonSyntaxHighlighter* pythonSyntax;
};

class GuiExport PythonEditor : public PythonWindow
{
	public:
		PythonEditor(QWidget *parent = 0,const char *name = 0);
		~PythonEditor();
};

class GuiExport PythonConsole : public PythonWindow
{
	public:
		PythonConsole(QWidget *parent = 0,const char *name = 0);
		~PythonConsole();

	protected:
		void keyPressEvent(QKeyEvent * e);
		int lastPara;
};

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

	private:
		int highlightLiterals( const QString&, int& from, int endStateOfLastPara );
		int highlightBlockComments( const QString&, int& from, int endStateOfLastPara );
		int highlightComments( const QString&, int& from, int endStateOfLastPara );
		int highlightOperators( const QString&, int& from, int endStateOfLastPara );
		int highlightNumbers( const QString&, int& from, int endStateOfLastPara );
		int highlightNormalText( const QString&, int& from, int endStateOfLastPara );
		int highlightKeywords( const QString&, int& from, int endStateOfLastPara );

		PythonSyntaxHighlighterP* d;
};

/**
 * A special view class which sends the messages from the application to
 * the Python editor and embeds the editor in a window
 */
class PythonView : public FCView
{
	public:
	  PythonView( QWidget* parent, const char* name);
	  ~PythonView();

    QTextEdit* GetEditor() const { return textEdit; }

    const char *GetName(void){return "Scintilla";}
		void Update(void){};

		/** Mesage handler.
     * Runs the action specified by pMsg.
     */
		bool OnMsg(const char* pMsg);
		/** Mesage handler test
     * Checks if the action pMsg is available. This is for enabling/disabling
     * the corresponding buttons or menu items for this action.
     */
		bool OnHasMsg(const char* pMsg);
		/** checking on close state */
		bool CanClose(void);
		void Print(QPainter& cPrinter);
    void OpenFile (const QString& fileName);

    /** @name Standard actions of the editor */
    //@{
    bool Save   (void);
    bool SaveAs (void);
    bool Open   (void);
		void Cut    (void);
		void Copy   (void);
		void Paste  (void);
    void Undo   (void);
    void Redo   (void);
    //@}

    
    void saveFile();
    bool isAlreadySavedBefore();
    QString _fileName;

	protected:
    QTextEdit* textEdit;
};

#endif // __PYTHON_EDITOR_H__
