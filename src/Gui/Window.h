/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef __Windows_h__
#define __Windows_h__


#ifndef _PreComp_
#endif

#include "../Base/Parameter.h"
#include "View.h"

class ApplicationWindow;

namespace Gui {

/** Adapter class to the parameter of FreeCAD for all Windows
 * Retrive the parameter group of the specific window
 * by the windowname.
 */
class GuiExport WindowParameter : public FCParameterGrp::ObserverType
{
public:
  WindowParameter(const char *name);
  virtual ~WindowParameter();

  bool setGroupName( const char* name );
  void OnChange(FCSubject<const char*> &rCaller, const char * sReason);

  /// get the parameters
  FCParameterGrp::handle getParameter(void);
  /// return the parameter group of this window
  FCParameterGrp::handle getWindowParameter(void);

  ApplicationWindow* applicationWindow(void);

private:
  FCParameterGrp::handle _handle;
};

} // namespace Gui

#endif
