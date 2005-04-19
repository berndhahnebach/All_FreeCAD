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
# include <qapplication.h>
# include <qclipboard.h>
# include <qfile.h>
# include <qmessagebox.h>
# include <qpaintdevicemetrics.h>
# include <qpainter.h>
# include <qprinter.h>
# include <qregexp.h>
# include <qsimplerichtext.h>
#endif

#include "PythonEditor.h"
#include "Application.h"
#include "FileDialog.h"
#include "DlgEditorImp.h"

#include "../Base/Interpreter.h"
#include "../Base/Exception.h"
#include "../Base/Parameter.h"

using namespace Gui;
using Gui::Dialog::GetDefCol;

/**
 *  Constructs a PythonWindow which is a child of 'parent', with the
 *  name 'name' and installs the Python syntax highlighter.
 */
PythonWindow::PythonWindow(QWidget *parent,const char *name)
    : TextEdit(parent, name), WindowParameter( "Editor" )
{
  nInsertTabs = 0;
  pythonSyntax = new PythonSyntaxHighlighter(this);

  FCParameterGrp::handle hPrefGrp = getWindowParameter();
  hPrefGrp->Attach( this );

  // set colors
  hPrefGrp->NotifyAll();
}

/** Destroys the object and frees any allocated resources */
PythonWindow::~PythonWindow()
{
  getWindowParameter()->Detach( this );
  delete pythonSyntax;
}

/** Sets the new color for \a rcColor. */  
void PythonWindow::OnChange( FCSubject<const char*> &rCaller,const char* rcColor )
{
  FCParameterGrp::handle hPrefGrp = getWindowParameter();

  long col = hPrefGrp->GetInt( rcColor, GetDefCol().color( rcColor ));
  QColor color;
  color.setRgb(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);

  pythonSyntax->setColor( rcColor, color );
}

// ------------------------------------------------------------------------

/**
 *  Constructs a PythonEditor which is a child of 'parent', with the
 *  name 'name'. 
 */
PythonEditor::PythonEditor(QWidget *parent,const char *name)
    : PythonWindow(parent, name)
{
  // set font
  FCParameterGrp::handle hPrefGrp = getWindowParameter();
  hPrefGrp->Notify( "FontSize" );
  hPrefGrp->Notify( "Font" );
}

/** Destroys the object and frees any allocated resources */
PythonEditor::~PythonEditor()
{
}

/** Sets the new color for \a rcColor. */  
void PythonEditor::OnChange( FCSubject<const char*> &rCaller,const char* sReason )
{
  FCParameterGrp::handle hPrefGrp = getWindowParameter();

  QFont font = currentFont();
  if (strcmp(sReason, "FontSize") == 0)
  {
    QString txt = hPrefGrp->GetASCII( "FontSize", "9" ).c_str();

    bool ok;
    int size = txt.toInt(&ok);
    if ( !ok ) size = 9; 

    font.setPointSize( size );
    setFont( font );
  }
  else if (strcmp(sReason, "Font") == 0)
  {
    QString family = hPrefGrp->GetASCII( "Font", "Courier" ).c_str();
    font.setFamily( family );
    setFont( font );
  }
  else
  {
    PythonWindow::OnChange( rCaller, sReason );
  }
}

// ------------------------------------------------------------------------

