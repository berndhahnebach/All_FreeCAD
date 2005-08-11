/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef MESH_WORK_BENCH_H
#define MESH_WORK_BENCH_H

#ifndef _PreComp_
#endif

#include <Gui/Workbench.h>

namespace MeshGui {

/**
 * @author Werner Mayer
 */
class GuiMeshExport MeshWorkbench : public Gui::Workbench
{
public:
  MeshWorkbench();
  virtual ~MeshWorkbench();
  /**
   * Activates the workbench and adds/remove GUI elements.
   */
  virtual bool activate();
  /**
   * Deactivates the workbench and undoes all changes, that were done
   * by the activate() method..
   */
  virtual bool deactivate();
};

} // namespace MeshGui


#endif // WORK_BENCH_H 
