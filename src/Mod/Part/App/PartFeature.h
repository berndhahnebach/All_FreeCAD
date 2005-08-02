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
#include <App/Feature.h>

class TFunction_Logbook;
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
class AppPartExport PartFeature: public App::Feature
{
public:
	/// Constructor
	PartFeature(void);

  /** @name methods overide Feature */
  //@{
  /// Initialize Feature structure
  virtual void initFeature(void);
  /// recalculate the Feature
  virtual int execute(TFunction_Logbook& log);
  /// Returns the Name/Type of the feature
  virtual const char *type(void){return "Part";};
  //@}


	/** @name methodes for handling the result shape */
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


  virtual Base::PyObjectBase *PartFeature::GetPyObject(void);

};


} //namespace Part



#endif
