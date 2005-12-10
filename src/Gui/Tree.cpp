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
#	include <TDataStd_Integer.hxx>
#	include <TDataStd_Name.hxx>
#	include <TNaming_NamedShape.hxx>
#	include <qheader.h>
# include <qlayout.h>
#endif

#include "Tree.h"
#include "Document.h"
#include "BitmapFactory.h"
#include "ViewProviderFeature.h"

#include "../App/Document.h"
#include "../App/Label.h"
#include "../App/Feature.h"

#include "../Base/Console.h"
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
DocItem::DocItem( QListViewItem* parent,Gui::Document* doc)
    : QListViewItem( parent ), _pcDocument(doc)
{
  setPixmap(0,*TreeView::pcLabelOpen);
  setText(0,QString(_pcDocument->getDocument()->getName()));

  bHighlight = false;
  bSelected = false;

  buildUp();
}

DocItem::~DocItem()
{
  //_pcDocument->Detach(this);
}

void DocItem::highlightFeature(const char* name, bool bOn)
{
  std::map<string,FeatItem*>::iterator pos;
  pos = FeatMap.find(name);
  
  if(pos != FeatMap.end())
    pos->second->highlightFeature(bOn);

  bHighlight = bOn;
  if (!isOpen() )
    repaint();

}

void DocItem::selectFeature(const char* name, bool bOn)
{
  std::map<string,FeatItem*>::iterator pos;
  pos = FeatMap.find(name);
  
  if(pos != FeatMap.end())
    pos->second->selectFeature(bOn);


}
void DocItem::clearSelection(void)
{
  for (std::map<string,FeatItem*>::iterator pos = FeatMap.begin();pos!=FeatMap.end();++pos)
  {
    pos->second->bSelected = false;
    pos->second->setSelected(false);
    pos->second->repaint();
  }
}

void DocItem::isSelectionUptodate(void)
{
  for (std::map<string,FeatItem*>::iterator pos = FeatMap.begin();pos!=FeatMap.end();++pos)
  {
    if(pos->second->bSelected != pos->second->isSelected())
    {
      if(pos->second->bSelected)
      {
        Base::Console().Log("Sel : Rmv tree selection (%s,%s)\n",_pcDocument->getDocument()->getName(),pos->first.c_str());
        Gui::Selection().rmvSelection(_pcDocument->getDocument()->getName(),pos->first.c_str());
        //pos->second->bSelected = false;
      }else{

        Base::Console().Log("Sel : Add tree selection (%s,%s)\n",_pcDocument->getDocument()->getName(),pos->first.c_str());
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

   QListViewItem::paintCell( p, _cg, column, width, align );

   _cg.setColor( QColorGroup::Text, c );

}



void DocItem::addViewProviderFeature(ViewProviderFeature* Provider)
{
  FeatMap[Provider->getFeature()->getName()] = dynamic_cast<FeatItem*>( Provider->getTreeItem(this) );
}

void DocItem::removeViewProviderFeature(ViewProviderFeature* Provider)
{

  // to implement
  assert(0);
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
  //puts("setOpen");

  if( o )
    setPixmap(0,*TreeView::pcLabelOpen);
  else
    setPixmap(0,*TreeView::pcLabelClosed);
 
  update();

  QListViewItem::setOpen ( o );
}


void DocItem::setup()
{
  //setExpandable( TRUE );
  QListViewItem::setup();
}

void DocItem::activate ()
{
  //puts("Activated");
}


//**************************************************************************
// FeatItem
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
FeatItem::FeatItem( QListViewItem* parent,App::Feature* pcFeat)
    : QListViewItem( parent ),
  _pcFeature(pcFeat)
{
  setPixmap(0,*TreeView::pcLabelOpen);
  setText(0,QString(pcFeat->getName()));

  bHighlight = false;
  bSelected = false;

  buildUp();
}

void FeatItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
   QColorGroup _cg( cg );
   QColor c = _cg.text();

   if ( bHighlight )
       _cg.setColor( QColorGroup::Base , QColor (200,200,255));

   QListViewItem::paintCell( p, _cg, column, width, align );

   _cg.setColor( QColorGroup::Text, c );

}

void FeatItem::highlightFeature(bool bOn)
{
  bHighlight = bOn;
  repaint();
}

void FeatItem::selectFeature(bool bOn)
{
  setSelected(bOn);
  bSelected =bOn;
  repaint();
}

void FeatItem::update(void)
{
  //puts("Updtate");

}

void FeatItem::buildUp(void)
{

}

void FeatItem::setOpen( bool o )
{

  if( o )
    setPixmap(0,*TreeView::pcLabelOpen);
  else
    setPixmap(0,*TreeView::pcLabelClosed);
 
  update();

  QListViewItem::setOpen ( o );
}


void FeatItem::setup()
{
  //setExpandable( TRUE );
  QListViewItem::setup();
}

void FeatItem::activate ()
{
  //puts("Activated");
}




QPixmap* TreeView::pcLabelOpen=0;
QPixmap* TreeView::pcLabelClosed=0;
QPixmap* TreeView::pcAttribute=0;

/* TRANSLATOR Gui::TreeView */
TreeView::TreeView(Gui::Document* pcDocument,QWidget *parent,const char *name)
  :DockView(pcDocument,parent,name),bFromOutside(false)
{
  QGridLayout* layout = new QGridLayout( this );
  _pcListView = new QListView(this,name);
  layout->addWidget( _pcListView, 0, 0 );

  // set defaults and the colums
  //_pcListView->setSorting(-1,false);
  _pcListView->addColumn(tr("Labels & Attributes"));
  _pcListView->setColumnWidthMode(0,QListView::Maximum);
  //_pcListView->setSorting(0,true);
//  _pcListView->addColumn(tr("Value"));
//  _pcListView->setColumnWidthMode(1,QListView::Manual );

  // retreive the Pixmaps
  pcLabelOpen   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelOpen"));
  pcLabelClosed = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));
  pcAttribute   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_Attr"));


  // Add the first main label
  _pcMainItem = new QListViewItem(_pcListView,"Application");
  _pcMainItem->setOpen(true);

  //_pcListView->setRootIsDecorated(true);

  _pcListView->setSelectionMode(QListView::Extended );
  connect( _pcListView, SIGNAL( selectionChanged() ), this, SLOT( selectionChanged() ) );


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
}

