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


#ifndef __Windows_h__
#define __Windows_h__


#ifndef _PreComp_
# include <qdockwindow.h>
#endif

#include "../Base/Parameter.h"
#include "View.h"

class ApplicationWindow;

namespace Gui {

class DockWindow;

/** Adapter class to the parameter of FreeCAD for all Windows
 * Retrive the parameter group of the specific window
 * by the windowname.
 */
class GuiExport WindowParameter : public FCParameterGrp::ObserverType
{
public:
  WindowParameter(const char *name);
  virtual ~WindowParameter();

  virtual void OnParameterChanged(void);

  /// get the parameters
  FCParameterGrp::handle GetParameter(void);
  /// return the parameter group of this window
  FCParameterGrp::handle GetWindowParameter(void);

  ApplicationWindow* GetAppWnd(void);

private:
  FCParameterGrp::handle _handle;
};


/** father of all FreeCAD windows (execept the view containers)
 */
class GuiExport DockWindow : public QWidget, public FCBaseView
{
  Q_OBJECT;

public:
  DockWindow ( FCGuiDocument* pcDocument=0l, QWidget *parent=0, const char *name=0, WFlags f=0 );
  ~DockWindow();

  /** @name methodes to overrride 
   */
  //@{
  /// get called when the document is updated
  virtual void Update(void){};
  /// returns the name of the view (important for messages)
  virtual const char *GetName(void){ return "DockWindow"; };
  /// Mesage handler
  virtual bool OnMsg(const char* pMsg){ return false; };
  /// Mesage handler test
  virtual bool OnHasMsg(const char* pMsg) { return false; };
  /// is called when the above function is called to handle document change stuff
  virtual void OnNewDocument(FCGuiDocument* pcOldDocument,FCGuiDocument* pcNewDocument){};
  /// overvrit when checking on close state
  virtual bool CanClose(void){return true;}
  //@}
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
class GuiExport DockView : public DockWindow
{
	Q_OBJECT;

public:
	/** View constructor
	  * Atach the view to the given document. If the document is NULL
	  * the view will attach to the active document. Be aware! there isn't
	  * allways a active document!
	  */
	DockView( FCGuiDocument* pcDocument, QWidget* parent, const char* name, WFlags f=0 );
	/** View destructor
	  * Detach the view from the document, if Atached!
	  */
	~DockView();

	/// get called when the document is updated
	virtual void Update(void){};

	/// returns the name of the view (important for messages)
	virtual const char *GetName(void)=0;

	/// Mesage handler
	virtual bool OnMsg(const char* pMsg);
	/// Mesage handler test
	virtual bool OnHasMsg(const char* pMsg);


signals:
	/// sends a message to the document
	void sendCloseView(MDIView* theView);
	
};

/**
 * Class that manages the widgets inside a QDockWindow.
 * @see DockWindow
 * \author Werner Mayer
 */
class GuiExport DockWindowManager
{
public:
  DockWindowManager();
  ~DockWindowManager();

  DockWindow* getDockWindow( const QString& name );
  QPtrList<DockWindow> getDockWindows();
  void removeDockWindow( const QString& name );
  void addDockWindow( const QString& name, DockWindow *pcDocWindow, Qt::Dock pos = Qt::DockUnmanaged,
                      bool stretch=false, int extWidth=0, int extHeight=0 );

private:
  struct DockWindowManagerP* d;
};

} // namespace Gui

#endif
