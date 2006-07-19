/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef DOCK_WINDOW_H
#define DOCK_WINDOW_H


#include "../Base/Parameter.h"
#include "View.h"

class QDockWindow;
class QScrollView;
class QStringList;

namespace Gui {

class Application;
class DockWindow;

/** Father of all dockable windows.
 * @author Jürgen Riegel
 */
class GuiExport DockWindow : public QWidget, public BaseView
{
  Q_OBJECT;

public:
  DockWindow ( Gui::Document* pcDocument=0l, QWidget *parent=0, const char *name=0, WFlags f=0 );
  ~DockWindow();

  /** @name methods to overrride 
   */
  //@{
  /// get called when the document is updated
  virtual void onUpdate(void){};
  /// returns the name of the view (important for messages)
  virtual const char *getName(void) const { return "DockWindow"; };
  /// Mesage handler
  virtual bool onMsg(const char* pMsg,const char** ppReturn){ return false; };
  /// Mesage handler test
  virtual bool onHasMsg(const char* pMsg) const { return false; };
  /// overvrit when checking on close state
  virtual bool canClose(void){return true;}
  //@}
  void setCaption ( const QString & );
  /** Returns the associated QDockWindow object to this widget.
   * @note A valid pointer is not returned before this widget has been docked with DockWindowManager::addDockWindow().
   * Otherwise 0 is returned.
   */
  QDockWindow* dockWindow() const;
  /**
   * Sets the dock window's preferred width for its fixed extent (size) to \a w.
   * All other dock windows in the same dock arae get enlarged then. If there is 
   * no other dock window then this method has no effect.
   */
  void setFixedExtentWidth(int w);
  /**
   * Sets the dock window's preferred height for its fixed extent (size) to \a h.
   * All other dock windows in the same dock arae get enlarged then. If there is 
   * no other dock window then this method has no effect.
   */
  void setFixedExtentHeight(int h);

protected slots:
  virtual void languageChange();

private:
  QDockWindow* _dw;
  QString _caption;

  friend class DockWindowManager;
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
 *  @author Jürgen Riegel
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
  DockView( Gui::Document* pcDocument, QWidget* parent, const char* name, WFlags f=0 );
  /** View destructor
    * Detach the view from the document, if Atached!
    */
  ~DockView();

  /// get called when the document is updated
  virtual void onUpdate(void){};

  /// Mesage handler
  virtual bool onMsg(const char* pMsg);
  /// Mesage handler test
  virtual bool onHasMsg(const char* pMsg) const;


signals:
  /// sends a message to the document
  void sendCloseView(MDIView* theView);
};

/** The DockContainer class provides a dockable container window to embed a widget.
 *
 * \code
 * // create a non-modal dialog
 * MyDialog* dlg = new MyDialog( getMainWindow(), "MyDialog", false);
 *
 * // embed this dialog into a dockable widget container
 * DockWindowManager* pDockMgr = DockWindowManager::instance();
 * DockContainer* pDockDlg = new DockContainer( getMainWindow(), "MyDialog" );
 * pDockMgr->addDockWindow("MyDialog", pDockDlg, Qt::DockLeft );
 *
 * // do not allow to hide
 * pDockDlg->dockWindow()->setCloseMode(QDockWindow::Never);
 * pDockDlg->setDockedWidget(dlg);
 * pDockDlg->show();
 * \endcode
 *
 * If you want to also close the dock window when you close the dialog then call the following code
 * in the corresponding slot function of the dialog:
 * \code
 * // This slot is connected to the dialog's 'Close' button
 * void MyDialog::closeWindow()
 * {
 *   DockWindowManager* pDockMgr = DockWindowManager::instance();
 *   // destructs the associated QDockWindow and all its children
 *   pDockMgr->removeDockedWidget(this);
 * }
 * \endcode
 *
 * \author Werner Mayer
 */
class GuiExport DockContainer : public DockWindow
{
  Q_OBJECT

public:
  DockContainer( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~DockContainer();

  void setDockedWidget( QWidget* w );

private:
  QScrollView* sv;
};

/**
 * Class that manages the widgets inside a QDockWindow.
 * @see DockWindow
 * \author Werner Mayer
 */
class GuiExport DockWindowManager
{
public:
  /** Creates the only instance of the DockWindowManager. */
  static DockWindowManager* instance();

  DockWindow* getDockWindow( const QString& name );
  QPtrList<DockWindow> getDockWindows();
  void removeDockWindow( const QString& name );
  void removeDockWindow( DockWindow* dock );
  void removeDockedWidget( QWidget* docked );
  void addDockWindow( const QString& name, DockWindow *pcDocWindow, Qt::Dock pos = Qt::DockUnmanaged,
                      bool stretch=false, int extWidth=0, int extHeight=0 );
  void showDockWindows( const QStringList& );
  void hideDockWindows( const QStringList& );

private:
  DockWindowManager();
  ~DockWindowManager();
  static DockWindowManager* _instance;
  struct DockWindowManagerP* d;
};

} // namespace Gui

#endif // DOCK_WINDOW_H 
