
// Handling for precompiled headers

#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qevent.h>
#	include <assert.h>
#	include "qapplication.h"
#	include <qpainter.h>
#endif

#include "MouseModel.h"


#include "View3D.h"
 

void FCMouseModel::initMouseModel(View3D *pcView3D)
{
  _pcView3D=pcView3D;
  m_cPrevCursor = _pcView3D->cursor();

  // do initialization of your mousemodel
  initialize();
}

void FCMouseModel::releaseMouseModel()
{
  // do termination of your mousemodel
  terminate();

  _pcView3D->setCursor(m_cPrevCursor);
}

void FCMouseModel::moveMouseEvent (QMouseEvent *cEvent)
{
  // do all the drawing stuff for us
  QPoint clPoint = cEvent->pos();
  draw();
  m_iXnew = clPoint.x(); 
  m_iYnew = clPoint.y();
  mouseMoveEvent(cEvent);
  draw();
}

void FCMouseModel::wheelMouseEvent (QWheelEvent *cEvent)
{
  // do all the drawing stuff for us
  QPoint clPoint = cEvent->pos();
  draw();
  wheelEvent(cEvent);
  draw();
}

void FCMouseModel::mousePressEvent(QMouseEvent* cEvent)
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
  };
}

void FCMouseModel::mouseReleaseEvent(QMouseEvent* cEvent)
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
  };
}

View3D            &FCMouseModel::GetView3D(void) 
{ 
	// first init the MouseModel -> initMouseModel(View3D &View3D)
	assert (_pcView3D);
	return *_pcView3D;
}

Handle_V3d_View   &FCMouseModel::GetView  (void) 
{ 
	// first init the MouseModel -> initMouseModel(View3D &View3D)
	assert (_pcView3D);
	return _pcView3D->GetView();
}

Handle_V3d_Viewer &FCMouseModel::GetViewer(void) 
{ 
	// first init the MouseModel -> initMouseModel(View3D &View3D)
	assert (_pcView3D);
	return _pcView3D->GetViewer();
}

Handle_AIS_InteractiveContext &FCMouseModel::GetContext(void)
{
	// first init the MouseModel -> initMouseModel(View3D &View3D)
	assert (_pcView3D);
	return _pcView3D->GetContext();
}

void FCMouseModel::drawRect( int x, int y, int w, int h, QPainter* p )
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

void FCMouseModel::drawNode ( int x, int y, int w, int h, QPainter* p )
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

void FCMouseModel::drawLine ( int x1, int y1, int x2, int y2, QPainter* p )
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

void FCMouseModel::drawCircle ( int x, int y, int r, QPainter* p )
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

void FCMouseModel::drawText ( int x, int y, const QString & str, QPainter* p )
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

FCMouseModelStd::FCMouseModelStd(void)
	:mode(nothing)
{
};

void FCMouseModelStd::initialize()
{
}

void FCMouseModelStd::terminate()
{
}

// Buttons to use
#define OCC_ROTATION 		5 		// MidButton & LeftButton
#define OCC_PANNING  		4		// MidButton
#define OCC_ZOOM     		6		// MidButton & RightButton
#define OCC_SELECTION 		1		// LeftButton
//#define OCC_ADDSELECTION 	9		// LeftButton & ShiftButton
#define OCC_ADDSELECTION 	17		// LeftButton & CtrlButton
#define OCC_SHOWPOPUP       2		// RightButton

void FCMouseModelStd::mouseLeftPressEvent		 ( QMouseEvent *cEvent)
{
  mousePressEvent(cEvent);
}

void FCMouseModelStd::mouseMiddlePressEvent	 ( QMouseEvent *cEvent)
{
  mousePressEvent(cEvent);
}

void FCMouseModelStd::mouseRightPressEvent		 ( QMouseEvent *cEvent)
{
  mousePressEvent(cEvent);
}

void FCMouseModelStd::mouseLeftReleaseEvent	 ( QMouseEvent *cEvent)
{
  mouseReleaseEvent(cEvent);
}

