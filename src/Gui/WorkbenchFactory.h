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


#ifndef WORK_BENCH_FACTORY_H
#define WORK_BENCH_FACTORY_H

#ifndef _PreComp_
#endif

#include <Base/Factory.h>

namespace Gui {
class Workbench;

/** 
 * The workbench factory provides methods for the dynamic creation of
 * special workbenches for each module. To create these workbenches once
 * they must be registered through a subclass of WorkbenchProducer.
 * @author Werner Mayer
 */
class GuiExport WorkbenchFactoryInst : public Base::Factory
{
public:
  static WorkbenchFactoryInst& instance();
  static void destruct ();

  Workbench* createWorkbench ( const char* sName ) const;
  QStringList workbenches() const;

private:
  static WorkbenchFactoryInst* _pcSingleton;

  WorkbenchFactoryInst(){}
  ~WorkbenchFactoryInst(){}
};

inline GuiExport WorkbenchFactoryInst& WorkbenchFactory()
{
  return WorkbenchFactoryInst::instance();
}

// --------------------------------------------------------------------

/**
 * @author Werner Mayer
 */
template <class CLASS>
class WorkbenchProducer: public Base::AbstractProducer
{
public:
  WorkbenchProducer ()
  {
  }

  virtual ~WorkbenchProducer (){}

  virtual void* Produce () const
  {
    return (void*)(new CLASS);
  }
};

} // namespace Gui 

#endif // WORK_BENCH_FACTORY_H 
