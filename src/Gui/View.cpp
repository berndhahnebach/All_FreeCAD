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
#endif



#include "Document.h"
#include "Tree.h"




FCView::FCView( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags )
    :QextMdiChildView( parent, name, wflags ),
	 _pcDocument(pcDocument)
{
	
	
}

FCView::~FCView()
{
  
}

FCGuiDocument* FCView::getDocument()
{
	return _pcDocument;
}

/// recife a message
bool FCView::OnMsg(const char* pMsg)
{
	return false;
}



#include "View_moc.cpp"