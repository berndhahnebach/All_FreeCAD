
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
}

void FCMouseModel::releaseMouseModel()
{
  _pcView3D->setCursor(m_cPrevCursor);
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


// **** MouseModelStd ********************************************************

FCMouseModelStd::FCMouseModelStd(void)
	:mode(nothing)
{
};

// Buttons to use
#define OCC_ROTATION 		5 		// MidButton & RightButton
#define OCC_PANNING  		4		// MidButton
#define OCC_ZOOM     		6		// MidButton & LeftButton
#define OCC_SELECTION 		1		// LeftButton
//#define OCC_ADDSELECTION 	9		// LeftButton & ShiftButton
#define OCC_ADDSELECTION 	17		// LeftButton & CtrlButton
#define OCC_SHOWPOPUP       2		// LeftButton

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

void FCMouseModelPolyPicker::initMouseModel(View3D *pcView3D)
{
  // base class initialization
  FCMouseModelStd::initMouseModel(pcView3D);
  _pcView3D->setCursor(QCursor(CrossCursor));
  color = QColor(255, 255, 255);
}

FCMouseModelPolyPicker::~FCMouseModelPolyPicker()
{
}

void FCMouseModelPolyPicker::mousePressEvent	( QMouseEvent *cEvent)
{
  QPoint point = cEvent->pos();

  // left button
  if (cEvent->button() == Qt::LeftButton)
  {
    // start working from now on
    if ( !m_bWorking )    
    {
      m_bWorking = true;
      // clear the old polygon
      _pcView3D->update();

      m_iXmin=point.x();  m_iYmin=point.y();
      m_iXmax=point.x();  m_iYmax=point.y();

      _cNodeVector.clear();
      _cNodeVector.push_back(point);

      // drawing the first point
      if (m_bDrawNodes == true)
      {
        QPainter p(_pcView3D);
        p.setPen(color);
      	p.setBrush(QBrush::white);
        p.drawEllipse(point.x()-m_iRadius,point.y()-m_iRadius,2*m_iRadius,2*m_iRadius);
      }
    }
    // drawing a new line
    else
    {
      QPainter p(_pcView3D);
      p.setPen(color);
      p.drawLine(m_iXmax, m_iYmax, point.x(), point.y());
      if (m_bDrawNodes == true)
      {
      	p.setBrush(QBrush::white);
        p.drawEllipse(point.x()-m_iRadius,point.y()-m_iRadius,2*m_iRadius,2*m_iRadius);
      }

      _cNodeVector.push_back(point);

      m_iXmin = point.x();  m_iYmin = point.y();
      m_iXmax = point.x();  m_iYmax = point.y();
      m_iXold = point.x();  m_iYold = point.y();
    }
  }
  // right button
  else if (cEvent->button() == Qt::RightButton)
  {
    if( m_bWorking )
    {
      m_bWorking = false;
  
      // close the polygon
      if(_cNodeVector.size() > 2)
      {
        QPainter p(_pcView3D);

        // clear old line first
        p.drawLine(m_iXmax,m_iYmax,m_iXold,m_iYold );
        
        p.setPen(color);
        p.drawLine(m_iXmax,m_iYmax,_cNodeVector[0].x(),_cNodeVector[0].y());
      }
    }
    else // right click twice
        _pcView3D->PopMouseModel();
  }
  // middle button
  else if (cEvent->button() == Qt::MidButton)
  {
    // do nothing
  }
}

void FCMouseModelPolyPicker::mouseReleaseEvent	( QMouseEvent *cEvent)
{
}

void FCMouseModelPolyPicker::mouseMoveEvent		( QMouseEvent *cEvent)
{
  QPoint point = cEvent->pos();

  if ( m_bWorking )
  {
    QPainter p(_pcView3D);

    // clear old line first
    p.drawLine(m_iXmax,m_iYmax,m_iXold,m_iYold );

    // draw new line
    p.setPen(color);
    p.drawLine(m_iXmax,m_iYmax,point.x(),point.y() );

    m_iXold = point.x();
    m_iYold = point.y();
  }

  // do this in a new if branch because of deletion of QPainter object above
  if ( m_bWorking )
    repaint();
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
  repaint();
}

void FCMouseModelPolyPicker::resizeEvent ( QResizeEvent * e)
{
  // the polygon becomes invalid and must be done again
  if ( m_bWorking )
  {
    _cNodeVector.clear();
  }
}

void FCMouseModelPolyPicker::repaint()
{
  QPainter p(_pcView3D);
  p.setPen(color);
  unsigned long ulLast = _cNodeVector.size() - 1;

  for (int i=1;i<=int(ulLast);i++)
  {
    p.drawLine(_cNodeVector[i-1].x(), _cNodeVector[i-1].y(), _cNodeVector[i].x(), _cNodeVector[i].y());
    if (m_bDrawNodes == true)
    {
      p.setBrush(QBrush::white);
      p.drawEllipse(_cNodeVector[i-1].x()-m_iRadius,_cNodeVector[i-1].y()-m_iRadius,2*m_iRadius,2*m_iRadius);
    }
  }

  // at least three points to draw line from last to first point
  if (_cNodeVector.size() > 2 && m_bWorking == false)
  {
    p.drawLine(_cNodeVector[ulLast].x(), _cNodeVector[ulLast].y(), _cNodeVector[0].x(), _cNodeVector[0].y());
  }
  // draw end point
  if (m_bDrawNodes == true && _cNodeVector.size() > 0)
  {
    p.setBrush(QBrush::white);
    p.drawEllipse(_cNodeVector[ulLast].x()-m_iRadius,_cNodeVector[ulLast].y()-m_iRadius,2*m_iRadius,2*m_iRadius);
  }
}