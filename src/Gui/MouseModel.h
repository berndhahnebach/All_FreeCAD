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

/**
 * The mouse model base class
 * In derived classes you must implement the methods
 * @ref initialize() and @ref terminate()
 * For all drawing stuff you just have to reimplement
 * the @ref draw() method. In general you need not do
 * something else.
 * \author Werner Mayer and Jürgen Riegel
 */
class FCMouseModel
{
public:
	FCMouseModel(void){};
  /// implement this in derived classes
	virtual void initialize() = 0;
  /// implement this in derived classes
	virtual void terminate () = 0;
	void initMouseModel(View3D *pcView3D);
	void releaseMouseModel(void);
  void moveMouseEvent (QMouseEvent *cEvent);
  void wheelMouseEvent (QWheelEvent *cEvent);

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

  virtual void paintEvent				( QPaintEvent * ){ draw(); } ;
  virtual void resizeEvent			( QResizeEvent * ){ draw(); };
	
	// Geter
	View3D							&GetView3D (void);
	Handle_V3d_View					&GetView   (void);
	Handle_V3d_Viewer				&GetViewer (void);
	Handle_AIS_InteractiveContext	&GetContext(void);

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




/**
 * The standard model class
 * \author Jürgen Riegel
 */
class FCMouseModelStd :public FCMouseModel
{
public:
	FCMouseModelStd(void);

  /// do nothing
	virtual void initialize();
  /// do nothing
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

/**
 * The poly picker mouse model class
 * Create a polygon
 * \author Werner Mayer
 */
class FCMouseModelPolyPicker : public FCMouseModelStd
{
  public:
    FCMouseModelPolyPicker();
    virtual ~FCMouseModelPolyPicker();

    /// set the new mouse cursor
    virtual void initialize();
    /// do nothing
  	virtual void terminate();
	  virtual void mouseLeftPressEvent		 ( QMouseEvent *cEvent);
	  virtual void mouseMiddlePressEvent	 ( QMouseEvent *cEvent);
	  virtual void mouseRightPressEvent		 ( QMouseEvent *cEvent);
  	virtual void mouseDoubleClickEvent	 ( QMouseEvent *cEvent);
    virtual void wheelEvent			    ( QWheelEvent  * cEvent );
    virtual void keyPressEvent		  ( QKeyEvent    * cEvent );

  protected:
    /// draw the polygon
    virtual void draw ();
    FCvector<QPoint> _cNodeVector;
    int  m_iRadius, m_iNodes;
    bool m_bWorking, m_bDrawNodes;
};

/**
 * The selection mouse model class
 * Draws a rectangle for selection
 * \author Werner Mayer
 */
class FCMouseModelSelection : public FCMouseModelStd 
{
  public:
    FCMouseModelSelection();
    virtual ~FCMouseModelSelection();

    /// do nothing
    virtual void initialize();
    /// do nothing
  	virtual void terminate();
	  virtual void mouseLeftPressEvent		 ( QMouseEvent *cEvent );
	  virtual void mouseLeftReleaseEvent	 ( QMouseEvent *cEvent );

  protected:
    /// draw the rectangle
    virtual void draw ();
  
  private:
    bool m_bWorking;
};

/**
 * The picker mouse model class
 * \author Werner Mayer
 */
class FCMouseModelCirclePicker : public FCMouseModelStd 
{
  public:
    FCMouseModelCirclePicker();
    virtual ~FCMouseModelCirclePicker();

    /// set the new mouse cursor
    virtual void initialize();
    /// call the @ref draw() method to clear the view
  	virtual void terminate();
	  virtual void mouseRightPressEvent( QMouseEvent  *cEvent );
    virtual void wheelEvent			     ( QWheelEvent  *cEvent );

  protected:
    /// draw circle and text
    virtual void draw ();

  private:
    int    _nRadius;
};

#endif
