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
#include "Quantity_Date.hxx"


class SoMaterial;
class SoDrawStyle;

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


  virtual void attach(App::Feature *pcFeature);
  /// returns a vector of all possible modes
  virtual std::vector<std::string> getModes(void);

  /// check if the Data has to be recalculated
  bool ifDataNewer(void);
  /// check if the matrial has to be recalculated
  bool ifMaterialNewer(void);

  virtual void update(const ChangeType&);

  virtual void updateData(void){};
  

  /// helper to copy material from a feature to a SoMeterial Node
  void copy(const App::Material&, SoMaterial*);
  /// helper to set the materials
  void setMatFromFeature(void);

  /// Set the transparency
  virtual void setTransparency(float);
  /// Set the color
  virtual void setColor(const App::Color &c);


protected:
  SoMaterial  *pcSolidMaterial;
  SoMaterial  *pcLineMaterial;
  SoMaterial  *pcPointMaterial;
  SoDrawStyle *pcLineStyle;
  SoDrawStyle *pcPointStyle;

  App::Feature *pcFeature;

  Quantity_Date calcMaterial,calcData;

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

