/** \file $RCSfile$
 *  \brief The document module
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
 
#ifndef __Document_h__
#define __Document_h__



#include "../Base/PyExport.h"
#include "../Base/Observer.h"
#include <TDocStd_Document.hxx>
//#include <TDF_Label.hxx>
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



class FCDocument;
class FCLabel;
class FCDocType;



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
class AppExport FCDocChanges
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
 *  @see FCLabel
 */
class AppExport FCDocument : public FCPyObject, public FCSubject<const FCDocChanges&>
{
	/// always start with Py_Header
	Py_Header;

public:
	FCDocument(const Handle_TDocStd_Document &hDoc, PyTypeObject *T = &Type);
	static PyObject *PyMake(PyObject *, PyObject *);

	~FCDocument();

	//---------------------------------------------------------------------
	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	void InitType(FCDocType *pcDocType);
	FCDocType *GetDocType(void);

	//---------------------------------------------------------------------
	// CasCade exported functions goes here +++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

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
	/// Get the Main Label of the document
	FCPyHandle<FCLabel> Main();
	/// Test if the document is empty
	bool IsEmpty() const;
	/// Returns False if the  document  contains notified modifications.
	bool IsValid() const;
	/// Set a special Label as modified
	void SetModified(FCLabel* L);
	/// /// Remove all modifications. After this call The document becomesagain Valid.
	void PurgeModified();
	/// Recompute if the document was  not valid and propagate the reccorded modification.
	void Recompute();
	/// Get the OCC Document Handle
	Handle_TDocStd_Document GetOCCDoc(void){return _hDoc;}

	
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

	
	/// Returns the storage string of the document.
	const short* StorageFormat() const;
	/// Change the storage format of the document.
	void ChangeStorageFormat(const short* sStorageFormat) ;

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
	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	virtual PyObject *_repr(void);  				// the representation
	PyObject *_getattr(char *attr);					// __getattr__ function
	int _setattr(char *attr, PyObject *value);		// __setattr__ function
	PyObject *PyUndo(PyObject *args);		// Python wrapper
	static PyObject *sPyUndo(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyUndo(args);};
	PyObject *PyRedo(PyObject *args);		// Python wrapper
	static PyObject *sPyRedo(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyRedo(args);};
	PyObject *PyClearUndos(PyObject *args);		// Python wrapper
	static PyObject *sPyClearUndos(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyClearUndos(args);};
	PyObject *PySaveAs(PyObject *args);		// Python wrapper
	static PyObject *sPySaveAs(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PySaveAs(args);};
	PyObject *PySave(PyObject *args);		// Python wrapper
	static PyObject *sPySave(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PySave(args);};
	PyObject *PySetModified(PyObject *args);		// Python wrapper
	static PyObject *sPySetModified(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PySetModified(args);};
	PyObject *PyPurgeModified(PyObject *args);		// Python wrapper
	static PyObject *sPyPurgeModified(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyPurgeModified(args);};
	PyObject *PyNewCommand(PyObject *args);		// Python wrapper
	static PyObject *sPyNewCommand(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyNewCommand(args);};
	PyObject *PyOpenCommand(PyObject *args);		// Python wrapper
	static PyObject *sPyOpenCommand(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyOpenCommand(args);};
	PyObject *PyCommitCommand(PyObject *args);		// Python wrapper
	static PyObject *sPyCommitCommand(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyCommitCommand(args);};
	PyObject *PyAbortCommand(PyObject *args);		// Python wrapper
	static PyObject *sPyAbortCommand(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyAbortCommand(args);};
	PyObject *PyRecompute(PyObject *args);		// Python wrapper
	static PyObject *sPyRecompute(PyObject *self, PyObject *args, PyObject *kwd){return ((FCDocument*)self)->PyRecompute(args);};



protected:
	/** @name atributes and methodes for label handling
	 */
	//@{
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
	FCLabel *HasPyLabel(TDF_Label cLabel);
	/// handle to the OCC document
	Handle_TDocStd_Document _hDoc;
	/// map of all existing python label wrappers (sorted)
	std::map <TDF_Label,FCLabel*,LabelLess> mcLabelMap;
	FCLabel *						_pcMain;
	//@}



};

#endif // __Document_h__
