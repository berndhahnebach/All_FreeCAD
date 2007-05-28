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
# include <q3header.h>
# include <qpainter.h>
# include <qstyle.h>
//Added by qt3to4:
#include <QResizeEvent>
#endif

#include "propertyeditor.h"

using namespace Gui::PropertyEditor;

PropertyEditor::PropertyEditor(QWidget *parent)
    : QTreeView(parent)
{
    propertyModel = new PropertyModel(this);
    setModel(propertyModel);

    PropertyItemDelegate* delegate = new PropertyItemDelegate(this);
    delegate->setItemEditorFactory(new PropertyItemEditorFactory);
    setItemDelegate(delegate);

    setAlternatingRowColors(true);
    setRootIsDecorated(true);
}

PropertyEditor::~PropertyEditor()
{
}

QStyleOptionViewItem PropertyEditor::viewOptions() const
{
    QStyleOptionViewItem option = QTreeView::viewOptions();
    option.showDecorationSelected = true;
    return option;
}

void PropertyEditor::currentChanged ( const QModelIndex & current, const QModelIndex & previous )
{
    QTreeView::currentChanged(current, previous);
    if (previous.isValid())
        closePersistentEditor(model()->buddy(previous));
    if (current.isValid())
        openPersistentEditor(model()->buddy(current));
}

void PropertyEditor::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = viewOptions();
    if (model()->hasChildren(index)) {
        opt.state |= QStyle::State_Children;

        QRect primitive(rect.left(), rect.top(), indentation(), rect.height());
        opt.rect = primitive;

        if (isExpanded(index))
            opt.state |= QStyle::State_Open;
        style()->drawPrimitive(QStyle::PE_IndicatorBranch, &opt, painter, this);
    }
}

void PropertyEditor::buildUp( const std::map<std::pair<std::string, int>, std::vector<App::Property*> >& props, unsigned long ct )
{
    propertyModel->buildUp(props, ct);
}

// --------------------------------------------------------------------

PropertyModel::PropertyModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    rootItem = (PropertyItem*)PropertyItem::create();
}

PropertyModel::~PropertyModel()
{
    delete rootItem;
}

QModelIndex PropertyModel::buddy ( const QModelIndex & index ) const
{
    if (index.column() == 1)
        return index;
    return index.sibling(index.row(), 1);
}

int PropertyModel::columnCount ( const QModelIndex & parent ) const
{
    // <name, value>, hence always 2
    if (parent.isValid())
        return static_cast<PropertyItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant PropertyModel::data ( const QModelIndex & index, int role ) const
{
    if (!index.isValid())
        return QVariant();

    PropertyItem *item = static_cast<PropertyItem*>(index.internalPointer());
    return item->data(index.column(), role);
}

bool PropertyModel::setData(const QModelIndex& index, const QVariant & value, int role)
{
    if (!index.isValid())
        return false;
    PropertyItem *item = static_cast<PropertyItem*>(index.internalPointer());
    return item->setData(value);
}

Qt::ItemFlags PropertyModel::flags(const QModelIndex &index) const
{
    PropertyItem *item = static_cast<PropertyItem*>(index.internalPointer());
    return item->flags(index.column());
}

QModelIndex PropertyModel::index ( int row, int column, const QModelIndex & parent ) const
{
    PropertyItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<PropertyItem*>(parent.internalPointer());

    PropertyItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex PropertyModel::parent ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return QModelIndex();

    PropertyItem *childItem = static_cast<PropertyItem*>(index.internalPointer());
    PropertyItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int PropertyModel::rowCount ( const QModelIndex & parent ) const
{
    PropertyItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<PropertyItem*>(parent.internalPointer());

    return parentItem->childCount();
}

QVariant PropertyModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if (orientation == Qt::Horizontal) {
        if (role != Qt::DisplayRole)
            return QVariant();
        if (section == 0)
            return tr("Name");
        if (section == 1)
            return tr("Value");
    }

    return QVariant();
}

bool PropertyModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role )
{
    return false;
}

void PropertyModel::buildUp( const std::map<std::pair<std::string, int>, std::vector<App::Property*> >& props, unsigned long ct )
{
    // fill up the listview with the properties
    rootItem->reset();
    std::map<std::pair<std::string, int>, std::vector<App::Property*> >::const_iterator it;
    for ( it = props.begin(); it != props.end(); ++it ) {
        // the property must be part of each selected object, i.e. the number of selected objects is equal 
        // to the number of properties with same name and id
        if ( it->second.size() == ct ) {
            App::Property* prop = (it->second)[0];
            QString editor = prop->getEditorName();
            if ( !editor.isEmpty() ) {
                Base::BaseClass* item = 0;
                try {
                    item = (Base::BaseClass*)Base::Type::createInstanceByName( prop->getEditorName(),true);
                } catch (...) {
                }
                if (!item) {
                    qWarning("No property item for type %s found\n", prop->getEditorName());
                    continue;
                }
                if (item->getTypeId().isDerivedFrom(PropertyItem::getClassTypeId())) {
                    PropertyItem* child = (PropertyItem*)item;
                    child->setParent(rootItem);
                    rootItem->appendChild(child);
                    child->setProperty(it->second);
                }
            }
        }
    }

    reset();
}



