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


//**************************************************************************
// FCBaseView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


FCBaseView::FCBaseView( FCGuiDocument* pcDocument)
:_pcDocument(pcDocument),
 bIsDetached(false)
{
	if(pcDocument){
		pcDocument->AttachView(this);
		bIsPassiv = false;
	}else{
		ApplicationWindow::Instance->AttachView(this);
		bIsPassiv = true;
	}
}

FCBaseView::~FCBaseView()
{
//	assert (bIsDetached);
	if(!bIsDetached && !ApplicationWindow::Instance->IsClosing() )
	{
		Close();
	}
}

void FCBaseView::Close(void)
{
	if(bIsDetached) return;

	if(bIsPassiv){
		ApplicationWindow::Instance->DetachView(this);
	}else{
		if(_pcDocument)
			_pcDocument->DetachView(this);
	}

	_pcDocument = 0;
	bIsDetached = true;
	/*
	if(_pcDocument)
		_pcDocument->DetachView(this);
	else
		ApplicationWindow::Instance->DetachView(this);
*/
}


void FCBaseView::SetDocument(FCGuiDocument* pcDocument)
{
	FCGuiDocument* pcOldDocument;
	// detach and attache the observer
	if(_pcDocument)
		_pcDocument->DetachView(this, true);
	if(pcDocument)
		pcDocument->AttachView(this,true);	

	// set the new document as the active one
	pcOldDocument = _pcDocument;
	_pcDocument = pcDocument;
	
	// tell the view
	OnNewDocument(pcOldDocument,_pcDocument);
}


//**************************************************************************
// FCView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



FCView::FCView( FCGuiDocument* pcDocument,QWidget* parent, const char* name, int wflags )
	:QextMdiChildView(parent, name, wflags),
	 FCBaseView(pcDocument)
{
	// sends the activation signal to the view, which set the active document and view in ApplicationWindow
	connect(this, SIGNAL(activated(QextMdiChildView*)), this, SLOT(SetActive()));

}

FCView::~FCView()
{
}


/// recife a message
bool FCView::OnMsg(const char* pMsg)
{
	return false;
}

bool FCView::OnHasMsg(const char* pMsg)
{
	return false;
}

void FCView::closeEvent(QCloseEvent *e)
{
	if(bIsPassiv){
		if(CanClose() ){
			e->accept();
			QextMdiChildView::closeEvent(e);
		}
	}else{
		if(GetGuiDocument()->IsLastView())
		{
			GetGuiDocument()->CanClose(e);

			if(e->isAccepted ())
				QextMdiChildView::closeEvent(e);
		}else
			e->accept();
	}
}


void FCView::SetActive(void)
{
	ApplicationWindow::Instance->ViewActivated(this);
}

void FCView::Print(QPainter& cPrinter)
{
	// print command specified but print methode not overriden!
	assert(0);
}


//**************************************************************************
// FCDockView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


FCDockView::FCDockView( FCGuiDocument* pcDocument,QWidget* parent, const char* name, int wflags )
	:FCDockWindow(parent, name, wflags),
	 FCBaseView(pcDocument)
{
}

FCDockView::~FCDockView()
{
}


/// recife a message
bool FCDockView::OnMsg(const char* pMsg)
{
	return false;
}

bool FCDockView::OnHasMsg(const char* pMsg)
{
	return false;
}

#include "moc_View.cpp"