// Observer message from the Selection
void TreeView::OnChange(Gui::SelectionSingelton::SubjectType &rCaller,Gui::SelectionSingelton::MessageType Reason)
{
  bFromOutside = true;
  if(Reason.Type != SelectionChanges::ClearSelection)
  {
    map<string, DocItem*>::iterator it = DocMap.find( Reason.pDocName );

    if(it!= DocMap.end())
    {
  
      if(Reason.Type == SelectionChanges::SetPreselect)
       it->second->highlightFeature(Reason.pFeatName,true);
      else  if(Reason.Type == SelectionChanges::RmvPreselect)
       it->second->highlightFeature(Reason.pFeatName,false);
      else if(Reason.Type == SelectionChanges::AddSelection)
       it->second->selectFeature(Reason.pFeatName,true);
      else  if(Reason.Type == SelectionChanges::RmvSelection)
       it->second->selectFeature(Reason.pFeatName,false);
    }
  }else{
    for (std::map<string,DocItem*>::iterator pos = DocMap.begin();pos!=DocMap.end();++pos)
    {
      pos->second->clearSelection();
    }

    _pcListView->clearSelection ();
    _pcListView->update();
  }

  bFromOutside = false;
}



void TreeView::selectionChanged ()
{
  if(bFromOutside) return;

  Base::Console().Log("Sel : QListView::selectionChanged()");

  for (std::map<string,DocItem*>::iterator pos = DocMap.begin();pos!=DocMap.end();++pos)
  {
    pos->second->isSelectionUptodate();
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

void TreeView::onNewDocument(Gui::Document* pcOldDocument,Gui::Document* pcNewDocument)
{

//  if(pcOldDocument && DocMap.find(pcOldDocument->getDocument()) != DocMap.end())
//    DocMap[pcOldDocument->getDocument()]->setOpen(false);
//  if(pcNewDocument && DocMap.find(pcNewDocument->getDocument()) != DocMap.end())
//    DocMap[pcNewDocument->getDocument()]->setOpen(true);
}

bool TreeView::onMsg(const char* pMsg)
{
  //printf("MsgTree: %s View: %p\n",pMsg,this);

  // no messages yet
  return false;
}

 DocItem * TreeView::NewDoc( Gui::Document* pDoc )
{
  DocItem *item = new DocItem(_pcMainItem,pDoc);
  DocMap[ pDoc->getDocument()->getName() ] = item;
  return item;
}

void TreeView::DeleteDoc( Gui::Document* pDoc )
{
  map<string, DocItem*>::iterator it = DocMap.find( pDoc->getDocument()->getName() );
  if(it!= DocMap.end())
  {
    DocMap.erase( it );
    delete it->second;
  }
}


#include "moc_Tree.cpp"