EditableListView::EditableListView( QWidget* parent )
  :Q3ListView( parent ), _editingItem( 0 )
{
  // disconnects the sectionHandleDoubleClicked signal to prevent the columns to be adjusted
  disconnect( header(), SIGNAL(sectionHandleDoubleClicked( int )), this, SLOT(adjustColumn( int )) );
  
  //setRootIsDecorated(true);
  setAllColumnsShowFocus( TRUE );
  setResizeMode(AllColumns);
  connect( this, SIGNAL( clicked( Q3ListViewItem* ) ),
           this, SLOT( mouseClick( Q3ListViewItem* ) ) );
  connect( this, SIGNAL( currentChanged( Q3ListViewItem*  ) ),
           this, SLOT( itemChanged( Q3ListViewItem* ) ) );
  connect( header(), SIGNAL( sizeChange ( int, int, int ) ),
           this, SLOT( updateItem() ) );
  connect( header(), SIGNAL( indexChange ( int , int , int ) ),
           this, SLOT( updateItem() ) );
}

void EditableListView::setOpen ( Q3ListViewItem * item, bool open )
{
  stopEdit();
  Q3ListView::setOpen( item, open );
}

void EditableListView::setSorting ( int column, bool ascending )
{
  stopEdit();
  Q3ListView::setSorting( column, ascending );
}

void EditableListView::stopEdit()
{
  if ( _editingItem )
    _editingItem->stopEdit();
  _editingItem = 0;
}

void EditableListView::buildUp( const std::map<std::pair<std::string, int>, std::vector<App::Property*> >& props, unsigned long ct )
{
    return;
  // clears the content before rebuilding
  this->stopEdit();
  this->clear();

  this->setUpdatesEnabled(FALSE);

  // fill up the listview with the properties
  EditableItem::parentView = this;
  std::map<std::pair<std::string, int>, std::vector<App::Property*> >::const_reverse_iterator it;
  for ( it = props.rbegin(); it != props.rend(); ++it )
  {
    // the property must be part of each selected object, i.e. the number of selected objects is equal 
    // to the number of properties with same name and id
    if ( it->second.size() == ct )
    {
      App::Property* prop = (it->second)[0];
      QString editor = prop->getEditorName();
      if ( !editor.isEmpty() )
      {
        EditableItem* item = (EditableItem*) Base::Type::createInstanceByName( prop->getEditorName(),true);
        if ( item )
        {
          assert(item->getTypeId().isDerivedFrom(EditableItem::getClassTypeId()));
          item->setText(0, QString(prop->getName()));
          item->setProperty( it->second );
        }
      }
    }
  }

  // enable update again
  this->setUpdatesEnabled(TRUE);
  this->repaint();
}

/*
void EditableListView::drawContentsOffset ( QPainter * p, int ox, int oy, int cx, int cy, int cw, int ch )
{
  QListView::drawContentsOffset( p, ox, oy, cx, cy, cw, ch);

  if ( rootIsDecorated() )
  {
    paintEmptyArea( p, QRect( 0, 0, 20, height() ) );
    p->save();
    p->rotate(90);
    p->drawText(20, -5, "Property editor");

    p->restore();

    QColorGroup cg = colorGroup();
    for( QListViewItemIterator it( this ); it.current(); ++it ) 
    {
      QListViewItem* item = it.current();
      if ( it.current()->childCount() > 0 && it.current()->depth() == 0 )
      {
        int pos = itemPos( it.current() );
        QRect r(0, 0, 20, height() );
        int bx = r.width()/2;
        int linebot = pos+9;
        p->save();
        p->setPen( cg.mid() );
        p->drawRect( bx-4, linebot-4 , 9, 9);
        p->setPen( cg.text() );
        p->drawLine( bx-2, linebot, bx+2, linebot );
        if ( !it.current()->isOpen() )
        p->drawLine( bx, linebot-2, bx, linebot+2 );
        p->restore();
      }
    }
  }
}
*/
void EditableListView::mouseClick( Q3ListViewItem* item )
{
  if ( !_editingItem )
    itemChanged( item );
}

void EditableListView::itemChanged( Q3ListViewItem* item )
{
  if ( _editingItem )
    _editingItem->stopEdit();

  _editingItem = dynamic_cast<EditableItem*>( item );
  if ( _editingItem ) {
    bool doEdit = _editingItem->startEdit( /*column*/1 );
    if (! doEdit )
      _editingItem = 0;
  }
}

void EditableListView::resizeEvent( QResizeEvent* e )
{
  updateItem();
  Q3ListView::resizeEvent( e );
}

void EditableListView::updateItem()
{
  if (_editingItem)
    _editingItem->update();
}


#include "moc_propertyeditor.cpp"
