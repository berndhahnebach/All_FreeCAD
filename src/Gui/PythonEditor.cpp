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

#include "PythonEditor.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "FileDialog.h"
#include "DlgEditorImp.h"
#include "Macro.h"

#include <Base/Interpreter.h>
#include <Base/Exception.h>
#include <Base/Parameter.h>

using namespace Gui;

namespace Gui {
struct PythonEditorP
{
  QMap<QString, QColor> colormap; // Color map
  PythonEditorP()
  {
    colormap["Text"] = Qt::black;
    colormap["Bookmark"] = Qt::cyan;
    colormap["Breakpoint"] = Qt::red;
    colormap["Keyword"] = Qt::blue;
    colormap["Comment"] = QColor(0, 170, 0);
    colormap["Block comment"] = QColor(160, 160, 164);
    colormap["Number"] = Qt::blue;
    colormap["String"] = Qt::red;
    colormap["Character"] = Qt::red;
    colormap["Class name"] = QColor(255, 170, 0);
    colormap["Define name"] = QColor(255, 170, 0);
    colormap["Operator"] = QColor(160, 160, 164);
    colormap["Python output"] = QColor(170, 170, 127);
    colormap["Python error"] = Qt::red;
  }
};
} // namespace Gui

/* TRANSLATOR Gui::PythonEditor */

/**
 *  Constructs a PythonEditor which is a child of 'parent' and does the
 *  syntax highlighting for the Python language. 
 */
PythonEditor::PythonEditor(QWidget* parent)
  : TextEdit(parent), WindowParameter( "Editor" )
{
  d = new PythonEditorP();
  pythonSyntax = new PythonSyntaxHighlighter(this);

#ifdef FC_OS_LINUX
    QFont serifFont( "Courier", 15, QFont::Normal );
#else
    QFont serifFont( "Courier", 10, QFont::Normal );
#endif
    setCurrentFont(serifFont);

  ParameterGrp::handle hPrefGrp = getWindowParameter();
  // set default to 4 characters
  hPrefGrp->SetInt( "TabSize", 4 );
  hPrefGrp->Attach( this );

  // set colors and font
  hPrefGrp->NotifyAll();

  // set acelerators
  QShortcut* comment = new QShortcut(this);
  comment->setKey(Qt::ALT + Qt::Key_C);
  connect(comment, SIGNAL(activated()), this, SLOT(onComment()));

  QShortcut* uncomment = new QShortcut(this);
  uncomment->setKey(Qt::ALT + Qt::Key_U);
  connect(uncomment, SIGNAL(activated()), this, SLOT(onUncomment()));
}

/** Destroys the object and frees any allocated resources */
PythonEditor::~PythonEditor()
{
  getWindowParameter()->Detach( this );
  delete pythonSyntax;
  delete d;
}

void PythonEditor::keyPressEvent ( QKeyEvent * e )
{
  if ( e->key() == Qt::Key_Tab )
  {
    ParameterGrp::handle hPrefGrp = getWindowParameter();
    int indent = hPrefGrp->GetInt( "IndentSize", 4 );
    bool space = hPrefGrp->GetBool( "Spaces", false );

    if ( space == true )
    {
      QString str(indent, ' ');
      QTextCursor cursor = textCursor();
      cursor.beginEditBlock();
      cursor.insertText(str);
      cursor.endEditBlock();
      return;
    }
  }

  TextEdit::keyPressEvent( e );
}

