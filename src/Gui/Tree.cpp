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

#include "../App/Document.h"
#include "../App/Label.h"
#include "../App/Feature.h"

#include "../Base/Console.h"
using Base::Console;

using namespace Gui;


/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
DocItem::DocItem( QListViewItem* parent,Gui::Document* doc)
    : QListViewItem( parent ),
  _pcDocument(doc)
{
  setPixmap(0,*TreeView::pcLabelOpen);
  setText(0,QString(_pcDocument->getDocument()->getName()));

  _pcDocument->getDocument()->Attach(this);

  buildUp();
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

void DocItem::OnChange(App::Document::SubjectType &rCaller,App::Document::MessageType Reason)
{
#ifdef FC_LOGUPDATECHAIN
  Base::Console().Log("Acti: Gui::DocItem::OnChange)");
#endif

  // remove the representation of Features no longer exist
  std::set<App::Feature*>::iterator It;
  for(It=Reason.DeletedFeatures.begin();It!=Reason.DeletedFeatures.end();It++)
  {
    std::map<App::Feature*,FeatItem*>::iterator pos;
    pos = FeatMap.find(*It);
  
    if(pos == FeatMap.end())
      delete FeatMap[*It];
  }

  // set up new providers
  for(It=Reason.NewFeatures.begin();It!=Reason.NewFeatures.end();It++)
  {
    FeatItem *item = new FeatItem(this,*It);
    FeatMap[*It] = item;
  }

  // update recalculated features
  for(It=Reason.UpdatedFeatures.begin();It!=Reason.UpdatedFeatures.end();It++)
  {
  }

}

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

  buildUp();
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
  :DockView(pcDocument,parent,name)
{
  QGridLayout* layout = new QGridLayout( this );
  _pcListView = new QListView(this,name);
  layout->addWidget( _pcListView, 0, 0 );

  // set defaults and the colums
  _pcListView->setSorting(-1,false);
  _pcListView->addColumn(tr("Labels & Attributes"));
  _pcListView->setColumnWidthMode(0,QListView::Maximum);
  _pcListView->setSorting(0,true);
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



  //_pcListView->setSize(200,400);
  resize( 200, 400 );

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
  Console().Log("TreeView::onNewDocument() activated %p\n",pcNewDocument);

  std::map<Gui::Document*,DocItem*>::iterator pos;

  if(pcNewDocument)
  {
    pos = DocMap.find(pcNewDocument);
  
    if(pos == DocMap.end())
    {
      DocItem *item = new DocItem(_pcMainItem,pcNewDocument);
      DocMap[pcNewDocument] = item;
    }
  }

  if(pcOldDocument)
    DocMap[pcOldDocument]->setOpen(false);
  if(pcNewDocument)
    DocMap[pcNewDocument]->setOpen(true);
  
}

bool TreeView::onMsg(const char* pMsg)
{
  //printf("MsgTree: %s View: %p\n",pMsg,this);

  // no messages yet
  return false;
}


#include "moc_Tree.cpp"

