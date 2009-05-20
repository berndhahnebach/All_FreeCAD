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
#include "TaskView/TaskView.h"
#include "propertyeditor/PropertyEditor.h"

using namespace Gui;
using namespace Gui::DockWnd;


CombiView::CombiView(Gui::Document* pcDocument, QWidget *parent)
  : DockWindow(pcDocument,parent)
{
    setWindowTitle(tr("CombiView"));

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

    // tree widget
    tree =  new TreeWidget(this);
    tree->setRootIsDecorated(false);
    splitter->addWidget(tree);

    // property view
    prop = new PropertyView(this);
    splitter->addWidget(prop);

    tabs->addTab(splitter,trUtf8("Project"));

    taskPanel = new Gui::TaskView::TaskView(this);
    //iisTaskBox *tb1 = new iisTaskBox(Gui::BitmapFactory().pixmap("document-new"),
    //                                 QLatin1String("Group of Tasks"),
    //                                 true, this);
    //taskPanel->setScheme(iisFreeCADTaskPanelScheme::defaultScheme());
    //tb1      ->setScheme(iisFreeCADTaskPanelScheme::defaultScheme());
    //taskPanel->addWidget(tb1);
    tabs->addTab(taskPanel, trUtf8("Tasks"));
}

CombiView::~CombiView()
{
}

#include "moc_CombiView.cpp"
