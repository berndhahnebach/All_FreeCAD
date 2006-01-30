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


#ifndef __PropteryGeo_H__
#define __PropteryGeo_H__

// Std. configurations

#include <Base/Vector3D.h>
#include <Base/Matrix.h>

#include "Property.h"

namespace Base {
  class Writer;
}

namespace App
{
class Feature;


/** Vector properties
 * This is the father of all properties handling Integers.
 */
class AppExport PropertyVector: public Property
{
  TYPESYSTEM_HEADER();

public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyVector();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	~PropertyVector();

	/** Sets the property 
	 */
  void setValue(const Base::Vector3D &vec);
  void setValue(float x, float y, float z);

	/** This method returns a string representation of the property
	 */
	const Base::Vector3D &getValue(void) const;

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer);
  virtual void Restore(Base::Reader &reader);


private:

  Base::Vector3D _cVec;

};

/** Vector properties
 * This is the father of all properties handling Integers.
 */
class AppExport PropertyMatrix: public Property
{
  TYPESYSTEM_HEADER();

public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyMatrix();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	~PropertyMatrix();

	/** Sets the property 
	 */
  void setValue(const Base::Matrix4D &mat);

	/** This method returns a string representation of the property
	 */
	const Base::Matrix4D &getValue(void) const;

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (short indent,std::ostream &str);
  virtual void Restore(Base::Reader &reader);


private:

  Base::Matrix4D _cMat;

};



} // namespace App

#endif // __PropteryStandard_H__