/** Sets the font, font size and tab size of the editor. */  
void PythonEditor::OnChange( Base::Subject<const char*> &rCaller,const char* sReason )
{
  ParameterGrp::handle hPrefGrp = getWindowParameter();
  if (strcmp(sReason, "FontSize") == 0)
  {
    bool ok;
#ifdef FC_OS_LINUX
    QString fontSize = hPrefGrp->GetASCII( "FontSize", "15" ).c_str();
    int size = fontSize.toInt(&ok);
    if ( !ok ) size = 15; 
#else
    QString fontSize = hPrefGrp->GetASCII( "FontSize", "10" ).c_str();
    int size = fontSize.toInt(&ok);
    if ( !ok ) size = 10; 
#endif

    // Create the new text format applying the given size
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();
    QFont font = format.font();
    font.setPointSize(size);
    format.setFont(font);
    // select the whole document and apply the text format
    cursor.setPosition(0);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
    cursor.clearSelection();
  }
  else if (strcmp(sReason, "Font") == 0)
  {
    QString fontFamily = hPrefGrp->GetASCII( "Font", "Courier" ).c_str();
    // select the whole document to apply the new format (that only works on selection)
    QTextCursor cursor = textCursor();
    QTextCharFormat format = cursor.charFormat();
    format.setFontFamily(fontFamily);
    cursor.setPosition(0);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(format);
    cursor.clearSelection();
  }
  else
  {
    QMap<QString, QColor>::ConstIterator it = d->colormap.find(sReason);
    if (it != d->colormap.end()) {
      QColor color = it.data();
      unsigned long col = (color.red() << 24) | (color.green() << 16) | (color.blue() << 8);
      col = hPrefGrp->GetUnsigned( sReason, col);
      color.setRgb((col>>24)&0xff, (col>>16)&0xff, (col>>8)&0xff);
      pythonSyntax->setColor( sReason, color );
    }
  }
  if (strcmp(sReason, "TabSize") == 0 || strcmp(sReason, "FontSize") == 0)
  {
    int tabWidth  = hPrefGrp->GetInt( "TabSize", 4 );
    QString str;
    for (int i=0; i<tabWidth;i++) str += '0';
    QFontMetrics fm( currentFont() );
    tabWidth = fm.width( str );
    setTabStopWidth( tabWidth );
  }
}

void PythonEditor::contextMenuEvent ( QContextMenuEvent * e )
{
  QMenu* menu = createStandardContextMenu();
  menu->addSeparator();
  menu->addAction( tr("Comment"), this, SLOT( onComment() ), Qt::ALT + Qt::Key_C );
  menu->addAction( tr("Uncomment"), this, SLOT( onUncomment() ), Qt::ALT + Qt::Key_U );

  menu->exec(e->globalPos());
  delete menu;
}

void PythonEditor::onComment()
{
    QTextCursor cursor = textCursor();
    int selStart = cursor.selectionStart();
    int selEnd = cursor.selectionEnd();
    QTextBlock block;
    cursor.beginEditBlock();
    for (block = document()->begin(); block.isValid(); block = block.next()) {
        int pos = block.position();
        int off = block.length()-1;
        // at least one char of the block is part of the selection
        if ( pos >= selStart || pos+off >= selStart) {
            if ( pos+1 > selEnd )
                break; // end of selection reached
            cursor.setPosition(block.position());
            cursor.insertText("#");
                selEnd++;
        }
    }

    cursor.endEditBlock();
}

void PythonEditor::onUncomment()
{
    QTextCursor cursor = textCursor();
    int selStart = cursor.selectionStart();
    int selEnd = cursor.selectionEnd();
    QTextBlock block;
    cursor.beginEditBlock();
    for (block = document()->begin(); block.isValid(); block = block.next()) {
        int pos = block.position();
        int off = block.length()-1;
        // at least one char of the block is part of the selection
        if ( pos >= selStart || pos+off >= selStart) {
            if ( pos+1 > selEnd )
                break; // end of selection reached
            if (block.text().startsWith("#")) {
                cursor.setPosition(block.position());
                cursor.deleteChar();
                selEnd--;
            }
        }
    }

    cursor.endEditBlock();
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

    cNormalText.setRgb(0, 0, 0); cComment.setRgb(0, 170, 0);
    cBlockcomment.setRgb(160, 160, 164); cLiteral.setRgb(255, 0, 0);
    cNumber.setRgb(0, 0, 255); cOperator.setRgb(160, 160, 164);
    cKeyword.setRgb(0, 0, 255); cClassName.setRgb(255, 170, 0);
    cDefineName.setRgb(255, 170, 0); cOutput.setRgb(170, 170, 127); 
    cError.setRgb(255, 0, 0);
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
  // rehighlight
  document()->setPlainText(document()->toPlainText());
}

int PythonSyntaxHighlighter::maximumUserState() const
{
    return 8;
}

/**
 * Detects all kinds of text to highlight them in the correct color.
 */
