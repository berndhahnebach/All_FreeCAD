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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qregexp.h>
# include <qtextedit.h>
# include <qsyntaxhighlighter.h>
# include <qclipboard.h>
# include <qmessagebox.h>
# include <qfile.h>
# include <qfiledialog.h>
#endif

#include "PythonEditor.h"
#include "Application.h"
#include "DlgEditorImp.h"
#include "../Base/Interpreter.h"
#include "../Base/Exception.h"
#include "../Base/Parameter.h"

using namespace Gui;
using namespace Gui::Dialog;

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
//  switch (e->key())
//  {
//    case Key_Colon:
//      nInsertTabs++;
//      break;
//
//    case Key_Return:
//    case Key_Enter:
//      if (nInsertTabs > 0)
//      {
//        for (int i=0; i<nInsertTabs;i++)
//        {
//          insert("\t");
//        }
//      }
//      else
//      {
//        try{
//        }
//        catch(...)
//        {
//        }
//        nInsertTabs=0;
//      }
//      break;
//  }
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
#ifdef FC_OS_LINUX
  QFont serifFont( "Courier", 15, QFont::Normal );
#else
  QFont serifFont( "Courier", 10, QFont::Normal );
#endif
  setFont(serifFont);

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
      bool bMute = GuiConsoleObserver::bMute;

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
        GuiConsoleObserver::bMute = true;
        if (!cmd.isEmpty())
          Base::Interpreter().Launch(cmd.latin1());
      }
      catch (const FCException& exc)
      {
        setColor(Qt::red);
        pythonSyntax->highlightError(true);
        insertAt( exc.what(), lastPara-1, 0);
        pythonSyntax->highlightError(false);
      }

      GuiConsoleObserver::bMute = bMute;
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

