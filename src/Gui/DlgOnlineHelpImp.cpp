/***************************************************************************
                          DlgOnlineHelpImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief
 *  \author Werner Mayer
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




#include "PreCompiled.h"
#ifndef _PreComp_
#	include <qbutton.h>
#	include <qlabel.h>
#endif

#include "DlgOnlineHelpImp.h"

FCOnlineHelp::FCOnlineHelp( QWidget* parent, const char* name, WFlags fl )
: FCOnlineHelpBase(parent, name, fl)
{
  append(UseProxy->getHandler());
  append(DownloadURL->getHandler());
  append(FCLineEdit2->getHandler());

  setModified(true);
}

FCOnlineHelp::~FCOnlineHelp()
{
}

#include "DlgOnlineHelp.cpp"
#include "moc_DlgOnlineHelp.cpp"
#include "moc_DlgOnlineHelpImp.cpp"
