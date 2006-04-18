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
class DocumentObject;


/** the general Link Poperty
 *  Main Purpose of this property is to Link Objects and Feautures in a document.
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
	void setValue(App::DocumentObject *);

	/** This method returns the linked DocumentObject
	 */
	App::DocumentObject * getValue(void) const;

	/** Returns the link type checked
	 */
  App::DocumentObject * getValue(Base::Type t) const;

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

protected:

  App::DocumentObject *_pcLink;

};

class AppExport PropertyLinkList: public PropertyLists
{
  TYPESYSTEM_HEADER();

public:

       
	/**
	 * A constructor.
	 * A more elaborate description of the constructor.
	 */
	PropertyLinkList();

	/**
	 * A destructor.
	 * A more elaborate description of the destructor.
	 */
	virtual ~PropertyLinkList();

  virtual void setSize(int newSize){_lValueList.resize(newSize);}   
  virtual int getSize(void) const {return _lValueList.size();}   

	/** Sets the property 
	 */
	void setValue(DocumentObject*);
  
  /// index operator
  DocumentObject* operator[] (const int idx) const {return _lValueList.operator[] (idx);} 
  
  
  void  set1Value (const int idx, DocumentObject* value){_lValueList.operator[] (idx) = value;}

  const std::vector<DocumentObject*> &getValues(void) const{return _lValueList;}

  virtual PyObject *getPyObject(void);
  virtual void setPyObject(PyObject *);

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  virtual Property *Copy(void) const;
  virtual void Paste(const Property &from);

private:
  std::vector<DocumentObject*> _lValueList;
};

} // namespace App

#endif // __PropteryStandard_H__
