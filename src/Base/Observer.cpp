/** \file Observer.cpp
 *  \brief Observer facility of FreeCAD
 *  \author $Author: wmayer $
 *  \version $Revision: 2.2 $
 *  \date    $Date: 2006/02/22 16:42:24 $
 *  Here the implemetation of the Observer pattern
 *  @see Parameter.h ParameterGrp
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
#include "PreCompiled.h"

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Observer.h"
#include "Console.h"
#include "Exception.h"



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// class FCObserver


/*
// **************************************************************************
// Construction/Destruction

// here the implemataion! description should take place in the header file!
template <class MessageType>
FCObserver::FCObserver()
{

}

template <class MessageType>
FCObserver::~FCObserver()
{

}






//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// class FCSubject


// **************************************************************************
// Construction/Destruction


FCSubject::FCSubject(){}


FCSubject::~FCSubject()
{
  if (_ObserverSet.size() > 0)
  {
    printf("Not detached all observers yet\n");
    throw;
  }
//	for(std::set<FCObserver * >::iterator Iter=_ObserverSet.begin();Iter!=_ObserverSet.end();Iter++)
//  {
//    (*Iter)->OnDestroy(*this);   // send OnChange-signal
//  }
}

// **************************************************************************
// Subject methods


void FCSubject::Attach(FCObserver *ToObserv)
{
	_ObserverSet.insert(ToObserv);
}


void FCSubject::Detach(FCObserver *ToObserv)
{
	_ObserverSet.erase(ToObserv);
}

void FCSubject::Notify(MessageType rcReason)
{
	for(std::set<FCObserver * >::iterator Iter=_ObserverSet.begin();Iter!=_ObserverSet.end();Iter++)
        (*Iter)->OnChange(*this,rcReason);   // send OnChange-signal
}

*/
