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
        colormap[QLatin1String("Text")] = Qt::black;
        colormap[QLatin1String("Bookmark")] = Qt::cyan;
        colormap[QLatin1String("Breakpoint")] = Qt::red;
        colormap[QLatin1String("Keyword")] = Qt::blue;
        colormap[QLatin1String("Comment")] = QColor(0, 170, 0);
        colormap[QLatin1String("Block comment")] = QColor(160, 160, 164);
        colormap[QLatin1String("Number")] = Qt::blue;
        colormap[QLatin1String("String")] = Qt::red;
        colormap[QLatin1String("Character")] = Qt::red;
        colormap[QLatin1String("Class name")] = QColor(255, 170, 0);
        colormap[QLatin1String("Define name")] = QColor(255, 170, 0);
        colormap[QLatin1String("Operator")] = QColor(160, 160, 164);
        colormap[QLatin1String("Python output")] = QColor(170, 170, 127);
        colormap[QLatin1String("Python error")] = Qt::red;
        colormap[QLatin1String("Line")] = QColor(224,224,224);
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
    QFont serifFont(QLatin1String("Courier"), 15, QFont::Normal );
#else
    QFont serifFont(QLatin1String("Courier"), 10, QFont::Normal );
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
    if ( e->key() == Qt::Key_Tab ) {
        ParameterGrp::handle hPrefGrp = getWindowParameter();
        int indent = hPrefGrp->GetInt( "IndentSize", 4 );
        bool space = hPrefGrp->GetBool( "Spaces", false );
        QString ch = space ? QString(indent, QLatin1Char(' '))
                           : QString::fromAscii("\t");

        QTextCursor cursor = textCursor();
        if (!cursor.hasSelection()) {
            // insert a single tab or several spaces
            cursor.beginEditBlock();
            cursor.insertText(ch);
            cursor.endEditBlock();
        } else {
            // for each selected block insert a tab or spaces
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
                    cursor.insertText(ch);
                        selEnd += ch.length();
                }
            }

            cursor.endEditBlock();
        }

        return;
    } else if (e->key() == Qt::Key_Backtab) {
        QTextCursor cursor = textCursor();
        if (!cursor.hasSelection())
            return; // Shift+Tab should not do anything
        // If some text is selected we remove a leading tab or
        // spaces from each selected block
        ParameterGrp::handle hPrefGrp = getWindowParameter();
        int indent = hPrefGrp->GetInt( "IndentSize", 4 );

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
                // if possible remove one tab or several spaces
                QString text = block.text();
                if (text.startsWith(QLatin1String("\t"))) {
                    cursor.setPosition(block.position());
                    cursor.deleteChar();
                    selEnd--;
                } else {
                    cursor.setPosition(block.position());
                    for (int i=0; i<indent; i++) {
                        if (!text.startsWith(QLatin1String(" ")))
                            break;
                        text = text.mid(1);
                        cursor.deleteChar();
                        selEnd--;
                    }
                }
            }
        }

        cursor.endEditBlock();
        return;
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
        QString fontFamily = QString::fromAscii(hPrefGrp->GetASCII( "Font", "Courier" ).c_str());
        
        QFont font(fontFamily, fontSize);
        setFont(font);
    } else {
        QMap<QString, QColor>::ConstIterator it = d->colormap.find(QString::fromAscii(sReason));
        if (it != d->colormap.end()) {
            QColor color = it.value();
            unsigned long col = (color.red() << 24) | (color.green() << 16) | (color.blue() << 8);
            col = hPrefGrp->GetUnsigned( sReason, col);
            color.setRgb((col>>24)&0xff, (col>>16)&0xff, (col>>8)&0xff);
            pythonSyntax->setColor(QLatin1String(sReason), color);
        }
    }

    if (strcmp(sReason, "TabSize") == 0 || strcmp(sReason, "FontSize") == 0) {
        int tabWidth = hPrefGrp->GetInt("TabSize", 4);
        QFontMetrics metric(font());
        int fontSize = metric.width(QLatin1String("0"));
        setTabStopWidth(tabWidth * fontSize);
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
    const QColor& color = d->colormap[QLatin1String("Line")];
    if ( color.isValid() )
        viewport()->update();
}

