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
#endif

#include "TextEdit.h"

using namespace Gui;

/**
 *  Constructs a TextEdit which is a child of 'parent'.
 */
TextEdit::TextEdit(QWidget* parent)
  : QTextEdit(parent), listBox(0)
{
  QShortcut* shortcut = new QShortcut(this);
  shortcut->setKey(Qt::CTRL+Qt::Key_Space);
  connect(shortcut, SIGNAL(activated()), this, SLOT(complete()));
}

/** Destroys the object and frees any allocated resources */
TextEdit::~TextEdit()
{
}

/**
 * Completes the word.
 */
void TextEdit::complete()
{return;
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
    
  QStringList list = QStringList::split(QRegExp("\\W+"), text());
  QMap<QString, QString> map;
  QStringList::Iterator it = list.begin();
  while (it != list.end()) {
    if ((*it).startsWith(wordPrefix) && (*it).length() > wordPrefix.length())
        map[(*it).lower()] = *it;
    ++it;
  }
    
  if (map.count() == 1) {
      insert((*map.begin()).mid(wordPrefix.length()));
  } else if (map.count() > 1) {
    if (!listBox)
      createListBox();
    listBox->clear();
    listBox->addItems(map.values());

    QPoint point = textCursorPoint();
//    listBox->adjustListBoxSize(qApp->desktop()->height() - point.y(), width() / 2);
    point.setX( point.x()+5 ); // insert space
    // list box is (partly) hidden
    if ( point.y() + listBox->height() > height() )
      point.setY( point.y() - listBox->height() );
//    listBox->popup( point );
    listBox->show();
  }
}

/**
 * Returns the textcursor position in coordinates.
 */
QPoint TextEdit::textCursorPoint() const
{
  QTextCursor cursor = textCursor();
  QTextBlock block = cursor.block();
  QRect rect = cursorRect(cursor);
  QPoint point(rect.left(), rect.bottom());
  return mapToGlobal(point);
}

/**
 * If an item was chosen (either by clicking or pressing enter) the rest of the word is completed.
 * The listbox is closed without destroying it.
 */
void TextEdit::itemChosen(QListWidgetItem *item)
{
    listBox->hide();
    QString text = item->data(Qt::UserRole).toString();
    //QString texte = item->text();
//    wordUnderCursor(QPoint(), true);
    textCursor().insertText( text );
    ensureCursorVisible();
    setFocus( Qt::OtherFocusReason );
    return;
  //if (item)
  //  insert(item->text().mid(wordPrefix.length()));
  //listBox->close();
  //setFocus();
}

/**
 * Creates the listbox containing all possibilities for the completion.
 * The listbox is closed when ESC is pressed.
 */
void TextEdit::createListBox()
{
    listBox = new CompletionList(this);
    installEventFilter(listBox);
    listBox->setSelectionMode( QAbstractItemView::SingleSelection );
    listBox->hide();
    connect(listBox, SIGNAL(itemActivated(QListWidgetItem *)), this, SLOT(itemChosen(QListWidgetItem *)) );
//  listBox = new CompletionBox(this);
//  connect(listBox, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(itemChosen(QListWidgetItem *)));
}

// ------------------------------------------------------------------------------

CompletionList::CompletionList(QTextEdit* parent)
  :  QListWidget(parent), textEdit(parent)
{
}

CompletionList::~CompletionList()
{
}

bool CompletionList::eventFilter(QObject * watched, QEvent * event)
{
    if (isVisible()) {
        if (watched == textEdit && event->type() == QEvent::KeyPress) {
            QKeyEvent* ke = (QKeyEvent*)event;
            if (ke->key() == Qt::Key_Up) {
                keyPressEvent(ke);
                return true;
            } else if (ke->key() == Qt::Key_Down) {
                keyPressEvent(ke);
                return true;
            }
        }
    }

    return QListWidget::eventFilter(watched, event);
}

// ------------------------------------------------------------------------------

CompletionBox::CompletionBox(QWidget* parent)
  :  QWidget(parent)
{
  setWindowFlags(Qt::Popup);
  listView = new QListWidget(this);
  connect(listView, SIGNAL(itemClicked(QListWidgetItem *)), this, SIGNAL(itemClicked(QListWidgetItem *)));

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setMargin(3);
  layout->addWidget(listView);
  setLayout(layout);

  listView->installEventFilter(this);
}

CompletionBox::~CompletionBox()
{
  listView->removeEventFilter(this);
}

void CompletionBox::popup(const QPoint& pos)
{
  move(pos);
  show();
  listView->setFocus();
  // select the first item
  listView->setCurrentRow(0);
}

void CompletionBox::adjustListBoxSize(int maxHeight, int maxWidth)
{
  if (!listView->count())
    return;

  QFontMetrics fm(listView->font());
  int totalHeight = 0;
  int maxItemWidth = 0;

  for (int i = 0; i < (int)listView->count(); ++i) {
    QString text = listView->item(i)->text();
    maxItemWidth = qMax<int>(maxItemWidth, fm.width(text));
    totalHeight += fm.height();
  }
  
  setFixedHeight(qMin<int>(totalHeight+40, maxHeight));
  setFixedWidth(qMin<int>(maxItemWidth+40, maxWidth));
}

void CompletionBox::addItems(const QStringList& items)
{
  listView->addItems(items);
}

void CompletionBox::clear()
{
  listView->clear();
}

bool CompletionBox::eventFilter(QObject * watched, QEvent * event)
{
  if (watched == listView && event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Return) {
      itemClicked(listView->currentItem());
      return true;
    }
  }

  return QWidget::eventFilter(watched, event);
}

#include "moc_TextEdit.cpp" 
