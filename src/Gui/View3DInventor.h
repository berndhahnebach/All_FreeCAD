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


#ifndef __VIEW3DINVENTOR__
#define __VIEW3DINVENTOR__

#include "View.h"

#include "Inventor/Qt/viewers/SoQtViewer.h"

class QMouseEvent;
class QSplitter;
class QWidget;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QWidgetStack;
class QTabBar;
class QVBox;
class SoQtExaminerViewer;
class SoQtViewer;

class SoSeparator;
class SoShapeHints;
class SoMaterial;


namespace Gui {
class Document;
class TreeView;
class View3D;
class MyView3DInventor;

/** The 3D View Window
 *  It consist out of the 3DView and the tree
 *  \author Jürgen Riegel
 */
class View3DInventor: public MDIView
{
  Q_OBJECT

public:
  View3DInventor( Gui::Document* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
  ~View3DInventor();

  /// Mesage handler
  virtual bool onMsg(const char* pMsg);
  virtual const char *getName(void);

  virtual void resizeEvent ( QResizeEvent * e);

  virtual void onUpdate(void);

  void updatePrefs(void);

  void setViewerDefaults(void);

  void setShape(void);


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
  SoSeparator * pcSepShapeRoot;
  SoSeparator * pcSepAxis;
  SoShapeHints* pcShapeHint; 
  SoMaterial	* pcShapeMaterial;
private:
//  SoQtExaminerViewer * _viewer;
  MyView3DInventor * _viewer;
  QWidget*      _pcWidget;
  QVBox*        _pcFrame;    
  QWidgetStack* _pcWidgetStack;
  QTabBar*      _pcTabBar;
};


/** The Inventor viewer
 *  
 */
class MyView3DInventor: public SoQtViewer
{
public:
    MyView3DInventor (QWidget *parent, const char *name=NULL, SbBool embed=true, Type type= SoQtViewer::BROWSER, SbBool build=true) 
      :SoQtViewer (parent, name, embed, type, build)
    {

    }

    ~MyView3DInventor(){}
};

} // namespace Gui

#endif  //__VIEW3DINVENTOR__

