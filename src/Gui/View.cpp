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
 
#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
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

#	include <qgrid.h>


FCView::FCView( FCGuiDocument* pcDocument,QWidget* parent, const char* name, int wflags )
	:FCWindow(parent, name, wflags),
	 _pcDocument(pcDocument)
{
	
	
}

FCView::~FCView()
{
  
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



FCSingelView::FCSingelView(FCView* pcView, QWidget* parent, const char* name, int wflags )
    :FCViewContainer(parent, name, wflags),
	 _pcView(pcView)
{	
/*
	// reparent the view to this widget	
	QVBox* vb2 = new QVBox( this );
	vb2->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
    QVBoxLayout* pcMainLayout = new QVBoxLayout(vb2);

	_pcView->reparent(vb2,wflags,QPoint(0,0));
	pcMainLayout->addWidget(vb2);
*/
	_pcView->reparent(this,wflags,QPoint(0,0));

	/*
	 QGrid* pcGrid = new QGrid(1,this);
	_pcView->reparent(pcGrid,wflags,QPoint(0,0));
*/
	
}

void FCSingelView::resizeEvent ( QResizeEvent * e) 
{
  _pcView->resize(e->size());
}


FCSingelView::~FCSingelView()
{
 
}



FCViewBar::FCViewBar( FCView* pcView, QWidget* parent, const char* name, int wflags )
	:FCWindow(parent,name,wflags),
	 _pcView(pcView)
{
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