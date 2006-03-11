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


#ifndef __PropteryLinks_H__
#define __PropteryLinks_H__

// Std. configurations


#include "Property.h"

namespace Base
{
  class Writer;
}

namespace App
{
class AbstractFeature;


/** Integer properties
 * This is the father of all properties handling Integers.
 */
class AppExport PropertyLink: public Property
{
  TYPESYSTEM_HEADER();

public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyLink();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	~PropertyLink();

	/** Sets the property 
	 */
	void setValue(App::AbstractFeature *);

	/** This method returns a string representation of the property
	 */
	App::AbstractFeature * getValue(void) const;

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::XMLReader &reader);


private:

  App::AbstractFeature *_pcLink;

};



} // namespace App

#endif // __PropteryStandard_H__
