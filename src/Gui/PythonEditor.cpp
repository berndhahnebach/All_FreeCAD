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
        colormap["Line"] = QColor(224,224,224);
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
    setFont(serifFont);

    ParameterGrp::handle hPrefGrp = getWindowParameter();
    // set default to 4 characters
    hPrefGrp->SetInt( "TabSize", 4 );
    hPrefGrp->Attach( this );

    // set colors and font
    hPrefGrp->NotifyAll();

    // set acelerators
    QShortcut* comment = new QShortcut(this);
    comment->setKey(Qt::ALT + Qt::Key_C);

    QShortcut* uncomment = new QShortcut(this);
    uncomment->setKey(Qt::ALT + Qt::Key_U);

    connect(comment, SIGNAL(activated()), 
            this, SLOT(onComment()));
    connect(uncomment, SIGNAL(activated()), 
            this, SLOT(onUncomment()));
    connect(this, SIGNAL(cursorPositionChanged()), 
            this, SLOT(onCursorPositionChanged()));
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
    if (strcmp(sReason, "FontSize") == 0 || strcmp(sReason, "Font") == 0) {
#ifdef FC_OS_LINUX
        int fontSize = hPrefGrp->GetInt("FontSize", 15);
#else
        int fontSize = hPrefGrp->GetInt("FontSize", 10);
#endif
        QString fontFamily = hPrefGrp->GetASCII( "Font", "Courier" ).c_str();
        
        QFont font(fontFamily, fontSize);
        setFont(font);
    } else {
        QMap<QString, QColor>::ConstIterator it = d->colormap.find(sReason);
        if (it != d->colormap.end()) {
            QColor color = it.data();
            unsigned long col = (color.red() << 24) | (color.green() << 16) | (color.blue() << 8);
            col = hPrefGrp->GetUnsigned( sReason, col);
            color.setRgb((col>>24)&0xff, (col>>16)&0xff, (col>>8)&0xff);
            pythonSyntax->setColor( sReason, color );
        }
    }

    if (strcmp(sReason, "TabSize") == 0 || strcmp(sReason, "FontSize") == 0) {
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

void PythonEditor::onCursorPositionChanged()
{
    const QColor& color = d->colormap["Line"];
    if ( color.isValid() )
        viewport()->update();
}

void PythonEditor::paintEvent ( QPaintEvent * e )
{
    const QColor& color = d->colormap["Line"];
    if ( color.isValid() )
    {
        QPainter painter( viewport() );
        QRect r = cursorRect();
        r.setX( 0 );
        r.setWidth( viewport()->width() );
        painter.fillRect( r, QBrush( color ) );
        painter.end();
    }

    TextEdit::paintEvent( e );
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
#if QT_VERSION >= 0x040200
    rehighlight();
#else
    document()->setPlainText(document()->toPlainText());
    document()->setModified(false);
#endif
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

#include "moc_PythonEditor.cpp"
