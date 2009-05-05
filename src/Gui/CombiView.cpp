/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
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

#include "CombiView.h"
#include "BitmapFactory.h"
#include "iisTaskPanel/include/iisTaskPanel"
#include "PropertyView.h"
#include "Application.h"
#include "Document.h"
#include "ViewProvider.h"
#include "Tree.h"
#include "propertyeditor/PropertyEditor.h"

using namespace Gui;
using namespace Gui::DockWnd;


CombiView::CombiView(Gui::Document* pcDocument, QWidget *parent)
  : DockWindow(pcDocument,parent)
{
    setWindowTitle(tr( "CombiView"));

    QGridLayout* pLayout = new QGridLayout( this ); 
    pLayout->setSpacing( 0 );
    pLayout->setMargin ( 0 );

    tabs = new QTabWidget ();
    tabs->setObjectName(QString::fromUtf8("combiTab"));
    tabs->setTabPosition(QTabWidget::North);
    //tabs->setTabShape(QTabWidget::Triangular);
    pLayout->addWidget( tabs, 0, 0 );


    QSplitter *splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    tree =  new TreeWidget();
    tree->setRootIsDecorated(false);
    
    splitter->addWidget(tree);
    
    proptertyTabs = new QTabWidget ();
    proptertyTabs->setObjectName(QString::fromUtf8("propertyTabs"));
    proptertyTabs->setTabPosition(QTabWidget::South);
    //proptertyTabs->setTabShape(QTabWidget::Triangular);
    pLayout->addWidget( proptertyTabs, 0, 0 );
    propertyEditorView = new Gui::PropertyEditor::PropertyEditor();
    proptertyTabs->addTab(propertyEditorView, trUtf8("View"));
    propertyEditorData = new Gui::PropertyEditor::PropertyEditor();
    proptertyTabs->addTab(propertyEditorData, trUtf8("Data"));

    splitter->addWidget(proptertyTabs);

    tabs->addTab(splitter,trUtf8("Project"));

    taskPanel = new iisTaskPanel(this);
    iisTaskBox *tb1 = new iisTaskBox(Gui::BitmapFactory().pixmap("document-new"),
                                     QLatin1String("Group of Tasks"),
                                     true, this);
    taskPanel->setScheme(iisFreeCADTaskPanelScheme::defaultScheme());
    tb1      ->setScheme(iisFreeCADTaskPanelScheme::defaultScheme());
    taskPanel->addWidget(tb1);
    tabs->addTab(taskPanel, trUtf8("Tasks"));

 

    onUpdate();

    //Gui::Selection().Attach(this);
}

CombiView::~CombiView()
{
    Gui::Selection().Detach(this);
}

void CombiView::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                            Gui::SelectionSingleton::MessageType Reason)
{
    std::string temp;

    if (Reason.Type == SelectionChanges::AddSelection )
    {
    }
    else if (Reason.Type == SelectionChanges::ClrSelection)
    {
    }
    else if (Reason.Type == SelectionChanges::RmvSelection ) 
    {
    }
}

void CombiView::onUpdate(void)
{
}

bool CombiView::onMsg(const char* pMsg)
{
    // no messages yet
    return false;
}

#include "moc_CombiView.cpp"