void PythonEditor::paintEvent ( QPaintEvent * e )
{
    const QColor& color = d->colormap[QLatin1String("Line")];
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
            cursor.insertText(QLatin1String("#"));
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
            if (block.text().startsWith(QLatin1String("#"))) {
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
    keywords << QLatin1String("and") << QLatin1String("assert")
             << QLatin1String("break") << QLatin1String("class")
             << QLatin1String("continue") << QLatin1String("def")
             << QLatin1String("del") << QLatin1String("elif")
             << QLatin1String("else") << QLatin1String("except")
             << QLatin1String("exec") << QLatin1String("finally")
             << QLatin1String("for") << QLatin1String("from")
             << QLatin1String("global") << QLatin1String("if")
             << QLatin1String("import") << QLatin1String("in")
             << QLatin1String("is") << QLatin1String("lambda")
             << QLatin1String("None") << QLatin1String("not")
             << QLatin1String("or") << QLatin1String("pass")
             << QLatin1String("print") << QLatin1String("raise")
             << QLatin1String("return") << QLatin1String("try")
             << QLatin1String("while") << QLatin1String("yield");

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
  if ( type == QLatin1String("Text") )
    d->cNormalText = col;
  else if ( type == QLatin1String("Comment") )
    d->cComment = col;
  else if ( type == QLatin1String("Block comment") )
    d->cBlockcomment = col;
  else if ( type == QLatin1String("Number") )
    d->cNumber = col;
  else if ( type == QLatin1String("String") )
    d->cLiteral = col;
  else if ( type == QLatin1String("Keyword") )
    d->cKeyword = col;
  else if ( type == QLatin1String("Class name") )
    d->cClassName = col;
  else if ( type == QLatin1String("Define name") )
    d->cDefineName = col;
  else if ( type == QLatin1String("Operator") )
    d->cOperator = col;
  else if ( type == QLatin1String("Python output") )
    d->cOutput = col;
  else if ( type == QLatin1String("Python error") )
    d->cError = col;
  colorChanged( type, col );
}

QColor PythonSyntaxHighlighter::color( const QString& type )
{
  if ( type == QLatin1String("Text") )
    return d->cNormalText;
  else if ( type == QLatin1String("Comment") )
    return d->cComment;
  else if ( type == QLatin1String("Block comment") )
    return d->cBlockcomment;
  else if ( type == QLatin1String("Number") )
    return d->cNumber;
  else if ( type == QLatin1String("String") )
    return d->cLiteral;
  else if ( type == QLatin1String("Keyword") )
    return d->cKeyword;
  else if ( type == QLatin1String("Class name") )
    return d->cClassName;
  else if ( type == QLatin1String("Define name") )
    return d->cDefineName;
  else if ( type == QLatin1String("Operator") )
    return d->cOperator;
  else if ( type == QLatin1String("Python output") )
    return d->cOutput;
  else if ( type == QLatin1String("Python error") )
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
            if ((i>=2) && text.at(i-1) == QLatin1Char('"') &&
                text.at(i-2) == QLatin1Char('"'))
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
            if ((i>=2) && text.at(i-1) == QLatin1Char('\'') && 
                text.at(i-2) == QLatin1Char('\''))
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
            if ( ch.isLetter() || ch == QLatin1Char('_') )
            {
              QString buffer;
              int j=i;
              while ( ch.isLetterOrNumber() || ch == QLatin1Char('_') ) {
                buffer += ch;
                ++j;
                if (j >= text.length())
                  break; // end of text
                ch = text.at(j);
              }

              if ( d->keywords.contains( buffer ) != 0 ) {
                if ( buffer == QLatin1String("def"))
                  endStateOfLastPara = DefineName;
                else if ( buffer == QLatin1String("class"))
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
        if ( ch == QLatin1Char('"') )
          endStateOfLastPara = Standard;
      } break;
    case Literal2:
      {
        setFormat( i, 1, d->cLiteral);
        if ( ch == QLatin1Char('\'') )
          endStateOfLastPara = Standard;
      } break;
    case Blockcomment1:
      {
        setFormat( i, 1, d->cBlockcomment);
        if ( i>=2 && ch == QLatin1Char('"') &&
            text.at(i-1) == QLatin1Char('"') &&
            text.at(i-2) == QLatin1Char('"'))
          endStateOfLastPara = Standard;
      } break;
    case Blockcomment2:
      {
        setFormat( i, 1, d->cBlockcomment);
        if ( i>=2 && ch == QLatin1Char('\'') &&
            text.at(i-1) == QLatin1Char('\'') &&
            text.at(i-2) == QLatin1Char('\''))
          endStateOfLastPara = Standard;
      } break;
    case DefineName:
      {
        if ( ch.isLetterOrNumber() || ch == QLatin1Char(' ') )
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
        if ( ch.isLetterOrNumber() || ch == QLatin1Char(' ') )
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
        if ( ch.isDigit() || ch == QLatin1Char('.') )
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
