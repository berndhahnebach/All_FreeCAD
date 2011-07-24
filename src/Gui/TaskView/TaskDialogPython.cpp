/***************************************************************************
 *   Copyright (c) 2011 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#include "TaskDialogPython.h"

using namespace Gui;
using namespace Gui::TaskView;


TaskDialogPython::TaskDialogPython()
{

}

TaskDialogPython::~TaskDialogPython()
{
}

void TaskDialogPython::open()
{

}

void TaskDialogPython::clicked(int)
{

}

bool TaskDialogPython::accept()
{
    return true;
}

bool TaskDialogPython::reject()
{
    return true;
}

void TaskDialogPython::helpRequested()
{

}

QDialogButtonBox::StandardButtons TaskDialogPython::getStandardButtons(void) const
{
    return QDialogButtonBox::Ok;
}

void TaskDialogPython::modifyStandardButtons(QDialogButtonBox*)
{
}

bool TaskDialogPython::isAllowedAlterDocument(void) const
{
    return false;
}

bool TaskDialogPython::isAllowedAlterView(void) const
{
    return false;
}

bool TaskDialogPython::isAllowedAlterSelection(void) const
{
    return false;
}

bool TaskDialogPython::needsFullSpace() const
{
    return false;
}

