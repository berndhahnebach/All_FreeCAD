/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <qcursor.h>
#	include <q3header.h>
# include <qlayout.h>
# include <q3popupmenu.h>
# include <qstatusbar.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <Q3GridLayout>
#include <QPixmap>
#endif

#include <Base/Console.h>
#include <App/Document.h>
#include <App/Feature.h>
#include <App/DocumentObjectGroup.h>

#include "Tree.h"
#include "Document.h"
#include "BitmapFactory.h"
#include "ViewProviderFeature.h"
#include "MenuManager.h"
#include "Application.h"
#include "MainWindow.h"


using Base::Console;

using namespace Gui;


//**************************************************************************
// DocItem
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
DocItem::DocItem( Q3ListViewItem* parent,Q3ListViewItem * after,Gui::Document* doc)
    : Q3ListViewItem( parent, after ), _pcDocument(doc)
{
  setPixmap(0,*TreeView::pcDocumentPixmap);
  setText(0,QString(_pcDocument->getDocument()->getName()));

  bHighlight = false;
  bSelected = false;

  buildUp();
}

DocItem::~DocItem()
{
  //_pcDocument->Detach(this);
}



bool DocItem::testStatus(void)
{
  bool bChanged = false;

  for (std::map<string,ObjectItem*>::iterator pos = FeatMap.begin();pos!=FeatMap.end();++pos)
  {
    if(pos->second->testStatus())
      bChanged = true;
  }
  return bChanged;

}

void DocItem::highlightFeature(const char* name, bool bOn)
{
  std::map<string,ObjectItem*>::iterator pos;
  pos = FeatMap.find(name);
  
  if(pos != FeatMap.end())
    pos->second->highlightFeature(bOn);

  bHighlight = bOn;
  if (!isOpen() )
    repaint();

}

void DocItem::selectFeature(const char* name, bool bOn)
{
  std::map<string,ObjectItem*>::iterator pos;
  pos = FeatMap.find(name);
  
  if(pos != FeatMap.end())
    pos->second->selectFeature(bOn);


}

void DocItem::clearSelection(void)
{
  for (std::map<string,ObjectItem*>::iterator pos = FeatMap.begin();pos!=FeatMap.end();++pos)
  {
    pos->second->selectFeature(false);
  }
}

void DocItem::isSelectionUptodate(void)
{
  for (std::map<string,ObjectItem*>::iterator pos = FeatMap.begin();pos!=FeatMap.end();++pos)
  {
    if(pos->second->bSelected != pos->second->isSelected())
    {
      if(pos->second->bSelected)
      {
        //Base::Console().Log("Sel : Rmv tree selection (%s,%s)\n",_pcDocument->getDocument()->getName(),pos->first.c_str());
        Gui::Selection().rmvSelection(_pcDocument->getDocument()->getName(),pos->first.c_str());
        //pos->second->bSelected = false;
      }else{

        //Base::Console().Log("Sel : Add tree selection (%s,%s)\n",_pcDocument->getDocument()->getName(),pos->first.c_str());
        Gui::Selection().addSelection(_pcDocument->getDocument()->getName(),pos->first.c_str());
        pos->second->bSelected = true;

      }
    }
    
  }



}

void DocItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
   QColorGroup _cg( cg );
   QColor c = _cg.text();

   if ( bHighlight && !isOpen() )
       _cg.setColor( QColorGroup::Base , QColor (200,200,255));

   Q3ListViewItem::paintCell( p, _cg, column, width, align );

   _cg.setColor( QColorGroup::Text, c );

}

