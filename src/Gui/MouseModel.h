#ifndef __MOUSEMODEL_H__
#define __MOUSEMODEL_H__

#include "Window.h"

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
	virtual void initialize() = 0;
	virtual void terminate () = 0;
	void initMouseModel(View3D *pcView3D);
	void releaseMouseModel(void);

	void mousePressEvent		( QMouseEvent *cEvent);
	void mouseReleaseEvent	( QMouseEvent *cEvent);
  virtual void mouseLeftPressEvent		 ( QMouseEvent *cEvent){};
  virtual void mouseMiddlePressEvent	 ( QMouseEvent *cEvent){};
  virtual void mouseRightPressEvent		 ( QMouseEvent *cEvent){};
  virtual void mouseLeftReleaseEvent	 ( QMouseEvent *cEvent){};
  virtual void mouseMiddleReleaseEvent ( QMouseEvent *cEvent){};
  virtual void mouseRightReleaseEvent	 ( QMouseEvent *cEvent){};
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
  void drawRect ( const QPoint& p1, const QPoint& p2, QPainter* p = NULL );
  void drawNode ( int x, int y, int w, int h, QPainter* p = NULL );
  void drawLine ( int x1, int y1, int x2, int y2, QPainter* p = NULL );
  void drawCircle ( int x, int y, int r, QPainter* p = NULL );
  void drawText ( int x, int y, const QString & str, QPainter* p = NULL );

protected:
	View3D *_pcView3D;
  QCursor m_cPrevCursor;
};




class FCMouseModelStd :public FCMouseModel
{
public:
	FCMouseModelStd(void);

	virtual void initialize();
	virtual void terminate();
	virtual void mousePressEvent	( QMouseEvent *cEvent);
	virtual void mouseReleaseEvent	( QMouseEvent *cEvent);
	virtual void mouseLeftPressEvent		 ( QMouseEvent *cEvent);
	virtual void mouseMiddlePressEvent	 ( QMouseEvent *cEvent);
	virtual void mouseRightPressEvent		 ( QMouseEvent *cEvent);
	virtual void mouseLeftReleaseEvent	 ( QMouseEvent *cEvent);
	virtual void mouseMiddleReleaseEvent ( QMouseEvent *cEvent);
	virtual void mouseRightReleaseEvent	 ( QMouseEvent *cEvent);
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

    virtual void initialize();
  	virtual void terminate();
	  virtual void mouseLeftPressEvent		 ( QMouseEvent *cEvent);
	  virtual void mouseMiddlePressEvent	 ( QMouseEvent *cEvent);
	  virtual void mouseRightPressEvent		 ( QMouseEvent *cEvent);
  	virtual void mouseDoubleClickEvent	 ( QMouseEvent *cEvent);
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

    virtual void initialize();
  	virtual void terminate();
	  virtual void mouseLeftPressEvent		 ( QMouseEvent *cEvent );
	  virtual void mouseLeftReleaseEvent	 ( QMouseEvent *cEvent );
    virtual void mouseMoveEvent		       ( QMouseEvent *cEvent );
  
  private:
    QPoint _start, _last;
    bool m_bWorking;
};

class FCMouseModelCirclePick : public FCMouseModelStd 
{
  public:
    FCMouseModelCirclePick();
    virtual ~FCMouseModelCirclePick();

    virtual void initialize();
  	virtual void terminate();
	  virtual void mouseRightPressEvent( QMouseEvent  *cEvent );
    virtual void wheelEvent			     ( QWheelEvent  *cEvent );
    virtual void mouseMoveEvent		   ( QMouseEvent  *cEvent );
    virtual void paintEvent          ( QPaintEvent  *cEvent );
    virtual void resizeEvent			   ( QResizeEvent *cEvent );

  private:
    void draw(const QPoint& point);

    QPoint _last;
    int    _nRadius;
};

#endif
