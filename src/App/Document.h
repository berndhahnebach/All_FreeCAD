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
#include <TDF_Label.hxx>
#include <TDF_ChildIterator.hxx>

#include <map>
#include <vector>

class FCDocument;

/** The OCC Label wrapper class
 *  This class wrapps the functionality of the TDFSdt_Label of OCC. 
 *  Its used for building up hirachy in a OCC document by representing
 *  Nodes and Leavs
 *  @see FCDocument
 */
class AppExport FCLabel :public FCPyObject
{
	/** always start with Py_Header */
	Py_Header;

public:


	//---------------------------------------------------------------------
	// construction / destruction +++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	/// Constructer 
	FCLabel(const TDF_Label &hLabel,FCDocument *pcDocument, PyTypeObject *T = &Type);
	/// for Construction in python 
	static PyObject *PyMake(PyObject *, PyObject *);
	/// Destruction 
	~FCLabel();


	//---------------------------------------------------------------------
	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	//---------------------------------------------------------------------

	/// Gets a child label
	FCPyHandle<FCLabel> GetLabel(int);

	/// Get all child labels
	std::vector<FCPyHandle<FCLabel> > GetLabels(void);

	/// return if the label has at least one child
	bool HasChild(void){return _cLabel.HasChild()!=0;}

	/// Gets the OCC Label
	TDF_Label GetOCCLabel(void){return _cLabel;}

	//---------------------------------------------------------------------
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	//---------------------------------------------------------------------

	PyObject *_getattr(char *attr);				// __getattr__ function
	// getter setter
	int _setattr(char *attr, PyObject *value);	// __setattr__ function
	// methods
	PyObject *PyGetLabel(PyObject *args);	// Python wrapper
	static PyObject *sPyGetLabel(PyObject *self, PyObject *args, PyObject *kwd){return ((FCLabel*)self)->PyGetLabel(args);};
	PYFUNCDEF_D (FCLabel,PyHasChild);


protected:

	/// The OCC Label
	TDF_Label _cLabel;
	/// Pointer to the FCDocument where the label comes from 
	FCDocument *_pcDocument;

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
class AppExport FCDocument :public FCPyObject, public FCSubject
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
	/// map of all existing python label wrappers (sorted)
	std::map <TDF_Label,FCLabel*,LabelLess> mcLabelMap;
	FCLabel *						_pcMain;
	//@}
	
	/// handle to the OCC document 
	Handle_TDocStd_Document _hDoc;


};

#endif // __Document_h__
