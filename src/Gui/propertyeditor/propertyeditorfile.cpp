/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <qpushbutton.h>
# include <qfiledialog.h>
# include <qcombobox.h>
# include <qpainter.h>
# include <qheader.h>
# include <qcheckbox.h>
# include <qlayout.h>
#endif

#include "../FileDialog.h"

#include "propertyeditorfile.h"
#include "propertyeditorlist.h"
#include "propertyeditorinput.h"

using Gui::PreviewLabel;
using namespace Gui::PropertyEditor;

/* TRANSLATOR Gui::PropertyEditor::FileEditorItem */

FileEditorItem::FileEditorItem( QListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setText( 0, text );
  setText(1, overrideValue().toCString());
}

QWidget* FileEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QPushButton* editor = new QPushButton( parent, "FileEditorItem::edit" );

  editor->setText( overrideValue().toCString() );

  connect(editor, SIGNAL(clicked()), this, SLOT(onChangeFile()));
  return editor;
}

void FileEditorItem::stopEdit( QWidget* editor, int column )
{
  QVariant var;
  var.asCString() = dynamic_cast<QPushButton*>(_editor)->text();
  setOverrideValue( var );
  setText( column, var.toCString() );
}

void FileEditorItem::setDefaultValue()
{
  QPushButton* btn = dynamic_cast<QPushButton*>(_editor);
  btn->setText( value().toCString() );
}

void FileEditorItem::onChangeFile()
{
  QPushButton* btn = dynamic_cast<QPushButton*>(_editor);

  if ( btn )
  {
    QString url = QFileDialog::getOpenFileName(QString::null, QString::null, listView(), tr("Choose a file"));
    if ( !url.isEmpty() )
    {
      onValueChanged();
      btn->setText( url );
      setModified( true );
    }
  }
}

// ======================================================================

/* TRANSLATOR Gui::PropertyEditor::PixmapEditorItem */

PixmapEditorItem::PixmapEditorItem( QListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setText( 0, text );
  setPixmap(1, overrideValue().toPixmap());
}

QWidget* PixmapEditorItem::createEditor( int column, QWidget* parent )
{
  if ( column == 0 )
    return 0;

  QPushButton* editor = new QPushButton( parent, "PixmapEditorItem::button" );

  editor->setPixmap( overrideValue().toPixmap() );

  connect(editor, SIGNAL(clicked()), this, SLOT(onChangePixmap()));
  return editor;
}

void PixmapEditorItem::stopEdit( QWidget* editor, int column )
{
  const QPixmap* p = dynamic_cast<QPushButton*>(_editor)->pixmap();

  if ( p )
  {
    setOverrideValue( *p );
    setPixmap( column, *p );
  }
}

void PixmapEditorItem::setDefaultValue()
{
  QPushButton* btn = dynamic_cast<QPushButton*>(_editor);
  btn->setPixmap( value().toPixmap() );
}

void PixmapEditorItem::onChangePixmap()
{
  QPushButton* btn = dynamic_cast<QPushButton*>(_editor);

  if ( btn )
  {
    PreviewLabel* p = new PreviewLabel;

    QFileDialog dlg( QString::null, QString("*.png *.xpm *.bmp *.jpg Pixmap Files"), listView(), 0, true );
    dlg.setContentsPreviewEnabled( true );
    dlg.setContentsPreview( p, p );
    dlg.setViewMode( QFileDialog::List );
    dlg.setPreviewMode( QFileDialog::Contents );
    dlg.setCaption( tr( "Choose a Pixmap..." ) );

    if ( dlg.exec() == QDialog::Accepted )
    {
      QString url = dlg.selectedFile();
      if ( !url.isEmpty() )
      {
        onValueChanged();
        QPixmap p;
        p.load(url);
        int w = QMIN(p.width(), height());
        int h = QMIN(p.height(), height());
        p.resize(w, h);
        btn->setPixmap( p );
      }
    }
  }
}

// ======================================================================

ChildrenEditorItem::ChildrenEditorItem( QListView* lv, const QString& text, const QVariant& value )
  :EditableItem( lv, value )
{
  setExpandable( true );
  setText( 0, text);
  setText( 1, value.toStringList().last());
  insertItem(new BoolEditorItem(lv, "Bool", 0));
  insertItem(new IntEditorItem(lv, "Width", 5));
  insertItem(new IntEditorItem(lv, "Height", 15));
}

QWidget* ChildrenEditorItem::createEditor( int column, QWidget* parent )
{
//    if ( column == 0 )
    return 0;

  QStringList items = overrideValue().toStringList();
  QComboBox* editor = new QComboBox( parent, "ListEditorItem::combo" );
  connect(editor, SIGNAL( activated(int) ), this, SLOT( onValueChanged() ) );

  QString txt = items.last();
  items.pop_back();
  editor->insertStringList( items );

  int cur = 0;
  for ( QStringList::Iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( txt == *it)
    {
      editor->setCurrentItem( cur );
      break;
    }

    cur++;
  }

  return editor;
}

void ChildrenEditorItem::stopEdit( QWidget* editor, int column )
{
  QComboBox* combo = dynamic_cast<QComboBox*>(editor);

  QVariant var = overrideValue();
  var.asStringList().last() = combo->currentText();
  setOverrideValue( var );
  setText( column, overrideValue().toStringList().last() );
}

void ChildrenEditorItem::setDefaultValue()
{
  QComboBox* combo = dynamic_cast<QComboBox*>(_editor);

  QStringList items = value().toStringList();

  QString txt = items.last();
  items.pop_back();

  int cur = 0;
  for ( QStringList::Iterator it = items.begin(); it != items.end(); ++it )
  {
    if ( txt == *it)
    {
      combo->setCurrentItem( cur );
      break;
    }

    cur++;
  }
}

#include "moc_propertyeditorfile.cpp"
