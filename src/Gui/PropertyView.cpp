/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <qdatetime.h>
# include <qlayout.h>
# include <qtabwidget.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "PropertyView.h"
#include "BitmapFactory.h"
//#include "../App/Property.h"

#include "propertyeditor/propertyeditor.h"
#include "propertyeditor/propertyeditorlist.h"
#include "propertyeditor/propertyeditorfile.h"
#include "propertyeditor/propertyeditorfont.h"
#include "propertyeditor/propertyeditorinput.h"
#include "propertyeditor/propertyeditordate.h"

using namespace Gui;
using namespace Gui::DockWnd;
using namespace Gui::PropertyEditor;


QPixmap* PropertyView::pcLabelOpen=0;
QPixmap* PropertyView::pcLabelClosed=0;
QPixmap* PropertyView::pcAttribute=0;

/* TRANSLATOR Gui::DockWnd::PropertyView */

PropertyView::PropertyView(Gui::Document* pcDocument,QWidget *parent,const char *name)
  :DockView(pcDocument,parent,name)
{
  setCaption( tr( "Property View" ) );

  QGridLayout* pLayout = new QGridLayout( this ); 
  pLayout->setSpacing( 0 );
  pLayout->setMargin ( 0 );

  QTabWidget* pTabs = new QTabWidget (this);
  pTabs->setTabPosition(QTabWidget::Bottom);
  pTabs->setTabShape(QTabWidget::Triangular);
  pLayout->addWidget( pTabs, 0, 0 );

  _pPropEditor = new EditableListView( pTabs );
  pTabs->insertTab(_pPropEditor, "Properties");
  _pPropEditor->addColumn("Name");
  _pPropEditor->addColumn( "Value" );

  // retrieve the Pixmaps
  pcLabelOpen   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));
  pcLabelClosed = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelOpen"));
  pcAttribute   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_Attr"));

  //_pcListView->setSize(200,400);
  resize( 200, 400 );

  onUpdate();
}

PropertyView::~PropertyView()
{
}

void PropertyView::onUpdate(void)
{

#ifdef FC_LOGUPDATECHAIN
  Base::Console().Log("Acti: Gui::PropertyView::onUpdate()");
#endif


/*
  PropertyBuffer* buf = new PropertyBuffer(_pPropEditor, "Test");

  // append sample properties
  buf->add(new Property("Boolean", QVariant(true,1)));
  buf->add(new Property("Integer", 9));
  buf->add(new Property("String", QString("Hallo")));
  buf->add(new Property("Double", 3.14));
  buf->add(new Property("Font", QFont()));
  buf->add(new Property("Pixmap", QPixmap()));
  buf->add(new Property("Color", Qt::blue));
  buf->add(new Property("Time", QTime()));
  buf->add(new Property("Cursor", QCursor()));
  buf->add(new Property("CString", QCString("Hallo")));
  buf->add(new Property("Date time", QDateTime()));
  buf->add(new Property("Date", QDate()));
  QStringList l; l << "Hallo" << "Hallo2";
  buf->add(new Property("List", "Test", l, l, "Descr."));
  _pPropEditor->setBuffer(buf);
*/
  if ( _pPropEditor->childCount() > 0)
    return;
  QStringList lst; lst << QString("This") << QString("is") << QString("my") << QString("first") << QString("list") << QString("first");
  new ListEditorItem(_pPropEditor, QString("List"), lst);
  new DateEditorItem(_pPropEditor, QString("Date"), QDate::currentDate ());
  new DateTimeEditorItem(_pPropEditor, QString("Datetime"), QDateTime::currentDateTime ());
  new FileEditorItem(_pPropEditor, QString("File name"), QCString());
  new CursorEditorItem(_pPropEditor, QString("Cursor"), QCursor(Qt::SplitVCursor));
  new TimeEditorItem(_pPropEditor, QString("Time"), QTime::currentTime ());
  new ColorEditorItem(_pPropEditor, QString("Color"), Qt::red);
  new ChildrenEditorItem(_pPropEditor, QString("Children"), 1);
  new PixmapEditorItem( _pPropEditor, QString("Pixmap"), QPixmap() );
  new FontEditorItem( _pPropEditor, QString("Font"), QFont() );
  new FloatEditorItem( _pPropEditor, QString("Float"), 0.85f );
  new TextEditorItem( _pPropEditor, QString("Text"), QString("Example text") );
  new IntEditorItem( _pPropEditor, QString("Integer"), 2 );
  new BoolEditorItem( _pPropEditor, QString("Boolean"), QVariant(true, 0) );
//  Base::Console().Log("Property Updated\n");
}

void PropertyView::onNewDocument(Gui::Document* pcOldDocument,Gui::Document* pcNewDocument)
{
}

bool PropertyView::onMsg(const char* pMsg)
{
  // no messages yet
  return false;
}

//#include "moc_PropertyView.cpp"
