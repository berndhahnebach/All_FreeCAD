/***************************************************************************
 *   Copyright (c) 2009 Juergen Riegel  (FreeCAD@juergen-riegel.net>              *
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


#ifndef GUI_SelectionObject_H
#define GUI_SelectionObject_H

#include <list>
#include <string>
#include <Base/BaseClass.h>

namespace Base {
class PyObjectBase;
}

namespace Gui {


/**
 * The Selection object class
 */
class GuiExport SelectionObject : public Base::BaseClass
{
    TYPESYSTEM_HEADER();

public:
    /** Constructs a SelectionObject object. */
    SelectionObject();
    virtual ~SelectionObject();
    /**
       /**
     * The default implementation returns an instance of @ref SelectionObjectPy.
     */
    PyObject* getPyObject();
};


} // namespace Gui


#endif // GUI_SelectionObject_H 
