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


#ifndef __FCview_h__
#define __FCview_h__


#ifndef _PreComp_
# include <vector>
# include <qmainwindow.h>
#endif

namespace App
{
  class Document;
}
class FCGuiDocument;
class QSplitter;
class QWidget;	
class QPrinter;

namespace Gui 
{
  class ViewProvider;
}

/** Base class of all windows belonging to a document
 *  there are two ways of belonging to a document. The 
 *  first way is to a fixed one. The second way is to allways
 *  belong to the Active document. that means switching every time
 *  the active document is changing. It also means that the view 
 *  belongs sometimes to no document at all!
 *  @see TreeView
 *  @see FCGuiDocument
 *  @see ApplicationWindow
 */
class FCBaseView
{
public:
	/** View constructor
	  * Atach the view to the given document. If the document is NULL
	  * the view will attach to the active document. Be aware! there isn't
	  * allways a active document!
	  */
	FCBaseView( FCGuiDocument* pcDocument=0l);
	/** View destructor
	  * Detach the view from the document, if Atached!
	  */
	virtual ~FCBaseView();


	/** @name methodes used by the Application and the GuiDocument 
	 */
	//@{
	/// sets the view to an oter document (called by ApplicationWindow)
	void SetDocument(FCGuiDocument* pcDocument);
	/// is send from the document in oder to close the document
	void Close(void);
	//@}

	

	/// returns the document the view is attached to
	FCGuiDocument* GetGuiDocument(){return _pcDocument;}
	/// returns the document the view is attached to
  App::Document* GetAppDocument();
	/// indikates if the view is in passiv mode
	bool IsPassiv(void){return bIsPassiv;}


	/** @name methodes to overrride 
	 */
	//@{
	/// get called when the document is updated
	virtual void Update(void)=0;
	/// returns the name of the view (important for messages)
	virtual const char *GetName(void)=0;
	/// Mesage handler
	virtual bool OnMsg(const char* pMsg)=0;
	/// Mesage handler test
	virtual bool OnHasMsg(const char* pMsg)=0;
	/// is called when the above function is called to handle document change stuff
	virtual void OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument){};
	/// overvrit when checking on close state
	virtual bool CanClose(void){return true;}
	//@}

	/** @name View provider handling 
	 * View provider handle the visual apperance of things in the document
	 * FreeCAD itself an all application modules can register View provider 
	 * to visualize its new Features or Datatypes
	 */
	//@{
	/// Register a new View provider
  static void AddViewProvider(Gui::ViewProvider* pcProvider);
	//@}




protected:
    FCGuiDocument*	_pcDocument;
	bool bIsDetached;
	bool bIsPassiv;

	// view provider container
  static std::vector<Gui::ViewProvider*> _vpcViewProvider;
};

/** Base class of all windows belonging to a document
 *  there are two ways of belonging to a document. The 
 *  first way is to a fixed one. The second way is to allways
 *  belong to the Active document. that means switching every time
 *  the active document is changing. It also means that the view 
 *  belongs sometimes to no document at all!
 *  @see TreeView
 *  @see FCGuiDocument
 *  @see ApplicationWindow
 */
class GuiExport MDIView : public QMainWindow,public FCBaseView
{
	Q_OBJECT;

public:
	/** View constructor
	  * Atach the view to the given document. If the document is NULL
	  * the view will attach to the active document. Be aware! there isn't
	  * allways a active document!
	  */
	MDIView( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	/** View destructor
	  * Detach the view from the document, if Atached!
	  */
	~MDIView();

	/// get called when the document is updated
	virtual void Update(void)=0;

	/// returns the name of the view (important for messages)
	virtual const char *GetName(void)=0;

	/// Mesage handler
	virtual bool OnMsg(const char* pMsg);
	/// Mesage handler test
	virtual bool OnHasMsg(const char* pMsg);
	/// overvrit when checking on close state
	virtual bool CanClose(void){return true;}
	/// print function of the view
	virtual void Print( QPrinter* printer );

  QSize minimumSizeHint () const;

signals:
	/// sends a message to the document
//	void sendCloseView(MDIView* theView);

  void message(const QString&, int );
	
public slots:	
	void SetActive(void);

protected:
	/// 
	void closeEvent(QCloseEvent *e);

};









/** The double view container class
 *  This class can handle two MDIView instances seperated by
 *  a splitter. Usuly its used to show a FCDocTree and a FC3dView
 */
/*
class FCDoubleView: public FCViewContainer
{
	Q_OBJECT;

public:
	FCDoubleView( MDIView* pcView1, FCView* pcView2, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCDoubleView();

	// a more sophisticated implementation later
	virtual MDIView* GetActiveView(void){return _pcView2;}
private:
	MDIView *		_pcView1;
	MDIView *		_pcView2;
	QSplitter*		_pcSplitter;

};
*/
/** The singel view container class
 *  This class can handle one MDIView instances 
 */
/*
class FCSingleView: public FCViewContainer
{
	Q_OBJECT;

public:
	FCSingleView( MDIView* pcView, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCSingleView();

	virtual void resizeEvent ( QResizeEvent * e);

	virtual MDIView* GetActiveView(void){return _pcView;}
protected:
	virtual void closeEvent(QCloseEvent *e);

private:
	MDIView *		_pcView;

};
*/

/** Holds a single view in a dock bar
 */
/*
class FCViewBar: public DockWindow
{
	Q_OBJECT;

public:
	FCViewBar( MDIView* pcView, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCViewBar();

	virtual MDIView* GetView(void){return _pcView;}

protected:
	virtual void resizeEvent ( QResizeEvent * e);

private:
	MDIView *		_pcView;

};
*/

#endif