void FCMouseModelStd::mouseMiddleReleaseEvent ( QMouseEvent *cEvent)
{
  mouseReleaseEvent(cEvent);
}

void FCMouseModelStd::mouseRightReleaseEvent	 ( QMouseEvent *cEvent)
{
  mouseReleaseEvent(cEvent);
}

void FCMouseModelStd::mousePressEvent( QMouseEvent *cEvent)
{
	iX = cEvent->x();
	iY = cEvent->y();
	//cout << "View3D: MousePress:"<< cEvent->state() +  cEvent->button()<<endl;
	switch(cEvent->state() + cEvent->button())
	{
		case OCC_ROTATION:
			GetView()->StartRotation(iX,iY);
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
			GetView3D().ShowPopup(iX,iY);
			break;
		default:
			mode = nothing;
	}
	//cout << "View3D: Mode:"<< (int) mode <<endl;
}

void FCMouseModelStd::mouseReleaseEvent( QMouseEvent *cEvent)
{
	//cout << "View3D: MouseRelease" <<endl;
	switch(mode)
	{
		case selection:
			GetContext()->MoveTo(cEvent->x(),cEvent->x(),GetView());
			GetContext()->Select();
			break;
		case addselection:
			GetContext()->MoveTo(cEvent->x(),cEvent->x(),GetView());
			GetContext()->ShiftSelect();
			break;
		default:
			;	
	}
	mode = nothing;
	
}

void FCMouseModelStd::mouseMoveEvent( QMouseEvent *cEvent)
{
#	ifndef WNT
		QApplication::flushX();
#	endif

	//cout << "View3D: MouseMove" <<endl;
	switch(mode)
	{
		case nothing:
			break;
		case rotation:
			GetView()->Rotation(cEvent->x(),cEvent->y());
			break;
		case panning:
		  //myView->Panning(cEvent->x()-iX,iY-cEvent->y()); // Dont work proper ?!?!
		  // get width size in Window system
		  V3d_Coordinate xc,yc;
		  GetView()->Center(xc,yc);	
		  Standard_Integer Xc,Yc;
		  GetView()->Convert(xc,yc,Xc,Yc);
		  // set the new center
		  GetView()->SetCenter(Xc - (cEvent->x()-iX),Yc - (cEvent->y()-iY));

			break;
		case zooming:
			GetView()->Zoom(iY,iX,cEvent->y(),cEvent->x());
			break;
		default:
			;
	}
	GetContext()->MoveTo(cEvent->x(),cEvent->y(),GetView());
	
	iX = cEvent->x();
	iY = cEvent->y();

}

void FCMouseModelStd::wheelEvent ( QWheelEvent * cEvent)
{
	int zDelta = cEvent->delta();
	Quantity_Length fWidth, fHeight;
	GetView()->Size(fWidth, fHeight);
	float fLog = float(log10(fWidth));
	int   nExp = int(fLog);

	// Zoom begrenzen
	if ((nExp > -6 && zDelta > 0) || (nExp < 8 && zDelta < 0))
	{
		GetView()->Zoom(0,0,zDelta,0);
    GetView3D().ShowDimension();
	}
	else if (zDelta > 0)
	{
		//GetConsole().Message("Cannot zoom in any more\n");
	}
	else
	{
		//GetConsole().Message("Cannot zoom out any more\n");
	}
}


void FCMouseModelStd::mouseDoubleClickEvent ( QMouseEvent * )
{


}

void FCMouseModelStd::keyPressEvent ( QKeyEvent * )
{


}

void FCMouseModelStd::keyReleaseEvent ( QKeyEvent * )
{


}

// **** MouseModelPolyPicker *************************************************

FCMouseModelPolyPicker::FCMouseModelPolyPicker() 
: FCMouseModelStd()
{
  m_iRadius    = 2;
  m_iNodes     = 0;
  m_bWorking   = false;
  m_bDrawNodes = true;
}

