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

#include <Base/Persistance.h>

namespace App
{

class Document;
class DocumentObject;
class Property;
class Transaction;
class AbstractFeature;

/** transport the changes of the Document
 *  This class transport closer information what was changed in a
 *  document. Its a optional information and not all commands set this
 *  information. If not set all observer of the document assume a full change
 *  and update everything (e.g 3D view). This is not a very good idea if, e.g. only
 *  a small parameter whas changed. There for one can use this class and make the
 *  update of the document much faster!
 *@see FCDocument
 *@see FCObserver
 *@bug not implemented so far...!
*/
class AppExport DocChanges
{
public:  
  enum {
    Recompute,
    Rename,
    UndoRedo
  } Why;

  // use a vector to preserve insertion order
  std::vector<DocumentObject*>   NewObjects;
  std::set<DocumentObject*>   UpdatedObjects;
  std::set<AbstractFeature*> ErrorFeatures;
  std::set<DocumentObject*>   DeletedObjects;
};

/** Represents an entry for an object in a Transaction
 */
class AppExport TransactionObject: public Base::Persistance
{
  TYPESYSTEM_HEADER();

public:
  /// Construction
  TransactionObject(const DocumentObject *pcObj,const char *NameInDocument=0);
  /// Destruction
  virtual ~TransactionObject();

  void apply(Document &Doc, DocumentObject *pcObj);


  void setProperty(const Property* pcProp);
  
  virtual void Save (Base::Writer &writer) const;
  /// This method is used to restore properties from an XML document.
  virtual void Restore(Base::XMLReader &reader);

  friend class Transaction;

protected:
  enum Status {New,Del,Chn} status;
  std::map<const Property*,Property*> _PropChangeMap;
  std::string _NameInDocument;
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

  /// apply the content to the document
  void apply(Document &Doc, DocChanges &ChangeList);

  // the utf-8 name of the transaction
  std::string Name; 


  virtual void Save (Base::Writer &writer) const;
  /// This method is used to restore properties from an XML document.
  virtual void Restore(Base::XMLReader &reader);

  /// get the position in the transaction history
  int getPos(void) const;


  friend class Document;

protected:
  void addObjectNew(const DocumentObject *Obj);
  void addObjectDel(const DocumentObject *Obj);
  void addObjectChange(const DocumentObject *Obj,const Property *Prop);
private:
  int iPos;
  std::map<const DocumentObject*,TransactionObject*> _Objects;

};



/** Represents a creation of a DocObject in a Transaction
 */
/*
class AppExport TransactionObjectNew: public TransactionObject
{
  TYPESYSTEM_HEADER();

public:
  /// Construction
  TransactionObjectNew();
  /// Destruction
  virtual ~TransactionObjectNew();

  
  virtual void Save (Base::Writer &writer) const; 
 
  virtual void Restore(Base::XMLReader &reader);

};

*/

} //namespace App

#endif // __Transaction_H__

