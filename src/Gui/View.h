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


#ifndef __FCVIEW_H__
#define __FCVIEW_H__


#ifndef _PreComp_
# include <vector>
# include <qmainwindow.h>
#endif

namespace App
{
  class Document;
}

class QSplitter;
class QWidget;
class QPrinter;

namespace Gui 
{
class Document;
class ViewProvider;

/** Base class of all windows belonging to a document
 *  there are two ways of belonging to a document. The 
 *  first way is to a fixed one. The second way is to allways
 *  belong to the Active document. that means switching every time
 *  the active document is changing. It also means that the view 
 *  belongs sometimes to no document at all!
 *  @see TreeView
 *  @see Gui::Document
 *  @see Application
 *  @author Jürgen Riegel
 */
class GuiExport BaseView
{
public:
  /** View constructor
   * Atach the view to the given document. If the document is NULL
   * the view will attach to the active document. Be aware! there isn't
   * allways a active document!
   */
  BaseView( Gui::Document* pcDocument=0l);
  /** View destructor
   * Detach the view from the document, if Atached!
   */
  virtual ~BaseView();


  /** @name methods used by the Application and the GuiDocument 
   */
  //@{
  /// sets the view to an oter document (called by Application)
  void setDocument(Gui::Document* pcDocument);
  /// is send from the document in oder to close the document
  void onClose(void);
  //@}

  /// returns the document the view is attached to
  Gui::Document* getGuiDocument(){return _pcDocument;}
  /// returns the document the view is attached to
  App::Document* getAppDocument();
  /// indikates if the view is in passiv mode
  bool isPassiv(void){return bIsPassiv;}


  /** @name methods to overrride 
   */
  //@{
  /// get called when the document is updated
  virtual void onUpdate(void)=0;
  /// get called when the document is updated
  virtual void onRename(Gui::Document *pDoc){};
  /// returns the name of the view (important for messages)
  virtual const char *getName(void)=0;
  /// Mesage handler
  virtual bool onMsg(const char* pMsg, const char** ppReturn)=0;
  /// Mesage handler test
  virtual bool onHasMsg(const char* pMsg)=0;
  /// is called when the above function is called to handle document change stuff
  virtual void onNewDocument(Gui::Document* pcOldDocument,Gui::Document* pcNewDocument){};
  /// overvrit when checking on close state
  virtual bool canClose(void){return true;}
  //@}

  /** @name View provider handling 
   * View provider handle the visual apperance of things in the document
   * FreeCAD itself an all application modules can register View provider 
   * to visualize its new Features or Datatypes
   */
  //@{
  /// Register a new View provider
  static void addViewProvider(Gui::ViewProvider* pcProvider);
  //@}

protected:
  Gui::Document*  _pcDocument;
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
 *  @see Gui::Document
 *  @see Application
 *  @author Jürgen Riegel, Werner Mayer
 */
class GuiExport MDIView : public QMainWindow,public BaseView
{
  Q_OBJECT;

public:
  /** View constructor
   * Atach the view to the given document. If the document is NULL
   * the view will attach to the active document. Be aware! there isn't
   * allways a active document!
   */
  MDIView( Gui::Document* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
  /** View destructor
   * Detach the view from the document, if Atached!
   */
  ~MDIView();

  /// get called when the document is updated
  virtual void onUpdate(void)=0;
  /// get called when the document is updated
  virtual void onRename(Gui::Document *pDoc);


  /// returns the name of the view (important for messages)
  virtual const char *getName(void)=0;

  /// Mesage handler
  virtual bool onMsg(const char* pMsg,const char** ppReturn);
  /// Mesage handler test
  virtual bool onHasMsg(const char* pMsg);
  /// overvrit when checking on close state
  virtual bool canClose(void){return true;}
  /// print function of the view
  virtual void print( QPrinter* printer );

  QSize minimumSizeHint () const;

  /// sets the caption and its appropriate tab of tab bar (if registered)
  virtual void setCaption ( const QString & );

  /// MDI view mode enum
  enum ViewMode {
    Normal,     /**< Normal viewing, view is docked inside the MDI application window */  
    TopLevel,   /**< The view becomes a top level window and can moved outsinde the Application windwo */  
    FullScreen  /**< the view go to full screen viewing */
  };
  /**
   * If \a b is set to \a FullScreen the MDI view is displayed in full screen mode, if \a b
   * is set to \a TopLevel then it is displayed as an own top-level window, otherwise (\a Normal)
   * as tabbed window. For more hints refer to the Qt documentation to
   * QWidget::showFullScreen ().
   */
  virtual void setCurrentViewMode( ViewMode b );
  ViewMode currentViewMode() const { return _actualMode; }

signals:
  /// sends a message to the document
//  void sendCloseView(MDIView* theView);

  void message(const QString&, int );

public slots:
  void setActive(void);

protected:
  void closeEvent    ( QCloseEvent *e );
  void keyPressEvent ( QKeyEvent   *e );

  ViewMode _actualMode;
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

} // namespace Gui

#endif // __FCVIEW_H__ 
