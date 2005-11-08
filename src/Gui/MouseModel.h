/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef MOUSEMODEL_H
#define MOUSEMODEL_H

#ifndef _PreComp_
# include <qcursor.h>
# include <vector>
#endif

// forwards
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;

namespace Gui {
  class View3DInventorViewer;
}

namespace Gui {

/**
 * The mouse model base class
 * In derived classes you must implement the methods @ref initialize() and @ref terminate()
 * For all drawing stuff you just have to reimplement the @ref draw() method. 
 * In general you need not to do anything else.
 * \author Werner Mayer and Jürgen Riegel
 */
class GuiExport AbstractMouseModel
{
public:
  AbstractMouseModel();
  virtual ~AbstractMouseModel(void){}
  /// implement this in derived classes
  virtual void initialize() = 0;
  /// implement this in derived classes
  virtual void terminate () = 0;
  void grabMouseModel(Gui::View3DInventorViewer*);
  void releaseMouseModel(void);

  /** @name Mouse events*/
  //@{
  bool handleEvent( const SoEvent * const ev, const SbViewportRegion& vp );
  virtual void mouseMoveEvent    ( QMouseEvent *cEvent );
  void mousePressEvent   ( QMouseEvent *cEvent );
  void mouseReleaseEvent ( QMouseEvent *cEvent );
  void wheelMouseEvent   ( QWheelEvent *cEvent );
  //@}

protected:
  virtual void mouseLeftPressEvent     ( QMouseEvent *cEvent ){};
  virtual void mouseMiddlePressEvent   ( QMouseEvent *cEvent ){};
  virtual void mouseRightPressEvent    ( QMouseEvent *cEvent ){};
  virtual void mouseLeftReleaseEvent   ( QMouseEvent *cEvent ){};
  virtual void mouseMiddleReleaseEvent ( QMouseEvent *cEvent ){};
  virtual void mouseRightReleaseEvent  ( QMouseEvent *cEvent ){};
  virtual void mouseDoubleClickEvent   ( QMouseEvent *cEvent ){};

  virtual void wheelEvent         ( QWheelEvent * ){};
  virtual void keyPressEvent      ( QKeyEvent * ){};
  virtual void keyReleaseEvent    ( QKeyEvent * ){}; 
  virtual void paintEvent         ( QPaintEvent * ){ draw(); } ;
  virtual void resizeEvent        ( QResizeEvent * ){ draw(); };

protected:
  /// drawing stuff
  virtual void draw (){};

protected:
  Gui::View3DInventorViewer*_pcView3D;
  QCursor m_cPrevCursor;
  int  m_iXold, m_iYold;
  int  m_iXnew, m_iYnew;

private:
  QTime _timer;
};

// -----------------------------------------------------------------------------------

/**
 * The standard model class
 * \author Jürgen Riegel
 */
class GuiExport BaseMouseModel : public AbstractMouseModel
{
public:
  BaseMouseModel();
  virtual ~BaseMouseModel(){}
};

// -----------------------------------------------------------------------------------

/**
 * The poly picker mouse model class
 * Create a polygon
 * \author Werner Mayer
 */
class GuiExport PolyPickerMouseModel : public BaseMouseModel
{
public:
  PolyPickerMouseModel();
  virtual ~PolyPickerMouseModel();

  /// set the new mouse cursor
  virtual void initialize();
  /// do nothing
  virtual void terminate();
  
  void mouseMoveEvent    ( QMouseEvent *cEvent );

protected:
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
class GuiExport SelectionMouseModel : public BaseMouseModel 
{
public:
  SelectionMouseModel();
  virtual ~SelectionMouseModel();

  /// do nothing
  virtual void initialize();
  /// do nothing
  virtual void terminate();

protected:
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
class GuiExport CirclePickerMouseModel : public BaseMouseModel 
{
public:
  CirclePickerMouseModel();
  virtual ~CirclePickerMouseModel();

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

#endif // MOUSEMODEL_H 