void DocItem::addViewProviderDocumentObject(ViewProviderDocumentObject* Provider)
{
  std::string name = Provider->getObject()->name.getValue();
  std::map<std::string,ObjectItem*>::iterator it = FeatMap.find( name );

  if ( it == FeatMap.end() ) {
    
    // get the associated document object
    App::DocumentObject* obj = Provider->getObject();
    
    // is the object part of a group?
    ObjectItem* groupItem=0;
    App::DocumentObjectGroup* grp = App::DocumentObjectGroup::getGroupOfObject( obj );
    if ( grp ) {
      std::string grpname = grp->name.getValue();
      std::map<std::string,ObjectItem*>::iterator jt = FeatMap.find( grpname );

      if ( jt != FeatMap.end() )
        groupItem = jt->second;

#ifdef FC_DEBUG
      else
        Base::Console().Warning("DocItem::addViewProviderDocumentObject: try to insert a group object before the group itself is inserted\n");
#endif
    }

    ObjectItem* item = Provider->getTreeItem(this);
    Q3ListViewItem* child=0;
    Q3ListViewItem* sibling=0;
    if ( groupItem )
    {
      // set the newly created item at the end of the group
      groupItem->setOpen(true);
      groupItem->insertItem(item);
      child = groupItem->firstChild();
    }
    else
    {
      // set the newly created item at the end of the document item
      child = firstChild();
    }

    // get the last item
    while (child)
    {
      sibling = child;
      child = child->nextSibling();
    }

    if ( sibling )
      item->moveItem(sibling);

    FeatMap[ name ] = item;

    if(FeatMap.size() == 1)
      setOpen(true);
  } else {
#ifdef FC_DEBUG
    Base::Console().Warning("DocItem::addViewProviderDocumentObject: cannot add view provider twice\n");
#endif
  }
}

void DocItem::removeViewProviderDocumentObject(ViewProviderDocumentObject* Provider)
{
  //FIXME: 
#if 1
  // If we remove an item from the list view all its children would get removed, too. Thus we must
  // move all children items one level up. 
  QString name = Provider->getObject()->name.getValue();
  std::map<std::string,ObjectItem*>::iterator it = FeatMap.find(name.latin1());
  if ( it != FeatMap.end() )
  {
    ObjectItem* item = it->second;
    FeatMap.erase(it);

    ObjectItem *child = (ObjectItem*)item->firstChild();
    Q3ListViewItem* parent = item->parent();
    while ( child ) {
      // move the items to the parent item
      ObjectItem* sibling = (ObjectItem*)child->nextSibling();
      item->takeItem(child);
      parent->insertItem(child);
      child = sibling;
    }

    delete item;
  }
#else
  // If we remove an item from the list view all its children get removed, too. Thus we must
  // make sure to´remove properly all concerning elements from our map, too. 
  QString name = Provider->getObject()->name.getValue();
  std::map<std::string,ObjectItem*>::iterator it = FeatMap.find(name.latin1());
  if ( it != FeatMap.end() )
  {
    ObjectItem* item = it->second;
    FeatMap.erase(it);

    ObjectItem *child = (ObjectItem*)item->firstChild();
    while ( child ) {
      ObjectItem* sibling = (ObjectItem*)child->nextSibling();
      // call recursively
      removeViewProviderDocumentObject( child->_pcViewProvider );
      child = sibling;
    }

    delete item;
  }
#endif
}

void DocItem::update(void)
{
  //puts("Updtate");

}

void DocItem::buildUp(void)
{

}

void DocItem::setOpen( bool o )
{
  Q3ListViewItem::setOpen ( o );
}

void DocItem::setup()
{
  //setExpandable( TRUE );
  Q3ListViewItem::setup();
}

void DocItem::activate ()
{
  //puts("Activated");
}

void DocItem::rename(void)
{
  setText(0,QString(_pcDocument->getDocument()->getName()));

}

//**************************************************************************
// ObjectItem
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
ObjectItem::ObjectItem( Q3ListViewItem* parent,Gui::ViewProviderDocumentObject* pcViewProvider)
    : Q3ListViewItem( parent ),
  _pcViewProvider(pcViewProvider)
{
  BaseColor = Qt::white;
  TextColor = Qt::black;
  HighlightColor = QColor (200,200,255);

//  setPixmap(0,pcViewProvider->getIcon());
//  setText(0,QString(pcViewProvider->getObject()->getName()));


  bHighlight = false;
  bSelected = false;

  buildUp();
}

ObjectItem::~ObjectItem()
{
}

bool ObjectItem::testStatus(void)
{
  bool ret = _pcViewProvider->testStatus();
  if ( ret )
    repaint();
  return ret;
}

