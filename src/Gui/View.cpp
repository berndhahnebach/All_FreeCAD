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
#	include <qaction.h>
#	include <qapplication.h>
#	include <qfiledialog.h>
#	include <qgrid.h>
#	include <qlayout.h>
#	include <qmessagebox.h>
#	include <qpushbutton.h>
#	include <qsplitter.h>
#	include <qtabbar.h>
#	include <qthread.h>
#	include <qtoolbar.h>
#	include <qvbox.h>
#	include <qwidgetstack.h>
#endif


#include "View.h"
#include "Document.h"
#include "Application.h"


//**************************************************************************
// FCBaseView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

std::vector<ViewProvider*> FCBaseView::_vpcViewProvider;

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

/// returns the document the view is attached to
App::Document* FCBaseView::GetAppDocument()
{
	if(!_pcDocument) return 0;
	return _pcDocument->GetDocument();
}

/// Register a new View provider
void FCBaseView::AddViewProvider(ViewProvider* pcProvider)
{
	_vpcViewProvider.push_back(pcProvider);
}


//**************************************************************************
// MDIView
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



MDIView::MDIView( FCGuiDocument* pcDocument,QWidget* parent, const char* name, int wflags )
	:QMainWindow(parent, name, wflags), FCBaseView(pcDocument)
{
}

MDIView::~MDIView()
{
}


/// recife a message
bool MDIView::OnMsg(const char* pMsg)
{
	return false;
}

bool MDIView::OnHasMsg(const char* pMsg)
{
	return false;
}

void MDIView::closeEvent(QCloseEvent *e)
{
	if(bIsPassiv){
		if(CanClose() ){
			e->accept();
			QMainWindow::closeEvent(e);
		}
	}else{
		if(GetGuiDocument()->IsLastView())
		{
			GetGuiDocument()->CanClose(e);

			if(e->isAccepted ())
				QMainWindow::closeEvent(e);
		}else
			e->accept();
	}
}


void MDIView::SetActive(void)
{
	ApplicationWindow::Instance->ViewActivated(this);
}

void MDIView::Print( QPrinter* printer )
{
	// print command specified but print methode not overriden!
	assert(0);
}

QSize MDIView::minimumSizeHint () const
{
  return QSize(100, 80);
}


#include "moc_View.cpp"
