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

class FeatItem;

class GuiExport ViewProviderFeature:public ViewProvider
{
public:
  /// constructor.
  ViewProviderFeature();

  /// destructor.
  virtual ~ViewProviderFeature();

  // returns the TreeLabel
  virtual QListViewItem* getTreeItem(QListViewItem* parent);

  virtual void attach(App::Feature *pcFeature);
  /// returns a vector of all possible modes
  virtual std::vector<std::string> getModes(void);

  /// check if the Data has to be recalculated
  bool ifDataNewer(void);
  /// check if the matrial has to be recalculated
  bool ifMaterialNewer(void); 

  virtual void update(void);

  virtual void updateData(void){};
  

  /// helper to copy material from a feature to a SoMeterial Node
  void copy(const App::Material&, SoMaterial*);
  /// helper to set the materials
  void setMatFromFeature(void);

  /// Set the transparency
  virtual void setTransparency(float);
  /// Set the color
  virtual void setColor(const App::Color &c);


  App::Feature *getFeature(void){return pcFeature;}

protected:
  SoMaterial  *pcSolidMaterial;
  SoMaterial  *pcLineMaterial;
  SoMaterial  *pcPointMaterial;
  SoDrawStyle *pcLineStyle;
  SoDrawStyle *pcPointStyle;

  App::Feature *pcFeature;

  FeatItem *pcFeatItem;

  Quantity_Date calcMaterial,calcData;

};





/** The FeatureFactory singleton
  */
class GuiExport ViewProviderFeatureFactorySingleton : public Base::Factory
{
public:
	static ViewProviderFeatureFactorySingleton& Instance(void);
	static void Destruct (void);

    /// produce the ViewProvider using the factory
	ViewProviderFeature *Produce (const char* sName) const;

private:
	static ViewProviderFeatureFactorySingleton* _pcSingleton;

	ViewProviderFeatureFactorySingleton(){}
	~ViewProviderFeatureFactorySingleton(){}
};

inline GuiExport ViewProviderFeatureFactorySingleton& ViewProviderFeatureFactory(void)
{
	return ViewProviderFeatureFactorySingleton::Instance();
}

// --------------------------------------------------------------------

template <class CLASS>
class ViewProviderFeatureProducer: public Base::AbstractProducer
{
	public:
		/// Constructor
		ViewProviderFeatureProducer ()
		{
			Gui::ViewProviderFeatureFactory().AddProducer(typeid(CLASS).name(), this);
		}

		virtual ~ViewProviderFeatureProducer (void){}

		/// Produce an instance
		virtual void* Produce (void) const
		{ 
			return (void*)(new CLASS);
		}
};




} // namespace Gui

#endif // __VIEWPROVIDER_H__

