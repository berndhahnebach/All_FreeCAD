#ifndef __MOUSEMODEL_H__
#define __MOUSEMODEL_H__

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
	virtual void initMouseModel(View3D *pcView3D){_pcView3D=pcView3D;};
	virtual void releaseMouseModel(void){};

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

};




class FCMouseModelStd :public FCMouseModel
{
public:
	FCMouseModelStd(void){};

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

















#endif