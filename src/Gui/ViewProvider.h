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


#ifndef __VIEWPROVIDER_H__
#define __VIEWPROVIDER_H__

#include "../Base/Factory.h"

namespace App
{
  class Feature;
}

class SoNode;
class QListViewItem;

namespace Gui {

/** Base class of all view provider
 *  \author Jürgen Riegel
 */
class GuiExport ViewProvider
{
public:
  /**
   * A constructor.
   * A more elaborate description of the constructor.
   */
  ViewProvider();

  /**
   * A destructor.
   * A more elaborate description of the destructor.
   */
  virtual ~ViewProvider();
};


class GuiExport ViewProviderTree:public ViewProvider
{
public:
  /**
   * A constructor.
   * A more elaborate description of the constructor.
   */
  ViewProviderTree();

  /**
   * A destructor.
   * A more elaborate description of the destructor.
   */
  virtual ~ViewProviderTree();

  QListViewItem* create();
};


class GuiExport ViewProviderInventor:public ViewProvider
{
public:
  /**
   * A constructor.
   * A more elaborate description of the constructor.
   */
  ViewProviderInventor();

  /**
   * A destructor.
   * A more elaborate description of the destructor.
   */
  virtual ~ViewProviderInventor();

  virtual SoNode* create(App::Feature *pcFeature)=0;
};


/** The FeatureFactory singleton
  */
class GuiExport ViewProviderInventorFactorySingleton : public Base::Factory
{
public:
	static ViewProviderInventorFactorySingleton& Instance(void);
	static void Destruct (void);

    /// produce the ViewProvider using the factory
	ViewProviderInventor *Produce (const char* sName) const;

private:
	static ViewProviderInventorFactorySingleton* _pcSingleton;

	ViewProviderInventorFactorySingleton(){}
	~ViewProviderInventorFactorySingleton(){}
};

inline GuiExport ViewProviderInventorFactorySingleton& ViewProviderInventorFactory(void)
{
	return ViewProviderInventorFactorySingleton::Instance();
}

// --------------------------------------------------------------------

template <class CLASS>
class ViewProviderInventorProducer: public Base::AbstractProducer
{
	public:
		/// Constructor
		ViewProviderInventorProducer ()
		{
			Gui::ViewProviderInventorFactory().AddProducer(typeid(CLASS).name(), this);
		}

		virtual ~ViewProviderInventorProducer (void){}

		/// Produce an instance
		virtual void* Produce (void) const
		{ 
			return (void*)(new CLASS);
		}
};




} // namespace Gui

#endif // __VIEWPROVIDER_H__

