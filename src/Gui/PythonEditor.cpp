/** \file PythonEditor.cpp
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


#include "PreCompiled.h"

#ifndef _PreComp_
#	include <qregexp.h>
#	include <qtextedit.h>
#	include <qsyntaxhighlighter.h>
#	include <qclipboard.h>
#	include <qmessagebox.h>
#	include <qfile.h>
#	include <qfiledialog.h>
#endif

#include "PythonEditor.h"
#include "Application.h"
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"



PythonWindow::PythonWindow(QWidget *parent,const char *name)
	: QTextEdit(parent, name)
{
	nInsertTabs = 0;
	pythonSyntax = new PythonSyntaxHighlighter(this);
}

PythonWindow::~PythonWindow()
{
	delete pythonSyntax;
}

void PythonWindow::keyPressEvent(QKeyEvent * e)
{
	QTextEdit::keyPressEvent(e);
//
//	switch (e->key())
//	{
//	  case Key_Colon:
//			nInsertTabs++;
//			break;
//
//	  case Key_Return:
//	  case Key_Enter:
//			if (nInsertTabs > 0)
//			{
//				for (int i=0; i<nInsertTabs;i++)
//				{
//					insert("\t");
//				}
//			}
//			else
//			{
//				try{
//				}
//				catch(...)
//				{
//				}
//				nInsertTabs=0;
//			}
//		  break;
//	}
}

// ------------------------------------------------------------------------

PythonEditor::PythonEditor(QWidget *parent,const char *name)
	: PythonWindow(parent, name)
{
}

PythonEditor::~PythonEditor()
{
}

// ------------------------------------------------------------------------

PythonConsole::PythonConsole(QWidget *parent,const char *name)
	: PythonWindow(parent, name), lastPara(-1)
{
	zoomIn(2);
	QFont serifFont( "Courier", 10, QFont::Normal );
	setFont(serifFont);

	int tabs = tabStopWidth();
	setTabStopWidth(32);
}

PythonConsole::~PythonConsole()
{
}

void PythonConsole::keyPressEvent(QKeyEvent * e)
{
	PythonWindow::keyPressEvent(e);

	switch (e->key())
	{
	  case Key_Return:
	  case Key_Enter:
			int para; int idx;
			getCursorPosition(&para, &idx);
			QString txt = text(para-1);
			if (txt.isEmpty() || txt == " ")
			{
				bool bMute = FCGuiConsoleObserver::bMute;

				try
				{
					// check if any python command was already launched
					int prev = lastPara > -1 ? lastPara : 0;
					QString cmd;
					// build the python command
					for (int i=prev; i<para; i++)
					{
						cmd += text(i);
						cmd += "\n";
					}
					lastPara = para;

					// launch the command now
					FCGuiConsoleObserver::bMute = true;
					if (!cmd.isEmpty())
					GetInterpreter().Launch(cmd.latin1());
				}
				catch (const FCException& exc)
				{
					setColor(Qt::red);
					pythonSyntax->highlightError(true);
					insertAt( exc.what(), lastPara-1, 0);
					pythonSyntax->highlightError(false);
				}

				FCGuiConsoleObserver::bMute = bMute;
			}
		  break;
	}

	// cursor must not be inside the text of the last command
	int para; int idx;
	getCursorPosition(&para, &idx);
	if (para < lastPara)
	{
		setCursorPosition(lastPara, idx);
	}
}

// ------------------------------------------------------------------------

PythonSyntaxHighlighter::PythonSyntaxHighlighter(QTextEdit* edit)
: QSyntaxHighlighter(edit), hlError(false)
{
	keywords << "and" << "assert" << "break" << "class" << "continue" << "def" << "del" << 
		"elif" << "else" << "except" << "exec" << "finally" << "for" << "from" << "global" << 
		"if" << "import" << "in" << "is" << "lambda" << "None" << "not" << "or" << "pass" << "print" <<
		"raise" << "return" << "try" << "while" << "yield";
}

PythonSyntaxHighlighter::~PythonSyntaxHighlighter()
{
}

void PythonSyntaxHighlighter::highlightError (bool b)
{
	hlError = b;
}

int PythonSyntaxHighlighter::highlightBlockComments( const QString& txt, int& from, int endStateOfLastPara )
{
	QRegExp rx("(\"\"\"|''')");
	int pos = txt.find(rx, from);
	// if this is part of a block search for the right end
	if (endStateOfLastPara == (int)Blockcomment)
		pos = txt.find(blockComment, from);
	else if (pos > -1)
		blockComment = rx.cap(1);
	QColor cm(160,160,164);

	// whole line is a comment
	if ( pos == -1 && endStateOfLastPara == (int)Blockcomment)
	{
		setFormat(0, txt.length(), QColor(160,160,164));
	}
	else
	{
		while (pos > -1)
		{
			int begin = pos;

			// search also for the end (if block begins with """ the 
			// it _must_ end with and vice versa)
			int end = txt.find(blockComment, begin+3);

			if (end > -1)
			{
				// all between "begin" and "end" is a comment
				if (endStateOfLastPara != (int)Blockcomment)
				{
					int len = end - begin + 3;
					setFormat(begin, len, cm);
				}
				// all before "begin" and after "end" is a comment
				else
				{
					int len = begin + 3;
					setFormat(0, len, cm);
					setFormat(end, 3, cm);
				}

				from = end+3;
			}
			else
			{
				// all after "begin" is a comment
				if (endStateOfLastPara != (int)Blockcomment)
				{
					int len = txt.length() - begin;
					setFormat(begin, len, cm);
					endStateOfLastPara = (int)Blockcomment;
				}
				// all before "begin" is a comment
				else
				{
					int len = begin + 3;
					setFormat(0, len, cm);
					endStateOfLastPara = (int)Normal;
				}

				from = begin+3;
			}

			// do it again
			pos = txt.find(rx, from);
			if (pos > -1)
				blockComment = rx.cap(1);
		}
	}

	return endStateOfLastPara;
}

int PythonSyntaxHighlighter::highlightLiterals( const QString& txt, int& from, int endStateOfLastPara )
{
	QRegExp rx("(\"|')");
	int pos = txt.find(rx, from);
	QString pat = rx.cap(1);
	while (pos > -1)
	{
		int begin = pos;

		// search also for the end
		int end = txt.find(pat, begin+1);

		if (end > -1)
		{
			int len = end - begin + 1;
			setFormat(begin, len, QColor(255, 0, 0));
			from = end+1;
		}
		else
		{
			int len = txt.length()-begin;
			setFormat(begin, len, QColor(255, 0, 0));
			from = pos+1;
		}

		pos = txt.find(rx, from);
		pat = rx.cap(1);
	}

	// no blocks for literals allowed
	return (int)Normal;
}

int PythonSyntaxHighlighter::highlightComments( const QString& txt, int& from, int endStateOfLastPara )
{
	// check for comments
	int pos = txt.find("#", from);
	if (pos != -1)
	{
		setFormat(pos, txt.length(), QColor(0, 170, 0));
	}

	return (int)Normal;
}

int PythonSyntaxHighlighter::highlightNormalText( const QString& txt, int& from, int endStateOfLastPara )
{
	// first colourize everything black
	setFormat(from, txt.length()-from, Qt::black);

	// and search then for all keywords to colourize
	for (QStringList::Iterator it = keywords.begin(); it != keywords.end(); ++it)
	{
		QRegExp keyw(QString("\\b%1\\b").arg(*it));
		int pos = txt.find(keyw);
		while (pos > -1)
		{
			QFont font = textEdit()->currentFont();
			font.setBold( true );
			setFormat(pos, (*it).length(), font, Qt::blue);
			pos = txt.find(keyw, pos+1);
		}
	}

	return 0;
}

int PythonSyntaxHighlighter::highlightOperators( const QString&, int& from, int endStateOfLastPara )
{
//		setFormat(0, text.length(), QColor(160,160,164));
	return 0;
}

int PythonSyntaxHighlighter::highlightNumbers( const QString&, int& from, int endStateOfLastPara )
{
//		setFormat(0, text.length(), QColor(0,0,255));
	return 0;
}

int PythonSyntaxHighlighter::highlightParagraph ( const QString & text, int endStateOfLastPara )
{
	QString txt(text);
	int ret;
	int from = 0;

	// highlight as error
	if ( hlError )
	{
		QFont font = textEdit()->currentFont();
		font.setItalic( true );
		setFormat(0, txt.length(), font, Qt::red);
		return 0;
	}

	switch (Paragraph(endStateOfLastPara))
	{
		// just continue to other case statements
		default:
		case Normal:
		{
			highlightNormalText(txt, from, endStateOfLastPara);
		}
		case Blockcomment:
		{
			// search for block comments
			ret = highlightBlockComments(txt, from, endStateOfLastPara);
			if (ret > 0) return ret;
		};

		case Literal:
		{
			// search for string literals
			//
			ret = highlightLiterals(txt, from, endStateOfLastPara);
			if (ret > 0) return ret;
		};

		case Comment:
		{
			// at least search for normal comments
			ret = highlightComments(txt, from, endStateOfLastPara);
			if (ret > 0) return ret;
		};

		case Number:
		{
		};

		case Operator:
		{
		};
	}

	return 0;
}

// ------------------------------------------------------------------------

PythonView::PythonView( QWidget* parent, const char* name)
: FCView(0,parent, name)  
{
	setCaption("Editor");
	setTabCaption("Editor");
	resize( 400, 300 );

	textEdit = new PythonEditor(this);

	QGridLayout *layout = new QGridLayout(this,0,0);
	layout->addWidget(textEdit,0,0);
}

PythonView::~PythonView()
{
	delete textEdit;
}

bool PythonView::OnMsg(const char* pMsg)
{
	if (strcmp(pMsg,"Save")==0){
		Save();
		return true;
	}
	if (strcmp(pMsg,"SaveAs")==0){
		SaveAs();
		return true;
	}
	if (strcmp(pMsg,"Cut")==0){
		Cut();
		return true;
	}
	if (strcmp(pMsg,"Copy")==0){
		Copy();
		return true;
	}
	if (strcmp(pMsg,"Paste")==0){
		Paste();
		return true;
	}
	if (strcmp(pMsg,"Undo")==0){
		Undo();
		return true;
	}
	if (strcmp(pMsg,"Redo")==0){
		Redo();
		return true;
	}

	GetConsole().Log("FCScintillaView::OnMsg() unhandled \"%s\"\n",pMsg);
	return false;
}

// Message handler test
bool PythonView::OnHasMsg(const char* pMsg)
{
	if (strcmp(pMsg,"Save")==0)  return true;
	if (strcmp(pMsg,"Print")==0) return true;
	if (strcmp(pMsg,"Cut")==0)
  {
		bool canWrite = !textEdit->isReadOnly();
    return (canWrite && (textEdit->hasSelectedText()));
  }
	if (strcmp(pMsg,"Copy")==0)
  {
    return ( textEdit->hasSelectedText() );
  }
	if (strcmp(pMsg,"Paste")==0)
  {
    QClipboard *cb = QApplication::clipboard();
    QString text;

    // Copy text from the clipboard (paste)
    text = cb->text();

		bool canWrite = !textEdit->isReadOnly();
    return ( !text.isEmpty() && canWrite );
  }
	if (strcmp(pMsg,"Undo")==0)
  {
		return textEdit->isUndoAvailable ();
  }
	if (strcmp(pMsg,"Redo")==0)
  {
		return textEdit->isRedoAvailable ();
  }
	return false;
}

bool PythonView::CanClose(void)
{
	if ( !textEdit->isModified() )
		return true;

  switch(QMessageBox::warning( this, "Unsaved document","Save file before close?","Yes","No","Cancel",0,2))
	{
		case 0:
			return Save();
		case 1:
			return true;
		case 2:
			return false;
		default:
			return false;
	}
}

bool PythonView::Save(void)
{
  if ( !textEdit->isModified() )
    return true;

  // check if saved ever before
	if (isAlreadySavedBefore())
  {
    saveFile();
    return true;
  }
	else
  {
		return SaveAs();
  }
}

bool PythonView::SaveAs(void)
{
	QString fn = QFileDialog::getSaveFileName(QString::null, "FreeCAD macro (*.FCMacro);;Python (*.py)", this);
	if (!fn.isEmpty())
	{
    _fileName = fn;
    saveFile();
    return true;
	}
	else
	{
    return false;
	}
}

bool PythonView::Open(void)
{
	QString file = QFileDialog::getOpenFileName(QString::null, "Macro files (*.py *.FCMacro);;Python (*.py);;FreeCAD macro (*.FCMacro)", this);
	if ( file.isEmpty() ) 
		return false;

	OpenFile(file);
	setCaption(file);
	QString name = file.left(file.findRev('.'));
	setTabCaption(name);

	return true;
}

void PythonView::OpenFile (const QString& fileName)
{
  QString line;
  _fileName = fileName;
  QFile file(fileName);
   
  if( !file.open(IO_ReadOnly))
     return;

  QTextStream in(&file);
  QString text;

  while( !in.atEnd() ){
    line = in.readLine();
    text = line + "\n";
		textEdit->append(text);
  }

  file.close();

	textEdit->setModified(false);
}

void PythonView::Cut(void)
{
	textEdit->cut();
}

void PythonView::Copy(void)
{
	textEdit->copy();
}

void PythonView::Paste(void)
{
	textEdit->paste();
}

void PythonView::Undo(void)
{
	textEdit->undo();
}

void PythonView::Redo(void)
{
	textEdit->redo();
}

void PythonView::Print(QPainter& cPrinter)
{
	// no printing yet ;-)
	assert(0);
}

void PythonView::saveFile()
{
	QFile file(_fileName);
  if( !file.open(IO_WriteOnly))
    return;
	
  QTextStream out(&file);
	out << textEdit->text();
  file.close();

	textEdit->setModified(false);
  
  return;
}

bool PythonView::isAlreadySavedBefore()
{
  return (!_fileName.isEmpty());
}
