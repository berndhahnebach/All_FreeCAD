/** \file $RCSfile$
 *  \brief The Viewer framwork
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
 
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <qvbox.h>
#	include <qtoolbar.h>
#	include <qfiledialog.h>
#	include <qapplication.h>
#	include <qmessagebox.h>
#	include <qsplitter.h>
#	include <qwidgetstack.h>
#	include <qpushbutton.h>
#	include <qtabbar.h>
#	include <qlayout.h>
#	include <qgrid.h>
#endif


#include "View.h"
#include "Document.h"
#include "Application.h"


//#	include <qgrid.h>


FCView::FCView( FCGuiDocument* pcDocument,QWidget* parent, const char* name, int wflags )
	:FCWindow(parent, name, wflags),
	 _pcDocument(pcDocument),
	 bIsDetached(false)
{
	if(pcDocument)
		pcDocument->AttachView(this);
	else
		ApplicationWindow::Instance->AttachView(this);
}

FCView::~FCView()
{
	if(!bIsDetached)
	{
		if(_pcDocument)
			_pcDocument->DetachView(this);
		else
			ApplicationWindow::Instance->DetachView(this);
	}
}

void FCView::Close(void)
{
	if(_pcDocument)
		_pcDocument->DetachView(this);
	else
		ApplicationWindow::Instance->DetachView(this);

	bIsDetached = true;
}


void FCView::SetDocument(FCGuiDocument* pcDocument)
{
	if(_pcDocument)
		_pcDocument->DetachView(this);
	if(pcDocument)
		pcDocument->AttachView(this);	
	Update();
}

/// recife a message
bool FCView::OnMsg(const char* pMsg)
{
	return false;
}

	
void FCView::ViewMsg(const char* pMsg)
{
	OnMsg(pMsg);
}

void FCView::SetActive(void)
{
	ApplicationWindow::Instance->ViewActivated(this);
}



FCDoubleView::FCDoubleView(FCView* pcView1, FCView* pcView2, QWidget* parent, const char* name, int wflags )
    :FCViewContainer(parent, name, wflags),
	 _pcView1(pcView1),
	 _pcView2(pcView2)
{
	// set up one splitter for the whole area	
    QVBoxLayout* pcMainLayout = new QVBoxLayout(this);
	//pcMainLayout->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	_pcSplitter = new QSplitter( QSplitter::Horizontal, this, "Main");
	pcMainLayout->addWidget(_pcSplitter);

	// set up first widget
	QVBox* pcLayoutView1 = new QVBox( _pcSplitter );
	pcView1->reparent(pcLayoutView1,wflags,QPoint(0,0));
	pcLayoutView1->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

	// set up first widget
	QVBox* pcLayoutView2 = new QVBox( _pcSplitter );
	pcView1->reparent(pcLayoutView2,wflags,QPoint(0,0));
	pcLayoutView2->setFrameStyle( QFrame::StyledPanel | QFrame::Raised );

	
	// set the splitter behavior
	_pcSplitter->setResizeMode(pcLayoutView1,QSplitter::KeepSize);
	QValueList<int> size;
	size.append(180);
	size.append(100);
	_pcSplitter->setSizes (size);
	
}

FCDoubleView::~FCDoubleView()
{
 
}



FCSingleView::FCSingleView(FCView* pcView, QWidget* parent, const char* name, int wflags )
    :FCViewContainer(parent, name, wflags),
	 _pcView(pcView)
{

	// sends the activation signal to the view, which set the active document and view in ApplicationWindow
	connect(this, SIGNAL(activated(QextMdiChildView*)), pcView, SLOT(SetActive()));

	// reparent the View to the ViewContainer
	_pcView->reparent(this,wflags,QPoint(0,0));

	
}


void FCSingleView::resizeEvent ( QResizeEvent * e) 
{
	// sends the rezise event to the view (reparent dont set the proper conection?!?!)
	_pcView->resize(e->size());
}

void FCSingleView::closeEvent(QCloseEvent *e)
{
	if(_pcView->GetGuiDocument()->IsLastView())
	{
		_pcView->GetGuiDocument()->CanClose(e);

		if(e->isAccepted ())
			QextMdiChildView::closeEvent(e);
	}
}


FCSingleView::~FCSingleView()
{
 
}



FCViewBar::FCViewBar( FCView* pcView, QWidget* parent, const char* name, int wflags )
	:FCWindow(parent,name,wflags),
	 _pcView(pcView)
{
  assert(_pcView);
	_pcView->reparent(this,wflags,QPoint(0,0));
	resize( 130, 600 );
}

FCViewBar::~FCViewBar()
{

}

void FCViewBar::resizeEvent ( QResizeEvent * e) 
{
  _pcView->resize(e->size());
}


#include "moc_View.cpp"