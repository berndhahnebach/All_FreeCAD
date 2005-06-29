/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef __GUIDOCUMENT_h__
#define __GUIDOCUMENT_h__

#include "View.h"

#ifndef _PreComp_
# include <AIS_InteractiveContext.hxx>
# include <V3d_Viewer.hxx>
# include <TPrsStd_AISPresentation.hxx>
#endif

#include "../Base/PyExport.h"
#include "../App/Document.h"

#ifdef _MSC_VER
# pragma warning( disable : 4251 )
# pragma warning( disable : 4503 )
# pragma warning( disable : 4786 )  // specifier longer then 255 chars
# pragma warning( disable : 4290 )  // not implemented throw specification
# pragma warning( disable : 4275 )  
#endif
namespace App
{
  class Feature;
}

namespace Gui {

class ViewProviderInventor;
class ApplicationWindow;

/** The Gui Document
 *  This is the document on GUI level. Its main responsibility is 
 *  keeping track off open windows for a document, handling the OCC
 *  interactiv context and warning on unsaved closes.
 *  All handled views on the document must inherit from MDIView
 *  @see FCDocument 
 *  @see MDIView
 *  @author Jürgen Riegel
 */
class GuiExport Document :public QObject, public App::Document::ObserverType
{
  Q_OBJECT

public:
  Document(App::Document* pcDocument, ApplicationWindow * app, const char * name=0);
  ~Document();


  /// Save the document
  bool save(void);

  /// Save the document under a new file name
  bool saveAs(void);

  /// Observer message from the App doc
  virtual void OnChange(App::Document::SubjectType &rCaller,App::Document::MessageType Reason);

  /// Geter for the Application 
  ApplicationWindow*    getAppWnd(){return _pcAppWnd;}	
  /// Geter for the OCC Interactive context
  Handle(AIS_InteractiveContext)  getContext(){return _hContext;}
  /// Gerer for the App Document 
  App::Document*  getDocument(void){return _pcDocument;}


  /** @name methodes for activation handling */
  //@{
  /// Geter for the Active View
  Gui::MDIView* getActiveView(void);
  //@}



  /** @name methodes for View handling */
  //@{
  /// Creat a new View
  void createView(const char* sType); 
  /** send Messages to the active view 
    * send a specific massage to the active view and is able to recive a
    * return massage
    */
  bool sendMsgToActiveView(const char* pMsg, const char** pReturn=0);
  /// send Messages to all views
  bool sendMsgToViews(const char* pMsg);
  /// Attach a view (get called by the MDIView constructor)
  void attachView(Gui::BaseView* pcView, bool bPassiv=false);
  /// Detach a view (get called by the MDIView destructor)
  void detachView(Gui::BaseView* pcView, bool bPassiv=false);
  /// call update on attached all views
  void onUpdate(void);
  /// close all attached views in order to close the gui document
  void closeAllViews(void);
  //@}


  /** @name methodes for the UNDO REDO handling */
  //@{
  /// Open a new Undo transaction on the document
  void openCommand(const char* sName=0);
  /// Commit the Undo transaction on the document
  void commitCommand(void);
  /// Abort the Undo transaction on the document
  void abortCommand(void);
  /// Get an Undo string vector with the Undo names
  std::vector<std::string> getUndoVector(void);
  /// Get an Redo string vector with the Redo names
  std::vector<std::string> getRedoVector(void);
  /// Will UNDO  one or more steps
  void undo(int iSteps);
  /// Will REDO  one or more steps
  void redo(int iSteps) ;
  //@}

  /// handels the application close event
  void closeEvent ( QCloseEvent * e );
  ///
  void canClose(QCloseEvent * e );
  /// 
  bool isLastView(void);

public slots:
//	void slotCloseView(MDIView* theView);


private:
  Handle(V3d_Viewer) Viewer(const Standard_CString aDisplay,const Standard_ExtString aName,
    const Standard_CString aDomain,const Standard_Real ViewSize , const V3d_TypeOfOrientation ViewProj,
    const Standard_Boolean ComputedMode,const Standard_Boolean aDefaultComputedMode );
private:
  static int _iDocCount;
  int        _iWinCount;
  int        _iDocId;
  ApplicationWindow*    _pcAppWnd;
  /// List of all registered views
  std::list<Gui::BaseView*> _LpcViews;
  /// List of all registered views
  std::list<Gui::BaseView*> _LpcPassivViews;
  /// Active view
  Gui::MDIView* _pcActiveView;
  Handle(V3d_Viewer)  _hViewer;
  Handle(AIS_InteractiveContext)  _hContext;
  // the doc/Document
  App::Document*  _pcDocument;
  Base::PyHandle<App::Document>    _hcDocument;

  Handle(TPrsStd_AISPresentation) _ActivePresentation;


  std::map<App::Feature*,ViewProviderInventor*> _ViewProviderMap;

  /** @name attributes for the UNDO REDO facility
   */
  //@{
  /// undo names list
  std::list<std::string> listUndoNames;
  /// redo names list
  std::list<std::string> listRedoNames;
  //@}
};

} // namespace Gui


#endif // __GUIDOCUMENT_h__
