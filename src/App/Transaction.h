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


#ifndef __Transaction_H__
#define __Transaction_H__

// Std. configurations

#ifndef _PreComp_
#endif

#include <Base/Persistance.h>

namespace App
{

class Document;
class DocumentObject;
class Property;

/** Represents a entry in a Transaction
 */
class AppExport TransactionObject: public Base::Persistance
{
  TYPESYSTEM_HEADER();

public:
  /// Construction
  TransactionObject();
  /// Destruction
  virtual ~TransactionObject();

  
  virtual void Save (Base::Writer &writer);
  /// This method is used to restore properties from an XML document.
  virtual void Restore(Base::XMLReader &reader);

};

/** Represents a atomic transaction of the document
 */
class AppExport Transaction: public Base::Persistance
{
  TYPESYSTEM_HEADER();

public:
  /// Construction
  Transaction();
  /// Construction
  Transaction(int pos);
  /// Destruction
  virtual ~Transaction();


  virtual void Save (Base::Writer &writer);
  /// This method is used to restore properties from an XML document.
  virtual void Restore(Base::XMLReader &reader);

  /// get the position in the transaction history
  int getPos(void) const;

  friend class Document;

protected:
  void addObjectNew(DocumentObject *Obj);
  void addObjectDel(DocumentObject *Obj);
  void addObjectChange(DocumentObject *Obj,Property *Prop);
private:
  int iPos;

};

/** Represents a creation of a DocObject in a Transaction
 */
class AppExport TransactionObjectNew: public TransactionObject
{
  TYPESYSTEM_HEADER();

public:
  /// Construction
  TransactionObjectNew();
  /// Destruction
  virtual ~TransactionObjectNew();

  
  virtual void Save (Base::Writer &writer); 
 
  virtual void Restore(Base::XMLReader &reader);

};



} //namespace App

#endif // __Transaction_H__

