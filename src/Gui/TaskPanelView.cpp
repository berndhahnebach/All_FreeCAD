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

#include "TaskPanelView.h"
#include "Application.h"
#include "Document.h"
#include "BitmapFactory.h"
#include "ViewProvider.h"


#include <3rdParty/iisTaskPanel/include/iisTaskPanel>

using namespace std;
using namespace Gui;
using namespace Gui::DockWnd;




TaskPanelView::TaskPanelView(Gui::Document* pcDocument, QWidget *parent)
  : DockWindow(pcDocument,parent)
{
    setWindowTitle( tr( "Task View" ) );

	//ui.setupUi(this);

	iisTaskBox *tb1 = new iisTaskBox(
		QPixmap(QString::fromAscii("D:/# Projekte/FreeCAD/FreeCAD_0.7/src/3rdParty/iisTaskPanel/tests/styles/images/win/filenew.png")),QString::fromAscii("Group of Tasks"),true, this);
	//ui.panel->addWidget(tb1);
	//this->addWidget(tb1);

	iisIconLabel *i1 = new iisIconLabel(QPixmap(QString::fromAscii(":/images/win/zoomin.png")),QString::fromAscii( "Do Task 1"), tb1);
	tb1->addIconLabel(i1);
	connect(i1, SIGNAL(activated()), this, SLOT(task1()));
	//tb1->groupLayout()->addWidget(i1);
	iisIconLabel *i2 = new iisIconLabel(QPixmap(QString::fromAscii(":/images/win/zoomout.png")),QString::fromAscii( "Do Task 2"), tb1);
	tb1->addIconLabel(i2);
	//tb1->groupLayout()->addWidget(i2);

	QHBoxLayout *hbl = new QHBoxLayout();
	tb1->groupLayout()->addLayout(hbl);

	iisIconLabel *i3 = new iisIconLabel(QPixmap(QString::fromAscii(":/images/win/editcopy.png")), QString::fromAscii("Do Task 3"), tb1);
	tb1->addIconLabel(i3, false);
	hbl->addWidget(i3);
	//tb1->groupLayout()->addWidget(i3);
	iisIconLabel *i4 = new iisIconLabel(QPixmap(QString::fromAscii(":/images/win/editcut.png")), QString::fromAscii("Do Task 4"), tb1);
	tb1->addIconLabel(i4, false);
	hbl->addWidget(i4);
	i4->setColors(Qt::red, Qt::green, Qt::gray);
	i4->setFocusPen(QPen());
	//tb1->groupLayout()->addWidget(i4);
	iisIconLabel *i5 = new iisIconLabel(QPixmap(QString::fromAscii(":/images/win/editpaste.png")), QString::fromAscii("Do Task 5"), tb1);
	tb1->addIconLabel(i5);
	//tb1->groupLayout()->addWidget(i5);

	iisTaskBox *tb2 = new iisTaskBox(QPixmap(QString::fromAscii(":/images/win/fileprint.png")), QString::fromAscii("Another Group of Tasks"), true, this);
	//this->addWidget(tb2);

	iisIconLabel *i21 = new iisIconLabel(QPixmap(QString::fromAscii(":/images/win/filenew.png")), QString::fromAscii("Do Task 2.1"), tb2);
	tb2->addIconLabel(i21);
	//tb2->groupLayout()->addWidget(i21);
	iisIconLabel *i22 = new iisIconLabel(QPixmap(QString::fromAscii(":/images/win/fileopen.png")), QString::fromAscii("Do Task 2.2"), tb2);
	tb2->addIconLabel(i22);
	i22->setEnabled(false);
	//tb2->groupLayout()->addWidget(i22);
	iisIconLabel *i23 = new iisIconLabel(QPixmap(QString::fromAscii(":/images/win/filesave.png")), QString::fromAscii("Do Task 2.3"), tb2);
	tb2->addIconLabel(i23);
	//tb2->groupLayout()->addWidget(i23);

	iisTaskBox *tb3 = new iisTaskBox(QPixmap(), QString::fromAscii("Group without Icons"), true, this);
	//this->addWidget(tb3);

	iisIconLabel *i31 = new iisIconLabel(QPixmap(), QString::fromAscii("Do Task 3.1"), tb3);
	tb3->addIconLabel(i31);

	iisIconLabel *i32 = new iisIconLabel(QPixmap(), QString::fromAscii("Do Task 3.2"), tb3);
	tb3->addIconLabel(i32);

	tb3->groupLayout()->addWidget(new QLabel(QString::fromAscii("Widgets also allowed:"), this));
	tb3->groupLayout()->addWidget(new QPushButton(QString::fromAscii("A Button"), this));


	// Other widgets can be also added to the panel
	QLabel *l1 = new QLabel(QString::fromAscii("A group without header"), this);
	//this->addWidget(l1);


	iisTaskGroup *tb4 = new iisTaskGroup(this);
	//this->addWidget(tb4);

	iisIconLabel *i41 = new iisIconLabel(QPixmap(QString::fromAscii(":/images/win/textbold.png")), QString::fromAscii("Do Task 4.1"), tb4);
	tb4->addIconLabel(i41);

	iisIconLabel *i42 = new iisIconLabel(QPixmap(), QString::fromAscii("Do Task 4.2"), tb4);
	tb4->addIconLabel(i42);

	//this->addStretch();

	//resize( 200, 200 );

    onUpdate();

    Gui::Selection().Attach(this);
}

TaskPanelView::~TaskPanelView()
{
    Gui::Selection().Detach(this);
 
}

void TaskPanelView::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                            Gui::SelectionSingleton::MessageType Reason)
{
    std::string temp;

    if (Reason.Type == SelectionChanges::AddSelection ) 
    {
        // insert the selection as item
        //temp = Reason.pDocName;
        //temp += ".";
        //temp += Reason.pObjectName;
        //if(Reason.pSubName[0] != 0 ){
        //   temp += ".";
        //   temp += Reason.pSubName;
        //}

        //new QListWidgetItem(QString::fromAscii(temp.c_str()), selectionView);
    }else

    if (Reason.Type == SelectionChanges::ClrSelection ) 
    {
        // remove all items
        //selectionView->clear();

    }else
    if (Reason.Type == SelectionChanges::RmvSelection ) 
    {
        // build name
        //temp = Reason.pDocName;
        //temp += ".";
        //temp += Reason.pObjectName;
        //if(Reason.pSubName[0] != 0){ 
        //   temp += ".";
        //   temp += Reason.pSubName;
        //}

        //// remove all items
        //QList<QListWidgetItem *> l = selectionView->findItems(QLatin1String(temp.c_str()),Qt::MatchExactly);
        //if(l.size() == 1)
        //    delete l[0];

    }
}

void TaskPanelView::onUpdate(void)
{
}

bool TaskPanelView::onMsg(const char* pMsg)
{
    // no messages yet
    return false;
}

void TaskPanelView::changeEvent(QEvent *e)
{
    //if (e->type() == QEvent::LanguageChange) {
    //    tabs->setTabText(0, trUtf8("View"));
    //    tabs->setTabText(1, trUtf8("Data"));
    //}

    DockWindow::changeEvent(e);
}

#include "moc_TaskPanelView.cpp"
