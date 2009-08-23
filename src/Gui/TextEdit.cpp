/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QKeyEvent>
# include <QPainter>
# include <QShortcut>
# include <QTextCursor>
#endif

#include "TextEdit.h"
#include "SyntaxHighlighter.h"

using namespace Gui;

/**
 *  Constructs a TextEdit which is a child of 'parent'.
 */
TextEdit::TextEdit(QWidget* parent)
    : QTextEdit(parent), listBox(0)
{
    //Note: Set the correct context to this shortcut as we may use several instances of this
    //class at a time
    QShortcut* shortcut = new QShortcut(this);
    shortcut->setKey(Qt::CTRL+Qt::Key_Space);
    shortcut->setContext(Qt::WidgetShortcut);
    connect(shortcut, SIGNAL(activated()), this, SLOT(complete()));
}

/** Destroys the object and frees any allocated resources */
TextEdit::~TextEdit()
{
}

/**
 * Set the approproriate item of the completion box or hide it, if needed.
 */
void TextEdit::keyPressEvent(QKeyEvent* e)
{
    QTextEdit::keyPressEvent(e);
    // This can't be done in CompletionList::eventFilter() because we must first perform
    // the event and afterwards update the list widget
    if (listBox && listBox->isVisible()) {
        // Get the word under the cursor
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::StartOfWord);
        // the cursor has moved to outside the word prefix
        if (cursor.position() < cursorPosition-wordPrefix.length() || 
            cursor.position() > cursorPosition) {
            listBox->hide();
            return;
        }
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        listBox->keyboardSearch(cursor.selectedText());
        cursor.clearSelection();
    }
}

/**
 * Completes the word.
 */
void TextEdit::complete()
{
    QTextBlock block = textCursor().block();
    if (!block.isValid())
        return;
    int cursorPos = textCursor().position()-block.position();
    QString para = block.text();
    int wordStart = cursorPos;
    while (wordStart > 0 && para[wordStart - 1].isLetterOrNumber())
        --wordStart;
    wordPrefix = para.mid(wordStart, cursorPos - wordStart);
    if (wordPrefix.isEmpty())
        return;
    
    QStringList list = toPlainText().split(QRegExp(QLatin1String("\\W+")));
    QMap<QString, QString> map;
    QStringList::Iterator it = list.begin();
    while (it != list.end()) {
        if ((*it).startsWith(wordPrefix) && (*it).length() > wordPrefix.length())
            map[(*it).toLower()] = *it;
        ++it;
    }
    
    if (map.count() == 1) {
        insertPlainText((*map.begin()).mid(wordPrefix.length()));
    } else if (map.count() > 1) {
        if (!listBox)
            createListBox();
        listBox->clear();
        listBox->addItems(map.values());
        listBox->setFont(QFont(font().family(), 8));

        this->cursorPosition = textCursor().position();

        // get the minimum width and height of the box
        int h = 0;
        int w = 0;
        for (int i = 0; i < listBox->count(); ++i) {
            QRect r = listBox->visualItemRect(listBox->item(i));
            w = qMax(w, r.width());
            h += r.height();
        }

        // Add an offset
        w += 2*listBox->frameWidth();
        h += 2*listBox->frameWidth();

        // get the start position of the word prefix
        QTextCursor cursor = textCursor();
        cursor.movePosition(QTextCursor::StartOfWord);
        QRect rect = cursorRect(cursor);
        int posX = rect.x();
        int posY = rect.y();
        int boxH = h;

        // Decide whether to show downstairs or upstairs
        if (posY > viewport()->height()/2) {
            h = qMin(qMin(h,posY), 250);
            if (h < boxH)
                w += style()->pixelMetric(QStyle::PM_ScrollBarExtent);
            listBox->setGeometry(posX,posY-h, w, h);
        } else {
            h = qMin(qMin(h,viewport()->height()-fontMetrics().height()-posY), 250);
            if (h < boxH)
                w += style()->pixelMetric(QStyle::PM_ScrollBarExtent);
            listBox->setGeometry(posX, posY+fontMetrics().height(), w, h);
        }
        
        listBox->setCurrentRow(0);
        listBox->show();
    }
}

/**
 * Creates the listbox containing all possibilities for the completion.
 * The listbox is closed when ESC is pressed, the text edit field looses focus or a
 * mouse button was pressed.
 */
