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

#pragma warning( disable : 4251 )


#include <qobject.h>

#include "View.h"
#include <list>

#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>

#ifdef __linux
#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>
#endif

class ApplicationWindow;
class FCDocument;
class MDIWindow;


/** The Gui Document
 *  This is the document on GUI level. Its main responsibility is 
 *  keeping track off open windows for a document, handling the OCC
 *  interactiv context and warning on unsaved closes.
 *  All handled views on the document must inherit from FCView
 *  @see FCDocument 
 *  @see FCView
 */
class GuiExport FCGuiDocument :public QObject
{
	Q_OBJECT

public:
	FCGuiDocument(FCDocument* pcDocument, ApplicationWindow * app, const char * name=0);
	~FCGuiDocument();

	/// Geter for the Application 
	ApplicationWindow*				GetApplication(){return _pcAppWnd;}	
	/// Geter for the OCC Interactive context
	Handle(AIS_InteractiveContext)	GetContext(){return _hContext;}
	/// Gerer for the App Document 
	FCDocument*						GetDocument(void){return _pcDocument;}

	/// Creat a new View
	void CreateView(const char* sType); 


	void OnLastViewClosed(void);

	/// send Messages to the active view
	bool SendMsgToActiveView(const char* pMsg);
	/// send Messages to all views
	bool SendMsgToViews(const char* pMsg);


	/// set the parameter to the active view or reset in case of 0
	void SetActive(FCView* pcView);
	/// Geter for the Active View
	FCView* GetActiveView(void);

public slots:
	void slotCloseView(FCView* theView);


private:
	Handle(V3d_Viewer) Viewer(const Standard_CString aDisplay,const Standard_ExtString aName,
		const Standard_CString aDomain,const Standard_Real ViewSize , const V3d_TypeOfOrientation ViewProj,
		const Standard_Boolean ComputedMode,const Standard_Boolean aDefaultComputedMode );
private:
	int								_iWinCount;
	ApplicationWindow*				_pcAppWnd;
	/// List of all registered views
	FClist<FCView*>					_LpcViews;
	/// Active view
	FCView*							_pcActiveView;
	Handle(V3d_Viewer)				_hViewer;
	Handle(AIS_InteractiveContext)	_hContext;
	// the doc/Document
	FCDocument*						_pcDocument;
};



#endif