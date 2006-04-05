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

#ifndef _PreComp_
#endif

#include <App/Document.h>
#include "DlgEvaluateMesh.h"

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
   cleanup();
};

/**
 * \author Werner Mayer
 */
class DlgEvaluateMeshImp : public DlgEvaluateMesh, public App::Document::ObserverType, public App::Application::ObserverType
{ 
public:
  DlgEvaluateMeshImp( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~DlgEvaluateMeshImp();

  /// Checks if the current feature is about to be removed
  void OnChange(App::Document::SubjectType &rCaller,App::Document::MessageType Reason);
  /// Checks if the current document is about to be closed
  void OnChange(App::Application::SubjectType &rCaller, App::Application::MessageType rcReason);

  void setMesh( Mesh::Feature* );
  void onRefreshInfo();
  void onAnalyzeOrientation();
  void onRepairOrientation();
  void onAnalyzeNonManifolds();
  void onRepairNonManifolds();
  void onAnalyzeIndices();
  void onRepairIndices();
  void onAnalyzeDegenerations();
  void onRepairDegenerations();
  void onAnalyzeDuplicatedFaces();
  void onRepairDuplicatedFaces();
  void onAnalyzeDuplicatedPoints();
  void onRepairDuplicatedPoints();

protected:
  void cleanInformation();
  void setFixedMesh();
  void addViewProvider( const char* vp );
  void removeViewProvider( const char* vp );

private:
  std::map<std::string, ViewProviderMeshDefects*> _vp;
  Mesh::Feature* _meshFeature;
  Gui::View3DInventorViewer* _viewer;
  App::Document* _pDoc;
};

/**
 * \author Werner Mayer
 */
class DockEvaluateMeshImp : public DlgEvaluateMeshImp
{ 
protected:
  DockEvaluateMeshImp( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~DockEvaluateMeshImp();

public:
  static DockEvaluateMeshImp* instance();
  static void destruct();
  static bool hasInstance();
  
  QSize sizeHint () const;

private:
  static DockEvaluateMeshImp* _instance;
};

} // namespace MeshGui

#endif // MESHGUI_DLG_EVALUATE_MESH_IMP_H
