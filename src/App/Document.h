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

#ifndef __Document_h__
#define __Document_h__



#include <Base/PyExport.h>
#include <Base/Observer.h>
#include <Base/Persistance.h>
#include <Base/Type.h>

#include "PropertyContainer.h"
#include "PropertyStandard.h"
//#include "Transaction.h"


#include <map>
#include <vector>
#include <stack>

#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#	pragma warning( disable : 4290 )  // not implemented throw specification
#	pragma warning( disable : 4275 )
#endif

namespace Base {
  class Writer;
}

//class FCLabel;
namespace App
{
  class DocumentObject;
  class Document;
  class AbstractFeature;
  class DocumentPy; // the python document class
  class Application;
  class Transaction;
}



namespace App
{

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



/// The document class
class AppExport Document :public App::PropertyContainer, public Base::Subject<const DocChanges&>
{
  PROPERTY_HEADER(App::Document);

public:

  /// holds the long name of the document (utf-8 coded)
  PropertyString Name;
  /// full qualified (with path) file name (utf-8 coded)
  PropertyString FileName;
  /// creators name (utf-8)
  PropertyString CreatedBy;
  PropertyString CreationDate;
  PropertyString LastModifiedBy;
  PropertyString LastModifiedDate;
  PropertyString Company;
  PropertyString Comment;


  /// Construction
	Document(void);
  /// Destruction
	virtual ~Document();

 	/** @name File handling of the document */
	//@{
	/// Save the Document under a new Name
	void saveAs (const char* Name);
	/// Save the document under the name its been opened
	bool save (void);
	/// Opens the document from its file name
	bool open (void);
	/// Is the document already saved to a file
	bool isSaved() const;
	/// Get the document name of a saved document 
	const char* getName() const;
	/// Get the path of a saved document 
	const char* getPath() const;
  //@}

  virtual void Save (Base::Writer &writer) const;
  virtual void Restore(Base::XMLReader &reader);

  /// returns the complet document mermory consumption, including all managed DocObjects and Undo Redo.
  unsigned int getMemSize (void) const;
  

  //FIXME: This is a hack to allow to save or read data from the Gui::Document.
  void installDocumentHook(Base::Persistance* pHook) {
    pDocumentHook = pHook;
  }
  void removeDocumentHook() {
    pDocumentHook=0;
  }


	/** @name Object handling  */
	//@{
  /// Add a feature of sType with sName to this document and set it active
	DocumentObject *addObject(const char* sType, const char* pObjectName=0);
  /// Remove a feature out of the document
	void remObject(const char* sName);
  /// Returns the active Object of this document
	DocumentObject *getActiveObject(void) const;
  /// Returns a Object of this document
	DocumentObject *getObject(const char *Name) const;
  /// Returns a Name of an Object or 0
	const char *getObjectName(DocumentObject *pFeat) const;
  /// Returns a Name of an Object or 0
  std::string getUniqueObjectName(const char *Name) const;
  /// Returns a list of all Objects
  std::vector<DocumentObject*> getObjects() const;
  std::vector<DocumentObject*> getObjectsOfType(const Base::Type& typeId) const;
  int countObjectsOfType(const Base::Type& typeId) const;
	//@}


	/** @name methods for modification and state handling
	 */
	//@{

	/// Remove all modifications. After this call The document becomes again Valid.
	void PurgeModified();
	/// Recompute if the document was  not valid and propagate the reccorded modification.
	void recompute();
  /// Recompute only this feature
  void recomputeFeature(AbstractFeature* Feat);
	//@}


	/** @name methods for the UNDO REDO handling */
	//@{
  /// switch the level of Undo/Redo
  void setUndoMode(int iMode);

	/// New Command (Short cut for Commit and Open command)
	void newCommand() ;
	/// Open a new command Undo/Redo, an UTF-8 name can be specified
	void openCommand(const char* name=0);
	/// Commit the Command transaction. Do nothing If there is no Command transaction open.
	void commitCommand();
	/// Abort the  Command  transaction. Do nothing If there is no Command transaction open.
	void abortCommand();
	/// Set the Undo limit in Byte!
	void SetUndoLimit(unsigned int MemSize=0);
	/// Remove all stored Undos and Redos
	void clearUndos();
	/// Returns the  number  of stored Undos. If greater than 0 Undo will be effective.
	int getAvailableUndos() const;
	/// Returns a list of the Undo names
  std::vector<std::string> getAvailableUndoNames() const;
  /// Returns the actual memory consumption of the Undo redo stuff.
  unsigned int getUndoMemSize (void) const;
	/// Will UNDO  one step, returns  False if no undo was done (Undos == 0).
	bool undo();
	/// Returns the number of stored Redos. If greater than 0 Redo will be effective.
	int getAvailableRedos() const;
	/// Returns a list of the Redo names.
	std::vector<std::string> getAvailableRedoNames() const;
	/// Will REDO  one step, returns  False if no redo was done (Redos == 0).
	bool redo() ;
	//@}


 	/** @name methods for the transaction handling*/
	//@{
  /// starts a new transaction
  void setTransactionMode(int iMode);
  /// starts a new transaction
  int  beginTransaction(void);
  /// revert all changes to the document since beginTransaction()
  void rollbackTransaction(void);
  /// ends the open Transaction
  int  endTransaction(void);
  /// returns the named Transaction
  const Transaction *getTransaction(int pos = -1) const;
  //@}


	virtual Base::PyObjectBase *GetPyObject(void);


	friend class DocumentPy;
	friend class Application;
	friend class AbstractFeature;
  /// because of transaction handling
	friend class DocumentObject;
	friend class Transaction;
	friend class TransactionObject;

protected:
  void _remObject(DocumentObject* pcObject);
  void _addObject(DocumentObject* pcObject, const char* pObjectName);

  /// callback from the Document objects bevor property will be changed
  void onBevorChangeProperty(const DocumentObject *Who, const Property *What);
  /// callback from the Document objects after property was changed
  void onChangedProperty(const DocumentObject *Who, const Property *What);
  /// helper which Recompute only this feature
  void _recomputeFeature(AbstractFeature* Feat);

  int _iTransactionMode;
  int iTransactionCount;
  std::map<int,Transaction*> mTransactions;
  Transaction *activTransaction;

  int _iUndoMode;
  std::stack<Transaction*> mUndoTransactions;
  std::stack<Transaction*> mRedoTransactions;
  Transaction *activUndoTransaction;



  DocumentObject* pActiveObject;
  std::map<std::string,DocumentObject*> ObjectMap;
  // Array to preserve the creation order of created objects
  std::vector<DocumentObject*> ObjectArray;
  Base::Persistance* pDocumentHook;

	// pointer to the python class
	DocumentPy *_pcDocPy;
};


} //namespace App


#endif // __Document_h__
