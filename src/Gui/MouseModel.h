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


#ifndef __MOUSEMODEL_H__
#define __MOUSEMODEL_H__

// forwards
class QMouseEvent;
class QKeyEvent;
class Handle_V3d_View;
class Handle_V3d_Viewer;
class Handle_AIS_InteractiveContext;
class View3D;

namespace Gui {

/**
 * The mouse model base class
 * In derived classes you must implement the methods @ref initialize() and @ref terminate()
 * For all drawing stuff you just have to reimplement the @ref draw() method. 
 * In general you need not to do anything else.
 * \author Werner Mayer and Jürgen Riegel
 */
class MouseModel
{
public:
  MouseModel(void){};
  virtual ~MouseModel(void){}
  /// implement this in derived classes
  virtual void initialize() = 0;
  /// implement this in derived classes
  virtual void terminate () = 0;
  void initMouseModel(View3D *pcView3D);
  void releaseMouseModel(void);
  void moveMouseEvent (QMouseEvent *cEvent);
  void wheelMouseEvent (QWheelEvent *cEvent);

  void mousePressEvent    ( QMouseEvent *cEvent );
  void mouseReleaseEvent  ( QMouseEvent *cEvent );
  
  virtual void mouseLeftPressEvent     ( QMouseEvent *cEvent ){};
  virtual void mouseMiddlePressEvent   ( QMouseEvent *cEvent ){};
  virtual void mouseRightPressEvent    ( QMouseEvent *cEvent ){};
  virtual void mouseLeftReleaseEvent   ( QMouseEvent *cEvent ){};
  virtual void mouseMiddleReleaseEvent ( QMouseEvent *cEvent ){};
  virtual void mouseRightReleaseEvent  ( QMouseEvent *cEvent ){};
  virtual void mouseMoveEvent          ( QMouseEvent *cEvent ){};
  virtual void mouseDoubleClickEvent   ( QMouseEvent *cEvent ){};

  virtual void wheelEvent         ( QWheelEvent * ){};
  virtual void keyPressEvent      ( QKeyEvent * ){};
  virtual void keyReleaseEvent    ( QKeyEvent * ){}; 
  virtual void paintEvent         ( QPaintEvent * ){ draw(); } ;
  virtual void resizeEvent        ( QResizeEvent * ){ draw(); };

  // Geter
  View3D                  &GetView3D (void);
  Handle_V3d_View         &GetView   (void);
  Handle_V3d_Viewer       &GetViewer (void);
  Handle_AIS_InteractiveContext &GetContext(void);

protected:
  /// drawing stuff
  virtual void draw (){};
  void drawRect ( int x, int y, int w, int h, QPainter* p = NULL );
  void drawNode ( int x, int y, int w, int h, QPainter* p = NULL );
  void drawLine ( int x1, int y1, int x2, int y2, QPainter* p = NULL );
  void drawCircle ( int x, int y, int r, QPainter* p = NULL );
  void drawText ( int x, int y, const QString & str, QPainter* p = NULL );

protected:
  View3D *_pcView3D;
  QCursor m_cPrevCursor;
  int  m_iXold, m_iYold;
  int  m_iXnew, m_iYnew;
};

// -----------------------------------------------------------------------------------

/**
 * The standard model class
 * \author Jürgen Riegel
 */
class MouseModelStd :public MouseModel
{
public:
  MouseModelStd(void);
  virtual ~MouseModelStd(){}

  /// do nothing
  virtual void initialize();
  /// do nothing
  virtual void terminate();
  virtual void mousePressEvent        ( QMouseEvent *cEvent);
  virtual void mouseReleaseEvent      ( QMouseEvent *cEvent);
  virtual void mouseLeftPressEvent    ( QMouseEvent *cEvent);
  virtual void mouseMiddlePressEvent  ( QMouseEvent *cEvent);
  virtual void mouseRightPressEvent   ( QMouseEvent *cEvent);
  virtual void mouseLeftReleaseEvent  ( QMouseEvent *cEvent);
  virtual void mouseMiddleReleaseEvent( QMouseEvent *cEvent);
  virtual void mouseRightReleaseEvent ( QMouseEvent *cEvent);
  virtual void mouseMoveEvent         ( QMouseEvent *cEvent);
  virtual void mouseDoubleClickEvent  ( QMouseEvent * );
  virtual void keyPressEvent          ( QKeyEvent * );
  virtual void keyReleaseEvent        ( QKeyEvent * );
  virtual void wheelEvent             ( QWheelEvent * );

protected:
  enum {
    nothing = 0,
    rotation,
    panning,
    zooming,
    selection,
    addselection
  } mode;

  int iX,iY;
};

// -----------------------------------------------------------------------------------

/**
 * The poly picker mouse model class
 * Create a polygon
 * \author Werner Mayer
 */
class MouseModelPolyPicker : public MouseModelStd
{
public:
  MouseModelPolyPicker();
  virtual ~MouseModelPolyPicker();

  /// set the new mouse cursor
  virtual void initialize();
  /// do nothing
  virtual void terminate();
  virtual void mouseLeftPressEvent     ( QMouseEvent *cEvent );
  virtual void mouseMiddlePressEvent   ( QMouseEvent *cEvent );
  virtual void mouseRightPressEvent    ( QMouseEvent *cEvent );
  virtual void mouseDoubleClickEvent   ( QMouseEvent *cEvent );
  virtual void wheelEvent              ( QWheelEvent *cEvent );
  virtual void keyPressEvent           ( QKeyEvent   *cEvent );

protected:
  /// draw the polygon
  virtual void draw ();
  std::vector<QPoint> _cNodeVector;
  int  m_iRadius, m_iNodes;
  bool m_bWorking, m_bDrawNodes;
};

// -----------------------------------------------------------------------------------

/**
 * The selection mouse model class
 * Draws a rectangle for selection
 * \author Werner Mayer
 */
class MouseModelSelection : public MouseModelStd 
{
public:
  MouseModelSelection();
  virtual ~MouseModelSelection();

  /// do nothing
  virtual void initialize();
  /// do nothing
  virtual void terminate();
  virtual void mouseLeftPressEvent    ( QMouseEvent *cEvent );
  virtual void mouseLeftReleaseEvent  ( QMouseEvent *cEvent );

protected:
  /// draw the rectangle
  virtual void draw ();

private:
  bool m_bWorking;
};

// -----------------------------------------------------------------------------------

/**
 * The picker mouse model class
 * \author Werner Mayer
 */
class MouseModelCirclePicker : public MouseModelStd 
{
public:
  MouseModelCirclePicker();
  virtual ~MouseModelCirclePicker();

  /// set the new mouse cursor
  virtual void initialize();
  /// call the @ref draw() method to clear the view
  virtual void terminate();
  virtual void mouseRightPressEvent  ( QMouseEvent  *cEvent );
  virtual void wheelEvent            ( QWheelEvent  *cEvent );

protected:
  /// draw circle and text
  virtual void draw ();

private:
  int    _nRadius;
};

} // namespace Gui

#endif
