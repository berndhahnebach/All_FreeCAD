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

class GuiExport FCGuiDocument :public QObject
{
	Q_OBJECT

public:
	FCGuiDocument(FCDocument* pcDocument, ApplicationWindow * app, const char * name=0);
	~FCGuiDocument();

	ApplicationWindow* GetApplication(){return _pcAppWnd;}	
	Handle(AIS_InteractiveContext) GetContext(){return _hContext;}

	void CreateView(void); 

	FCDocument*	 GetDoc(void){return _pcDocument;}

	void OnLastViewClosed(void);

public slots:
	void slotCloseView(FCView* theView);


private:
	Handle(V3d_Viewer) Viewer(const Standard_CString aDisplay,const Standard_ExtString aName,
		const Standard_CString aDomain,const Standard_Real ViewSize , const V3d_TypeOfOrientation ViewProj,
		const Standard_Boolean ComputedMode,const Standard_Boolean aDefaultComputedMode );
private:
	int								_iWinCount;
	ApplicationWindow*				_pcAppWnd;
//#	pragma warning( disable : 4251 )
	FClist<FCView*>			_LpcViews;
	Handle(V3d_Viewer)				_hViewer;
	Handle(AIS_InteractiveContext)	_hContext;
//#	pragma warning( default : 4251 )
	// the doc/Document
	FCDocument*						_pcDocument;
};



#endif