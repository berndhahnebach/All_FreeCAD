/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/* Modifications for FreeCAD from 06-13-2004
    + include FreeCAD's PreCompiled header stuff
    + comment out use of KDE class kDebug
    + added pixmap resetproperty_xpm
*/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qheader.h>
# include <qevent.h>
# include <qfontmetrics.h>
# include <qpushbutton.h>
#endif

//#include <klocale.h>
//#include <kdebug.h>
//#include <kiconloader.h>

#include "kexipropertyeditor.h"
#include "kexipropertybuffer.h"
#include "propertyeditorlist.h"
#include "propertyeditorinput.h"
#include "propertyeditorfile.h"
#include "propertyeditorfont.h"
#include "propertyeditordate.h"

/* XPM */
static const char * resetproperty_xpm[] = {
      "7 6 4 1",
      "   c None",
      ".  c #C00000",
      "+  c #FF0000",
      "@  c None",
      ".+@@@+.",
      "@.+@+.@",
      "@@.+.@@",
      "@@+.+@@",
      "@+.@.+@",
      "+.@@@.+"};

using namespace Gui::Kexi;

PropertyEditor::PropertyEditor(QWidget *parent, bool autoSync, const char *name)
    : QListView(parent, name)
    , m_items(101)
{
  m_items.setAutoDelete(false);

  addColumn(tr("Property"));//, 145);
  addColumn(tr("Value"));//, 100);

  m_buffer = 0;
  m_topItem = 0;
  m_editItem = 0;
  m_sync = autoSync;
  slotValueChanged_enabled = true;

  connect(this, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(slotClicked(QListViewItem *)));
  connect(this, SIGNAL(expanded(QListViewItem *)), this, SLOT(slotExpanded(QListViewItem *)));
  connect(this, SIGNAL(collapsed(QListViewItem *)), this, SLOT(slotCollapsed(QListViewItem *)));
  connect(header(), SIGNAL(sizeChange(int, int, int)), this, SLOT(slotColumnSizeChanged(int, int, int)));
  connect(header(), SIGNAL(clicked(int)), this, SLOT(moveEditor()));
  connect(header(), SIGNAL(sectionHandleDoubleClicked (int)), this, SLOT(slotColumnSizeChanged(int)));

  m_defaults = new QPushButton(viewport());
  m_defaults->setPixmap(QPixmap(resetproperty_xpm));
  m_defaults->hide();
  connect(m_defaults, SIGNAL(clicked()), this, SLOT(resetItem()));

  //setFullWidth(true);
  setShowSortIndicator(false);
  //setTooltipColumn(0);
  setResizeMode(AllColumns);
  setSorting(0);
  setItemMargin(3);
  header()->setMovingEnabled( false );
}

PropertyEditor::~PropertyEditor()
{
}

void
PropertyEditor::slotClicked(QListViewItem *item)
{
  if (!item)
    return;
  //int y = viewportToContents(QPoint(0, itemRect(item).y())).y();
  //kdDebug() << "PropertyEditor::slotClicked() y: " << y << endl;
  //QRect g(columnWidth(0), y, columnWidth(1), item->height());
  PropertyEditorItem *i = static_cast<PropertyEditorItem *>(item);
  createEditor(i);//, g);
}

void
PropertyEditor::slotExpanded(QListViewItem *item)
{
  if (!item)
    return;
  moveEditor();
}

void
PropertyEditor::slotCollapsed(QListViewItem *item)
{
  if (!item)
    return;
  moveEditor();
}


