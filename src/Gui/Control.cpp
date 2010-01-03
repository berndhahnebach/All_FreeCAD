/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
#endif

/// Here the FreeCAD includes sorted by Base,App,Gui......
#include "Control.h"

#include <Gui/MainWindow.h>
#include <Gui/CombiView.h>
#include <Gui/DockWindowManager.h>


using namespace Gui;
using namespace std;

ControlSingelton* ControlSingelton::_pcSingleton = 0;

ControlSingelton::ControlSingelton()
{
    
}

ControlSingelton::~ControlSingelton()
{
    
}


void ControlSingelton::showDialog(Gui::TaskView::TaskDialog *dlg)
{
    Gui::DockWnd::CombiView* pcCombiView = (Gui::DockWnd::CombiView*) Gui::DockWindowManager::instance()->getDockWindow("Combo View");
    // shut return the pointer to compi view
    assert(pcCombiView);
    pcCombiView->showDialog(dlg);
}




// -------------------------------------------

ControlSingelton& ControlSingelton::instance(void)
{
    if (_pcSingleton == NULL)
        _pcSingleton = new ControlSingelton;
    return *_pcSingleton;
}

void ControlSingelton::destruct (void)
{
    if (_pcSingleton != NULL)
        delete _pcSingleton;
    _pcSingleton = 0;
}


// -------------------------------------------


#include "moc_Control.cpp"