namespace Gui {
class PythonSyntaxHighlighterP
{
public:
  PythonSyntaxHighlighterP() : hlOutput(false), hlError(false)
  {
    keywords << "and" << "assert" << "break" << "class" << "continue" << "def" << "del" <<
    "elif" << "else" << "except" << "exec" << "finally" << "for" << "from" << "global" <<
    "if" << "import" << "in" << "is" << "lambda" << "None" << "not" << "or" << "pass" << "print" <<
    "raise" << "return" << "try" << "while" << "yield";

    long col = GetDefCol().color( "Text" );
    cNormalText     = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    col = GetDefCol().color( "Comment" );
    cComment        = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    col = GetDefCol().color( "Block comment" );
    cBlockcomment   = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    col = GetDefCol().color( "String" );
    cLiteral        = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    col = GetDefCol().color( "Number" );
    cNumber         = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    col = GetDefCol().color( "Operator" );
    cOperator       = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    col = GetDefCol().color( "Keyword" );
    cKeyword        = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    col = GetDefCol().color( "Class name" );
    cClassName      = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    col = GetDefCol().color( "Define name" );
    cDefineName     = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    col = GetDefCol().color( "Python output" );
    cOutput        = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
    col = GetDefCol().color( "Python error" );
    cError         = QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
  }

  QStringList keywords;
  QString blockComment;
  bool hlOutput, hlError;
  QColor cNormalText, cComment, cBlockcomment, cLiteral, cNumber,
  cOperator, cKeyword, cClassName, cDefineName, cOutput, cError;
};
} // namespace Gui

/**
 * Constructs a Python syntax highlighter.
 */
PythonSyntaxHighlighter::PythonSyntaxHighlighter(QTextEdit* edit)
    : QSyntaxHighlighter(edit)
{
  d = new PythonSyntaxHighlighterP;
}

/** Destroys this object. */
PythonSyntaxHighlighter::~PythonSyntaxHighlighter()
{
  delete d;
}

/** Sets the color \a col to the paragraph type \a type. 
 * Afterwards relighlight() is called, therefore you should
 * not call this method very often because the whóle text has
 * to be rehighlighted.
 */
void PythonSyntaxHighlighter::setColor( Paragraph type, const QColor& col )
{
  // Rehighlighting is very expensive, thus avoid it if this color is already set
  QColor old = color( type );
  if ( !old.isValid() )
    return; // no such type
  if ( old == col )
    return; 
  switch ( type )
  {
  case Normal:
    d->cNormalText = col;
    break;
  case Comment:
    d->cComment = col;
    break;
  case Blockcomment:
    d->cBlockcomment = col;
    break;
  case Literal:
    d->cLiteral = col;
    break;
  case Number:
    d->cNumber = col;
    break;
  case Operator:
    d->cOperator = col;
    break;
  case Keywords:
    d->cKeyword = col;
    break;
  case Output:
    d->cOutput = col;
    break;
  case Errors:
    d->cError = col;
    break;
  default:
    break;
  }

  rehighlight();
}

/** Sets the color \a col to the paragraph type \a type. 
 * This method is provided for convenience to specify the paragraph type
 * by its name.
 */
void PythonSyntaxHighlighter::setColor( const QString& type, const QColor& col )
{
  // Rehighlighting is very expensive, thus avoid it if this color is already set
  QColor old = color( type );
  if ( !old.isValid() )
    return; // no such type
  if ( old == col )
    return; 
  if ( type == "Text" )
    d->cNormalText = col;
  else if ( type == "Comment" )
    d->cComment = col;
  else if ( type == "Block comment" )
    d->cBlockcomment = col;
  else if ( type == "Number" )
    d->cNumber = col;
  else if ( type == "String" )
    d->cLiteral = col;
  else if ( type == "Keyword" )
    d->cKeyword = col;
  else if ( type == "Class name" )
    d->cClassName = col;
  else if ( type == "Define name" )
    d->cDefineName = col;
  else if ( type == "Operator" )
    d->cOperator = col;
  else if ( type == "Python output" )
    d->cOutput = col;
  else if ( type == "Python error" )
    d->cError = col;
  rehighlight();
}

QColor PythonSyntaxHighlighter::color( Paragraph type )
{
  QColor col;
  switch ( type )
  {
  case Normal:
    col = d->cNormalText;
    break;
  case Comment:
    col = d->cComment;
    break;
  case Blockcomment:
    col = d->cBlockcomment;
    break;
  case Literal:
    col = d->cLiteral;
    break;
  case Number:
    col = d->cNumber;
    break;
  case Operator:
    col = d->cOperator;
    break;
  case Keywords:
    col = d->cKeyword;
    break;
  case Output:
    col = d->cOutput;
    break;
  case Errors:
    col = d->cError;
    break;
  default:
    break;
  }

  return col;
}