void
PropertyEditor::createEditor(PropertyEditorItem *i)//, const QRect &geometry)
{
  //kdDebug() << "PropertyEditor::createEditor: Create editor for type " << i->type() << endl;
  int y = viewportToContents(QPoint(0, itemRect(i).y())).y();
  QRect geometry(columnWidth(0), y, columnWidth(1), i->height());

  if(m_currentEditor)
  {
    slotEditorAccept(m_currentEditor);
    delete m_currentEditor;
  }

  //m_defaults->hide();

  m_editItem = i;

  PropertySubEditor *editor=0;
  switch(i->type())
  {
  case QVariant::Bool:
    editor = new PropertyEditorBool(viewport(), i->property());
    break;

  case QVariant::String:
    editor = new PropertyEditorInput(viewport(), i->property());
    break;

  case QVariant::CString:
    editor = new PropertyEditorFile(viewport(), i->property());
    break;

  case QVariant::Int:
    editor = new PropertyEditorSpin(viewport(), i->property());
    break;

  case QVariant::Double:
    editor = new PropertyEditorDblSpin(viewport(), i->property());
    break;

  case QVariant::StringList:
    if(i->property()->value().type() == QVariant::StringList)
      editor = new PropertyEditorMultiList(viewport(), i->property());
    else
      editor = new PropertyEditorList(viewport(), i->property());
    break;

  case QVariant::BitArray:
    //editor = new EventEditorEditor(viewport(), i);
    break;

  case QVariant::Font:
    editor = new PropertyEditorFont(viewport(), i->property());
    break;

  case QVariant::Pixmap:
  case QVariant::Invalid:
    editor = new PropertyEditorPixmap(viewport(), i->property());
    break;

  case QVariant::Color:
    editor = new PropertyEditorColor(viewport(), i->property());
    break;

  case QVariant::Date:
    editor = new PropertyEditorDate(viewport(), i->property());
    break;

  case QVariant::Time:
    editor = new PropertyEditorTime(viewport(), i->property());
    break;

  case QVariant::DateTime:
    editor = new PropertyEditorDateTime(viewport(), i->property());
    break;

  case QVariant::Cursor:
    editor = new PropertyEditorCursor(viewport(), i->property());
    break;

  default:
    //m_currentEditor = 0;
    setFocus();
    //kdDebug() << "PropertyEditor::createEditor: No editor created!" << endl;
    //return;
  }

  if (editor) {
    connect(editor, SIGNAL(reject(PropertySubEditor *)), this,
            SLOT(slotEditorReject(PropertySubEditor *)));

    connect(editor, SIGNAL(accept(PropertySubEditor *)), this,
            SLOT(slotEditorAccept(PropertySubEditor *)));

    connect(editor, SIGNAL(changed(PropertySubEditor *)), this,
            SLOT(slotValueChanged(PropertySubEditor *)));

    addChild(editor);
    moveChild(editor, geometry.x(), geometry.y());
    editor->show();

    editor->setFocus();
  }
  m_currentEditor = editor;
  //m_editItem = i;
  showDefaultsButton( i->property()->changed() );
}

void
PropertyEditor::showDefaultsButton( bool show )
{
  int y = viewportToContents(QPoint(0, itemRect(m_editItem).y())).y();
  QRect geometry(columnWidth(0), y, columnWidth(1), m_editItem->height());
  m_defaults->resize(geometry.height(), geometry.height());

  if (!show) {
    if (m_currentEditor) {
      if (m_currentEditor->leavesTheSpaceForRevertButton()) {
        geometry.setWidth(geometry.width()-m_defaults->width());
      }
      m_currentEditor->resize(geometry.width(), geometry.height());
    }
    m_defaults->hide();
    return;
  }

  QPoint p = contentsToViewport(QPoint(0, geometry.y()));
  m_defaults->move(geometry.x() + geometry.width() - m_defaults->width(), p.y());
  if (m_currentEditor) {
    m_currentEditor->move(m_currentEditor->x(), p.y());
    m_currentEditor->resize(geometry.width()-m_defaults->width(), geometry.height());
  }
  m_defaults->show();
}

void
PropertyEditor::slotValueChanged(PropertySubEditor *editor)
{
  if (!slotValueChanged_enabled)
    return;
  if(m_currentEditor)
  {
    QVariant value = m_currentEditor->value();
    //js: not needed  m_editItem->setValue(value);
    bool sync = (m_editItem->property()->autoSync() != 0 && m_editItem->property()->autoSync() != 1) ?
                m_sync : (bool)m_editItem->property()->autoSync();
    if(m_buffer && sync)
    {
      if (m_editItem->property()->parent()) {
        m_editItem->property()->setValue( value );
      }
      else {
        m_buffer->changeProperty(m_editItem->name(), value);//getComposedValue());
      }

      /*if(m_editItem->depth()==1) {
          m_buffer->changeProperty(m_editItem->name().latin1(), value);
        }
        else if(m_editItem->depth()==2)
        {
          PropertyEditorItem *parent = static_cast<PropertyEditorItem*>(m_editItem->parent());
      //js: TODO    m_buffer->changeProperty(parent->name().latin1(), parent->getComposedValue());
        }*/
    }
    else
    {
      if(m_editItem->depth()==2)
      {
        PropertyEditorItem *parent = static_cast<PropertyEditorItem*>(m_editItem->parent());
        //js: TODO    parent->getComposedValue();
      }
    }
    m_editItem->updateValue();
    showDefaultsButton( m_editItem->property()->changed() );
    emit valueChanged(m_editItem->name(), value);
  }
}