void ObjectItem::displayStatusInfo()
{
  App::DocumentObject* Obj = _pcViewProvider->getObject();

  if(Obj->getTypeId().isDerivedFrom(App::AbstractFeature::getClassTypeId()) )
  {
    App::AbstractFeature *feat = dynamic_cast<App::AbstractFeature *>(Obj);
    QString info = feat->getStatusString();
    if ( feat->getStatus() == App::AbstractFeature::Error )
      info += QString(" (%1)").arg(feat->getErrorString());
    else if ( feat->mustExecute() )
      info += QString(" (but must be executed)");
    getMainWindow()->statusBar()->message( info );
  }
}

void ObjectItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
   QColorGroup _cg( cg );
   QColor c = _cg.text();

   if ( bHighlight )
     _cg.setColor( QColorGroup::Base , HighlightColor);
   //else 
   //  _cg.setColor( QColorGroup::Base , BaseColor);

   //_cg.setColor( QColorGroup::Text , TextColor);
   
   Q3ListViewItem::paintCell( p, _cg, column, width, align );

   //_cg.setColor( QColorGroup::Text, c );

}

void ObjectItem::highlightFeature(bool bOn)
{
  bHighlight = bOn;
  repaint();
}

void ObjectItem::selectFeature(bool bOn)
{
  setSelected(bOn);
  bSelected = bOn;
  repaint();
}

void ObjectItem::update(void)
{
  //puts("Updtate");

}

void ObjectItem::buildUp(void)
{

}

void ObjectItem::setOpen( bool o )
{

  Q3ListViewItem::setOpen ( o );
}


void ObjectItem::setup()
{
  //setExpandable( TRUE );
  Q3ListViewItem::setup();
}

void ObjectItem::activate ()
{
  //puts("Activated");
}




QPixmap* TreeView::pcDocumentPixmap=0;

/* TRANSLATOR Gui::TreeView */
TreeView::TreeView(Gui::Document* pcDocument,QWidget *parent)
  : DockWindow(pcDocument,parent),bFromOutside(false), _lastDocItem(0)
{
  Q3GridLayout* layout = new Q3GridLayout( this );
  _pcListView = new Q3ListView(this);
  layout->addWidget( _pcListView, 0, 0 );

  // set defaults and the colums
  _pcListView->setSortColumn(-1);
  _pcListView->addColumn(tr("Labels & Attributes"));
  _pcListView->setColumnWidthMode(0,Q3ListView::Maximum);
  //_pcListView->setSorting(0,true);
//  _pcListView->addColumn(tr("Value"));
//  _pcListView->setColumnWidthMode(1,QListView::Manual );

  // retreive the Pixmaps
  pcDocumentPixmap   = new QPixmap(Gui::BitmapFactory().pixmap("Document"));
  //pcLabelClosed = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));
  //pcAttribute   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_Attr"));


  // Add the first main label
  _pcMainItem = new Q3ListViewItem(_pcListView,"Application");
  _pcMainItem->setOpen(true);

  //_pcListView->setRootIsDecorated(true);

  _pcListView->setSelectionMode(Q3ListView::Extended );
  connect( _pcListView, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
  connect( _pcListView, SIGNAL( onItem(Q3ListViewItem*) ), this, SLOT( onItem(Q3ListViewItem*) ) );


  QPalette pal = _pcListView->palette();
  pal.setInactive( pal.active() );
  _pcListView->setPalette( pal );


  //_pcListView->setSize(200,400);
  resize( 200, 400 );

  Gui::Selection().Attach(this);


}

TreeView::~TreeView()
{
  Gui::Selection().Detach(this);
  delete pcDocumentPixmap;
}

void TreeView::contextMenuEvent ( QContextMenuEvent * e )
{
  // ask workbenches and view provider, ...
  MenuItem* view = new MenuItem;
  Gui::Application::Instance->setupContextMenu("Tree", view);

  Q3PopupMenu ContextMenu;
  MenuManager::getInstance()->setupContextMenu(view,ContextMenu);
  delete view;
  ContextMenu.exec( QCursor::pos() );
}

void TreeView::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::LanguageChange) {
    _pcListView->setColumnText(0, tr("Labels & Attributes"));
  }

  DockWindow::changeEvent(e);
}