void PythonSyntaxHighlighter::highlightBlock (const QString & text)
{
  int i = 0;
  QChar prev, ch;

  const int Standard      = 0;     // Standard text
  const int Digit         = 1;     // Digits
  const int Comment       = 2;     // Comment begins with #
  const int Literal1      = 3;     // String literal beginning with "
  const int Literal2      = 4;     // Other string literal beginning with '
  const int Blockcomment1 = 5;     // Block comments beginning and ending with """
  const int Blockcomment2 = 6;     // Other block comments beginning and ending with '''
  const int ClassName     = 7;     // Text after the keyword class
  const int DefineName    = 8;     // Text after the keyword def

  int endStateOfLastPara = previousBlockState();
  if (endStateOfLastPara < 0 || endStateOfLastPara > maximumUserState()) 
    endStateOfLastPara = Standard;

  while ( i < text.length() )
  {
    ch = text.at( i );

    switch ( endStateOfLastPara )
    {
    case Standard:
      {
        switch ( ch.unicode() )
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
            if ((i>=2) && text.at(i-1) == '"' && text.at(i-2) == '"')
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
            if ((i>=2) && text.at(i-1) == '\'' && text.at(i-2) == '\'')
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
          } break;
        case '(': case ')': case '[': case ']': 
        case '+': case '-': case '*': case '/': 
        case ':': case '°': case '^': case '~': 
        case '!': case '=': case '<': case '>': // possibly two characters
          {
            setFormat( i, 1, d->cOperator );
            endStateOfLastPara=Standard;
          } break;
        default:
          {
            // Check for normal text
            if ( ch.isLetter() || ch == '_' )
            {
              QString buffer;
              int j=i;
              while ( ch.isLetterOrNumber() || ch == '_' ) {
                buffer += ch;
                ++j;
                if (j >= text.length())
                  break; // end of text
                ch = text.at(j);
              }

              if ( d->keywords.contains( buffer ) != 0 ) {
                if ( buffer == "def")
                  endStateOfLastPara = DefineName;
                else if ( buffer == "class")
                  endStateOfLastPara = ClassName;

                QTextCharFormat keywordFormat;
                keywordFormat.setForeground(d->cKeyword);
                keywordFormat.setFontWeight(QFont::Bold);
                setFormat( i, buffer.length(), keywordFormat);
              }
              else {
                setFormat( i, buffer.length(),d->cNormalText);
              }

              // increment i
              if ( !buffer.isEmpty() )
                i = j-1;
            }
            // this is the beginning of a number
            else if ( ch.isDigit() )
            {
              setFormat( i, 1, d->cNumber);
              endStateOfLastPara=Digit;
            }
            // probably an operator
            else if ( ch.isSymbol() || ch.isPunct() )
            {
              setFormat( i, 1, d->cOperator);
            }
          }
        }
      } break;
    case Comment:
      {
        setFormat( i, 1, d->cComment);
      } break;
    case Literal1:
      {
        setFormat( i, 1, d->cLiteral);
        if ( ch == '"' )
          endStateOfLastPara = Standard;
      } break;
    case Literal2:
      {
        setFormat( i, 1, d->cLiteral);
        if ( ch == '\'' )
          endStateOfLastPara = Standard;
      } break;
    case Blockcomment1:
      {
        setFormat( i, 1, d->cBlockcomment);
        if ( i>=2 && ch == '"' && text.at(i-1) == '"' && text.at(i-2) == '"')
          endStateOfLastPara = Standard;
      } break;
    case Blockcomment2:
      {
        setFormat( i, 1, d->cBlockcomment);
        if ( i>=2 && ch == '\'' && text.at(i-1) == '\'' && text.at(i-2) == '\'')
          endStateOfLastPara = Standard;
      } break;
    case DefineName:
      {
        if ( ch.isLetterOrNumber() || ch == ' ' )
        {
          setFormat( i, 1, d->cDefineName);
        }
        else
        {
          if ( ch.isSymbol() || ch.isPunct() )
            setFormat( i, 1, d->cOperator );
          endStateOfLastPara = Standard;
        }
      } break;
    case ClassName:
      {
        if ( ch.isLetterOrNumber() || ch == ' ' )
        {
          setFormat( i, 1, d->cClassName);
        }
        else
        {
          if ( ch.isSymbol() || ch.isPunct() )
            setFormat( i, 1, d->cOperator );
          endStateOfLastPara = Standard;
        }
      } break;
    case Digit:
      {
        if ( ch.isDigit() || ch == '.' )
        {
          setFormat( i, 1, d->cNumber);
        }
        else
        {
          if ( ch.isSymbol() || ch.isPunct() )
            setFormat( i, 1, d->cOperator );
          endStateOfLastPara = Standard;
        }
      }break;
    }

    prev = ch;
    i++;
  }

  // only block comments can have several lines
  if ( endStateOfLastPara != Blockcomment1 && endStateOfLastPara != Blockcomment2 ) 
  {
    endStateOfLastPara = Standard ;
  } 

  setCurrentBlockState(endStateOfLastPara);
}

