/***************************************************************************
 *   Copyright (c) 2004 J�rgen Riegel <juergen.riegel@web.de>              *
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
# include <assert.h>
# include <stack>
# include <vector>
# include <qapplication.h>
# include <qcursor.h>
# include <qevent.h>
# include <qpainter.h>
# include <qpixmap.h>
# include <qvbox.h>
#endif

#include "MouseModel.h"
#include "../Base/Console.h"


#include "View3D.h"

using namespace Gui; 

void MouseModel::initMouseModel(View3D *pcView3D)
{
  _pcView3D=pcView3D;
  m_cPrevCursor = _pcView3D->cursor();

  // do initialization of your mousemodel
  initialize();
}

void MouseModel::releaseMouseModel()
{
  // do termination of your mousemodel
  terminate();

  _pcView3D->setCursor(m_cPrevCursor);
}

void MouseModel::moveMouseEvent (QMouseEvent *cEvent)
{
  // do all the drawing stuff for us
  QPoint clPoint = cEvent->pos();
  draw();
  m_iXnew = clPoint.x(); 
  m_iYnew = clPoint.y();
  mouseMoveEvent(cEvent);
  draw();
}

void MouseModel::wheelMouseEvent (QWheelEvent *cEvent)
{
  // do all the drawing stuff for us
  //QPoint clPoint = cEvent->pos();
  draw();
  wheelEvent(cEvent);
  draw();
}

void MouseModel::mousePressEvent(QMouseEvent* cEvent)
{
  switch (cEvent->button())
  {
    case Qt::LeftButton:
      mouseLeftPressEvent(cEvent);
      draw();
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

void MouseModel::mouseReleaseEvent(QMouseEvent* cEvent)
{
  switch (cEvent->button())
  {
    case Qt::LeftButton:
      draw();
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

View3D& MouseModel::getView3D(void) 
{ 
  // first init the MouseModel -> initMouseModel(View3D &View3D)
  assert (_pcView3D);
  return *_pcView3D;
}

Handle_V3d_View& MouseModel::getView  (void) 
{ 
  // first init the MouseModel -> initMouseModel(View3D &View3D)
  assert (_pcView3D);
  return _pcView3D->getView();
}

Handle_V3d_Viewer& MouseModel::getViewer(void) 
{
  // first init the MouseModel -> initMouseModel(View3D &View3D)
  assert (_pcView3D);
  return _pcView3D->getViewer();
}

Handle_AIS_InteractiveContext& MouseModel::getContext(void)
{
  // first init the MouseModel -> initMouseModel(View3D &View3D)
  assert (_pcView3D);
  return _pcView3D->getContext();
}

void MouseModel::drawRect( int x, int y, int w, int h, QPainter* p )
{
  if (p)
    p->drawRect( QRect( QPoint( QMIN( x, w ), QMIN( y, h ) ),
         QPoint( QMAX( x, w ), QMAX( y, h ) ) ) );
  else
  {
    QPainter p(_pcView3D);
    p.setPen( Qt::white );
    p.setRasterOp( QPainter::XorROP );
    drawRect( x, y, w, h, &p );
  }
}

void MouseModel::drawNode ( int x, int y, int w, int h, QPainter* p )
{
  if (p)
    p->drawEllipse( x, y, w, h );
  else
  {
    QPainter p(_pcView3D);
    p.setPen( Qt::white );
    p.setBrush(QBrush::white);
    p.setRasterOp( QPainter::XorROP );
    drawNode( x, y, w, h, &p );
  }
}

void MouseModel::drawLine ( int x1, int y1, int x2, int y2, QPainter* p )
{
  if (p)
    p->drawLine( x1, y1, x2, y2 );
  else
  {
    QPainter p(_pcView3D);
    p.setPen( Qt::white );
    p.setRasterOp( QPainter::XorROP );
    drawLine( x1, y1, x2, y2, &p );
  }
}

void MouseModel::drawCircle ( int x, int y, int r, QPainter* p )
{
  if (p)
  {
    QPoint center(x-r/2, y-r/2);
    p->drawEllipse( center.x(), center.y(), r, r );
  }
  else
  {
    QPainter p(_pcView3D);
    p.setPen( Qt::green );
    p.setRasterOp( QPainter::XorROP );
    drawCircle( x, y, r, &p );
  }
}

void MouseModel::drawText ( int x, int y, const QString & str, QPainter* p )
{
  if (p)
    p->drawText( x, y, str);
  else
  {
    QPainter p(_pcView3D);
    p.setPen( Qt::white );
    p.setRasterOp( QPainter::XorROP );
    drawText( x, y, str, &p );
  }
}

// **** MouseModelStd ********************************************************

MouseModelStd::MouseModelStd(void)
  :mode(nothing)
{
};

void MouseModelStd::initialize()
{
}

void MouseModelStd::terminate()
{
}

// Buttons to use
#define OCC_ROTATION    5   // MidButton & LeftButton
#define OCC_PANNING     4   // MidButton
#define OCC_ZOOM        6   // MidButton & RightButton
#define OCC_SELECTION   1   // LeftButton
//#define OCC_ADDSELECTION  9 // LeftButton & ShiftButton
#define OCC_ADDSELECTION  17  // LeftButton & CtrlButton
#define OCC_SHOWPOPUP       2 // RightButton

void MouseModelStd::mouseLeftPressEvent ( QMouseEvent *cEvent)
{
  mousePressEvent(cEvent);
}

void MouseModelStd::mouseMiddlePressEvent ( QMouseEvent *cEvent)
{
  mousePressEvent(cEvent);
}

void MouseModelStd::mouseRightPressEvent ( QMouseEvent *cEvent)
{
  mousePressEvent(cEvent);
}

void MouseModelStd::mouseLeftReleaseEvent ( QMouseEvent *cEvent)
{
  mouseReleaseEvent(cEvent);
}

void MouseModelStd::mouseMiddleReleaseEvent ( QMouseEvent *cEvent)
{
  mouseReleaseEvent(cEvent);
}

void MouseModelStd::mouseRightReleaseEvent ( QMouseEvent *cEvent)
{
  mouseReleaseEvent(cEvent);
}

void MouseModelStd::mousePressEvent( QMouseEvent *cEvent)
{
  iX = cEvent->x();
  iY = cEvent->y();
  //cout << "View3D: MousePress:"<< cEvent->state() +  cEvent->button()<<endl;
  switch(cEvent->state() + cEvent->button())
  {
    case OCC_ROTATION:
      getView()->StartRotation(iX,iY);
      mode = rotation;
      break;
    case OCC_PANNING:
      mode = panning;
      break;
    case OCC_ZOOM:
      mode = zooming;
      break;
    case OCC_SELECTION:
      mode = selection;
      break;
    case OCC_ADDSELECTION:
      mode = addselection;
      break;
    case OCC_SHOWPOPUP:
      getView3D().showPopup(iX,iY);
      break;
    default:
      mode = nothing;
  }
  //cout << "View3D: Mode:"<< (int) mode <<endl;
}

void MouseModelStd::mouseReleaseEvent( QMouseEvent *cEvent)
{
  //cout << "View3D: MouseRelease" <<endl;
  switch(mode)
  {
    case selection:
      getContext()->MoveTo(cEvent->x(),cEvent->x(),getView());
      getContext()->Select();
      break;
    case addselection:
      getContext()->MoveTo(cEvent->x(),cEvent->x(),getView());
      getContext()->ShiftSelect();
      break;
    default:
  ;
  }
  mode = nothing;
}

void MouseModelStd::mouseMoveEvent( QMouseEvent *cEvent)
{
#ifndef FC_OS_WIN32
  QApplication::flushX();
#endif

  //cout << "View3D: MouseMove" <<endl;
  switch(mode)
  {
    case nothing:
      break;
    case rotation:
      getView()->Rotation(cEvent->x(),cEvent->y());
      break;
    case panning:
      getView()->Pan(cEvent->x()-iX,iY-cEvent->y()); // Use "Pan(...)" instead of "Panning(...)" !!!
      break;
    case zooming:
      getView()->Zoom(iY,iX,cEvent->y(),cEvent->x());
      break;
    default:
      break;
  }
  getContext()->MoveTo(cEvent->x(),cEvent->y(),getView());

  iX = cEvent->x();
  iY = cEvent->y();
}

void MouseModelStd::wheelEvent ( QWheelEvent * cEvent)
{
  int zDelta = cEvent->delta()/3;
  //Console().Log("Wheel Delta=%d\n",zDelta);
  Quantity_Length fWidth, fHeight;
  getView()->Size(fWidth, fHeight);
  float fLog = float(log10(fWidth));
  int   nExp = int(fLog);

  // Zoom begrenzen
  if ((nExp > -6 && zDelta > 0) || (nExp < 8 && zDelta < 0))
  {
    getView()->Zoom(0,0,zDelta,0);
    getView3D().showDimension();
  }
  else if (zDelta > 0)
  {
    Base::Console().Message(QObject::tr("Cannot zoom in any more\n").latin1());
  }
  else
  {
    Base::Console().Message(QObject::tr("Cannot zoom out any more\n").latin1());
  }
}

void MouseModelStd::mouseDoubleClickEvent ( QMouseEvent * )
{

}

void MouseModelStd::keyPressEvent ( QKeyEvent * )
{

}

void MouseModelStd::keyReleaseEvent ( QKeyEvent * )
{

}

// -----------------------------------------------------------------------------------

MouseModelPolyPicker::MouseModelPolyPicker() 
: MouseModelStd()
{
  m_iRadius    = 2;
  m_iNodes     = 0;
  m_bWorking   = false;
  m_bDrawNodes = true;
}

void MouseModelPolyPicker::initialize()
{
  _pcView3D->setCursor(QCursor(QCursor::CrossCursor));
}

void MouseModelPolyPicker::terminate()
{
}

void MouseModelPolyPicker::draw ()
{
  if ( m_bWorking )
  {
    if (m_iNodes < int(_cNodeVector.size()))
    {
      m_iNodes = int(_cNodeVector.size());
      // drawing the point
      if (m_bDrawNodes == true)
      {
        drawNode(m_iXnew-m_iRadius,m_iYnew-m_iRadius,2*m_iRadius,2*m_iRadius);
      }
    }
    else
    {
      drawLine(m_iXnew,m_iYnew,m_iXold,m_iYold );
    }
  }
}

MouseModelPolyPicker::~MouseModelPolyPicker()
{
}

void MouseModelPolyPicker::mouseLeftPressEvent		 ( QMouseEvent *cEvent)
{
  QPoint point = cEvent->pos();

  // start working from now on
  if ( !m_bWorking )    
  {
    m_bWorking = true;
    // clear the old polygon
    _cNodeVector.clear();
    _pcView3D->update();
  }

  _cNodeVector.push_back(point);

  m_iXnew = point.x();  m_iYnew = point.y();
  m_iXold = point.x();  m_iYold = point.y();
}

void MouseModelPolyPicker::mouseMiddlePressEvent	 ( QMouseEvent *cEvent)
{
}

void MouseModelPolyPicker::mouseRightPressEvent		 ( QMouseEvent *cEvent)
{
}

void MouseModelPolyPicker::wheelEvent ( QWheelEvent * e)
{
  // do nothing
}

void MouseModelPolyPicker::keyPressEvent ( QKeyEvent * e)
{
  switch (e->key())
  {
    case Qt::Key_Escape:
      _pcView3D->popMouseModel();
      break;
    default:
      MouseModelStd::keyPressEvent(e);
      break;
  }
}

void MouseModelPolyPicker::mouseDoubleClickEvent	 ( QMouseEvent *cEvent)
{
  //QPoint point = cEvent->pos();

  if( m_bWorking )
  {
    if (_cNodeVector.size() == 0) return; // no node
    draw();
    m_iXold = _cNodeVector[0].x();
    m_iYold = _cNodeVector[0].y();
    draw();
    m_bWorking = false;
  }

  _pcView3D->popMouseModel();
}

// -----------------------------------------------------------------------------------

MouseModelSelection::MouseModelSelection()
: MouseModelStd()
{
  m_bWorking = false;
}

MouseModelSelection::~MouseModelSelection()
{
}

void MouseModelSelection::initialize()
{
}

void MouseModelSelection::terminate()
{
}

void MouseModelSelection::draw ()
{
  if (m_bWorking)
    drawRect( m_iXold, m_iYold, m_iXnew, m_iYnew );
}

void MouseModelSelection::mouseLeftPressEvent		 ( QMouseEvent *cEvent)
{
  m_bWorking = true;
  QPoint p = cEvent->pos();
  m_iXold = m_iXnew = p.x(); 
  m_iYold = m_iYnew = p.y();
}

void MouseModelSelection::mouseLeftReleaseEvent	 ( QMouseEvent *cEvent)
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

MouseModelCirclePicker::MouseModelCirclePicker()
: MouseModelStd(), _nRadius(50)
{
  QPoint p = QCursor::pos();
  m_iXnew = p.x(); 
  m_iYnew = p.y();
}

MouseModelCirclePicker::~MouseModelCirclePicker()
{
}

void MouseModelCirclePicker::initialize()
{
  QPixmap p(xpm_cursor);
  QCursor cursor( p );
  _pcView3D->setCursor(cursor);
}

void MouseModelCirclePicker::terminate()
{
  draw();
}

void MouseModelCirclePicker::draw ()
{
  char szBuf[20];
  float fRad = float(_pcView3D->getView()->Convert(Standard_Integer(_nRadius)));

  sprintf(szBuf, "%.2f", fRad);
  drawCircle(m_iXnew, m_iYnew, _nRadius);
  drawText(m_iXnew+9, m_iYnew-9, szBuf);
}

void MouseModelCirclePicker::mouseRightPressEvent		 ( QMouseEvent *cEvent)
{
  _pcView3D->popMouseModel();
}


void MouseModelCirclePicker::wheelEvent			    ( QWheelEvent  * cEvent )
{
  int delta = cEvent->delta();
  _nRadius = 5>(_nRadius + delta / 10)?5:(_nRadius + delta / 10);
}
