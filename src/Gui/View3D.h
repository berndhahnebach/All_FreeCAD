/** \file View3D.h
 *  \brief 3D View Window
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  @see FCView.cpp
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
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

#include "View.h"
#include <stack>

class QMouseEvent;
class FCMouseModel;
class FCGuiDocument;
class FCGuiDocument;
class View3D;
class FCTree;
class QSplitter;
class QWidget;		
class QPushButton;	
class QVBoxLayout;	
class QHBoxLayout;	
class QWidgetStack;
class QTabBar;


/** The 3D View Window
 *  It consist out of the 3DView and the tree
 */
class FCView3D: public FCView
{
	Q_OBJECT

public:
	FCView3D( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCView3D();
	FCGuiDocument* getDocument();

	/// Mesage handler
	virtual bool OnMsg(const char* pMsg);

signals:
	//void message(const QString&, int );
	void sendCloseView(FCView* theView);
	
public slots:
	void closeEvent(QCloseEvent* e);        
	void onWindowActivated ();
	void setCursor(const QCursor&);
	void dump();
	virtual const char *GetName(void);

protected:
	//void createViewActions();
private:
	View3D*			_pcView3D;
	QSplitter*		_pcSplitter;
	FCTree*		    _pcTree;
	QWidget*		_pcWidget;
	QVBoxLayout*	_pcVBoxLayout;    
	QWidgetStack*	_pcWidgetStack;
	QTabBar*		_pcTabBar;

};



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
  void PrintDimensions (void) const;
  void PostHandleMovement (/*TViewChange tChange*/);
  void PreHandleMovement (/*TViewChange tChange*/);

	bool ScreenDump(Standard_CString theFile);


protected:
	// user interaction events (got mainly handled in the MouseModel classes)
	virtual void mousePressEvent		    ( QMouseEvent * );
	virtual void mouseReleaseEvent		  ( QMouseEvent * );
	virtual void mouseMoveEvent			    ( QMouseEvent * );
	virtual void mouseDoubleClickEvent	( QMouseEvent * ); 
	virtual void keyPressEvent			    ( QKeyEvent   * );
	virtual void keyReleaseEvent		    ( QKeyEvent   * ); 
	virtual void wheelEvent             ( QWheelEvent * );
  virtual void focusInEvent           ( QFocusEvent * );
  virtual void hideEvent              ( QHideEvent  * );

	/** Update the view when paint event occur. */
	virtual void paintEvent				      ( QPaintEvent * );
	/** Update the view when resize event occur.  */
	virtual void resizeEvent			      ( QResizeEvent* );


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

	FCstack<FCMouseModel *>      _cMouseStack;
	
	bool bIsInit;
	
};

#endif

