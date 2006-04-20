/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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

 


#ifndef _PartFeature_h_
#define _PartFeature_h_

//#include <Base/Factory.h>
//#include <Base/PyExport.h>
#include <TopoDS_Shape.hxx>
#include <App/Feature.h>

class PyObjectBase;
class FeaturePy;

namespace Base{
  class PyObjectBase;
}

namespace Part
{

class Property;

/** Base class of all Feature classes in FreeCAD
 */
class AppPartExport Feature: public App::AbstractFeature
{
  PROPERTY_HEADER(Part::Feature);

public:
	/// Constructor
	Feature(void);

  /** @name methods overide Feature */
  //@{
  /// recalculate the Feature
  virtual int execute(void);
  //@}

  /// returns the type name of the ViewProvider
  virtual const char* getViewProviderName(void){return "PartGui::ViewProviderPart";}


	/** @name methods for handling the result shape */
	//@{
	/** Set the result shape
	 *
	 */
  void setShape(const TopoDS_Shape &Shape);

	/** Get the actual result shape
	 *
	 */
  TopoDS_Shape getShape(void);

	//@}


  virtual Base::PyObjectBase *Feature::GetPyObject(void);

private:
  TopoDS_Shape _Shape;

};


} //namespace Part



#endif
