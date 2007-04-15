/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef GUI_MDIVIEW_H
#define GUI_MDIVIEW_H

#include "View.h"

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

namespace Gui 
{
class Document;

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
class GuiExport MDIView : public QMainWindow, public BaseView
{
  Q_OBJECT;

  TYPESYSTEM_HEADER();


public:
  /** View constructor
   * Atach the view to the given document. If the document is NULL
   * the view will attach to the active document. Be aware! there isn't
   * allways a active document!
   */
  MDIView( Gui::Document* pcDocument, QWidget* parent, const char* name, Qt::WFlags wflags=Qt::WDestructiveClose );
  /** View destructor
   * Detach the view from the document, if Atached!
   */
  ~MDIView();

  /// get called when the document is updated
  virtual void onUpdate(void)=0;
  /// get called when the document is updated
  virtual void onRename(Gui::Document *pDoc);

  /// Mesage handler
  virtual bool onMsg(const char* pMsg,const char** ppReturn);
  /// Mesage handler test
  virtual bool onHasMsg(const char* pMsg) const;
  /// overvrit when checking on close state
  virtual bool canClose(void){return true;}
  /// print function of the view
  virtual void print();

  QSize minimumSizeHint () const;

  /// sets the caption and its appropriate tab of tab bar (if registered)
  virtual void setCaption ( const QString & );

  /// MDI view mode enum
  enum ViewMode {
    Normal,     /**< Normal viewing, view is docked inside the MDI application window */  
    TopLevel,   /**< The view becomes a top level window and can be moved outsinde the application window */  
    FullScreen  /**< The view goes to full screen viewing */
  };
  /**
   * If \a b is set to \a FullScreen the MDI view is displayed in full screen mode, if \a b
   * is set to \a TopLevel then it is displayed as an own top-level window, otherwise (\a Normal)
   * as tabbed window. For more hints refer to the Qt documentation to
   * QWidget::showFullScreen ().
   */
  virtual void setCurrentViewMode( ViewMode b );
  ViewMode currentViewMode() const { return _actualMode; }

Q_SIGNALS:
  void message(const QString&, int );

public Q_SLOTS:
  void setActiveView();

protected Q_SLOTS:
  virtual void showActiveView( MDIView* );

protected:
  void windowActivationChange( bool oldActive );
  void closeEvent    ( QCloseEvent *e );

  ViewMode _actualMode;
};

} // namespace Gui

#endif // GUI_MDIVIEW_H 
