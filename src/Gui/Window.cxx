/** \file $RCSfile$
 *  \brief he windows base class, father of all windows
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
#endif


#include "Window.h"
#include "../App/Application.h"


//**************************************************************************
//**************************************************************************
// FCWindow
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

FCWindow::FCWindow(const char *name)
{
	FCHandle<FCParameterGrp> h;

	// not allowed to use a FCWindow without a name, see the constructor of a FCDockWindow or a other QT Widget
	assert(name);
	//printf("Instanceate:%s\n",name);

	// geting the group for the window
	h = GetApplication().GetSystemParameter().GetGroup("BaseApp");
	h = h->GetGroup("Windows");
	_handle = h->GetGroup(name);

	_handle->SetBool("Works",true);
	bool bTest = _handle->GetBool("Works");


}

FCWindow::~FCWindow()
{

}


FCParameterGrp::handle  FCWindow::GetWindowParameter(void)
{
	return _handle;
}

FCParameterGrp::handle  FCWindow::GetParameter(void)
{
	return GetApplication().GetUserParameter().GetGroup("BaseApp");
}


//**************************************************************************
//**************************************************************************
// FCDockWindow
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

FCDockWindow::FCDockWindow(QWidget *parent, const char *name, WFlags f)
	:QWidget(parent,name,f),
	FCWindow(name)
{

}


FCDockWindow::~FCDockWindow()
{

}


//**************************************************************************
//**************************************************************************
// FCViewWindow
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

FCViewWindow::FCViewWindow( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags )
    :QextMdiChildView( parent, name, wflags ),
	 _pcDocument(pcDocument),
	 FCWindow(name)
{
		
}

FCViewWindow::~FCViewWindow()
{
  
}




#include "Window_moc.cpp"