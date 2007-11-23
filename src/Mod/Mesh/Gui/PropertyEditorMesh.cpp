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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <qlineedit.h>
#endif

#include <Mod/Mesh/App/MeshFeature.h>
#include "PropertyEditorMesh.h"

using namespace MeshGui;
using MeshCore::MeshKernel;


TYPESYSTEM_SOURCE(MeshGui::PropertyMeshKernelItem, Gui::PropertyEditor::PropertyItem);

PropertyMeshKernelItem::PropertyMeshKernelItem()
{
}

QVariant PropertyMeshKernelItem::value(const App::Property*) const
{
    int ctP = 0;
    int ctE = 0;
    int ctF = 0;

    std::vector<App::Property*> props = getProperty();
    for ( std::vector<App::Property*>::const_iterator pt = props.begin(); pt != props.end(); ++pt ) {
        Mesh::PropertyMeshKernel* pPropMesh = (Mesh::PropertyMeshKernel*)(*pt);
        const MeshKernel& rMesh = pPropMesh->getValue().getKernel();
        ctP += (int)rMesh.CountPoints();
        ctE += (int)rMesh.CountEdges();
        ctF += (int)rMesh.CountFacets();
    }

    QString  str = QString("[Points: %1, Edges: %2 Faces: %3]").arg(ctP).arg(ctE).arg(ctF);
    return QVariant(str);
}

QVariant PropertyMeshKernelItem::toolTip(const App::Property* prop) const
{
    return value(prop);
}

void PropertyMeshKernelItem::setValue(const QVariant& value)
{
}

QWidget* PropertyMeshKernelItem::createEditor(QWidget* parent, const QObject* receiver, const char* method) const
{
    return 0;
}

void PropertyMeshKernelItem::setEditorData(QWidget *editor, const QVariant& data) const
{
}

QVariant PropertyMeshKernelItem::editorData(QWidget *editor) const
{
    return QVariant();
}



#if 0
TYPESYSTEM_SOURCE(MeshGui::PropertyEditorMesh, Gui::PropertyEditor::EditableItem);

PropertyEditorMesh::PropertyEditorMesh()
{
  setReadOnly(true);
  setExpandable( true );
  setOpen(true);
  Gui::PropertyEditor::IntEditorItem* item=0;
  item = new Gui::PropertyEditor::IntEditorItem(EditableItem::parentView, "Faces", 0);
  item->setReadOnly(true);
  insertItem(item);
  item = new Gui::PropertyEditor::IntEditorItem(EditableItem::parentView, "Points", 0);
  item->setReadOnly(true);
  insertItem(item);
}

QWidget* PropertyEditorMesh::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QLineEdit* editor = new QLineEdit( parent, "TextEditorItem::edit" );
  editor->setText( overrideValue().toString() );
  editor->setFocus();
  connect(editor, SIGNAL( textChanged(const QString&) ), this, SLOT( onValueChanged() ) );
  return editor;
}

void PropertyEditorMesh::stopEdit( int column )
{
  setText( column, overrideValue().toString() );
}

void PropertyEditorMesh::setDefaultEditorValue( QWidget* editor )
{
  QLineEdit* edit = dynamic_cast<QLineEdit*>(editor);
  edit->setText( value().toString() );
}

QVariant PropertyEditorMesh::convertFromProperty(const std::vector<App::Property*>& prop)
{
  int ctPts = 0;
  int ctFts = 0;

  for ( std::vector<App::Property*>::const_iterator pt = prop.begin(); pt != prop.end(); ++pt )
  {
    Mesh::PropertyMeshKernel* pPropMesh = (Mesh::PropertyMeshKernel*)(*pt);
    const MeshKernel& rMesh = pPropMesh->getValue();
    ctPts += (int)rMesh.CountPoints();
    ctFts += (int)rMesh.CountFacets();
  }

  QString  str = QString("[Points: %1, Faces: %2]").arg(ctPts).arg(ctFts);
  QVariant value( str );
  setText( 1, value.toString() );

  // set children
  Gui::PropertyEditor::EditableItem* item = (Gui::PropertyEditor::EditableItem*)firstChild();
  QVariant pts(ctPts);
  item->setText( 1, pts.toString() );
  item = (Gui::PropertyEditor::EditableItem*)item->nextSibling();
  QVariant fts(ctFts);
  item->setText( 1, fts.toString() );
  return value;
}

void PropertyEditorMesh::convertToProperty(const QVariant&)
{
}

// --------------------------------------------------------------------
#endif
