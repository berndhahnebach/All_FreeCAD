#ifndef __MOUSEMODEL_H__
#define __MOUSEMODEL_H__

#include "window.h"

// forwards
class QMouseEvent;
class QKeyEvent;
class Handle_V3d_View;
class Handle_V3d_Viewer;
class Handle_AIS_InteractiveContext;
class View3D;


class FCMouseModel
{
public:
	FCMouseModel(void){};
	virtual void initMouseModel(View3D *pcView3D);
	virtual void releaseMouseModel(void);

	virtual void mousePressEvent		( QMouseEvent *cEvent) = 0;
	virtual void mouseReleaseEvent		( QMouseEvent *cEvent) = 0;
	virtual void mouseMoveEvent			( QMouseEvent *cEvent){};
	virtual void mouseDoubleClickEvent	( QMouseEvent * ){};

	virtual void wheelEvent				( QWheelEvent * ){};

	virtual void keyPressEvent			( QKeyEvent * ){};
	virtual void keyReleaseEvent		( QKeyEvent * ){}; 

	virtual void paintEvent				( QPaintEvent * ){} ;
	virtual void resizeEvent			( QResizeEvent * ) {};
	
	// Geter
	View3D							&GetView3D (void);
	Handle_V3d_View					&GetView   (void);
	Handle_V3d_Viewer				&GetViewer (void);
	Handle_AIS_InteractiveContext	&GetContext(void);


protected:
	View3D *_pcView3D;
  QCursor m_cPrevCursor;
};




class FCMouseModelStd :public FCMouseModel
{
public:
	FCMouseModelStd(void);

	virtual void mousePressEvent	( QMouseEvent *cEvent);
	virtual void mouseReleaseEvent	( QMouseEvent *cEvent);
	virtual void mouseMoveEvent		( QMouseEvent *cEvent);
	virtual void mouseDoubleClickEvent ( QMouseEvent * );
	virtual void keyPressEvent		( QKeyEvent * );
	virtual void keyReleaseEvent	( QKeyEvent * );
	virtual void wheelEvent			( QWheelEvent * );
	
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

class FCMouseModelPolyPicker : public FCMouseModelStd
{
  public:
    FCMouseModelPolyPicker();
    virtual ~FCMouseModelPolyPicker();

    virtual void initMouseModel(View3D *pcView3D);
    virtual void mousePressEvent	  ( QMouseEvent  * cEvent );
    virtual void mouseReleaseEvent	( QMouseEvent  * cEvent );
    virtual void wheelEvent			    ( QWheelEvent  * cEvent );
    virtual void mouseMoveEvent		  ( QMouseEvent  * cEvent );
    virtual void keyPressEvent		  ( QKeyEvent    * cEvent );
    virtual void paintEvent         ( QPaintEvent  * cEvent );
    virtual void resizeEvent			  ( QResizeEvent * cEvent );

  protected:
    FCvector<QPoint> _cNodeVector;
    int  m_iRadius;
    bool m_bWorking, m_bDrawNodes;
    int  m_iXold, m_iYold;
    int  m_iXmin, 
         m_iXmax, 
         m_iYmin, 
         m_iYmax; //extenst of the input vertices
};

class FCMouseModelSelection : public FCMouseModelStd 
{
  public:
    FCMouseModelSelection();
    virtual ~FCMouseModelSelection();

  protected:
    virtual void initMouseModel(View3D *pcView3D);
    virtual void mousePressEvent	  ( QMouseEvent  * cEvent );
    virtual void mouseReleaseEvent	( QMouseEvent  * cEvent );
    virtual void wheelEvent			    ( QWheelEvent  * cEvent );
    virtual void mouseMoveEvent		  ( QMouseEvent  * cEvent );
    virtual void paintEvent         ( QPaintEvent  * cEvent );
    virtual void resizeEvent			  ( QResizeEvent * cEvent );
  private:
    void drawRect( QPainter* p, const QPoint& p1, const QPoint& p2 );
    QPoint _start, _last;
    bool m_bWorking;
};

#endif