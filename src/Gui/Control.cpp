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

ControlSingleton* ControlSingleton::_pcSingleton = 0;

ControlSingleton::ControlSingleton()
  : ActiveDialog(0)
{

}

ControlSingleton::~ControlSingleton()
{

}

void ControlSingleton::showDialog(Gui::TaskView::TaskDialog *dlg)
{
    Gui::DockWnd::CombiView* pcCombiView = qobject_cast<Gui::DockWnd::CombiView*>
        (Gui::DockWindowManager::instance()->getDockWindow("Combo View"));
    // should return the pointer to combo view
    assert(pcCombiView);
    // only one dialog at a time
    assert(!ActiveDialog || ActiveDialog==dlg);
    pcCombiView->showDialog(dlg);
    // make sure that the combo view is shown
    QDockWidget* dw = qobject_cast<QDockWidget*>(pcCombiView->parentWidget());
    if (dw) {
        dw->setVisible(true);
        dw->toggleViewAction()->setVisible(true);
    }

    ActiveDialog = dlg;
    connect(dlg, SIGNAL(destroyed()), this, SLOT(closedDialog()));
}

Gui::TaskView::TaskDialog* ControlSingleton::activeDialog() const
{
    return ActiveDialog;
}

void ControlSingleton::closeDialog()
{
    Gui::DockWnd::CombiView* pcCombiView = qobject_cast<Gui::DockWnd::CombiView*>
        (Gui::DockWindowManager::instance()->getDockWindow("Combo View"));
    // should return the pointer to combo view
    assert(pcCombiView);
    pcCombiView->closeDialog();
}

void ControlSingleton::closedDialog()
{
    ActiveDialog = 0;
    Gui::DockWnd::CombiView* pcCombiView = qobject_cast<Gui::DockWnd::CombiView*>
        (Gui::DockWindowManager::instance()->getDockWindow("Combo View"));
    // should return the pointer to combo view
    assert(pcCombiView);
    pcCombiView->showTreeView();
}

bool ControlSingleton::isAllowedAlterDocument(void) const
{
    if (ActiveDialog)
        return ActiveDialog->isAllowedAlterDocument();
    return true;
}

bool ControlSingleton::isAllowedAlterView(void) const
{
    if (ActiveDialog)
        return ActiveDialog->isAllowedAlterView();
    return true;
}

bool ControlSingleton::isAllowedAlterSelection(void) const
{
    if (ActiveDialog)
        return ActiveDialog->isAllowedAlterSelection();
    return true;
}

// -------------------------------------------

ControlSingleton& ControlSingleton::instance(void)
{
    if (_pcSingleton == NULL)
        _pcSingleton = new ControlSingleton;
    return *_pcSingleton;
}

void ControlSingleton::destruct (void)
{
    if (_pcSingleton != NULL)
        delete _pcSingleton;
    _pcSingleton = 0;
}


// -------------------------------------------


#include "moc_Control.cpp"

