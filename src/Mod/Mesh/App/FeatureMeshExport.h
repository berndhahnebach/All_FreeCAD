/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef MESH_FEATUREMESHEXPORT_H
#define MESH_FEATUREMESHEXPORT_H

#include <App/Feature.h>
#include <App/PropertyStandard.h>
#include <App/PropertyLinks.h>

namespace Mesh
{

/**
 * The Export class writes any supported mesh format into a file.
 * @author Werner Mayer
 */
class AppMeshExport Export : public App::AbstractFeature
{
    PROPERTY_HEADER(Mesh::Export);

public:
    Export();

    App::PropertyLink   Source;
    App::PropertyString FileName;
    App::PropertyString Format;
    //FIXME: We should have an empty view provider which just does nothing
    //or even allow to have objects with no view provider related.
    virtual const char* getViewProviderName(void) const {
    return "MeshGui::ViewProviderExport";
    }

    /** @name methods override feature */
    //@{
    /// recalculate the Feature
    virtual int execute(void);
    //@}
};

}

#endif // MESH_FEATUREMESHEXPORT_H 