QColor PythonSyntaxHighlighter::color( const QString& type )
{
  if ( type == "Text" )
    return d->cNormalText;
  else if ( type == "Comment" )
    return d->cComment;
  else if ( type == "Block comment" )
    return d->cBlockcomment;
  else if ( type == "Number" )
    return d->cNumber;
  else if ( type == "String" )
    return d->cLiteral;
  else if ( type == "Keyword" )
    return d->cKeyword;
  else if ( type == "Class name" )
    return d->cClassName;
  else if ( type == "Define name" )
    return d->cDefineName;
  else if ( type == "Operator" )
    return d->cOperator;
  else if ( type == "Python output" )
    return d->cOutput;
  else if ( type == "Python error" )
    return d->cError;
  else
    return QColor(); // not found
}

/**
 * If \a b is set to true the following input to the editor is highlighted as normal output.
 */
void PythonSyntaxHighlighter::highlightOutput (bool b)
{
  d->hlOutput = b;
}

/**
 * If \a b is set to true the following input to the editor is highlighted as error.
 */
void PythonSyntaxHighlighter::highlightError (bool b)
{
  d->hlError = b;
}

/**
 * Highlights a part of the text \a txt as block comment.
 */
int PythonSyntaxHighlighter::highlightBlockComment( const QString& txt, int& from, int endStateOfLastPara )
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

/**
 * Highlights a part of the text \a txt as string literal.
 */
int PythonSyntaxHighlighter::highlightLiteral( const QString& txt, int& from, int endStateOfLastPara )
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

/**
 * Highlights a part of the text \a txt as comment.
 */
int PythonSyntaxHighlighter::highlightComment( const QString& txt, int& from, int endStateOfLastPara )
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

/**
 * Highlights a part of the text \a txt as normal text.
 */
int PythonSyntaxHighlighter::highlightNormalText( const QString& txt, int& from, int endStateOfLastPara )
{
  // colourize everything black
  setFormat(from, txt.length()-from, d->cNormalText);

  return 0;
}

/**
 * Highlights all keywords of the text \a txt.
 */
int PythonSyntaxHighlighter::highlightKeyword( const QString& txt, int& from, int endStateOfLastPara )
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

/**
 * Highlights all operators of the text \a txt.
 */
int PythonSyntaxHighlighter::highlightOperator( const QString& txt, int& from, int endStateOfLastPara )
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

/**
 * Highlights all numbers of the text \a txt.
 */
int PythonSyntaxHighlighter::highlightNumber( const QString& txt, int& from, int endStateOfLastPara )
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

/**
 * Detects all kinds of text to highlight them in the correct color.
 */