void TextEdit::createListBox()
{
    listBox = new CompletionList(this);
    listBox->setFrameStyle(QFrame::Box|QFrame::Raised);
    listBox->setLineWidth(2);
    installEventFilter(listBox);
    viewport()->installEventFilter(listBox);
    listBox->setSelectionMode( QAbstractItemView::SingleSelection );
    listBox->hide();
}

// ------------------------------------------------------------------------------

namespace Gui {
struct TextEditorP
{
    QMap<QString, QColor> colormap; // Color map
    TextEditorP()
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

/**
 *  Constructs a TextEditor which is a child of 'parent' and does the
 *  syntax highlighting for the Python language. 
 */
TextEditor::TextEditor(QWidget* parent)
  : TextEdit(parent), WindowParameter("Editor"), highlighter(0)
{
    d = new TextEditorP();

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
    connect(this, SIGNAL(cursorPositionChanged()), 
            this, SLOT(onCursorPositionChanged()));
}

/** Destroys the object and frees any allocated resources */
TextEditor::~TextEditor()
{
    getWindowParameter()->Detach(this);
    delete highlighter;
    delete d;
}

void TextEditor::keyPressEvent (QKeyEvent * e)
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
void TextEditor::OnChange(Base::Subject<const char*> &rCaller,const char* sReason)
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
            if (this->highlighter)
                this->highlighter->setColor(QLatin1String(sReason), color);
        }
    }

    if (strcmp(sReason, "TabSize") == 0 || strcmp(sReason, "FontSize") == 0) {
        int tabWidth = hPrefGrp->GetInt("TabSize", 4);
        QFontMetrics metric(font());
        int fontSize = metric.width(QLatin1String("0"));
        setTabStopWidth(tabWidth * fontSize);
    }
}

void TextEditor::onCursorPositionChanged()
{
    const QColor& color = d->colormap[QLatin1String("Line")];
    if ( color.isValid() )
        viewport()->update();
}

void TextEditor::paintEvent (QPaintEvent * e)
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

// ------------------------------------------------------------------------------

CompletionList::CompletionList(QTextEdit* parent)
  :  QListWidget(parent), textEdit(parent)
{
    // make the user assume that the widget is active
    QPalette pal = parent->palette();
    pal.setColor(QPalette::Inactive, QPalette::Highlight, pal.color(QPalette::Active, QPalette::Highlight));
    pal.setColor(QPalette::Inactive, QPalette::HighlightedText, pal.color(QPalette::Active, QPalette::HighlightedText));
    parent->setPalette( pal );

    connect(this, SIGNAL(itemActivated(QListWidgetItem *)), 
            this, SLOT(completionItem(QListWidgetItem *)));
}

CompletionList::~CompletionList()
{
}

void CompletionList::findCurrentWord(const QString& wordPrefix)
{ 
    for (int i=0; i<count(); ++i) {
        QString text = item(i)->text();
        if (text.startsWith(wordPrefix)) {
            setCurrentRow(i);
            return;
        }
    }

    setItemSelected(currentItem(), false);
}

/** 
 * Get all incoming events of the text edit and redirect some of them, like key up and
 * down, mouse press events, ... to the widget itself.
 */
bool CompletionList::eventFilter(QObject * watched, QEvent * event)
{
    if (isVisible() && watched == textEdit->viewport()) {
        if (event->type() == QEvent::MouseButtonPress)
            hide();
    } else if (isVisible() && watched == textEdit) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* ke = (QKeyEvent*)event;
            if (ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down) {
                keyPressEvent(ke);
                return true;
            } else if (ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown) {
                keyPressEvent(ke);
                return true;
            } else if (ke->key() == Qt::Key_Escape) {
                hide();
                return true;
            } else if (ke->key() == Qt::Key_Space) {
                hide();
                return false;
            } else if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
                itemActivated(currentItem());
                return true;
            }
        } else if (event->type() == QEvent::FocusOut) {
            if (!hasFocus())
                hide();
        }
    }

    return QListWidget::eventFilter(watched, event);
}

/**
 * If an item was chosen (either by clicking or pressing enter) the rest of the word is completed.
 * The listbox is closed without destroying it.
 */
void CompletionList::completionItem(QListWidgetItem *item)
{
    hide();
    QString text = item->text();
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::StartOfWord);
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    cursor.insertText( text );
    textEdit->ensureCursorVisible();
}

#include "moc_TextEdit.cpp" 
