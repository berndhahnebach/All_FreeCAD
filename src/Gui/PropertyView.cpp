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

/// Here the FreeCAD includes sorted by Base,App,Gui......

#include <App/PropertyStandard.h>
#include <App/PropertyGeo.h>
#include <App/PropertyLinks.h>
#include <App/PropertyContainer.h>
#include <App/Feature.h>
#include <App/Document.h>

#include "PropertyView.h"
#include "Application.h"
#include "Document.h"
#include "BitmapFactory.h"
#include "ViewProvider.h"

#include "propertyeditor/propertyeditor.h"
#include "propertyeditor/propertyeditorlist.h"
#include "propertyeditor/propertyeditorfile.h"
#include "propertyeditor/propertyeditorfont.h"
#include "propertyeditor/propertyeditorinput.h"
#include "propertyeditor/propertyeditordate.h"

using namespace std;
using namespace Gui;
using namespace Gui::DockWnd;
using namespace Gui::PropertyEditor;


QPixmap* PropertyView::pcLabelOpen=0;
QPixmap* PropertyView::pcLabelClosed=0;
QPixmap* PropertyView::pcAttribute=0;

/* TRANSLATOR Gui::DockWnd::PropertyView */

PropertyView::PropertyView(Gui::Document* pcDocument, QWidget *parent)
  : DockWindow(pcDocument,parent)
{
    setWindowTitle( tr( "Property View" ) );

    QGridLayout* pLayout = new QGridLayout( this ); 
    pLayout->setSpacing( 0 );
    pLayout->setMargin ( 0 );

    tabs = new QTabWidget (this);
    tabs->setObjectName(QString::fromUtf8("propertyTab"));
    tabs->setTabPosition(QTabWidget::South);
    tabs->setTabShape(QTabWidget::Triangular);
    pLayout->addWidget( tabs, 0, 0 );

    propertyEditorView = new Gui::PropertyEditor::PropertyEditor();
    tabs->addTab(propertyEditorView, trUtf8("View"));
    propertyEditorData = new Gui::PropertyEditor::PropertyEditor();
    tabs->addTab(propertyEditorData, trUtf8("Data"));


    // retrieve the Pixmaps
    pcLabelOpen   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelOpen"));
    pcLabelClosed = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));
    pcAttribute   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_Attr"));

    //_pcListView->setSize(200,400);
    resize( 200, 400 );

    onUpdate();

    Gui::Selection().Attach(this);
}

PropertyView::~PropertyView()
{
  Gui::Selection().Detach(this);
  delete pcLabelOpen;
  delete pcLabelClosed;
  delete pcAttribute;
}


void PropertyView::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                            Gui::SelectionSingleton::MessageType Reason)
{
  //FIXME: We get notified for each object that has changed its selection status. It is sufficient to be called only once.
  //

  if(Reason.Type == SelectionChanges::AddSelection || Reason.Type == SelectionChanges::RmvSelection || Reason.Type == SelectionChanges::ClearSelection)
  {
    // We must not listen neither to the given rCaller nor to the Reason object only, because there can be selected items from
    // documents other than the currently active one.

    // group the properties by <name,id>
    std::map<std::pair<std::string, int>, std::vector<App::Property*> > propDataMap;
    std::map<std::pair<std::string, int>, std::vector<App::Property*> > propViewMap;
    vector<SelectionSingleton::SelObj> array = Gui::Selection().getCompleteSelection();
    for ( vector<SelectionSingleton::SelObj>::const_iterator it = array.begin(); it != array.end(); ++it )
    {
      std::map<std::string,App::Property*> dataMap;
      std::map<std::string,App::Property*> viewMap;
      if ( (*it).pObject ) {
        (*it).pObject->getPropertyMap(dataMap);

        // get also the properties of the associated view provider
        Gui::Document* doc = Gui::Application::Instance->getDocument(it->pDoc);
        ViewProvider* vp = doc->getViewProvider((*it).pObject);
        if(!vp) continue;
        vp->getPropertyMap(viewMap);
      }
      else if ( (*it).pDoc ) {
        (*it).pDoc->getPropertyMap(dataMap);
      }

      // store the properties with <name,id> as key in a map
      std::map<std::string,App::Property*>::iterator pt;
      for( pt = dataMap.begin(); pt != dataMap.end(); ++pt )
      {
        std::pair<std::string, int> nameType = std::make_pair<std::string, int>( pt->first, pt->second->getTypeId().getKey());
        propDataMap[nameType].push_back(pt->second);
      }
      // the same for the view properties
      for( pt = viewMap.begin(); pt != viewMap.end(); ++pt )
      {
        std::pair<std::string, int> nameType = std::make_pair<std::string, int>( pt->first, pt->second->getTypeId().getKey());
        propViewMap[nameType].push_back(pt->second);
      }
    }

    propertyEditorView->buildUp(propViewMap, array.size());
    propertyEditorData->buildUp(propDataMap, array.size());
  }
}

void PropertyView::onUpdate(void)
{
}

bool PropertyView::onMsg(const char* pMsg)
{
  // no messages yet
  return false;
}

void PropertyView::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::LanguageChange) {
    tabs->setTabText(0, trUtf8("View"));
    tabs->setTabText(1, trUtf8("Data"));
  }

  DockWindow::changeEvent(e);
}

#include "moc_PropertyView.cpp"
