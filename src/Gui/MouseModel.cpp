
// Handling for precompiled headers
#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <qevent.h>
#	include <assert.h>
#	include "qapplication.h"
#endif

#include "MouseModel.h"
#include "View3D.h"
 

void FCMouseModel::initMouseModel(View3D *pcView3D)
{
  _pcView3D=pcView3D;
  m_cPrevCursor = _pcView3D->cursor();

  // do initialization of your mousemodel
  initMouseModel();
}

void FCMouseModel::releaseMouseModel()
{
  _pcView3D->setCursor(m_cPrevCursor);
}

void FCMouseModel::mousePressEvent(QMouseEvent* cEvent)
{
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
  };
}

void FCMouseModel::mouseReleaseEvent(QMouseEvent* cEvent)
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

void FCMouseModel::drawRect( const QPoint& p1, const QPoint& p2, QPainter* p )
{
  if (p)
    p->drawRect( QRect( QPoint( QMIN( p1.x(), p2.x() ), QMIN( p1.y(), p2.y() ) ),
         QPoint( QMAX( p1.x(), p2.x() ), QMAX( p1.y(), p2.y() ) ) ) );
  else
  {
    QPainter p(_pcView3D);
    p.setPen( Qt::white );
    p.setRasterOp( QPainter::XorROP );
    drawRect( p1, p2, &p );
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

void FCMouseModelStd::initMouseModel()
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
  m_bWorking   = false;
  m_bDrawNodes = true;
}

void FCMouseModelPolyPicker::initMouseModel()
{
  _pcView3D->setCursor(QCursor(CrossCursor));
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

  // drawing the point
  if (m_bDrawNodes == true)
  {
    drawNode(point.x()-m_iRadius,point.y()-m_iRadius,2*m_iRadius,2*m_iRadius);
  }

  _cNodeVector.push_back(point);

  m_iXmin = point.x();  m_iYmin = point.y();
  m_iXmax = point.x();  m_iYmax = point.y();
  m_iXold = point.x();  m_iYold = point.y();
}

void FCMouseModelPolyPicker::mouseMiddlePressEvent	 ( QMouseEvent *cEvent)
{
}

void FCMouseModelPolyPicker::mouseRightPressEvent		 ( QMouseEvent *cEvent)
{
  QPoint point = cEvent->pos();

  if( m_bWorking )
  {
    if (_cNodeVector.size() == 0) return; // no node
    unsigned long ulLast = _cNodeVector.size() - 1;
    m_bWorking = false;
    // clear old line first
    drawLine(m_iXmax,m_iYmax,m_iXold,m_iYold );
    // draw new line
    drawLine(_cNodeVector[ulLast].x(), _cNodeVector[ulLast].y(), _cNodeVector[0].x(), _cNodeVector[0].y());
  }
  else // right click twice
    _pcView3D->PopMouseModel();
}

void FCMouseModelPolyPicker::mouseLeftReleaseEvent	 ( QMouseEvent *cEvent)
{
}

void FCMouseModelPolyPicker::mouseMiddleReleaseEvent ( QMouseEvent *cEvent)
{
}

void FCMouseModelPolyPicker::mouseRightReleaseEvent	 ( QMouseEvent *cEvent)
{
}

void FCMouseModelPolyPicker::mouseMoveEvent		( QMouseEvent *cEvent)
{
  QPoint point = cEvent->pos();

  if ( m_bWorking )
  {
    // clear old line first
    drawLine(m_iXmax,m_iYmax,m_iXold,m_iYold );
    // draw new line
    drawLine(m_iXmax,m_iYmax,point.x(),point.y() );

    m_iXold = point.x();
    m_iYold = point.y();
  }
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

void FCMouseModelPolyPicker::paintEvent ( QPaintEvent *e )
{
  if (_cNodeVector.size() == 0) return; // no node
  unsigned long ulLast = _cNodeVector.size() - 1;

  for (int i=1;i<=int(ulLast);i++)
  {
    drawLine(_cNodeVector[i-1].x(), _cNodeVector[i-1].y(), _cNodeVector[i].x(), _cNodeVector[i].y());
    if (m_bDrawNodes == true)
    {
      drawNode(_cNodeVector[i-1].x()-m_iRadius,_cNodeVector[i-1].y()-m_iRadius,2*m_iRadius,2*m_iRadius);
    }
  }

  // at least three points to draw line from last to first point
  if (_cNodeVector.size() > 2 && m_bWorking == false)
  {
    drawLine(_cNodeVector[ulLast].x(), _cNodeVector[ulLast].y(), _cNodeVector[0].x(), _cNodeVector[0].y());
  }
  // draw end point
  if (m_bDrawNodes == true && _cNodeVector.size() > 0)
  {
    drawNode(_cNodeVector[ulLast].x()-m_iRadius,_cNodeVector[ulLast].y()-m_iRadius,2*m_iRadius,2*m_iRadius);
  }
}

void FCMouseModelPolyPicker::resizeEvent ( QResizeEvent * e)
{
  // the polygon becomes invalid and must be done again
//  _cNodeVector.clear();
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

void FCMouseModelSelection::initMouseModel()
{
}

void FCMouseModelSelection::wheelEvent( QWheelEvent  * cEvent )
{
}

void FCMouseModelSelection::paintEvent ( QPaintEvent  * cEvent )
{
}

void FCMouseModelSelection::resizeEvent ( QResizeEvent * cEvent )
{
}

void FCMouseModelSelection::mouseLeftPressEvent		 ( QMouseEvent *cEvent)
{
  m_bWorking = true;
  _start = _last = cEvent->pos();
}

void FCMouseModelSelection::mouseMiddlePressEvent	 ( QMouseEvent *cEvent)
{
}

void FCMouseModelSelection::mouseRightPressEvent		 ( QMouseEvent *cEvent)
{
}

void FCMouseModelSelection::mouseLeftReleaseEvent	 ( QMouseEvent *cEvent)
{
  drawRect( _start, _last );
  m_bWorking = false;
}

void FCMouseModelSelection::mouseMiddleReleaseEvent ( QMouseEvent *cEvent)
{
}

void FCMouseModelSelection::mouseRightReleaseEvent	 ( QMouseEvent *cEvent)
{
}

void FCMouseModelSelection::mouseMoveEvent( QMouseEvent* e )
{
  if (m_bWorking)
  {
    if( _start != _last ) drawRect( _start, _last );
    _last = e->pos();
    drawRect( _start, _last );
  }
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

FCMouseModelCirclePick::FCMouseModelCirclePick()
: FCMouseModelStd(), _nRadius(50)
{
  _last = QCursor::pos();
}

FCMouseModelCirclePick::~FCMouseModelCirclePick()
{
}

void FCMouseModelCirclePick::initMouseModel()
{
  QPixmap p(xpm_cursor);
  QCursor cursor( p );
  _pcView3D->setCursor(cursor);
}

void FCMouseModelCirclePick::releaseMouseModel()
{
  draw(_last);
  FCMouseModel::releaseMouseModel();
}

void FCMouseModelCirclePick::mouseLeftPressEvent		 ( QMouseEvent *cEvent)
{
}

void FCMouseModelCirclePick::mouseMiddlePressEvent	 ( QMouseEvent *cEvent)
{
}

void FCMouseModelCirclePick::mouseRightPressEvent		 ( QMouseEvent *cEvent)
{
  _pcView3D->PopMouseModel();
}

void FCMouseModelCirclePick::mouseLeftReleaseEvent	 ( QMouseEvent *cEvent)
{
}

void FCMouseModelCirclePick::mouseMiddleReleaseEvent ( QMouseEvent *cEvent)
{
}

void FCMouseModelCirclePick::mouseRightReleaseEvent	 ( QMouseEvent *cEvent)
{
}

void FCMouseModelCirclePick::wheelEvent			    ( QWheelEvent  * cEvent )
{
  int delta = cEvent->delta();
  draw(_last);
  _nRadius = FCmax<int>(5, _nRadius + delta / 10);
  draw(_last);
}

void FCMouseModelCirclePick::mouseMoveEvent		  ( QMouseEvent  * cEvent )
{
  QPoint clPoint = cEvent->pos();
  draw(_last);
  draw(clPoint);
  _last = clPoint;
}

void FCMouseModelCirclePick::paintEvent         ( QPaintEvent  * cEvent )
{
  draw(_last);
}

void FCMouseModelCirclePick::resizeEvent			  ( QResizeEvent * cEvent )
{
  draw(_last);
}

void FCMouseModelCirclePick::draw(const QPoint& point)
{
  char szBuf[20];
  float fRad = float(_pcView3D->GetView()->Convert(Standard_Integer(_nRadius)));

  sprintf(szBuf, "%.2f", fRad);
  drawCircle(point.x(), point.y(), _nRadius);
  drawText(point.x()+9, point.y()-9, szBuf);
}
