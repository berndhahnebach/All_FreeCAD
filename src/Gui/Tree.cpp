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

#include "../Base/Console.h"


using namespace Gui;

GUIDDefs AttrNames[] = {
  {0,0}
};


TreeLabel::TreeLabel( TreeView * parent)
  :QListViewItem(parent->_pcListView), _pcDocument(parent->_pcDocument)
{
  if(_pcDocument){
    setText(0,QObject::tr("Main Label"));
    setPixmap(0,*TreeView::pcLabelOpen);
  //  _hcLabel = parent->_pcDocument->GetDocument()->Main();
    _hcTDFLabel = parent->_pcDocument->getDocument()->Main();
    buildUp();
    setOpen(true);
  }else{
    setPixmap(0,*TreeView::pcLabelClosed);
    //setPixmap(new QPixmap(px));
    setText(0,QObject::tr("No Active Document"));
  }
}

/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
TreeLabel::TreeLabel( TreeLabel * parent, TDF_Label &hcLabel )
    : QListViewItem( parent ),
  _hcTDFLabel(hcLabel),
  _pcDocument(parent->_pcDocument)
{
  QString cString;

  cString.sprintf("Tag:%d",_hcTDFLabel.Tag());
  setPixmap(0,*TreeView::pcLabelClosed);
  setText(0,cString);

  buildUp();
}

void TreeLabel::update(void)
{
  //puts("Updtate");


  /*
  // quieck an dirty
  if(_pcDocument && _hcLabel->GetOCCLabel().HasChild() && !isOpen())
  {
    //for(QListViewItem* pItem = firstChild (); pItem!=0 ; pItem = pItem->nextSibling () )
    //  delete pItem;

    std::vector<FCPyHandle<FCLabel> > vpcLabels = _hcLabel->GetLabels();

    for(std::vector<FCPyHandle<FCLabel> >::iterator It=vpcLabels.begin();It != vpcLabels.end(); It++)
    {
      new TreeLabel(this,*It);
    }

    setPixmap(0,*FCTree::pcLabelOpen);

  }
*/

}

void TreeLabel::buildUp(void)
{

/*  std::vector<FCPyHandle<FCLabel> > vpcLabels = _hcLabel->GetLabels();

  for(std::vector<FCPyHandle<FCLabel> >::reverse_iterator It=vpcLabels.rbegin();It != vpcLabels.rend(); It++)
  {
    new TreeLabel(this,*It);
  }

  TDF_Label l = _hcLabel->GetOCCLabel();

  if( l.IsAttribute(TNaming_NamedShape::GetID()) )
    (new QListViewItem(this,"Shape","1"))->setPixmap(0,*FCTree::pcAttribute);
  if( l.IsAttribute(TDataStd_Integer::GetID()) )
    (new QListViewItem(this,"Int","1"))->setPixmap(0,*FCTree::pcAttribute);
  if( l.IsAttribute(TDataStd_Name::GetID()) )
    (new QListViewItem(this,"String","1"))->setPixmap(0,*FCTree::pcAttribute);


  if(childCount()>0)
    setExpandable( TRUE );
*/
  }

void TreeLabel::setOpen( bool o )
{
  //puts("setOpen");

  if( o )
  {
    setPixmap(0,*TreeView::pcLabelOpen);

    update();

  }else{
    setPixmap(0,*TreeView::pcLabelClosed);
  }

  QListViewItem::setOpen ( o );
}


void TreeLabel::setup()
{
  //setExpandable( TRUE );
  QListViewItem::setup();
}

void TreeLabel::activate ()
{
  //puts("Activated");
}


QPixmap* TreeView::pcLabelOpen=0;
QPixmap* TreeView::pcLabelClosed=0;
QPixmap* TreeView::pcAttribute=0;


TreeView::TreeView(Gui::Document* pcDocument,QWidget *parent,const char *name)
  :DockView(pcDocument,parent,name)
{
  QGridLayout* layout = new QGridLayout( this );
  _pcListView = new QListView(this,name);
  layout->addWidget( _pcListView, 0, 0 );

  // set defaults and the colums
  _pcListView->setSorting(-1,false);
  _pcListView->addColumn(tr("Labels & Attr."));
  _pcListView->setColumnWidthMode(0,QListView::Maximum);
//  _pcListView->addColumn(tr("Value"));
//  _pcListView->setColumnWidthMode(1,QListView::Manual );

  // retreive the Pixmaps
  pcLabelOpen   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));
  pcLabelClosed = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelOpen"));
  pcAttribute   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_Attr"));


  // Add the first main label
  _pcMainItem = new TreeLabel(this);

  //_pcListView->setRootIsDecorated(true);



  //_pcListView->setSize(200,400);
  resize( 200, 400 );

}

void TreeView::onUpdate(void)
{
  Base::Console().Log("Tree Updated\n");

  // quick and dirty so far
  delete _pcMainItem;
  _pcMainItem = new TreeLabel(this);
}

void TreeView::onNewDocument(Gui::Document* pcOldDocument,Gui::Document* pcNewDocument)
{
//  Console().Log("Tree doc activated %p\n",pcNewDocument);

  if(pcOldDocument != pcNewDocument)
  {
    delete _pcMainItem;
    _pcMainItem = new TreeLabel(this);
  }
}

bool TreeView::onMsg(const char* pMsg)
{
  //printf("MsgTree: %s View: %p\n",pMsg,this);

  // no messages yet
  return false;
}


#include "moc_Tree.cpp"

