/** \file $RCSfile$
 *  \brief The Gui Document
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
#ifndef __GUIDocument_h__
#define __GUIDocument_h__




#include "View.h"

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>
#include <TPrsStd_AISViewer.hxx>

#include "../Base/PyExport.h"
#include "../App/Document.h"

#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#	pragma warning( disable : 4290 )  // not implemented throw specification
#	pragma warning( disable : 4275 )  
#endif

class ApplicationWindow;
class MDIWindow;


/** The Gui Document
 *  This is the document on GUI level. Its main responsibility is 
 *  keeping track off open windows for a document, handling the OCC
 *  interactiv context and warning on unsaved closes.
 *  All handled views on the document must inherit from FCView
 *  @see FCDocument 
 *  @see FCView
 */
class GuiExport FCGuiDocument :public QObject, public FCDocument::ObserverType
{
	Q_OBJECT

public:
	FCGuiDocument(FCDocument* pcDocument, ApplicationWindow * app, const char * name=0);
	~FCGuiDocument();


	/// Save the document
	bool Save(void);

	/// Save the document under a new file name
	bool SaveAs(void);

	/// Observer message from the App doc
	virtual void OnChange(FCDocument::SubjectType &rCaller,FCDocument::MessageType Reason);

	/// Geter for the Application 
	ApplicationWindow*				GetAppWnd(){return _pcAppWnd;}	
	/// Geter for the OCC Interactive context
	Handle(AIS_InteractiveContext)	GetContext(){return _hContext;}
	/// Gerer for the App Document 
	FCDocument*						GetDocument(void){return _pcDocument;}


	/** @name methodes for activation handling */
	//@{
	/// Geter for the Active View
	FCView* GetActiveView(void);
	//@}



	/** @name methodes for View handling */
	//@{
	/// Creat a new View
	void CreateView(const char* sType); 
	/// send Messages to the active view
	bool SendMsgToActiveView(const char* pMsg);
	/// send Messages to all views
	bool SendMsgToViews(const char* pMsg);
	/// Attach a view (get called by the FCView constructor)
	void AttachView(FCBaseView* pcView, bool bPassiv=false);
	/// Detach a view (get called by the FCView destructor)
	void DetachView(FCBaseView* pcView, bool bPassiv=false);
	/// call update on attached all views
	void Update(void);
	/// close all attached views in order to close the gui document
	void CloseAllViews(void);
	//@}


	/** @name methodes for the UNDO REDO handling */
	//@{
	/// Open a new Undo transaction on the document
	void OpenCommand(const char* sName=0);
	/// Commit the Undo transaction on the document
	void CommitCommand(void);
	/// Abort the Undo transaction on the document
	void AbortCommand(void);
	/// Get an Undo string vector with the Undo names
	std::vector<std::string> GetUndoVector(void);
	/// Get an Redo string vector with the Redo names
	std::vector<std::string> GetRedoVector(void);
	/// Will UNDO  one or more steps
	void Undo(int iSteps);
	/// Will REDO  one or more steps
	void Redo(int iSteps) ;
	//@}



	/// handels the application close event
	void closeEvent ( QCloseEvent * e );
	///
	void CanClose(QCloseEvent * e );
	/// 
	bool IsLastView(void);

public slots:
//	void slotCloseView(FCView* theView);


private:
	Handle(V3d_Viewer) Viewer(const Standard_CString aDisplay,const Standard_ExtString aName,
		const Standard_CString aDomain,const Standard_Real ViewSize , const V3d_TypeOfOrientation ViewProj,
		const Standard_Boolean ComputedMode,const Standard_Boolean aDefaultComputedMode );
private:
	int								_iWinCount;
	ApplicationWindow*				_pcAppWnd;
	/// List of all registered views
	std::list<FCBaseView*>					_LpcViews;
	/// List of all registered views
	std::list<FCBaseView*>					_LpcPassivViews;
	/// Active view
	FCView*							_pcActiveView;
	Handle(V3d_Viewer)				_hViewer;
	Handle(AIS_InteractiveContext)	_hContext;
	// the doc/Document
	FCDocument*						_pcDocument;
	FCPyHandle<FCDocument>			_hcDocument;

	/** @name attributes for the UNDO REDO facility
	 */
	//@{
	/// undo names list
	std::list<std::string> listUndoNames;
	/// redo names list
	std::list<std::string> listRedoNames;	
	//@}

};



#endif
