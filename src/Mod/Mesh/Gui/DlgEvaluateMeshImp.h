/***************************************************************************
 *   Copyright (c) 2006 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef MESHGUI_DLG_EVALUATE_MESH_IMP_H
#define MESHGUI_DLG_EVALUATE_MESH_IMP_H

#include <map>
#include <App/Application.h>
#include <App/Document.h>
#include "ui_DlgEvaluateMesh.h"

namespace Gui {
class View3DInventorViewer;
}
namespace Mesh {
  class Feature;
}

namespace MeshGui {
class ViewProviderMeshDefects;

/**
 * The GuiCleanupHandler class is used to cleanup GUI elements from the MeshGui module when the application is about to be 
 * closed.
 * @author Werner Mayer
 */
class CleanupHandler : public QObject
{
  Q_OBJECT

public:
   CleanupHandler();

public slots:
   void cleanup();
};

/**
 * \author Werner Mayer
 */
class DlgEvaluateMeshImp : public QDialog, public Ui_DlgEvaluateMesh//, public App::Document::ObserverType, public App::Application::ObserverType
{ 
  Q_OBJECT

public:
  DlgEvaluateMeshImp( QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~DlgEvaluateMeshImp();

  /// Checks if the current feature is about to be removed
  //void OnChange(App::Document::SubjectType &rCaller,App::Document::MessageType Reason);
  /// Checks if the current document is about to be closed
  //void OnChange(App::Application::SubjectType &rCaller, App::Application::MessageType rcReason);

  void setMesh( Mesh::Feature* );

  void slotDeletedDocument(App::Document& Doc);
  void slotDeletedObject(App::DocumentObject& Obj);

protected Q_SLOTS:
  void on_checkOrientationButton_clicked();
  void on_analyzeOrientationButton_clicked();
  void on_repairOrientationButton_clicked();

  void on_checkDuplicatedFacesButton_clicked();
  void on_analyzeDuplicatedFacesButton_clicked();
  void on_repairDuplicatedFacesButton_clicked();

  void on_checkDuplicatedPointsButton_clicked();
  void on_analyzeDuplicatedPointsButton_clicked();
  void on_repairDuplicatedPointsButton_clicked();

  void on_checkNonmanifoldsButton_clicked();
  void on_analyzeNonmanifoldsButton_clicked();
  void on_repairNonmanifoldsButton_clicked();

  void on_checkDegenerationButton_clicked();
  void on_analyzeDegeneratedButton_clicked();
  void on_repairDegeneratedButton_clicked();

  void on_checkIndicesButton_clicked();
  void on_analyzeIndicesButton_clicked();
  void on_repairIndicesButton_clicked();

  void on_refreshButton_clicked();
  void on_meshNameButton_activated(int);

protected:
  void cleanInformation();
  void setFixedMesh();
  void addViewProvider( const char* vp );
  void removeViewProvider( const char* vp );
  void removeViewProviders();

private:
  std::map<std::string, ViewProviderMeshDefects*> _vp;
  Mesh::Feature* _meshFeature;
  Gui::View3DInventorViewer* _viewer;
  App::Document* _pDoc;
};

/**
 * The DockEvaluateMeshImp class creates a single instance and embeds it into a dock window.
 * \author Werner Mayer
 */
class DockEvaluateMeshImp : public DlgEvaluateMeshImp
{ 
  Q_OBJECT

protected:
  DockEvaluateMeshImp( QWidget* parent = 0, Qt::WFlags fl = 0 );
  ~DockEvaluateMeshImp();
  void closeEvent(QCloseEvent* e);

public:
  static DockEvaluateMeshImp* instance();
  static void destruct();
  static bool hasInstance();
  
  QSize sizeHint () const;

private:
  static DockEvaluateMeshImp* _instance;
  QByteArray state;
};

} // namespace MeshGui

#endif // MESHGUI_DLG_EVALUATE_MESH_IMP_H
