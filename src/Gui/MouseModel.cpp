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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qevent.h>
# include <qpainter.h>
# include <qpixmap.h>
#endif

#include <Base/Console.h>

#include "MouseModel.h"
#include "View3DInventor.h"
#include "View3DInventorViewer.h"

using namespace Gui; 

AbstractMouseModel::AbstractMouseModel() : _pcView3D(0)
{
}

void AbstractMouseModel::grabMouseModel( Gui::View3DInventorViewer* viewer )
{
  _pcView3D=viewer;
  m_cPrevCursor = _pcView3D->getWidget()->cursor();

  // do initialization of your mousemodel
  initialize();
}

void AbstractMouseModel::releaseMouseModel()
{
  // do termination of your mousemodel
  terminate();

  _pcView3D->getWidget()->setCursor(m_cPrevCursor);
  _pcView3D = 0;
}

void AbstractMouseModel::mouseMoveEvent( QMouseEvent *cEvent )
{
  // do all the drawing stuff for us
  QPoint clPoint = cEvent->pos();
  draw();
  m_iXnew = clPoint.x(); 
  m_iYnew = clPoint.y();
  draw();
}

void AbstractMouseModel::wheelMouseEvent( QWheelEvent *cEvent )
{
  // do all the drawing stuff for us
  //QPoint clPoint = cEvent->pos();
  draw();
  wheelEvent(cEvent);
  draw();
}

void AbstractMouseModel::mousePressEvent(QMouseEvent* cEvent)
{
  if ( cEvent->type() == QEvent::MouseButtonDblClick )
  {
    mouseDoubleClickEvent(cEvent);
    return;
  }

  switch (cEvent->button())
  {
    case Qt::LeftButton:
      mouseLeftPressEvent(cEvent);
      break;
    case Qt::MidButton:
      mouseMiddlePressEvent(cEvent);
      break;
    case Qt::RightButton:
      mouseRightPressEvent(cEvent);
      break;
    default:
      break;
  };
}

void AbstractMouseModel::mouseReleaseEvent(QMouseEvent* cEvent)
{
  switch (cEvent->button())
  {
    case Qt::LeftButton:
      mouseLeftReleaseEvent(cEvent);
      break;
    case Qt::MidButton:
      mouseMiddleReleaseEvent(cEvent);
      break;
    case Qt::RightButton:
      mouseRightReleaseEvent(cEvent);
      break;
    default:
      break;
  };
}

// -----------------------------------------------------------------------------------

BaseMouseModel::BaseMouseModel()
  :AbstractMouseModel()
{
}

// -----------------------------------------------------------------------------------

PolyPickerMouseModel::PolyPickerMouseModel() 
{
  m_iRadius    = 2;
  m_iNodes     = 0;
  m_bWorking   = false;
  m_bDrawNodes = true;
}

void PolyPickerMouseModel::initialize()
{
  _pcView3D->getWidget()->setCursor(QCursor(QCursor::CrossCursor));
}

void PolyPickerMouseModel::terminate()
{
  _pcView3D->getGLWidget()->releaseMouse();
}

void PolyPickerMouseModel::draw ()
{
  if ( m_bWorking )
  {
    if (m_iNodes < int(_cNodeVector.size()))
    {
      m_iNodes = int(_cNodeVector.size());
      // drawing the point
      if (m_bDrawNodes == true)
      {
        _pcView3D->drawNode(m_iXnew-m_iRadius,m_iYnew-m_iRadius,2*m_iRadius,2*m_iRadius);
      }

      if ( _cNodeVector.size() > 2 )
      {
        QPoint start = _cNodeVector.front();
        _pcView3D->drawLine(m_iXnew,m_iYnew,start.x(), start.y() );
      }
    }
    else
    {
      _pcView3D->drawLine(m_iXnew,m_iYnew,m_iXold,m_iYold );
      if ( _cNodeVector.size() > 1 )
      {
        QPoint start = _cNodeVector.front();
        _pcView3D->drawLine(m_iXnew,m_iYnew,start.x(), start.y() );
      }
    }
  }
}

PolyPickerMouseModel::~PolyPickerMouseModel()
{
}

void PolyPickerMouseModel::mouseMoveEvent( QMouseEvent *cEvent )
{
  // do all the drawing stuff for us
  QPoint clPoint = cEvent->pos();

  if ( m_bWorking )    
  {
    // check the position
    QRect r = _pcView3D->getGLWidget()->rect();
    if ( !r.contains( clPoint ) )
    {
      if( clPoint.x() < r.left() )
          clPoint.setX( r.left() );
      if( clPoint.x() > r.right() )
          clPoint.setX( r.right() );
      if( clPoint.y() < r.top() )
          clPoint.setY( r.top() );
      if( clPoint.y() > r.bottom() )
          clPoint.setY( r.bottom() );

      QPoint newPos = _pcView3D->getGLWidget()->mapToGlobal( clPoint );
      QCursor::setPos( newPos );
    }
  }

  draw();
  m_iXnew = clPoint.x(); 
  m_iYnew = clPoint.y();
  draw();
}