void FCMouseModelPolyPicker::initialize()
{
  _pcView3D->setCursor(QCursor(CrossCursor));
}

void FCMouseModelPolyPicker::terminate()
{
}

void FCMouseModelPolyPicker::draw ()
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

FCMouseModelPolyPicker::~FCMouseModelPolyPicker()
{
}

void FCMouseModelPolyPicker::mouseLeftPressEvent		 ( QMouseEvent *cEvent)
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

void FCMouseModelPolyPicker::mouseMiddlePressEvent	 ( QMouseEvent *cEvent)
{
}

void FCMouseModelPolyPicker::mouseRightPressEvent		 ( QMouseEvent *cEvent)
{
}

void FCMouseModelPolyPicker::wheelEvent ( QWheelEvent * e)
{
  // do nothing
}

void FCMouseModelPolyPicker::keyPressEvent ( QKeyEvent * e)
{
  switch (e->key())
  {
    case Qt::Key_Escape:
      _pcView3D->PopMouseModel();
      break;
    default:
      FCMouseModelStd::keyPressEvent(e);
      break;
  }
}

void FCMouseModelPolyPicker::mouseDoubleClickEvent	 ( QMouseEvent *cEvent)
{
  QPoint point = cEvent->pos();

  if( m_bWorking )
  {
    if (_cNodeVector.size() == 0) return; // no node
    draw();
    m_iXold = _cNodeVector[0].x();
    m_iYold = _cNodeVector[0].y();
    draw();
    m_bWorking = false;
  }

  _pcView3D->PopMouseModel();
}

// **** MouseModelSelection *************************************************

FCMouseModelSelection::FCMouseModelSelection()
: FCMouseModelStd()
{
  m_bWorking = false;
}

FCMouseModelSelection::~FCMouseModelSelection()
{
}

void FCMouseModelSelection::initialize()
{
}

void FCMouseModelSelection::terminate()
{
}

void FCMouseModelSelection::draw ()
{
  if (m_bWorking)
    drawRect( m_iXold, m_iYold, m_iXnew, m_iYnew );
}

void FCMouseModelSelection::mouseLeftPressEvent		 ( QMouseEvent *cEvent)
{
  m_bWorking = true;
  QPoint p = cEvent->pos();
  m_iXold = m_iXnew = p.x(); 
  m_iYold = m_iYnew = p.y();
}

void FCMouseModelSelection::mouseLeftReleaseEvent	 ( QMouseEvent *cEvent)
{
  m_bWorking = false;
}

// **** MouseModelSelection *************************************************
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

FCMouseModelCirclePicker::FCMouseModelCirclePicker()
: FCMouseModelStd(), _nRadius(50)
{
  QPoint p = QCursor::pos();
  m_iXnew = p.x(); 
  m_iYnew = p.y();
}

FCMouseModelCirclePicker::~FCMouseModelCirclePicker()
{
}

void FCMouseModelCirclePicker::initialize()
{
  QPixmap p(xpm_cursor);
  QCursor cursor( p );
  _pcView3D->setCursor(cursor);
}

void FCMouseModelCirclePicker::terminate()
{
  draw();
}

void FCMouseModelCirclePicker::draw ()
{
  char szBuf[20];
  float fRad = float(_pcView3D->GetView()->Convert(Standard_Integer(_nRadius)));

  sprintf(szBuf, "%.2f", fRad);
  drawCircle(m_iXnew, m_iYnew, _nRadius);
  drawText(m_iXnew+9, m_iYnew-9, szBuf);
}

void FCMouseModelCirclePicker::mouseRightPressEvent		 ( QMouseEvent *cEvent)
{
  _pcView3D->PopMouseModel();
}

void FCMouseModelCirclePicker::wheelEvent			    ( QWheelEvent  * cEvent )
{
  int delta = cEvent->delta();
  _nRadius = std::max<int>(5, _nRadius + delta / 10);
}
