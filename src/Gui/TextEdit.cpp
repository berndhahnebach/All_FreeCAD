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

//#ifndef _PreComp_
//# include <q3accel.h>
//# include <qapplication.h>
//# include <q3listbox.h>
//# include <qregexp.h>
////Added by qt3to4:
//#include <QMouseEvent>
//#endif

#include "TextEdit.h"

using namespace Gui;

/**
 *  Constructs a TextEdit which is a child of 'parent', with the
 *  name 'name' and installs the Python syntax highlighter.
 */
TextEdit::TextEdit(QWidget *parent,const char *name)
    : Q3TextEdit(parent, name), listBox( 0 )
{
  Q3Accel *accel = new Q3Accel(this);
  accel->connectItem(accel->insertItem(Qt::CTRL+Qt::Key_Space), this, SLOT(complete()));
}

/** Destroys the object and frees any allocated resources */
TextEdit::~TextEdit()
{
}

/**
 * Completes the word.
 */
void TextEdit::complete()
{
  int cursorPara;
  int cursorPos;
  getCursorPosition(&cursorPara, &cursorPos);

  QString para = text(cursorPara);
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
    listBox->insertStringList(map.values());

    QPoint point = textCursorPoint();
    adjustListBoxSize(qApp->desktop()->height() - point.y(), width() / 2);
    point.setX( point.x()+5 ); // insert space
    int h = visibleHeight();
    // list box is (partly) hidden
    if ( point.y() + listBox->height() > h )
      point.setY( point.y() - listBox->height() );
    listBox->popup( point );
  }
}

/**
 * Returns the textcursor position in coordinates.
 */
QPoint TextEdit::textCursorPoint() const
{
  int cursorPara;
  int cursorPos;
  getCursorPosition(&cursorPara, &cursorPos);
  QRect rect = paragraphRect(cursorPara);
  QPoint point(rect.left(), rect.bottom());
  while (charAt(point, 0) < cursorPos)
    point.rx() += 10;
  return mapToGlobal(contentsToViewport(point));
}

/**
 * If an item was chosen (either by clicking or pressing enter) the rest of the word is completed.
 * The listbox is closed without destroying it.
 */
void TextEdit::itemChosen(Q3ListBoxItem *item)
{
  if (item)
    insert(item->text().mid(wordPrefix.length()));
  listBox->close();
  setFocus();
}

/**
 * Creates the listbox containing all possibilities for the completion.
 * The listbox is closed when ESC is pressed.
 */
void TextEdit::createListBox()
{
  listBox = new CompletionBox(this, "listBox");
  Q3Accel *accel = new Q3Accel(listBox);
  accel->connectItem(accel->insertItem(Qt::Key_Escape), listBox, SLOT(close()));

  connect(listBox, SIGNAL(clicked(Q3ListBoxItem *)),
            this, SLOT(itemChosen(Q3ListBoxItem *)));
  connect(listBox,
            SIGNAL(returnPressed(Q3ListBoxItem *)),
            this, SLOT(itemChosen(Q3ListBoxItem *)));
}

void TextEdit::adjustListBoxSize(int maxHeight, int maxWidth)
{
  if (!listBox->count())
    return;
  int totalHeight = listBox->itemHeight(0) * listBox->count();
  if (listBox->variableHeight()) {
    totalHeight = 0;
    for (int i = 0; i < (int)listBox->count(); ++i)
      totalHeight += listBox->itemHeight(i);
   }
   listBox->setFixedHeight(QMIN(totalHeight+20, maxHeight));
   listBox->setFixedWidth(qMin<int>(listBox->maxItemWidth()+20, maxWidth));
}

// ------------------------------------------------------------------------------

CompletionBox::CompletionBox( QWidget* parent, const char*  name )
  :  Q3ListBox( parent, name, Qt::WType_Popup )
{
  setFrameStyle( WinPanel|Raised );
  setMouseTracking( true );
  setMargin(3);
}

CompletionBox::~CompletionBox()
{
}

void CompletionBox::popup( const QPoint& pos )
{
  move( pos );
  show();
  // select the first item
  setSelected ( 0, true );
//  raise();
//  setFocus();
}

void CompletionBox::mousePressEvent( QMouseEvent* e )
{
  Q3ListBox::mousePressEvent( e );
  // if the mouse button was pressed outside
  if  (!rect().contains( e->pos() ))
  {
    close();
  }
}

#include "moc_TextEdit.cpp" 
