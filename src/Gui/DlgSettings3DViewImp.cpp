/** \file DlgSettings3DViewImp.cpp
 *  \brief  
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *   
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2003                                                   *
 ***************************************************************************/


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include "PrefWidgets.h"
#include "DlgSettings3DViewImp.h"

/* 
 *  Constructs a FCDlgSettings3DViewImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
FCDlgSettings3DView::FCDlgSettings3DView( QWidget* parent,  const char* name, WFlags fl )
    : DlgSettings3DView( parent, name, fl )
{
	append(UseAntialiasing->getHandler());
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCDlgSettings3DView::~FCDlgSettings3DView()
{
    // no need to delete child widgets, Qt does it all for us
}

#include "DlgSettings3DView.cpp"
#include "moc_DlgSettings3DView.cpp"
#include "moc_DlgSettings3DViewImp.cpp"
