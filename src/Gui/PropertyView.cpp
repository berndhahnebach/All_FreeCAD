/** \file PropertyView.cpp
 *  \brief Framework to show and work with all kind of properies
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) J�rgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#	include <vector>
#	include <qaction.h>
#	include <qbutton.h>
#	include <qbuttongroup.h>
#	include <qcolordialog.h>
#	include <qcombobox.h>
#	include <qcursor.h>
#	include <qguardedptr.h>
#	include <qhbox.h>
#	include <qheader.h>
#	include <qinputdialog.h>
#	include <qlabel.h>
#	include <qlayout.h>
#	include <qlistbox.h>
#	include <qmessagebox.h>
#	include <qtextbrowser.h>
#	include <qobjectlist.h>
#	include <qpainter.h>
#	include <qprocess.h>
#	include <qstyle.h>
#	include <qtabbar.h>
#	include <qtabwidget.h>
#	include <qthread.h>
#	include <qurl.h>
#	include <qvalidator.h>
#	include <qwhatsthis.h>
#	include <qworkspace.h>
#	include <ctype.h>
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Widgets.h"
#include "PropertyView.h"
#include "Tools.h"

#include "Application.h"
#include "HtmlView.h"
#include "BitmapFactory.h"
#include "../Base/Console.h"
#include "../App/Property.h"

#include "propertyeditor/kexipropertyeditor.h"
#include "propertyeditor/kexipropertybuffer.h"

using Gui::Kexi::PropertyEditor;
using Gui::Kexi::PropertyBuffer;
using Gui::Kexi::Property;

//**************************************************************************
//**************************************************************************
// FCPropertyView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

QPixmap* FCPropertyView::pcLabelOpen=0;
QPixmap* FCPropertyView::pcLabelClosed=0;
QPixmap* FCPropertyView::pcAttribute=0;


//**************************************************************************
// Construction/Destruction
FCPropertyView::FCPropertyView(FCGuiDocument* pcDocument,QWidget *parent,const char *name)
	:FCDockView(pcDocument,parent,name)
{
  setCaption( tr( "Property View" ) );

  QGridLayout* pLayout = new QGridLayout( this ); 
  pLayout->setSpacing( 0 );
  pLayout->setMargin ( 0 );

	QTabWidget* pTabs = new QTabWidget (this);
	pTabs->setTabPosition(QTabWidget::Bottom);
	pTabs->setTabShape(QTabWidget::Triangular);
  pLayout->addWidget( pTabs, 0, 0 );

	_pPropEditor = new PropertyEditor( pTabs );
	pTabs->insertTab(_pPropEditor, "Properties");

	// retrieve the Pixmaps
	pcLabelOpen   = new QPixmap(Gui::BitmapFactory().GetPixmap("RawTree_LabelClosed"));
	pcLabelClosed = new QPixmap(Gui::BitmapFactory().GetPixmap("RawTree_LabelOpen"));
	pcAttribute   = new QPixmap(Gui::BitmapFactory().GetPixmap("RawTree_Attr"));

	//_pcListView->setSize(200,400);
  resize( 200, 400 );

	Update();
}

FCPropertyView::~FCPropertyView()
{
}

void FCPropertyView::Update(void)
{
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

//	Base::Console().Log("Property Updated\n");
}

void FCPropertyView::OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument)
{
}

bool FCPropertyView::OnMsg(const char* pMsg)
{
	// no messages yet
	return false;
}

//#include "moc_PropertyView.cpp"
