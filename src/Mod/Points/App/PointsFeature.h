/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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



#ifndef Points_FEATURE_H
#define Points_FEATURE_H

#include <App/Feature.h>
#include "Points.h"


namespace Base{
  class PyObjectBase;
  class Writer;
}

namespace App{
  class Color;
}

namespace Points
{
class PointsWithProperty;
class Property;

/** Base class of all Points feature classes in FreeCAD.
 * This class holds an PointsKernel object.
 */
class PointsAppExport Feature : public App::Feature
{
  PROPERTY_HEADER(Points::Feature);

public:
  /// Constructor
  Feature(void);
  virtual ~Feature(void);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::XMLReader &reader);
  virtual void SaveDocFile (Base::Writer &writer);
  virtual void RestoreDocFile(Base::Reader &reader);

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  //@}

  /// returns the type name of the ViewProvider
  virtual const char* getViewProviderName(void){return "PointsGui::ViewProviderPoints";}


  /** @name methods for convenient handling of parameter (properties) */
  //@{
  /** Get the actual result Points. */
  virtual PointsWithProperty &getPoints(void){return _Points;}
  /// sets the points 
  void setPoints(const PointsWithProperty&);
  //@}

  /// set line size
  virtual void setTransparency(float trans) {_pointMaterial.transparency = trans;}
  /// get line Size
  virtual float getTransparency(void) const {return _pointMaterial.transparency;}
  /// set color
  virtual void setColor(const App::Color &c) {_pointMaterial.diffuseColor = c;}
  /// get color
  virtual const App::Color &getColor(void) const {return _pointMaterial.diffuseColor;}

  // handling the PointsPy object
  virtual Base::PyObjectBase *Feature::GetPyObject(void);

protected:
  PointsWithProperty _Points;

};

} //namespace Points



#endif 
