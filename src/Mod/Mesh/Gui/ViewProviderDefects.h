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

#ifndef MESHGUI_VIEWPROVIDER_MESH_DEFECTS_H
#define MESHGUI_VIEWPROVIDER_MESH_DEFECTS_H

#include "ViewProvider.h"

class SoCoordinate3;
class SoLineSet;
class SoFaceSet;

namespace MeshGui {

/** The ViewProviderMeshDefects class is used to display the most known types of defects of a polymesh.
 * In subclasses defects like non-manifolds, wrong oriented facets, degenerated facets, duplicates, .... are displayed.
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderMeshDefects : public Gui::ViewProviderDocumentObject
{
  PROPERTY_HEADER(MeshGui::ViewProviderMeshDefects);

public:
  ViewProviderMeshDefects();
  virtual ~ViewProviderMeshDefects();

  // Build up the initial Inventor node
  virtual void attach(App::AbstractFeature* pcFeature) = 0;
  /// Fill up the Inventor node with data
  virtual void showDefects() = 0;

protected:
  SoCoordinate3 * pcCoords;
};

/** The ViewProviderMeshOrientation class displays wrong oriented facets in orange. 
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderMeshOrientation : public ViewProviderMeshDefects
{
  PROPERTY_HEADER(MeshGui::ViewProviderMeshOrientation);

public:
  ViewProviderMeshOrientation();
  virtual ~ViewProviderMeshOrientation();

  void attach(App::AbstractFeature* pcFeature);
  void showDefects();

protected:
  SoFaceSet* pcFaces;
};

/** The ViewProviderMeshManifolds class displays edges with more than two faces attached in red. 
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderMeshManifolds : public ViewProviderMeshDefects
{
  PROPERTY_HEADER(MeshGui::ViewProviderMeshManifolds);

public:
  ViewProviderMeshManifolds();
  virtual ~ViewProviderMeshManifolds();

  void attach(App::AbstractFeature* pcFeature);
  void showDefects();

protected:
  SoLineSet* pcLines;
};

/** The ViewProviderMeshDuplicatedFaces class displays duplicated faces in red. 
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderMeshDuplicatedFaces : public ViewProviderMeshDefects
{
  PROPERTY_HEADER(MeshGui::ViewProviderMeshDuplicatedFaces);

public:
  ViewProviderMeshDuplicatedFaces();
  virtual ~ViewProviderMeshDuplicatedFaces();

  void attach(App::AbstractFeature* pcFeature);
  void showDefects();

protected:
  SoFaceSet* pcFaces;
};

/** The ViewProviderMeshDegenerations class displays degenerated faces to a line or even a point in orange. 
 * @author Werner Mayer
 */
class GuiMeshExport ViewProviderMeshDegenerations : public ViewProviderMeshDefects
{
  PROPERTY_HEADER(MeshGui::ViewProviderMeshDegenerations);

public:
  ViewProviderMeshDegenerations();
  virtual ~ViewProviderMeshDegenerations();

  void attach(App::AbstractFeature* pcFeature);
  void showDefects();

protected:
  SoLineSet* pcLines;
};

class GuiMeshExport ViewProviderMeshDuplicatedPoints : public ViewProviderMeshDefects
{
  PROPERTY_HEADER(MeshGui::ViewProviderMeshDuplicatedPoints);

public:
  ViewProviderMeshDuplicatedPoints();
  virtual ~ViewProviderMeshDuplicatedPoints();

  void attach(App::AbstractFeature* pcFeature);
  void showDefects();
};

class GuiMeshExport ViewProviderMeshIndices : public ViewProviderMeshDefects
{
  PROPERTY_HEADER(MeshGui::ViewProviderMeshIndices);

public:
  ViewProviderMeshIndices();
  virtual ~ViewProviderMeshIndices();

  void attach(App::AbstractFeature* pcFeature);
  void showDefects();
};

} // namespace MeshGui


#endif // MESHGUI_VIEWPROVIDER_MESH_DEFECTS_H

