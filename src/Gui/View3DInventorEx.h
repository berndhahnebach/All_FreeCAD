/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef __VIEW3DINVENTOREX__
#define __VIEW3DINVENTOREX__

#include "View.h"

#ifndef _PreComp_
# include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#endif

class QMouseEvent;
class FCGuiDocument;
class View3D;
class TreeView;
class QSplitter;
class QWidget;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QVBox;
class QWidgetStack;
class QTabBar;
class View3DInventor;

class SoSeparator ;
class SoShapeHints;
class SoMaterial	;
class SoRotationXYZ;

namespace Gui {
class MyExaminerViewer;

/** The 3D View Window
 *  It consist out of the 3DView and the tree
 *  \author Jürgen Riegel
 */
class View3DInventorEx: public MDIView
{
  Q_OBJECT

public:
  View3DInventorEx( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
  ~View3DInventorEx();

  /// Mesage handler
  virtual bool OnMsg(const char* pMsg);
  virtual bool OnHasMsg(const char* pMsg);


  virtual const char *GetName(void);

  virtual void resizeEvent ( QResizeEvent * e);

  virtual void Update(void);

  void UpdatePrefs(void);

  void SetViewerDefaults(void);

//  void SetShape(void);


//signals:
  //void message(const QString&, int );
  
public slots:
//  void closeEvent(QCloseEvent* e);        
  void onWindowActivated ();
  void setCursor(const QCursor&);
  void dump();

protected:
  // inventor nodes:
  SoSeparator * pcSepUserSpecial;
  SoSeparator * pcSepRoot;
//  SoSeparator * pcSepShapeRoot;
  SoSeparator * pcSepAxis;
  SoShapeHints* pcShapeHint; 
  SoMaterial	* pcShapeMaterial;
private:
//  SoQtExaminerViewer * _viewer;
  MyExaminerViewer * _viewer;
  QWidget*      _pcWidget;
  QVBox*        _pcFrame;
  QWidgetStack* _pcWidgetStack;
  QTabBar*      _pcTabBar;
};


class MyExaminerViewer : public SoQtExaminerViewer {

public:
  MyExaminerViewer(QWidget *parent, const char * filename);
  ~MyExaminerViewer();

protected:
  virtual void actualRedraw(void);

private:
  SoSeparator * bckgroundroot;
  SoSeparator * foregroundroot;
  SoRotationXYZ * arrowrotation;
};


/** The Inventor viewer
 *  
class View3DInventor: public SoQtViewer
{
public:

    View3DInventor (QWidget *parent, const char *name=NULL, SbBool embed=true, Type type= SoQtViewer::BROWSER, SbBool build=true) 
      :SoQtViewer (parent, name, embed, type, build)
    {

    }

    ~View3DInventor(){}
};

 */

} // namespace Gui

#endif  //__VIEW3DINVENTOR__

