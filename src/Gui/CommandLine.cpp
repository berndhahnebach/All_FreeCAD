/** \file $RCSfile$
 *  \brief The command line module
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
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/

#include "CommandLine.h"


FCCommandLine::FCCommandLine(void)
: QLineEdit(NULL)
{
  setFixedWidth(400);
}

FCCommandLine::~FCCommandLine(void)
{
}

void FCCommandLine::SetParent(QWidget* parent)
{
  _pcSingleton->reparent(parent, QPoint());
}

//**************************************************************************
// Singleton stuff

FCCommandLine * FCCommandLine::_pcSingleton = 0;


void FCCommandLine::Destruct(void)
{
	// not initialized or double destruct!
  assert(_pcSingleton);
	delete _pcSingleton;
}

FCCommandLine & FCCommandLine::Instance(void)
{
	// not initialized?
	if(!_pcSingleton)
	{
		_pcSingleton = new FCCommandLine;
	}

  return *_pcSingleton;
}


#include "CommandLine_moc.cpp"