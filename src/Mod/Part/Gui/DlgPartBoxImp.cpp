/** \file DlgPartBoxImp.cpp
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
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "../../../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#endif

#include "DlgPartBoxImp.h"

/* 
 *  Constructs a DlgPartBox which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgPartBoxImp::DlgPartBoxImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgPartBox( parent, name, modal, fl ), FCWindow(name)
{
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgPartBoxImp::~DlgPartBoxImp()
{
    // no need to delete child widgets, Qt does it all for us
}

/* 
 * public slot
 */
void DlgPartBoxImp::OnApply()
{
    qWarning( "DlgPartBox::OnApply() not yet implemented!" ); 
}

#include "DlgPartBox.cpp"
#include "moc_DlgPartBox.cpp"
#include "moc_DlgPartBoxImp.cpp"