void
PropertyEditor::slotEditorAccept(PropertySubEditor *editor)
{
  //kdDebug() << "PropertyEditor::slotEditorAccept" << endl;

  if(m_currentEditor)
  {
    QVariant value = m_currentEditor->value();
    //js: not needed    m_editItem->setValue(value);
    if(m_buffer)
    {
      m_buffer->debug();

      if (m_editItem->property()->parent()) {
        m_editItem->property()->setValue( value );
      }
      else {
        m_buffer->changeProperty(m_editItem->name(), value);
      }

      /*      if(m_editItem->depth()==1) {
              m_buffer->changeProperty(m_editItem->name().latin1(), value);
            }
            else if(m_editItem->depth()==2)
            {
              PropertyEditorItem *parent = static_cast<PropertyEditorItem*>(m_editItem->parent());
              m_buffer->changeProperty(parent->name(), parent->value());//getComposedValue());
            }*/
    }
    //    m_currentEditor->hide();
    //    m_currentEditor->clearFocus();
    m_editItem->updateValue();
    emit valueChanged(m_editItem->name(), value); //todo: only when changed
  }
}

void
PropertyEditor::slotEditorReject(PropertySubEditor *editor)
{
  if(m_currentEditor)
  {
    bool sync = (m_editItem->property()->autoSync() != 0 && m_editItem->property()->autoSync() != 1) ?
                m_sync : (bool)m_editItem->property()->autoSync();
    if(!sync)
    {
      //js: not needed m_editItem->setValue(m_editItem->property()->value());
      m_currentEditor->setValue(m_editItem->property()->value());
    }
    else
    {
      //js: not needed m_editItem->setValue(m_editItem->oldValue());
      m_currentEditor->setValue(m_editItem->property()->oldValue());
    }
    m_editItem->updateValue();
  }
  //editor->hide();
  //editor->setFocusPolicy(QWidget::NoFocus);
}

void
PropertyEditor::slotColumnSizeChanged(int section, int, int newS)
{
  if(m_currentEditor)
  {
    if(section == 0)
    {
      m_currentEditor->move(newS, m_currentEditor->y());
    }
    else
    {
      if(m_defaults->isVisible())
        m_currentEditor->resize(newS - m_defaults->width(), m_currentEditor->height());
      else
        m_currentEditor->resize(
          newS-(m_currentEditor->leavesTheSpaceForRevertButton()?m_defaults->width():0),
          m_currentEditor->height());
    }
  }
}

void
PropertyEditor::slotColumnSizeChanged(int section)
{
  setColumnWidth(1, viewport()->width() - columnWidth(0));
  slotColumnSizeChanged(section, 0, header()->sectionSize(section));
  if(m_currentEditor)
  {
    if(m_defaults->isVisible())
      m_currentEditor->resize(columnWidth(1) - m_defaults->width(), m_currentEditor->height());
    else
      m_currentEditor->resize(
        columnWidth(1)-(m_currentEditor->leavesTheSpaceForRevertButton()?m_defaults->width():0),
        m_currentEditor->height());
  }
}

void
PropertyEditor::reset(bool editorOnly)
{
  delete m_currentEditor;
  m_currentEditor = 0;
  if(m_defaults->isVisible())
    m_defaults->hide();

  if(!editorOnly)
  {
    clear();
    m_topItem = 0;
  }
}

QSize PropertyEditor::sizeHint() const
{
  return QSize( QFontMetrics(font()).width(columnText(0)+columnText(1)+"   "),
                QListView::sizeHint().height());
  //if (firstChild())
  //   return QSize(KListView::sizeHint().width(), firstChild()->height()*childCount());
  //return KListView::sizeHint();
}

