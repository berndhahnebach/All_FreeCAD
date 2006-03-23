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
# include <qapplication.h>
# include <qlineedit.h>
# include <qlistview.h>
# include <qmessagebox.h>
# include <qpainter.h>
# include <qvariant.h>
#endif

#include <Mod/Mesh/App/MeshFeature.h>
#include "PropertyEditorMesh.h"

using namespace MeshGui;
using MeshCore::MeshKernel;


TYPESYSTEM_SOURCE(MeshGui::PropertyEditorMesh, Gui::PropertyEditor::EditableItem);

PropertyEditorMesh::PropertyEditorMesh()
{
  setEditable(false);
  setExpandable( true );
  setOpen(true);
  Gui::PropertyEditor::IntEditorItem* item=0;
  item = new Gui::PropertyEditor::IntEditorItem(EditableItem::parentView, "Faces", 0);
  item->setEditable(false);
  insertItem(item);
  item = new Gui::PropertyEditor::IntEditorItem(EditableItem::parentView, "Points", 0);
  item->setEditable(false);
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

void PropertyEditorMesh::stopEdit( QWidget* editor, int column )
{
  setOverrideValue( dynamic_cast<QLineEdit*>(editor)->text() );
  setText( column, overrideValue().toString() );
}

void PropertyEditorMesh::setDefaultValue()
{
  QLineEdit* edit = dynamic_cast<QLineEdit*>(_editor);
  edit->setText( value().toString() );
}

void PropertyEditorMesh::convertFromProperty(const std::vector<App::Property*>& prop)
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
  setValue( value );
  setText( 1, value.toString() );

  // set children
  Gui::PropertyEditor::EditableItem* item = (Gui::PropertyEditor::EditableItem*)firstChild();
  QVariant pts(ctPts);
  item->setValue(pts);
  item->setText( 1, pts.toString() );
  item = (Gui::PropertyEditor::EditableItem*)item->nextSibling();
  QVariant fts(ctFts);
  item->setValue(fts);
  item->setText( 1, fts.toString() );
}

void PropertyEditorMesh::convertToProperty(const QVariant&)
{
}
