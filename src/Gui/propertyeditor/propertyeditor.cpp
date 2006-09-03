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
# include <qheader.h>
# include <qpainter.h>
# include <qstyle.h>
#endif

#include "propertyeditor.h"

using namespace Gui::PropertyEditor;


EditableListView::EditableListView( QWidget* parent, const char* name )
  :QListView( parent, name ), _editingItem( 0 )
{
  setRootIsDecorated(true);
  header()->setClickEnabled ( FALSE, header()->count() - 1 );
  header()->setResizeEnabled( FALSE, header()->count() - 1 );

  setSorting( -1, false );
  setAllColumnsShowFocus( TRUE );
  setResizeMode(AllColumns);
  connect( this, SIGNAL( clicked( QListViewItem* ) ),
           this, SLOT( mouseClick( QListViewItem* ) ) );
  connect( this, SIGNAL( currentChanged( QListViewItem*  ) ),
           this, SLOT( itemChanged( QListViewItem* ) ) );
  connect( header(), SIGNAL( sizeChange ( int, int, int ) ),
           this, SLOT( updateItem() ) );
  connect( header(), SIGNAL( indexChange ( int , int , int ) ),
           this, SLOT( updateItem() ) );
}

void EditableListView::setOpen ( QListViewItem * item, bool open )
{
  stopEdit();
  QListView::setOpen( item, open );
}

void EditableListView::stopEdit()
{
  if ( _editingItem )
    _editingItem->stopEdit();
  _editingItem = 0;
}

void EditableListView::buildUp( const std::map<std::pair<std::string, int>, std::vector<App::Property*> >& props, unsigned long ct )
{
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
void EditableListView::mouseClick( QListViewItem* item )
{
  if ( !_editingItem )
    itemChanged( item );
}

void EditableListView::itemChanged( QListViewItem* item )
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
  QListView::resizeEvent( e );
}

void EditableListView::updateItem()
{
  if (_editingItem)
    _editingItem->update();
}


#include "moc_propertyeditor.cpp"
