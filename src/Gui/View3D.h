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


#ifndef __VIEW3D__
#define __VIEW3D__

#ifndef _PreComp_
# include <stack>
# include <qgl.h>
# include <AIS_InteractiveContext.hxx>
# include <V3d_OrthographicView.hxx>
// some window system dependend
# ifndef FC_OS_WIN32
#   include <Xw_Window.hxx>
#   include <Graphic3d_GraphicDevice.hxx>
# else
#   include <WNT_Window.hxx>
#   include <Graphic3d_WNTGraphicDevice.hxx>
# endif
#endif

#include "View.h"

class QMouseEvent;
class QSplitter;
class QWidget;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QWidgetStack;
class QTabBar;

namespace Gui {
class Document;
class TreeView;
class View3D;
class MouseModel;


/** The 3D View Window
 * It consist out of the 3DView and the tree
 * \author Jürgen Riegel
 */
class MDIView3D: public Gui::MDIView
{
  Q_OBJECT

public:
  MDIView3D( Gui::Document* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
  ~MDIView3D();

  /// Mesage handler
  virtual bool onMsg(const char* pMsg);
  virtual const char *getName(void);


  virtual void onUpdate(void);


//signals:
  //void message(const QString&, int );

public slots:
//  void closeEvent(QCloseEvent* e);        
  void onWindowActivated ();
  void setCursor(const QCursor&);
  void dump();

protected:
  //void createViewActions();
private:
  View3D*     _pcView3D;
  QWidget*    _pcWidget;
  QWidgetStack* _pcWidgetStack;
  QTabBar*    _pcTabBar;
};

/**
 * The OpenCasCade Graphics rendering widget
 *
 * @author Juergen Riegel
 * @version 0.1
 */
class View3D: public QGLWidget
{
  Q_OBJECT

public:
  /** construtor of 3DView. Does nothing importend, the
   * tricky stuff about the COpenCasCade init comes with InitCasCadeView()
   * @param Handle(Graphic3d_GraphicDevice) OCC Graphic Device (DISPLAY0:0) comes from the Application
   * @param Handle(V3d_Viewer) OCC Viewer (note: _not_ View) from the Document
   * @param Handle(AIS_InteractiveContext) OCC Interactive Context comes from the Document
   * @see QGLWidget InitCasCadeView
   */
  View3D(Gui::Document*  pcDocument, QWidget *parent, const char *name="View3D", WFlags f=WDestructiveClose );

  /** Initialation of the OCC Graphics. Here is the bundling of the QGLWidget
   * with the OCCGraphics3D_Window. Its very importend that the XWindow is fully
   * showed bevor this function is called! There for its not done in the
   * constructor
   */
  bool initCasCadeView(void);

  // Managing MouseModels
  void pushMouseModel (Gui::MouseModel *);
  void popMouseModel (void);
  Gui::MouseModel* getMouseModel	(void);

  // Confiniance Geter
  Handle_V3d_View &getView   (void){return _hView;}
  Handle_V3d_Viewer &getViewer (void){return _hViewer;}
  Handle_AIS_InteractiveContext &getContext(void){return _hContext;}

  void showPopup(int x,int y);
  void showDimension (void) const;

  bool screenDump(Standard_CString theFile);

  bool onMsg(const char* pMsg);

protected:
  // user interaction events (got mainly handled in the MouseModel classes)
  virtual void mousePressEvent       ( QMouseEvent * );
  virtual void mouseReleaseEvent     ( QMouseEvent * );
  virtual void mouseMoveEvent        ( QMouseEvent * );
  virtual void mouseDoubleClickEvent ( QMouseEvent * );
  virtual void keyPressEvent         ( QKeyEvent   * );
  virtual void keyReleaseEvent       ( QKeyEvent   * );
  virtual void wheelEvent            ( QWheelEvent * );
  virtual void focusInEvent          ( QFocusEvent * );
  virtual void hideEvent             ( QHideEvent  * );

  /** Update the view when paint event occur. */
  virtual void paintEvent ( QPaintEvent * );
  /** Update the view when resize event occur.  */
  virtual void resizeEvent( QResizeEvent* );


private:
  Handle(V3d_View)  _hView;
  Handle(AIS_InteractiveContext)  _hContext;
  Handle(V3d_Viewer)              _hViewer;

  Gui::Document*  _pcDocument;

  std::stack<Gui::MouseModel *>      _cMouseStack;

  bool bIsInit;
};

} // namespace Gui

#endif