class PythonSyntaxHighlighterP
{
public:
  PythonSyntaxHighlighterP() : hlError(false)
  {
    keywords << "and" << "assert" << "break" << "class" << "continue" << "def" << "del" <<
    "elif" << "else" << "except" << "exec" << "finally" << "for" << "from" << "global" <<
    "if" << "import" << "in" << "is" << "lambda" << "None" << "not" << "or" << "pass" << "print" <<
    "raise" << "return" << "try" << "while" << "yield";

    // set colors
    FCParameterGrp::handle hPrefGrp = GetApplication().GetUserParameter().GetGroup("BaseApp");
    hPrefGrp = hPrefGrp->GetGroup("Windows")->GetGroup("Editor");

    long c;
    c = hPrefGrp->GetInt("Text", GetDefCol().color("Text"));
    cNormalText.setRgb(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
    c = hPrefGrp->GetInt("Comment", GetDefCol().color("Comment"));
    cComment.setRgb(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
    c = hPrefGrp->GetInt("Block comment", GetDefCol().color("Block comment"));
    cBlockcomment.setRgb(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
    c = hPrefGrp->GetInt("Number", GetDefCol().color("Number"));
    cNumber.setRgb(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
    c = hPrefGrp->GetInt("String", GetDefCol().color("String"));
    cLiteral.setRgb(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
    c = hPrefGrp->GetInt("Keyword", GetDefCol().color("Keyword"));
    cKeyword.setRgb(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
    c = hPrefGrp->GetInt("Class name", GetDefCol().color("Class name"));
    cClassName.setRgb(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
    c = hPrefGrp->GetInt("Define name", GetDefCol().color("Define name"));
    cDefineName.setRgb(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
    c = hPrefGrp->GetInt("Operator", GetDefCol().color("Operator"));
    cOperator.setRgb(c & 0xff, (c >> 8) & 0xff, (c >> 16) & 0xff);
  }

  QStringList keywords;
  QString blockComment;
  bool hlError;
  QColor cNormalText, cComment, cBlockcomment, cLiteral, cNumber,
  cOperator, cKeyword, cClassName, cDefineName;
};

PythonSyntaxHighlighter::PythonSyntaxHighlighter(QTextEdit* edit)
    : QSyntaxHighlighter(edit)
{
  d = new PythonSyntaxHighlighterP;
}

PythonSyntaxHighlighter::~PythonSyntaxHighlighter()
{
  delete d;
}

void PythonSyntaxHighlighter::highlightError (bool b)
{
  d->hlError = b;
}

int PythonSyntaxHighlighter::highlightBlockComments( const QString& txt, int& from, int endStateOfLastPara )
{
  QRegExp rx("(\"\"\"|''')");
  int pos = txt.find(rx, from);
  // if this is part of a block search for the closing tag
  if (endStateOfLastPara == (int)Blockcomment)
    pos = txt.find(d->blockComment, from);
  else if (pos > -1)
    d->blockComment = rx.cap(1);

  // whole line is a comment
  if ( pos == -1 && endStateOfLastPara == (int)Blockcomment)
  {
    setFormat(0, txt.length(), d->cBlockcomment);
  }
  // tag for block comments found
  else if ( pos > -1)
  {
    int begin = pos;

    // not part of a block comment
    if ( endStateOfLastPara != (int)Blockcomment )
    {
      // search also for the closing tag
      int end = txt.find(d->blockComment, begin+3);

      // all between "begin" and "end" is a comment
      if ( end > -1 )
      {
        int len = end - begin + 3;
        setFormat(begin, len, d->cBlockcomment);
        from = end+3;
      }
      // all after "begin" is a comment
      else
      {
        int len = txt.length() - begin;
        setFormat(begin, len, d->cBlockcomment);
        from = txt.length();
        endStateOfLastPara = (int)Blockcomment;
      }
    }
    // previous line is part of a block comment
    else
    {
      int len = begin + 3;
      setFormat(0, len, d->cBlockcomment);
      from = begin+3;
      endStateOfLastPara = (int)Normal;
    }
  }

  return endStateOfLastPara;
}

int PythonSyntaxHighlighter::highlightLiterals( const QString& txt, int& from, int endStateOfLastPara )
{
  QRegExp rx("(\"|')");
  int pos = txt.find(rx, from);
  QString pat = rx.cap(1);
  if (pos > -1)
  {
    int begin = pos;

    // search also for the closing tag
    int end = txt.find(pat, begin+1);

    if (end > -1)
    {
      int len = end - begin + 1;
      setFormat(begin, len, d->cLiteral);
      from = end+1;
      endStateOfLastPara = (int)Normal;
    }
    else
    {
      // whole line is a literal
      int len = txt.length()-begin;
      setFormat(begin, len, d->cLiteral);
      from = txt.length();
      endStateOfLastPara = (int)Literal;
    }
  }

  // no blocks for literals allowed
  return (int)endStateOfLastPara;
}

int PythonSyntaxHighlighter::highlightComments( const QString& txt, int& from, int endStateOfLastPara )
{
  // check for comments
  int pos = txt.find("#", from);
  if (pos != -1)
  {
    int len = txt.length()-pos;
    setFormat(pos, len, d->cComment);
    endStateOfLastPara = (int)Comment;
  }

  return (int)endStateOfLastPara;
}

int PythonSyntaxHighlighter::highlightNormalText( const QString& txt, int& from, int endStateOfLastPara )
{
  // colourize everything black
  setFormat(from, txt.length()-from, d->cNormalText);

  return 0;
}

int PythonSyntaxHighlighter::highlightKeywords( const QString& txt, int& from, int endStateOfLastPara )
{
  // search for all keywords to colourize
  for (QStringList::Iterator it = d->keywords.begin(); it != d->keywords.end(); ++it)
  {
    QRegExp keyw(QString("\\b%1\\b").arg(*it));
    int pos = txt.find(keyw, from);
    while (pos > -1)
    {
      QFont font = textEdit()->currentFont();
      font.setBold( true );
      setFormat(pos, (*it).length(), font, d->cKeyword);

      // make next word bold
      if ( (*it) == "def" || (*it) == "class")
      {
        QRegExp word("\\b[A-Za-z0-9]+\\b");
        int wd = txt.find(word, pos+(*it).length());
        if ( wd > -1)
        {
          int len = word.matchedLength();
          QFont font = textEdit()->currentFont();
          font.setBold( true );
          if ((*it) == "def")
            setFormat(wd, len, font, d->cDefineName);
          else // *it == "class"
            setFormat(wd, len, font, d->cClassName);
        }
      }

      pos = txt.find(keyw, pos+(*it).length());
    }
  }

  return 0;
}

int PythonSyntaxHighlighter::highlightOperators( const QString& txt, int& from, int endStateOfLastPara )
{
  QRegExp rx( "[\\[\\]\\{\\}\\(\\)\\+\\*\\-/<>]" );
  int pos = rx.search( txt, from );
  while ( pos > -1 )
  {
    int len = rx.matchedLength();
    setFormat(pos, len, d->cOperator);
    pos = rx.search( txt, pos+len );
  }

  return 0;
}

int PythonSyntaxHighlighter::highlightNumbers( const QString& txt, int& from, int endStateOfLastPara )
{
  QRegExp rx( "\\b(\\d+)\\b" );
  int pos = rx.search( txt, from );
  while ( pos > -1 )
  {
    int len = rx.matchedLength();
    setFormat(pos, len, d->cNumber);
    pos = rx.search( txt, pos+len );
  }

  return 0;
}

int PythonSyntaxHighlighter::highlightParagraph ( const QString & text, int endStateOfLastPara )
{
  QString txt(text);
  int from = 0;

  // highlight as error
  if ( d->hlError )
  {
    QFont font = textEdit()->currentFont();
    font.setItalic( true );
    setFormat(0, txt.length(), font, Qt::red);
    return 0;
  }

  // colourize all keywords, operators, numbers, normal text
  highlightNormalText(txt, from, endStateOfLastPara);
  highlightKeywords  (txt, from, endStateOfLastPara);
  highlightOperators (txt, from, endStateOfLastPara);
  highlightNumbers   (txt, from, endStateOfLastPara);

  QString comnt("#");
  QRegExp blkcm("(\"\"\"|''')");
  QRegExp strlt("(\"|')");
  while ( from < (int)txt.length() && txt != " ")
  {
    if ( endStateOfLastPara == int(Blockcomment) )
    {
      // search for closing block comments
      endStateOfLastPara = highlightBlockComments(txt, from, endStateOfLastPara);
      if (endStateOfLastPara > 0) return endStateOfLastPara;
    }
    else
    {
      // search for the first tag
      int com = txt.find(comnt, from);
      if (com < 0) com = txt.length();

      int bcm = txt.find(blkcm, from);
      if (bcm < 0) bcm = txt.length();

      int str = txt.find(strlt, from);
      if (str < 0) str = txt.length();

      // comment
      if (com < bcm && com < str)
      {
        from = com;
        highlightComments(txt, from, endStateOfLastPara);
        return 0;
      }
      // block comment
      else if (bcm < com && bcm <= str)
      {
        from = bcm;
        endStateOfLastPara = highlightBlockComments(txt, from, endStateOfLastPara);
        if (endStateOfLastPara > 0) return endStateOfLastPara;
      }
      // string literal
      else if (str < com && str < bcm)
      {
        from = str;
        endStateOfLastPara = highlightLiterals(txt, from, endStateOfLastPara);
        if (endStateOfLastPara > 0) return 0;
      }
      // no tag found
      else
      {
        from = txt.length();
      }
    }
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

  Base::Console().Log("FCScintillaView::OnMsg() unhandled \"%s\"\n",pMsg);
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