void TreeView::testStatus(void)
{
  bool bChanged = false;

  for (std::map<Gui::Document*,DocItem*>::iterator pos = DocMap.begin();pos!=DocMap.end();++pos)
  {
    if(pos->second->testStatus())
      bChanged = true;
  }

  if ( bChanged )
    repaint();
}

// Observer message from the Selection
void TreeView::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,Gui::SelectionSingleton::MessageType Reason)
{
  bFromOutside = true;
  if(Reason.Type != SelectionChanges::ClearSelection)
  {
    Gui::Document* pDoc = Application::Instance->getDocument( Reason.pDocName );
    map<Gui::Document*, DocItem*>::iterator it = DocMap.find( pDoc );

    if(it!= DocMap.end())
    {
  
      if(Reason.Type == SelectionChanges::SetPreselect)
       it->second->highlightFeature(Reason.pObjectName,true);
      else  if(Reason.Type == SelectionChanges::RmvPreselect)
       it->second->highlightFeature(Reason.pObjectName,false);
      else if(Reason.Type == SelectionChanges::AddSelection)
       it->second->selectFeature(Reason.pObjectName,true);
      else  if(Reason.Type == SelectionChanges::RmvSelection)
       it->second->selectFeature(Reason.pObjectName,false);
    }
  }else{
    // clears the complete selection
    if ( strcmp(Reason.pDocName,"") == 0 ) {
      for (std::map<Gui::Document*,DocItem*>::iterator pos = DocMap.begin();pos!=DocMap.end();++pos)
      {
        pos->second->clearSelection();
      }

      _pcListView->clearSelection ();
    } else {
      // clears the selection of the given document
      Gui::Document* pDoc = Application::Instance->getDocument(Reason.pDocName);
      std::map<Gui::Document*, DocItem*>::iterator pos = DocMap.find(pDoc);
      if ( pos != DocMap.end() ) {
        pos->second->clearSelection();
      }
    }

    _pcListView->update();
  }

  bFromOutside = false;
}



void TreeView::onSelectionChanged ()
{
  if(bFromOutside) return;

  //Base::Console().Log("Sel : QListView::selectionChanged()");

  for (std::map<Gui::Document*,DocItem*>::iterator pos = DocMap.begin();pos!=DocMap.end();++pos)
  {
    pos->second->isSelectionUptodate();
  }

}

void TreeView::onItem(Q3ListViewItem* item)
{
  // feature item selected
  if ( item && item->rtti() == 3100 )
  {
    ObjectItem* feat = reinterpret_cast<ObjectItem*>(item);
    feat->displayStatusInfo();
  }
}

void TreeView::onUpdate(void)
{
#ifdef FC_LOGUPDATECHAIN
  Base::Console().Log("Acti: Gui::TreeView::onUpdate()");
#endif

  // quick and dirty so far
//  delete _pcMainItem;
//  _pcMainItem = new TreeLabel(this);
}

void TreeView::onRename(Gui::Document *pDoc)
{
  std::map<Gui::Document*,DocItem*>::iterator pos = DocMap.find( pDoc );
  if ( pos != DocMap.end() ) {
    pos->second->rename();
  }
}

bool TreeView::onMsg(const char* pMsg)
{
  //printf("MsgTree: %s View: %p\n",pMsg,this);

  // no messages yet
  return false;
}

DocItem * TreeView::NewDoc( Gui::Document* pDoc )
{
  // set the new ctreated item at the end
  DocItem *item = new DocItem(_pcMainItem,_lastDocItem,pDoc);
  _lastDocItem = item;
  DocMap[ pDoc ] = item;
  return item;
}

void TreeView::DeleteDoc( Gui::Document* pDoc )
{
  std::map<Gui::Document*,DocItem*>::iterator it = DocMap.find( pDoc );
  if ( it != DocMap.end() )
  {
    // now we must search for the matching listview item
    Q3ListViewItem* item = _pcMainItem->firstChild();
    Q3ListViewItem* sibling=0;
    while ( item ) {
      // make sure that we have a ObjectItem
      if ( item == it->second )
      {
        // in case we remove the last item
        if ( _lastDocItem == item )
          _lastDocItem = sibling;
        delete item;
        DocMap.erase(it);
        return;
      }

      sibling = item;
      item = item->nextSibling();  
    }
  }
}


#include "moc_Tree.cpp"