// ------------------------------------------------------------------------

/* TRANSLATOR Gui::PythonEditView */

/**
 *  Constructs a PythonEditView which is a child of 'parent', with the
 *  name 'name'.
 */
PythonEditView::PythonEditView( const QString& file, QWidget* parent)
    : MDIView(0,parent, 0, Qt::WDestructiveClose), WindowParameter( "Editor" )
{
  // create the editor first
  _textEdit = new PythonEditor();
  _textEdit->setLineWrapMode( QTextEdit::NoWrap );
  _lineMarker = new LineMarker();
  _lineMarker->setTextEdit(_textEdit);

  // Create the layout containing the workspace and a tab bar
  QFrame* hbox = new QFrame(this);
  hbox->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
  QHBoxLayout* layout = new QHBoxLayout();
  layout->setMargin(1);
  layout->addWidget(_lineMarker);
  layout->addWidget(_textEdit);
  hbox->setLayout(layout);
  setCentralWidget(hbox);

  setIcon( Gui::BitmapFactory().pixmap("python_small") );

  ParameterGrp::handle hPrefGrp = getWindowParameter();
  hPrefGrp->Attach( this );
  hPrefGrp->NotifyAll();

  _pcActivityTimer = new QTimer(this);
  connect( _pcActivityTimer, SIGNAL(timeout()),this, SLOT(checkTimestamp()) );
  connect( _textEdit, SIGNAL(textChanged()),this, SLOT(onModified()) );
  openFile( file );
}

/** Destroys the object and frees any allocated resources */
PythonEditView::~PythonEditView()
{
  _pcActivityTimer->stop();
  delete _pcActivityTimer;
  getWindowParameter()->Detach( this );
}

void PythonEditView::OnChange( Base::Subject<const char*> &rCaller,const char* rcReason )
{
  ParameterGrp::handle hPrefGrp = getWindowParameter();
  if (strcmp(rcReason, "EnableLineNumber") == 0)
  {
    bool show = hPrefGrp->GetBool( "EnableLineNumber", true );
    if ( show )
      _lineMarker->show();
    else
      _lineMarker->hide();
  }
}

void PythonEditView::checkTimestamp()
{
  QFileInfo fi(_fileName);
  uint timeStamp =  fi.lastModified().toTime_t();
  if ( timeStamp != _timeStamp )
  {
    switch( QMessageBox::question( this, tr("Modified file"), 
      tr("%1.\n\nThis has been modified outside of the source editor. Do you want to reload it?").arg( _fileName ),
      QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) )
    {
    case QMessageBox::Yes:
      // updates time stamp and timer
      openFile( _fileName );
      return;
    case QMessageBox::No:
      _timeStamp = timeStamp;
      break;
    }
  }

  _pcActivityTimer->start( 3000, true );
}

void PythonEditView::onModified()
{
    setWindowModified(_textEdit->document()->isModified());
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
  else if (strcmp(pMsg,"Run")==0){
    run();
    return true;
  }
  else if (strcmp(pMsg,"SaveAs")==0){
    saveAs();
    return true;
  }
  else if (strcmp(pMsg,"Cut")==0){
    cut();
    return true;
  }
  else if (strcmp(pMsg,"Copy")==0){
    copy();
    return true;
  }
  else if (strcmp(pMsg,"Paste")==0){
    paste();
    return true;
  }
  else if (strcmp(pMsg,"Undo")==0){
    undo();
    return true;
  }
  else if (strcmp(pMsg,"Redo")==0){
    redo();
    return true;
  }
  else if (strcmp(pMsg,"ViewFit")==0){
    // just ignore this
    return true;
  }

  return false;
}

/**
 * Checks if the action \a pMsg is available. This is for enabling/disabling
 * the corresponding buttons or menu items for this action.
 */
