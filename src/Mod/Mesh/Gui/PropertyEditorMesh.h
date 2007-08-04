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

#ifndef MESHGUI_PROPERTYEDITOR_MESH_H
#define MESHGUI_PROPERTYEDITOR_MESH_H

#ifndef __Qt3All__
# include <Gui/Qt3All.h>
#endif

#include <Gui/propertyeditor/propertyeditorinput.h>

namespace MeshGui {

/**
 * Display data of a mesh kernel.
 * \author Werner Mayer
 */
class GuiMeshExport PropertyMeshKernelItem: public Gui::PropertyEditor::PropertyItem
{
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

protected:
    virtual QVariant toolTip(const App::Property*) const;
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyMeshKernelItem();
};



#if 0
/**
 * Change a text property.
 * \author Werner Mayer
 */
class GuiMeshExport PropertyEditorMesh : public Gui::PropertyEditor::EditableItem
{
  TYPESYSTEM_HEADER();

protected:
  QWidget* createEditor( int column, QWidget* parent );
  virtual void stopEdit( int column );
  virtual void setDefaultEditorValue( QWidget* editor );
  virtual QVariant convertFromProperty(const std::vector<App::Property*>&);
  virtual void convertToProperty(const QVariant&);

private:
  PropertyEditorMesh();
};
#endif
} // namespace MeshGui


#endif // MESHGUI_PROPERTYEDITOR_MESH_H