int PythonSyntaxHighlighter::highlightParagraph ( const QString & text, int endStateOfLastPara )
{
  QString txt(text);
  int from = 0;

  // highlight as error
  if ( d->hlOutput )
  {
    QFont font = textEdit()->currentFont();
    font.setBold( false );
    font.setItalic( true );
    setFormat(0, txt.length(), font, d->cOutput);
    return 0;
  }

  // highlight as error
  if ( d->hlError )
  {
    QFont font = textEdit()->currentFont();
    font.setBold( false );
    font.setItalic( true );
    setFormat(0, txt.length(), font, d->cError);
    return 0;
  }

  // colourize all keywords, operators, numbers, normal text
  highlightNormalText(txt, from, endStateOfLastPara);
  highlightKeyword   (txt, from, endStateOfLastPara);
  highlightOperator  (txt, from, endStateOfLastPara);
  highlightNumber    (txt, from, endStateOfLastPara);

  QString comnt("#");
  QRegExp blkcm("(\"\"\"|''')");
  QRegExp strlt("(\"|')");
  while ( from < (int)txt.length() && txt != " ")
  {
    if ( endStateOfLastPara == int(Blockcomment) )
    {
      // search for closing block comments
      endStateOfLastPara = highlightBlockComment (txt, from, endStateOfLastPara);
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
        highlightComment (txt, from, endStateOfLastPara);
        return 0;
      }
      // block comment
      else if (bcm < com && bcm <= str)
      {
        from = bcm;
        endStateOfLastPara = highlightBlockComment (txt, from, endStateOfLastPara);
        if (endStateOfLastPara > 0) return endStateOfLastPara;
      }
      // string literal
      else if (str < com && str < bcm)
      {
        from = str;
        endStateOfLastPara = highlightLiteral (txt, from, endStateOfLastPara);
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

/* TRANSLATOR Gui::PythonEditView */

/**
 *  Constructs a PythonEditView which is a child of 'parent', with the
 *  name 'name'.
 */
PythonEditView::PythonEditView( QWidget* parent, const char* name)
    : MDIView(0,parent, name, WDestructiveClose)
{
  textEdit = new PythonEditor(this);
  textEdit->setWordWrap( QTextEdit::NoWrap );

  setFocusProxy( textEdit );
  setCentralWidget( textEdit );
}

/** Destroys the object and frees any allocated resources */
PythonEditView::~PythonEditView()
{
  delete textEdit;
}

/**
 * Runs the action specified by \a pMsg.
 */
bool PythonEditView::onMsg(const char* pMsg,const char** ppReturn)
{
  if (strcmp(pMsg,"Save")==0){
    save();
    return true;
  }
  if (strcmp(pMsg,"SaveAs")==0){
    saveAs();
    return true;
  }
  if (strcmp(pMsg,"Cut")==0){
    cut();
    return true;
  }
  if (strcmp(pMsg,"Copy")==0){
    copy();
    return true;
  }
  if (strcmp(pMsg,"Paste")==0){
    paste();
    return true;
  }
  if (strcmp(pMsg,"Undo")==0){
    undo();
    return true;
  }
  if (strcmp(pMsg,"Redo")==0){
    redo();
    return true;
  }

  return false;
}

/**
 * Checks if the action \a pMsg is available. This is for enabling/disabling
 * the corresponding buttons or menu items for this action.
 */
bool PythonEditView::onHasMsg(const char* pMsg)
{
  if (strcmp(pMsg,"Save")==0)  return true;
  if (strcmp(pMsg,"SaveAs")==0)  return true;
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

/** Checking on close state. */
bool PythonEditView::canClose(void)
{
  if ( !textEdit->isModified() )
    return true;

  switch( QMessageBox::warning( this, tr("Unsaved document"),
                                tr("Save changes to %1?").arg( caption() ),
                                tr("Yes"), tr("No"), tr("Cancel"),0,2))
  {
  case 0:
    return save();
  case 1:
    return true;
  case 2:
    return false;
  default:
    return false;
  }
}

/**
 * Saves the changes of the editor to a file. If the input has not been saved yet before
 * a file dialog appears.
 */
bool PythonEditView::save()
{
  if ( !textEdit->isModified() )
    return true;

  // check if saved ever before
  if ( isSavedOnce() )
  {
    saveFile();
    return true;
  }
  else
  {
    return saveAs();
  }
}

/**
 * Saves the content of the editor to a file specified by the appearing file dialog.
 */
bool PythonEditView::saveAs(void)
{
  QString fn = FileDialog::getSaveFileName(QString::null, "FreeCAD macro (*.FCMacro);;Python (*.py)", 
                                           this, QObject::tr("Save Macro"));
  if (!fn.isEmpty())
  {
    _fileName = fn;
    saveFile();
    return true;
  }
  else
  {
    emit message( tr("Saving aborted"), 2000 );
    return false;
  }
}

/**
 * Opens a file specified by the appearing file dialog.
 */
bool PythonEditView::open(void)
{
  QString file = QFileDialog::getOpenFileName(QString::null, "Macro files (*.py *.FCMacro);;Python (*.py);;FreeCAD macro (*.FCMacro)", this);
  if ( file.isEmpty() )
    return false;

  openFile(file);
  setCaption(file);

  return true;
}

/**
 * Opens the file \a fileName.
 */
void PythonEditView::openFile (const QString& fileName)
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

  emit message( tr("Loaded document %1").arg( fileName ), 2000 );
}

/**
 * Copies the selected text to the clipboard and deletes it from the text edit.
 * If there is no selected text nothing happens.
 */
void PythonEditView::cut(void)
{
  textEdit->cut();
}

/**
 * Copies any selected text to the clipboard.
 */
void PythonEditView::copy(void)
{
  textEdit->copy();
}

/**
 * Pastes the text from the clipboard into the text edit at the current cursor position. 
 * If there is no text in the clipboard nothing happens.
 */
void PythonEditView::paste(void)
{
  textEdit->paste();
}

/**
 * Undoes the last operation.
 * If there is no operation to undo, i.e. there is no undo step in the undo/redo history, nothing happens.
 */
void PythonEditView::undo(void)
{
  textEdit->undo();
}

/**
 * Redoes the last operation.
 * If there is no operation to undo, i.e. there is no undo step in the undo/redo history, nothing happens.
 */
void PythonEditView::redo(void)
{
  textEdit->redo();
}

/**
 * \todo: Shows the printer dialog.
 */
void PythonEditView::print( QPrinter* printer )
{
#ifndef QT_NO_PRINTER
  int pageNo = 1;

  if ( printer->setup(this) ) 
  {
    // printer dialog
    printer->setFullPage( TRUE );
    emit message( tr("Printing..."), 0 );

    QPainter p;
    if ( !p.begin( printer ) )
      return; // paint on printer

    QPaintDeviceMetrics metrics( p.device() );
    int dpiy = metrics.logicalDpiY();
    int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
    QRect body( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
    QSimpleRichText richText( QStyleSheet::convertFromPlainText(textEdit->text()),
          QFont(),
          textEdit->context(),
          textEdit->styleSheet(),
          textEdit->mimeSourceFactory(),
          body.height() );
    richText.setWidth( &p, body.width() );
    QRect view( body );

    int page = 1;
    do 
    {
      richText.draw( &p, body.left(), body.top(), view, colorGroup() );
      view.moveBy( 0, body.height() );
      p.translate( 0 , -body.height() );
      p.drawText( view.right() - p.fontMetrics().width( QString::number( page ) ),
      view.bottom() + p.fontMetrics().ascent() + 5, QString::number( page ) );
      if ( view.top()  >= richText.height() )
        break;
      QString msg( "Printing (page " );
      msg += QString::number( ++pageNo );
      msg += ")...";
      emit message( msg, 0 );
      printer->newPage();
      page++;
    } while (true);

    p.end();
  }
#endif
}

/**
 * Saves the contents to a file.
 */
void PythonEditView::saveFile()
{
  QFile file(_fileName);
  if( !file.open(IO_WriteOnly))
    return;

  QTextStream out(&file);
  out << textEdit->text();
  file.close();

  textEdit->setModified(false);

  emit message( tr( "File %1 saved" ).arg( _fileName ), 2000 );

  return;
}

/**
 * Checks if the contents has been saved once to a file.
 */
bool PythonEditView::isSavedOnce()
{
  return (!_fileName.isEmpty());
}

/**
 * \Todo: Get the undo history.
 */
QStringList PythonEditView::undoActions() const
{
  QStringList lst;
  return lst;
}

/**
 * \Todo: Get the redo history.
 */
QStringList PythonEditView::redoActions() const
{
  QStringList lst;
  return lst;
}


#include "moc_PythonEditor.cpp"