bool PythonEditView::onHasMsg(const char* pMsg) const
{
  if (strcmp(pMsg,"Save")==0)  return true;
  if (strcmp(pMsg,"Run")==0)  return true;
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
#ifndef FC_DEBUG
    setFocus();
#endif
  switch( QMessageBox::question( this, tr("Unsaved document"), tr("Save changes to %1?").arg( caption() ),
                                 QMessageBox::Yes|QMessageBox::Default, QMessageBox::No, 
                                 QMessageBox::Cancel|QMessageBox::Escape))
  {
  case QMessageBox::Yes:
    return save();
  case QMessageBox::No:
    return true;
  case QMessageBox::Cancel:
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
    message( tr("Saving aborted"), 2000 );
    return false;
  }
}

/**
 * Opens the file \a fileName.
 */
void PythonEditView::openFile (const QString& fileName)
{
  QString line;
  _fileName = fileName;
  QFile file(fileName);

  if( !file.open(QIODevice::ReadOnly))
    return;

  // file name with placeholder to mark as modified
  QString title = fileName + "[*]";
  setWindowTitle(title);

  QTextStream in(&file);
  _textEdit->setPlainText( in.read() );

  file.close();
  QFileInfo fi(_fileName);
  _timeStamp =  fi.lastModified().toTime_t();
  _pcActivityTimer->start( 3000, true );	

  setWindowModified(false);
  _textEdit->document()->setModified(false);

  message( tr("Loaded document %1").arg( fileName ), 2000 );
}

/**
 * Runs the opened script in the macro manager.
 */
void PythonEditView::run(void)
{
  Application::Instance->macroManager()->run(Gui::MacroManager::File,_fileName.latin1());
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
 * Shows the printer dialog.
 */
void PythonEditView::print()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        this->_textEdit->document()->print(&printer);
    }
}

/**
 * Prints the document into a Pdf file.
 */
void PythonEditView::printPdf()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Print Pdf file (*.pdf)"), QString(), tr("Pdf file"));
    if (!filename.isEmpty()) {
        QPrinter printer(QPrinter::HighResolution);
        printer.setFullPage(true);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setColorMode(QPrinter::Color);
        printer.setOutputFileName(filename);
        this->_textEdit->document()->print(&printer);
    }
}

/**
 * Saves the contents to a file.
 */
void PythonEditView::saveFile()
{
  QFile file(_fileName);
  if( !file.open(QIODevice::WriteOnly))
    return;

  QTextStream out(&file);
  out << _textEdit->text();
  file.close();

  _textEdit->setModified(false);
  QFileInfo fi(_fileName);
  _timeStamp =  fi.lastModified().toTime_t();

  setWindowTitle(_fileName + "[*]");

  message( tr( "File %1 saved" ).arg( _fileName ), 2000 );

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
    return QStringList();
}

/**
 * \Todo: Get the redo history.
 */
QStringList PythonEditView::redoActions() const
{
    return QStringList();
}

// ---------------------------------------------------------

LineMarker::LineMarker(QWidget* parent)
    : QWidget(parent)
{
    setFixedWidth( fontMetrics().width( QString("0000") ) );
}

LineMarker::~LineMarker()
{
}

void LineMarker::setTextEdit(QTextEdit *edit)
{
    this->edit = edit;
    connect( edit->document()->documentLayout(), SIGNAL( update(const QRectF &) ),
	     this, SLOT( update() ) );
    connect( edit->verticalScrollBar(), SIGNAL(valueChanged(int) ),
	     this, SLOT( update() ) );
}

void LineMarker::paintEvent( QPaintEvent* )
{
    QAbstractTextDocumentLayout *layout = edit->document()->documentLayout();
    int contentsY = edit->verticalScrollBar()->value();
    qreal pageBottom = contentsY + edit->viewport()->height();
    const QFontMetrics fm = fontMetrics();
    const int ascent = fontMetrics().ascent() + 1; // height = ascent + descent + 1
    int lineCount = 1;

    QPainter p(this);

    for ( QTextBlock block = edit->document()->begin();
	  block.isValid(); block = block.next(), ++lineCount ) {

        const QRectF boundingRect = layout->blockBoundingRect( block );

        QPointF position = boundingRect.topLeft();
        if ( position.y() + boundingRect.height() < contentsY )
            continue;
        if ( position.y() > pageBottom )
            break;

        const QString txt = QString::number( lineCount );
        p.drawText( width() - fm.width(txt), qRound( position.y() ) - contentsY + ascent, txt );
    }
}

#include "moc_PythonEditor.cpp"
