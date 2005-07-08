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


#ifndef __VIEWPROVIDERFEATURE_H__
#define __VIEWPROVIDERFEATURE_H__

#include "../Base/Factory.h"
#include "ViewProvider.h"


class SoMaterial;

namespace App
{
  class Feature;
  class Material;
}


namespace Gui {


class GuiExport ViewProviderInventorFeature:public ViewProviderInventor
{
public:
  /// constructor.
  ViewProviderInventorFeature();

  /// destructor.
  virtual ~ViewProviderInventorFeature();

  virtual void attache(App::Feature *pcFeature)=0;


  virtual void selected(View3DInventorViewer *, SoPath *);
  virtual void unselected(View3DInventorViewer *, SoPath *);



  /// helper to copy material from a feature to a SoMeterial Node
  void copy(const App::Material&, SoMaterial*);
  /// helper to set the materials
  void setMatFromFeature(void);

  /// Set the transparency
  virtual void setTransparency(float);

protected:
  SoMaterial  *pcShadedMaterial;
  SoMaterial  *pcLineMaterial;
  SoMaterial  *pcPointMaterial;
  float fLineSize;
  float fPointSize;

  App::Feature *pcFeature;
};





/** The FeatureFactory singleton
  */
class GuiExport ViewProviderInventorFeatureFactorySingleton : public Base::Factory
{
public:
	static ViewProviderInventorFeatureFactorySingleton& Instance(void);
	static void Destruct (void);

    /// produce the ViewProvider using the factory
	ViewProviderInventorFeature *Produce (const char* sName) const;

private:
	static ViewProviderInventorFeatureFactorySingleton* _pcSingleton;

	ViewProviderInventorFeatureFactorySingleton(){}
	~ViewProviderInventorFeatureFactorySingleton(){}
};

inline GuiExport ViewProviderInventorFeatureFactorySingleton& ViewProviderInventorFeatureFactory(void)
{
	return ViewProviderInventorFeatureFactorySingleton::Instance();
}

// --------------------------------------------------------------------

template <class CLASS>
class ViewProviderInventorFeatureProducer: public Base::AbstractProducer
{
	public:
		/// Constructor
		ViewProviderInventorFeatureProducer ()
		{
			Gui::ViewProviderInventorFeatureFactory().AddProducer(typeid(CLASS).name(), this);
		}

		virtual ~ViewProviderInventorFeatureProducer (void){}

		/// Produce an instance
		virtual void* Produce (void) const
		{ 
			return (void*)(new CLASS);
		}
};




} // namespace Gui

#endif // __VIEWPROVIDER_H__

