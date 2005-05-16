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
# include <qhbox.h> 
# include <qmessagebox.h>
# include <qpaintdevicemetrics.h>
# include <qpainter.h>
# include <qprinter.h>
# include <qregexp.h>
# include <qsimplerichtext.h>
# include <private/qrichtext_p.h>
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
 *  Constructs a PythonEditor which is a child of 'parent', with the
 *  name 'name'. 
 */
PythonEditor::PythonEditor(QWidget *parent,const char *name)
    : TextEdit(parent, name), WindowParameter( "Editor" )
{
  pythonSyntax = new PythonSyntaxHighlighter(this);

  FCParameterGrp::handle hPrefGrp = getWindowParameter();
  hPrefGrp->Attach( this );

  // set colors and font
  hPrefGrp->NotifyAll();
}

/** Destroys the object and frees any allocated resources */
PythonEditor::~PythonEditor()
{
  getWindowParameter()->Detach( this );
  delete pythonSyntax;
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
    long col = hPrefGrp->GetInt( sReason, GetDefCol().color( sReason ));
    QColor color;
    color.setRgb(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);

    pythonSyntax->setColor( sReason, color );
  }
}

// ------------------------------------------------------------------------

namespace Gui {
class PythonSyntaxHighlighterP
{
public:
  PythonSyntaxHighlighterP()
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
  colorChanged( type, col );
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

void PythonSyntaxHighlighter::colorChanged( const QString& type, const QColor& col )
{
  rehighlight();
}

/**
 * Detects all kinds of text to highlight them in the correct color.
 */
int PythonSyntaxHighlighter::highlightParagraph ( const QString & text, int endStateOfLastPara )
{
  uint i = 0;
  QChar prev, ch;
  QString buffer;

  const int Standard      = 0;     // Standard text
  const int Digit         = 1;     // Digits
  const int Comment       = 2;     // Comment begins with #
  const int Literal1      = 3;     // String literal beginning with "
  const int Literal2      = 4;     // Other string literal beginning with '
  const int Blockcomment1 = 5;     // Block comments beginning and ending with """
  const int Blockcomment2 = 6;     // Other block comments beginning and ending with '''
  const int ClassName     = 7;     // Text after the keyword class
  const int DefineName    = 8;     // Text after the keyword def

  if (endStateOfLastPara==-2) 
    endStateOfLastPara=Standard;

  while ( i < text.length() )
  {
    ch = text.at( i ).latin1();

    switch ( endStateOfLastPara )
    {
    case Standard:
      {
        switch ( ch )
        {
        case '#':
          {
            // begin a comment
            setFormat( i, 1, d->cComment);
            endStateOfLastPara=Comment;
          } break;
        case '"':
          {
            // Begin either string literal or block comment
            if ( text.at(i-1) == '"' && text.at(i-2) == '"')
            {
              setFormat( i-2, 3, d->cBlockcomment);
              endStateOfLastPara=Blockcomment1;
            }
            else
            {
              setFormat( i, 1, d->cLiteral);
              endStateOfLastPara=Literal1;
            }
          } break;
        case '\'':
          {
            // Begin either string literal or block comment
            if ( text.at(i-1) == '\'' && text.at(i-2) == '\'')
            {
              setFormat( i-2, 3, d->cBlockcomment);
              endStateOfLastPara=Blockcomment2;
            }
            else
            {
              setFormat( i, 1, d->cLiteral);
              endStateOfLastPara=Literal2;
            }
          } break;
        case ' ':
        case '\t':
          {
            // ignore whitespaces
            buffer = QString::null;
          } break;
        default:
          {
            // this is the beginning of a number
            if ( ch.isDigit() && !prev.isLetter() && buffer.isEmpty() )
            {
              setFormat( i, 1, d->cNumber);
              endStateOfLastPara=Digit;
            }
            // probably an operator
            else if ( ch.isSymbol() )
            {
              setFormat( i, 1, d->cOperator);
              buffer = QString::null;
            }
            // normal text
            else
            {
              if ( !ch.isLetterOrNumber() )
                buffer = QString::null;
              buffer += ch;

              // search for keywords
              if ( buffer.length() > 0 )
              {
                bool keywordFound=false;
                for ( QStringList::Iterator it = d->keywords.begin(); it != d->keywords.end(); ++it ) 
                {
                  if ( buffer == *it)
                  {
                    if ( buffer == "def")
                      endStateOfLastPara = DefineName;
                    else if ( buffer == "class")
                      endStateOfLastPara = ClassName;

                    QFont font = textEdit()->currentFont();
                    font.setBold( true );
                    setFormat( i - buffer.length()+1, buffer.length(),font,d->cKeyword);
                    keywordFound = true;
                    break;
                  }
                }

                // no keywords matches
                if (!keywordFound)
                {
                  setFormat( i - buffer.length()+1, buffer.length(),d->cNormalText);
                }
              }
            }
          }
        }
      }break;
    case Comment:
      {
        setFormat( i, 1, d->cComment);
        buffer = QString::null;
      } break;
    case Literal1:
      {
        setFormat( i, 1, d->cLiteral);
        buffer = QString::null;
        if ( ch == '"' )
          endStateOfLastPara = Standard;
      } break;
    case Literal2:
      {
        setFormat( i, 1, d->cLiteral);
        buffer = QString::null;
        if ( ch == '\'' )
          endStateOfLastPara = Standard;
      } break;
    case Blockcomment1:
      {
        setFormat( i, 1, d->cBlockcomment);
        buffer = QString::null;
        if ( ch == '"' && text.at(i-1) == '"' && text.at(i-2) == '"')
          endStateOfLastPara = Standard;
      } break;
    case Blockcomment2:
      {
        setFormat( i, 1, d->cBlockcomment);
        buffer = QString::null;
        if ( ch == '\'' && text.at(i-1) == '\'' && text.at(i-2) == '\'')
          endStateOfLastPara = Standard;
      } break;
    case DefineName:
      {
        if ( ch.isLetter() || ch == ' ' )
        {
          setFormat( i, 1, d->cDefineName);
        }
        else
        {
          endStateOfLastPara = Standard;
        }
        buffer = QString::null;
      } break;
    case ClassName:
      {
        if ( ch.isLetter() || ch == ' ' )
        {
          setFormat( i, 1, d->cClassName);
        }
        else
        {
          endStateOfLastPara = Standard;
        }
        buffer = QString::null;
      } break;
    case Digit:
      {
        if ( ch.isDigit() || ch == '.' )
        {
          setFormat( i, 1, d->cNumber);
        }
        else
        {
          endStateOfLastPara = Standard;
        }
        buffer = QString::null;
      }break;
    }

    prev = ch;
    i++;
  }

  // only block comments can have several lines
  if ( endStateOfLastPara != Blockcomment1 && endStateOfLastPara != Blockcomment1 ) 
  {
    endStateOfLastPara = Standard ;
  } 

  return endStateOfLastPara;
}

// ------------------------------------------------------------------------

/* TRANSLATOR Gui::PythonEditView */

/**
 *  Constructs a PythonEditView which is a child of 'parent', with the
 *  name 'name'.
 */
PythonEditView::PythonEditView( QWidget* parent, const char* name)
    : MDIView(0,parent, name, WDestructiveClose), WindowParameter( "Editor" )
{
  QHBox* hbox = new QHBox( this );

  // create the editor first
  _textEdit = new PythonEditor(this);
  _lineMarker = new LineMarker( reinterpret_cast<PythonEditor*>(_textEdit), hbox,"LineMarker");
  // and reparent it 
  _textEdit->reparent(hbox, QPoint());
  _lineMarker->show();
  _textEdit->setWordWrap( QTextEdit::NoWrap );


  setFocusProxy( _textEdit );
  setCentralWidget( hbox );

  FCParameterGrp::handle hPrefGrp = getWindowParameter();
  hPrefGrp->Attach( this );
  hPrefGrp->NotifyAll();
}

/** Destroys the object and frees any allocated resources */
PythonEditView::~PythonEditView()
{
  getWindowParameter()->Detach( this );
  delete _textEdit;
  delete _lineMarker;
}

void PythonEditView::OnChange( FCSubject<const char*> &rCaller,const char* rcReason )
{
  FCParameterGrp::handle hPrefGrp = getWindowParameter();
  if (strcmp(rcReason, "EnableLineNumber") == 0)
  {
    bool show = hPrefGrp->GetBool( "EnableLineNumber", true );
    if ( show )
      _lineMarker->show();
    else
      _lineMarker->hide();
  }
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
    bool canWrite = !_textEdit->isReadOnly();
    return (canWrite && (_textEdit->hasSelectedText()));
  }
  if (strcmp(pMsg,"Copy")==0)
  {
    return ( _textEdit->hasSelectedText() );
  }
  if (strcmp(pMsg,"Paste")==0)
  {
    QClipboard *cb = QApplication::clipboard();
    QString text;

    // Copy text from the clipboard (paste)
    text = cb->text();

    bool canWrite = !_textEdit->isReadOnly();
    return ( !text.isEmpty() && canWrite );
  }
  if (strcmp(pMsg,"Undo")==0)
  {
    return _textEdit->isUndoAvailable ();
  }
  if (strcmp(pMsg,"Redo")==0)
  {
    return _textEdit->isRedoAvailable ();
  }
  return false;
}

/** Checking on close state. */
bool PythonEditView::canClose(void)
{
  if ( !_textEdit->isModified() )
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
  if ( !_textEdit->isModified() )
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
  _textEdit->setText( in.read() );
  _lineMarker->onRepaint();

  file.close();

  _textEdit->setModified(false);

  emit message( tr("Loaded document %1").arg( fileName ), 2000 );
}

/**
 * Copies the selected text to the clipboard and deletes it from the text edit.
 * If there is no selected text nothing happens.
 */
void PythonEditView::cut(void)
{
  _textEdit->cut();
}

/**
 * Copies any selected text to the clipboard.
 */
void PythonEditView::copy(void)
{
  _textEdit->copy();
}

/**
 * Pastes the text from the clipboard into the text edit at the current cursor position. 
 * If there is no text in the clipboard nothing happens.
 */
void PythonEditView::paste(void)
{
  _textEdit->paste();
}

/**
 * Undoes the last operation.
 * If there is no operation to undo, i.e. there is no undo step in the undo/redo history, nothing happens.
 */
void PythonEditView::undo(void)
{
  _textEdit->undo();
}

/**
 * Redoes the last operation.
 * If there is no operation to undo, i.e. there is no undo step in the undo/redo history, nothing happens.
 */
void PythonEditView::redo(void)
{
  _textEdit->redo();
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
    QSimpleRichText richText( QStyleSheet::convertFromPlainText(_textEdit->text()),
          QFont(),
          _textEdit->context(),
          _textEdit->styleSheet(),
          _textEdit->mimeSourceFactory(),
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
  out << _textEdit->text();
  file.close();

  _textEdit->setModified(false);

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


LineMarker::LineMarker( TextEdit* textEdit, QWidget* parent, const char* name )
	: QWidget( parent, name, WRepaintNoErase | WStaticContents | WResizeNoErase ),
	  _textEdit( textEdit )
{
	connect( _textEdit->verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( onRepaint() ) );
	connect( _textEdit, SIGNAL( textChanged() ), this, SLOT( onRepaint() ) );
  onRepaint();

  QFontMetrics fm( _font );
  setFixedWidth( fm.width( "0000" ) + 10 );
}

LineMarker::~LineMarker()
{
}

void LineMarker::setFont( QFont f )
{
  _font = f;
}  

void LineMarker::resizeEvent( QResizeEvent *e )
{
	_buffer.resize( e->size() );
	QWidget::resizeEvent( e );
}

void LineMarker::paintEvent( QPaintEvent* /*e*/ )
{
	_buffer.fill();

	QTextParagraph *p = _textEdit->document()->firstParagraph();

  QPainter painter( &_buffer );
	int y = _textEdit->contentsY();
	while ( p ) 
  {
		if ( !p->isVisible() ) 
    {
			p = p->next();
			continue;
		}

		if ( p->rect().y() + p->rect().height() - y < 0 ) 
    {
			p = p->next();
			continue;
		}

		if ( p->rect().y() - y > height() )
			break;
    painter.setFont( _font );
		painter.drawText( 0, p->rect().y() - y,
				  _buffer.width() - 10, p->rect().height(),
				  AlignRight | AlignTop,
				  QString::number(p->paragId()+1) );
    if (p->paragId()==5)
    {
      bool b=p->isListItem();
      p->setListItem(true);
    }
/*      painter.save();
      painter.setBrush(QColor(0,255,255));
      painter.drawRoundRect(5,p->rect().y()-y, p->rect().height()+8, p->rect().height());
      painter.setBrush(Qt::red);
      painter.drawEllipse(8,p->rect().y()-y, p->rect().height(), p->rect().height());
      painter.restore();
    }
    else if (p->paragId()==6)
    {
      int w=p->rect().height();
      int b=p->rect().y()-y;
      painter.drawRect(5,b, w-1, w-1);
      painter.save();
      painter.setPen(Qt::blue);
      painter.moveTo(QPoint(8,b+w/2-1));
      painter.lineTo(QPoint(w,b+w/2-1));
      painter.moveTo(QPoint(4+w/2,b+3));
      painter.lineTo(QPoint(4+w/2,b+w-5));
      painter.restore();
    }
    else if (p->paragId()>=8&&p->paragId()<12)
    {
      p->hide();
    }*/
		p = p->next();
	}

	painter.end();
	bitBlt( this, 0, 0, &_buffer );
}


#include "moc_PythonEditor.cpp"
