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


#include "../Base/Console.h"
#include "Window.h"
#include "../App/Application.h"



//**************************************************************************
//**************************************************************************
// FCWindowParameter
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

FCWindowParameter::FCWindowParameter(const char *name)
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

FCWindowParameter::~FCWindowParameter()
{

}

void FCWindowParameter::OnParameterChanged(void)
{
	GetConsole().Warning("FCWindowParameter::OnParameterChanged(): Parameter has changed and window (%s) has not overriden this function!",_handle->GetGroupName());
}


FCParameterGrp::handle  FCWindowParameter::GetWindowParameter(void)
{
	return _handle;
}

FCParameterGrp::handle  FCWindowParameter::GetParameter(void)
{
	return GetApplication().GetUserParameter().GetGroup("BaseApp");
}



//**************************************************************************
//**************************************************************************
// FCWindow
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

FCWindow::FCWindow(QWidget *parent, const char *name, WFlags f)
	:QWidget(parent,name,f),
	FCWindowParameter(name)
{

}


FCWindow::~FCWindow()
{

}


//**************************************************************************
//**************************************************************************
// FCViewContainer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


//**************************************************************************
// Construction/Destruction

FCViewContainer::FCViewContainer(QWidget* parent, const char* name, int wflags )
    :QextMdiChildView( parent, name, wflags ),
	 FCWindowParameter(name)
{
		
}

FCViewContainer::~FCViewContainer()
{
  
}




#include "moc_Window.cpp"