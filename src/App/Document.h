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



#include "../Base/PyExport.h"
#include "../Base/Observer.h"
#include <TDocStd_Document.hxx>
#include <TFunction_Logbook.hxx>
//#include <TDF_ChildIterator.hxx>

#include <map>
#include <vector>

#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#	pragma warning( disable : 4290 )  // not implemented throw specification
#	pragma warning( disable : 4275 )
#endif


//class FCLabel;
namespace App
{
  class LabelPy;
  class Document;
  class Feature;
  class DocumentPy; // the python document class
  class Application;
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
};



/** The Document
 *  This is bisides the Applcation class the most importand class in FreeCAD
 *  It wrapps the OCC Document and contains all the data of the opend, saved
 *  or newly created FreeCAD Document. The data are organized in trees with
 *  the FCLabel class.
 *  The Document manage the Undo and Redo mechanism and the linking of documents.
 *  Note: the documents are not free objects. They are completly handled by the
 *  Application. Only the Application can Open or destroy a document.
 *  The standard document can customiced by deriving a subclass in a application
 *  module. The custom document can handle different application specific behavior.
 *  @see Label
 */
class AppExport Document :public Base::PyHandler, public FCSubject<const DocChanges&>
{

public:
  /// init with an OpenCasCade Document (done by App::Application)
	Document(const Handle_TDocStd_Document &hDoc);
  /// Destruction
	virtual ~Document();

	//---------------------------------------------------------------------
	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	//void InitType(App::DocType *pcDocType);
	//App::DocType *GetDocType(void);

	//---------------------------------------------------------------------
	// CasCade exported functions goes here +++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

 	/** @name File handling of the document */
	//@{
	/// Save the Document under a new Name
	void SaveAs (const char* Name);
	/// Save the document under the name its been opened
	void Save (void);
	/// Is the document already saved to a file
	bool IsSaved() const;
	/// Get the document name of a saved document (UNICODE)
	const short* GetName() const;
	/// Get the path of a saved document (UNICODE)
	const short* GetPath() const;
	/// Returns the storage string of the document.
	const short* StorageFormat() const;
	/// Change the storage format of the document.
	void ChangeStorageFormat(const short* sStorageFormat) ;
  //@}

	/** @name Feature handling  */
	//@{
  /// Add a feature (by name) to this document and set it active
	Feature *AddFeature(const char* sName);
  /// Returns the active Feature of this document
	Feature *GetActiveFeature(void);
  /// Get the label of the active Feature
  TDF_Label GetActive(void){return _lActiveFeature;}
	//@}

  /** sets up the document
	 *  Get called when the 
	 */
  virtual void Init (void);

	/** @name methodes for modification and state handling
	 */
	//@{
	/// Get the Main Label of the document
	TDF_Label Main();
	/// Test if the document is empty
	bool IsEmpty() const;
	/// Returns False if the  document  contains notified modifications.
	bool IsValid() const;
    /// Mark a label as modified by the user
  void TouchState(const TDF_Label &);
  /// Set a label modified
  void ImpactState(const TDF_Label &);
	/// Set a special Label as modified
	//void SetModified(FCPyHandle<FCLabel> L);
	/// Remove all modifications. After this call The document becomes again Valid.
	void PurgeModified();
	/// Recompute if the document was  not valid and propagate the reccorded modification.
	void Recompute();

	/** @name methodes for the UNDO REDO handling
	 *  this methodes are usaly used by the GUI document! Its not intended
	 *  to use them directly. If the GUI is not up, there is usaly no UNDO / REDO
	 *  nececary.
	 */
	//@{
	/// New Command (Short cut for Commit and Open transaction)
	void NewCommand() ;
	/// returns True if a Command transaction is open
	bool HasOpenCommand() const;
	/** Open a new command transaction.
	 *  Raise If a Command is already open.
	 *  You may   check  it with the   previous method <HasOpenCommand>.
	 */
	void OpenCommand();
	/// Commit the Command transaction. Do nothing If there is no Command transaction open.
	void CommitCommand();
	/// Abort the  Command  transaction. Do nothing If there is no Command transaction open.
	void AbortCommand();
	/// The current limit on the number of undos
	int GetUndoLimit() const;
	/** Set the limit on the number of Undo Deltas stored.
	 *  0 will disable Undo on the document A negative value
	 *  means no limit. Note that by default Undo is disabled.
	 *  Enabling it will take effect with the next call to
	 *  NewCommand. Of course this limit is the same for Redo.
	 */
	void SetUndoLimit(const int L);
	/// Remove all stored Undos and Redos
	void ClearUndos();
	/// Returns the  number  of stored Undos. If greater than 0 Undo will be effective.
	int GetAvailableUndos() const;
	/// Will UNDO  one step, returns  False if no undo was done (Undos == 0).
	bool Undo();
	/// Returns the number   of stored Redos. If greater than 0 Redo will be effective.
	int GetAvailableRedos() const;
	/// Will REDO  one step, returns  False if no redo was done (Redos == 0).
	bool Redo() ;
	//@}


  /// Dumps the Document to stdout
  void Dump(void);

	/// Get the OCC Document Handle
	Handle_TDocStd_Document GetOCCDoc(void){return _hDoc;}


	virtual Base::PyObjectBase *GetPyObject(void);



	/* Not mapped so far:
virtual  void Update(const Handle(CDM_Document)& aToDocument,const Standard_Integer aReferenceIdentifier,const Standard_Address aModifContext) ;
  void SetData(const Handle(TDF_Data)& data) ;
  Handle_TDF_Data GetData() const;
 const TDF_LabelMap& GetModified() const;
 const TDF_DeltaList& GetUndos() const;
 const TDF_DeltaList& GetRedos() const;
  Standard_Boolean InitDeltaCompaction() ;
  Standard_Boolean PerformDeltaCompaction() ;
  void UpdateReferences(const TCollection_AsciiString& aDocEntry) ;
*/

	friend class DocumentPy;
	friend class LabelPy;
	friend class Application;

protected:
	/** @name atributes and methodes for label handling
	 */
/*	//@{
	/// less funktion for the map sorting of TDF_Labels
	struct LabelLess{
		bool operator () (const TDF_Label &cLabel1, const TDF_Label &cLabel2) const
		{
			return (((unsigned short) cLabel1.Depth()<<16)|(unsigned short) cLabel1.Tag()) <
				   (((unsigned short) cLabel2.Depth()<<16)|(unsigned short) cLabel2.Tag()) ;
		}
	};
	/// friend daclaration to allow access
	friend class FCLabel;
	FCPyHandle<FCLabel> HasLabel(TDF_Label cLabel);
	/// map of all existing python label wrappers (sorted)
	std::map <TDF_Label,FCPyHandle<FCLabel>,LabelLess> mcLabelMap;
	FCPyHandle<FCLabel> _hcMain;
	//@}
*/
	/// handle to the OCC document
	Handle_TDocStd_Document _hDoc;

	// pointer to the python class
	DocumentPy *_pcDocPy;

protected:
  /// Base label 
	TDF_Label _lBase;
  /// label for the position of the document
	TDF_Label _lPos;
  /// label where the features goes 
	TDF_Label _lFeature;
  /// next free label for features 
	int       _iNextFreeFeature;
  /// Label of the Active Feature
	TDF_Label _lActiveFeature;

  /// The Logbook for this document
  TFunction_Logbook _LogBook;


};


} //namespace App


#endif // __Document_h__
