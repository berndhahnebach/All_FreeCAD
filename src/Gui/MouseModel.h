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

#include <qcursor.h>
#include <vector>
#include <Inventor/SbLinear.h>
#include <Inventor/SbVec2f.h>

// forwards
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;
class SoEvent;
class SbViewportRegion;
class SoMouseButtonEvent;
class SoLocation2Event;
class SoKeyboardEvent;

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
  enum { Continue=0, Restart=1, Finish=2, Cancel=3 };

  AbstractMouseModel();
  virtual ~AbstractMouseModel(void){}
  /// implement this in derived classes
  virtual void initialize() = 0;
  /// implement this in derived classes
  virtual void terminate () = 0;
  void grabMouseModel(Gui::View3DInventorViewer*);
  void releaseMouseModel(void);
  const std::vector<SbVec2f>& getPolygon() const { return _clPoly; }

  virtual void redraw() = 0;

  /** @name Mouse events*/
  //@{
  int handleEvent( const SoEvent * const ev, const SbViewportRegion& vp );
  //@}

protected:
  virtual int mouseButtonEvent( const SoMouseButtonEvent * const e, const QPoint& pos ){ return 0; };
  virtual int locationEvent   ( const SoLocation2Event   * const e, const QPoint& pos ){ return 0; };
  virtual int keyboardEvent   ( const SoKeyboardEvent    * const e )                   { return 0; };

  /// drawing stuff
  virtual void draw (){};

protected:
  Gui::View3DInventorViewer*_pcView3D;
  QCursor m_cPrevCursor;
  int  m_iXold, m_iYold;
  int  m_iXnew, m_iYnew;

private:
  std::vector<SbVec2f> _clPoly;
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

  virtual void redraw();

protected:
  virtual int mouseButtonEvent( const SoMouseButtonEvent * const e, const QPoint& pos );
  virtual int locationEvent   ( const SoLocation2Event   * const e, const QPoint& pos );
  virtual int keyboardEvent   ( const SoKeyboardEvent    * const e );

  /// draw the polygon
  virtual void draw ();

protected:
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
  virtual int mouseButtonEvent( const SoMouseButtonEvent * const e, const QPoint& pos );
  virtual int locationEvent   ( const SoLocation2Event   * const e, const QPoint& pos );
  virtual int keyboardEvent   ( const SoKeyboardEvent    * const e );

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

protected:
  virtual int mouseButtonEvent( const SoMouseButtonEvent * const e, const QPoint& pos );
  virtual int locationEvent   ( const SoLocation2Event   * const e, const QPoint& pos );
  virtual int keyboardEvent   ( const SoKeyboardEvent    * const e );
  /// draw circle and text
  virtual void draw ();

private:
  int    _nRadius;
  bool   _bWorking;
};

} // namespace Gui

#endif // MOUSEMODEL_H 