void PolyPickerMouseModel::mouseLeftPressEvent( QMouseEvent *cEvent)
{
  QPoint point = cEvent->pos();

  // start working from now on
  if ( !m_bWorking )    
  {
    m_bWorking = true;
    // clear the old polygon
    _cNodeVector.clear();
    _pcView3D->getGLWidget()->update();
    _pcView3D->getGLWidget()->grabMouse();
  }

  _cNodeVector.push_back(point);

  m_iXnew = point.x();  m_iYnew = point.y();
  m_iXold = point.x();  m_iYold = point.y();
}

void PolyPickerMouseModel::mouseMiddlePressEvent( QMouseEvent *cEvent)
{
}

void PolyPickerMouseModel::mouseRightPressEvent( QMouseEvent *cEvent)
{
}

void PolyPickerMouseModel::wheelEvent ( QWheelEvent * e)
{
  // do nothing
}

void PolyPickerMouseModel::keyPressEvent( QKeyEvent * e)
{
  switch (e->key())
  {
    case Qt::Key_Escape:
//      _pcView3D->popMouseModel();
      break;
    default:
      BaseMouseModel::keyPressEvent(e);
      break;
  }
}

void PolyPickerMouseModel::mouseDoubleClickEvent( QMouseEvent *cEvent)
{
  if( m_bWorking )
  {
    m_bWorking = false;
  }

//  _pcView3D->popMouseModel();
}

// -----------------------------------------------------------------------------------

SelectionMouseModel::SelectionMouseModel()
{
  m_bWorking = false;
}

SelectionMouseModel::~SelectionMouseModel()
{
}

void SelectionMouseModel::initialize()
{
}

void SelectionMouseModel::terminate()
{
}

void SelectionMouseModel::draw ()
{
  if (m_bWorking)
    _pcView3D->drawRect( m_iXold, m_iYold, m_iXnew, m_iYnew );
}

void SelectionMouseModel::mouseLeftPressEvent( QMouseEvent *cEvent)
{
  m_bWorking = true;
  QPoint p = cEvent->pos();
  m_iXold = m_iXnew = p.x(); 
  m_iYold = m_iYnew = p.y();
}

void SelectionMouseModel::mouseLeftReleaseEvent( QMouseEvent *cEvent)
{
  m_bWorking = false;
}

// -----------------------------------------------------------------------------------

/* XPM */
static const char *xpm_cursor[]={
"32 32 2 1",
". c None",
"# c #ffffff",
"................................",
"................................",
"................................",
"................................",
".............######.............",
"..........###......###..........",
".........#............#.........",
".......##..............##.......",
"......#..................#......",
".....#....................#.....",
".....#....................#.....",
"....#......................#....",
"....#......................#....",
"....#......................#....",
"...#........................#...",
"...#...........##...........#...",
"...#...........##...........#...",
"...#........................#...",
"....#......................#....",
"....#......................#....",
"....#......................#....",
".....#....................#.....",
".....#....................#.....",
"......#..................#......",
".......##..............##.......",
".........#............#.........",
"..........###......###..........",
".............######.............",
"................................",
"................................",
"................................",
"................................"};

CirclePickerMouseModel::CirclePickerMouseModel()
: _nRadius(50)
{
  QPoint p = QCursor::pos();
  m_iXnew = p.x(); 
  m_iYnew = p.y();
}

CirclePickerMouseModel::~CirclePickerMouseModel()
{
}

void CirclePickerMouseModel::initialize()
{
  QPixmap p(xpm_cursor);
  QCursor cursor( p );
  _pcView3D->getWidget()->setCursor(cursor);
}

void CirclePickerMouseModel::terminate()
{
  draw();
}

void CirclePickerMouseModel::draw ()
{
  char szBuf[20];
  float fRad = 0.0f;//float(getView()->Convert(Standard_Integer(_nRadius)));

  sprintf(szBuf, "%.2f", fRad);
  _pcView3D->drawCircle(m_iXnew, m_iYnew, _nRadius);
  _pcView3D->drawText(m_iXnew+9, m_iYnew-9, szBuf);
}

void CirclePickerMouseModel::mouseRightPressEvent( QMouseEvent *cEvent)
{
//  _pcView3D->popMouseModel();
}

void CirclePickerMouseModel::wheelEvent( QWheelEvent  * cEvent )
{
  int delta = cEvent->delta();
  _nRadius = 5>(_nRadius + delta / 10)?5:(_nRadius + delta / 10);
}