void
PropertyEditor::setBuffer(PropertyBuffer *b)
{
  if (m_buffer) {
    m_buffer->disconnect(this);
  }
  m_buffer = b;
  if (m_buffer) {
    //receive property changes
    connect(m_buffer,SIGNAL(propertyChanged(PropertyBuffer&,Property&)),
            this,SLOT(slotPropertyChanged(PropertyBuffer&,Property&)));
    connect(m_buffer,SIGNAL(propertyReset(PropertyBuffer&,Property&)),
            this, SLOT(slotPropertyReset(PropertyBuffer&,Property&)));
    connect(m_buffer,SIGNAL(destroying()), this, SLOT(slotBufferDestroying()));
  }
  fill();
}

void PropertyEditor::slotPropertyReset(PropertyBuffer &buf,Property &prop)
{
  if (m_currentEditor) {
    slotValueChanged_enabled = false;
    m_currentEditor->setValue(m_editItem->property()->value());
    slotValueChanged_enabled = true;
  }
  else {
    m_editItem->updateValue();
  }
  //update children
  m_editItem->updateChildrenValue();

  showDefaultsButton( false );
}

void PropertyEditor::slotBufferDestroying()
{
  m_buffer = 0;
  fill();
}

void
PropertyEditor::fill()
{
  reset(false);

  if (!m_buffer)
    return;

  Property::ListIterator it(*m_buffer->list());

  if(!m_topItem)
  {
    m_topItem = new PropertyEditorItem(this,"Top Item");
  }

  m_items.clear();

  PropertyEditorItem *item=0;
  for(;it.current(); ++it)
  {
    if (it.current()->isVisible()) {
      item = new PropertyEditorItem(m_topItem, it.current(), item);
      m_items.insert(it.current()->name(), item);
    }
  }
}

void
PropertyEditor::resetItem()
{
  if(m_editItem)
  {
    //if (m_currentEditor) {
    //  m_currentEditor->setValue(m_editItem->property()->oldValue());
    //}

    //m_editItem->property()->setValue( m_editItem->property()->oldValue(), false );
    m_editItem->property()->resetValue();
    /*
    if (!m_currentEditor) {
      m_editItem->updateValue();
    }
    //update children
    m_editItem->updateChildrenValue();

    showDefaultsButton( false );
    */
    //js: not needed  else
    //js: not needed    m_editItem->setValue(m_editItem->property()->oldValue());
  }
}

void
PropertyEditor::moveEditor()
{
  //if (m_editItem) {
  if (m_currentEditor) {
    QPoint p = contentsToViewport(QPoint(0, itemPos(m_editItem)));
    //if(m_currentEditor)
    m_currentEditor->move(m_currentEditor->x(), p.y());
    if(m_defaults->isVisible())
      m_defaults->move(m_defaults->x(), p.y());
  }
}

void
PropertyEditor::resizeEvent(QResizeEvent *ev)
{
  QListView::resizeEvent(ev);
  resizeColumns();
  if(m_defaults->isVisible())
  {
    QRect r = itemRect(m_editItem);
    if(r.y()) // r.y() == 0 if the item is not visible on the screen
      m_defaults->move(r.x() + r.width() - m_defaults->width(), r.y());
    //if(m_currentEditor)
    //  m_currentEditor->resize(columnWidth(1) - m_defaults->width(), m_currentEditor->height());
  }

  if(m_currentEditor) {
    m_currentEditor->resize(
      columnWidth(1)-((m_currentEditor->leavesTheSpaceForRevertButton()||m_defaults->isVisible()) ? m_defaults->width() : 0),
      m_currentEditor->height());
  }
}

void
PropertyEditor::resizeColumns()
{
  int lvwidth = viewportSize(0, contentsHeight()).width();
  int count = columns();
  for (int j = 0; j < count ; ++j)
  {
    setColumnWidth(j,lvwidth/count);
  }
}

void
PropertyEditor::slotPropertyChanged(PropertyBuffer &buf,Property &prop)
{
  if (static_cast<PropertyBuffer*>(m_buffer)!=&buf)
    return;
  PropertyEditorItem* item = m_items[prop.name()];
  if (!item) //this property is not visible here
    return;
  if(item == m_editItem)
    m_currentEditor->setValue(prop.value());
  item->updateValue();
  item->updateChildrenValue();
}


#include "kexipropertyeditor.moc"
