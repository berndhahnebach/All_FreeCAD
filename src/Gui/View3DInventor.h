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

#include <Base/Parameter.h>

class QMouseEvent;
class QSplitter;
class QWidget;
class QPushButton;
class QTabBar;
class QVBox;
class SoQtViewer;

class SoSeparator;
class SoShapeHints;
class SoMaterial;


namespace Gui {
class Document;
class TreeView;
class View3DInventor;
class View3DInventorViewer;
class MyView3DInventor;
class ViewProviderFeature;


/** The 3D View Window
 *  It consist out of the 3DView 
 *  \author Jürgen Riegel
 */
class GuiExport View3DInventor: public MDIView,public ParameterGrp::ObserverType
{
  Q_OBJECT

public:
  View3DInventor( Gui::Document* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
  ~View3DInventor();

  /// Mesage handler
  virtual bool onMsg(const char* pMsg, const char** ppReturn);
  virtual bool onHasMsg(const char* pMsg) const;

  /// Observer message from the ParameterGrp
  virtual void OnChange(ParameterGrp::SubjectType &rCaller,ParameterGrp::MessageType Reason);
  /// get called when the document is updated
  virtual void onRename(Gui::Document *pDoc);

  /// handle dropt files on this document
  void import(const char* FileName);

  virtual const char *getName(void) const;

  virtual void onUpdate(void);

  void updatePrefs(void);

  void setViewerDefaults(void);

  bool setCamera(const char* pCamera);
  void toggleClippingPlane();
  bool hasClippingPlane() const;

  /// helper to apply a SoWriteAction to a node and write it to a string
  static const std::string &View3DInventor::writeNodesToString(SoNode * root);

  View3DInventorViewer *getViewer(void) {return _viewer;}

public slots:
//  void closeEvent(QCloseEvent* e);
  void onWindowActivated ();
  void setCursor(const QCursor&);
  void dump(const char* filename);

protected:
  void dropEvent        ( QDropEvent      * e );
  void dragEnterEvent   ( QDragEnterEvent * e );
  void keyPressEvent    ( QKeyEvent       * e );
  void keyReleaseEvent  ( QKeyEvent       * e );

  /// handle to the viewer parameter group
  ParameterGrp::handle hGrp;

private:
  View3DInventorViewer * _viewer;
};

} // namespace Gui

#endif  //__VIEW3DINVENTOR__

