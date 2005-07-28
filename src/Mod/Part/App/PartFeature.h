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

	/** Init the Label the Feature is attached to
	 *  This methode will be called when the Feature is mounted 
	 *  to a Label in the document. It need to be overwriten in 
	 *  every Feature
	 */
	virtual void InitLabel(const TDF_Label &rcLabel);


	/** @name methodes used for recalculation (update) */
	//@{
	/** MustExecute
	 *  We call this method to check if the object was modified to
	 *  be invoked. If the object label or an argument is modified,
	 *  we must recompute the object - to call the method Execute().
	 */
//	virtual bool MustExecute(const TFunction_Logbook& log);

	/** Validate
	 *  We compute the object and topologically name it.
	 *  If during the execution we found something wrong,
	 *  we return the number of the failure. For example:
	 *  1 - an attribute hasn't been found,
	 *  2 - algorithm failed
	 *  0 - no mistakes were found.
	 */
	virtual Standard_Integer Execute(TFunction_Logbook& log);

	/** Validate
	 * Validation of the object label, its arguments and its results.
	 */
	virtual void Validate(TFunction_Logbook& log);
	//@}

    /// Returns the Name/Type of the feature
  virtual const char *Type(void){return "Part";};


	/** @name methodes for conviniant handling of Parameter (Properties) */
	//@{
	/** Set the result shape
	 *  
	 */
  void setShape(TopoDS_Shape &Shape);
  
	/** Get the actual result shape
	 *  
	 */
  TopoDS_Shape getShape(void);
  
	//@}


  virtual Base::PyObjectBase *PartFeature::GetPyObject(void);

};


} //namespace Part



#endif
