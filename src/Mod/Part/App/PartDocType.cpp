/** \file PartDocType.cpp
 *  \brief document type system
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2000 - 2003                 *   
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
#include "PreCompiled.h"

#ifndef _PreComp_
#	include <assert.h>
#endif


/// Here the FreeCAD includes sorted by Base,App,Gui......
#include <Base/PyExport.h>

#include <App/Label.h>
#include <App/Document.h>


#include "PartDocType.h"





//**************************************************************************
// Construction/Destruction

// here the implemataion! description should take place in the header file!
FCPartDocType::FCPartDocType()
{

}

FCPartDocType::~FCPartDocType()
{

}


void FCPartDocType::Init (FCDocument *pcDoc)
{
	FCPyHandle<FCLabel> hcMainLabel = pcDoc->Main();

	FCPyHandle<FCLabel> hcPosLabel = hcMainLabel->GetLabel("Pos");
	  FCPyHandle<FCLabel> hcXYLabel  = hcPosLabel->GetLabel("XY");
	  FCPyHandle<FCLabel> hcXZLabel  = hcPosLabel->GetLabel("XZ");
	  FCPyHandle<FCLabel> hcZYLabel  = hcPosLabel->GetLabel("ZY");

	FCPyHandle<FCLabel> hcBodyLabel = hcMainLabel->GetLabel("Body");
	  FCPyHandle<FCLabel> hcMainBodyLabel = hcMainLabel->GetLabel("MainBody");


}

const char *FCPartDocType::GetTypeName(void)
{
	return "Part";
}
