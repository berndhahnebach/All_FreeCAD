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

 


#ifndef _RayFeature_h_
#define _RayFeature_h_

//#include <Base/Factory.h>
//#include <Base/PyExport.h>

#include "../../Part/App/PartFeature.h"

class PyObjectBase;
class FeaturePy;

namespace Raytracing
{

class Property;

/** Base class of all Feature classes in FreeCAD
 */
//class RayFeature: public Part::PartFeature
class RayFeature: public App::AbstractFeature
{
public:
	/// Constructor
	RayFeature(void);



	/** @name methods used for recalculation (update) */
	//@{
	/** Validate
	 *  We compute the object and topologically name it.
	 *  If during the execution we found something wrong,
	 *  we return the number of the failure. For example:
	 *  1 - an attribute hasn't been found,
	 *  2 - algorithm failed
	 *  0 - no mistakes were found.
	 */
	virtual Standard_Integer Execute(void)=0;

	//@}


};


} //namespace Raytracing



#endif //_RayFeature_h_
