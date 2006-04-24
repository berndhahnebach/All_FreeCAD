/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2006     *
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



#ifndef __FEATUREPYTHON_H__
#define __FEATUREPYTHON_H__


#include "Feature.h"
#include "PropertyStandard.h"
#include "PropertyGeo.h"
#include "PropertyLinks.h"

namespace App
{

class FeaturePythonPy;

class FeaturePython :public AbstractFeature
{
  PROPERTY_HEADER(App::FeaturePython);

public:
  FeaturePython();  

  App::PropertyString  ExecuteCode;

  
  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  /// returns the type name of the ViewProvider
  //FIXME: Propably it makes sense to have a view provider for unittests (e.g. Gui::ViewProviderTest)
  virtual const char* getViewProviderName(void){return "Gui::ViewProviderDocumentObject";}
  //@}

  void addDynamicProperty(const char* type, const char* name=0);

  std::string getUniquePropertyName(const char *Name) const;

	virtual Base::PyObjectBase *GetPyObject(void);

  friend FeaturePythonPy;

private:
  std::map<std::string,Property*> objectProperies;
  //PropertyData objectProperies;

};



} //namespace App

#endif // __FeaturePython_H__
