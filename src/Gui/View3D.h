/***************************************************************************
                          View3D.h  -  description
                             -------------------
    begin                : Mon Jan 1 2001
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@bluewin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __VIEW3D__
#define __VIEW3D__

#include <qgl.h>
#include <AIS_InteractiveContext.hxx>
#include <V3d_OrthographicView.hxx>
// some window system dependend
#ifndef WNT
#	include <Xw_Window.hxx>
#	include <Graphic3d_GraphicDevice.hxx>
#else
#	include <WNT_Window.hxx>
#	include <Graphic3d_WNTGraphicDevice.hxx>
#endif

class QMouseEvent;
class FCMouseModel;
class FCGuiDocument;


#include <stack>


/**
  * The OpenCasCade Graphics rendering widget
  *
  * @author Juergen Riegel
  * @version 0.1
  */


class View3D: public QGLWidget
{
	Q_OBJECT

public:
	/** construtor of 3DView. Does nothing importend, the
	*  tricky stuff about the COpenCasCade init comes with InitCasCadeView()
	* @param Handle(Graphic3d_GraphicDevice) OCC Graphic Device (DISPLAY0:0) comes from the Application
	* @param Handle(V3d_Viewer) OCC Viewer (note: _not_ View) from the Document
	* @param Handle(AIS_InteractiveContext) OCC Interactive Context comes from the Document
	* @see QGLWidget InitCasCadeView
	*/	
	View3D(FCGuiDocument*  pcDocument,
		   QWidget *parent, 
		   const char *name="View3D", 
		   WFlags f=WDestructiveClose
	       );
	/** Initialation of the OCC Graphics. Here is the bundling of the QGLWidget
	* with the OCCGraphics3D_Window. Its very importend that the XWindow is fully
	* showed bevor this function is called! There for its not done in the
	* constructor
	*/	
	bool InitCasCadeView(void);

	// Managing MouseModels
	void			PushMouseModel	(FCMouseModel *);
	void			PopMouseModel	(void);
	FCMouseModel*	GetMouseModel	(void);

	// Confiniance Geter
	Handle_V3d_View					&GetView   (void){return _hView;}
	Handle_V3d_Viewer				&GetViewer (void){return _hViewer;}
	Handle_AIS_InteractiveContext	&GetContext(void){return _hContext;}

	void ShowPopup(int x,int y);

	bool ScreenDump(Standard_CString theFile);


protected:
	// user interaction events (got mainly handled in the MouseModel classes)
	virtual void mousePressEvent		( QMouseEvent *);
	virtual void mouseReleaseEvent		( QMouseEvent *);
	virtual void mouseMoveEvent			( QMouseEvent *);
	virtual void mouseDoubleClickEvent	( QMouseEvent * ); 
	virtual void keyPressEvent			( QKeyEvent * );
	virtual void keyReleaseEvent		( QKeyEvent * ); 

	/** Update the view when paint event occur. */
	virtual void paintEvent				(QPaintEvent * ) ;
	/** Update the view when resize event occur.  */
	virtual void resizeEvent			(QResizeEvent * ) ;


private slots:
	void SetViewFitAll(){_hView->FitAll();_hView->ZFitAll();}
	void SetViewFront() {_hView->SetProj(V3d_Yneg);}
	void SetViewBottom(){_hView->SetProj(V3d_Zneg);SetViewFitAll();}
	void SetViewLeft()  {_hView->SetProj(V3d_Xpos);SetViewFitAll();}
	void SetViewRear()  {_hView->SetProj(V3d_Ypos);SetViewFitAll();}
	void SetViewRight() {_hView->SetProj(V3d_Xneg);SetViewFitAll();}
	void SetViewTop()   {_hView->SetProj(V3d_Zpos);SetViewFitAll();}
	void SetViewAxo()   {_hView->SetProj(V3d_XposYnegZpos);}
	
private:
	Handle(V3d_View)				_hView;
	Handle(AIS_InteractiveContext)  _hContext;
	Handle(V3d_Viewer)              _hViewer;

	FCGuiDocument*  _pcDocument;

	stlport::stack<FCMouseModel *>      _cMouseStack;
	
	bool bIsInit;
	
};

#endif

