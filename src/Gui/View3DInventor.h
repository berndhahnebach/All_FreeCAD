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
class ViewProviderInventorFeature;
class MouseModel;


/** The 3D View Window
 *  It consist out of the 3DView and the tree
 *  \author Jürgen Riegel
 */
class GuiExport View3DInventor: public MDIView
{
  Q_OBJECT

public:
  View3DInventor( Gui::Document* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
  ~View3DInventor();

  /// Mesage handler
  virtual bool onMsg(const char* pMsg, const char** ppReturn);
  virtual bool onHasMsg(const char* pMsg);

  /// handle dropt files on this document
  void import(const char* FileName);

  virtual const char *getName(void);

  virtual void resizeEvent ( QResizeEvent * e);

  virtual void contextMenuEvent ( QContextMenuEvent * e);

  virtual void onUpdate(void);

  void updatePrefs(void);

  void setViewerDefaults(void);

  bool setCamera(const char* pCamera);
  /// helper to apply a SoWriteAction to a node and write it to a string
  static const std::string &View3DInventor::writeNodesToString(SoNode * root);

  void showDimension (void) const;

  virtual void hideEvent             ( QHideEvent  * );

  View3DInventorViewer *getViewer(void) {return _viewer;}

//signals:
  //void message(const QString&, int );

public slots:
//  void closeEvent(QCloseEvent* e);        
  void onWindowActivated ();
  void setCursor(const QCursor&);
  void dump();

protected:

  void dropEvent        ( QDropEvent      * e );
  void dragEnterEvent   ( QDragEnterEvent * e );

  SoSeparator *createAxis(void);

  // inventor nodes:
  SoSeparator * pcSepUserSpecial;
  SoSeparator * pcSepRoot;
  SoSeparator * pcSepShapeRoot;
  SoSeparator * pcSepAxis;
  SoShapeHints* pcShapeHint; 
  SoMaterial	* pcShapeMaterial;

  std::string            cViewProviderName;
  ViewProviderInventorFeature * pcActViewProvider;

private:

  View3DInventorViewer * _viewer;
};

} // namespace Gui

#endif  //__VIEW3DINVENTOR__

