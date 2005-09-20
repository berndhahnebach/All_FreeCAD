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
# include <qlabel.h>
#endif

#include "DlgUndoRedo.h"
#include "Application.h"
#include "MainWindow.h"
#include "Document.h"
#include "PythonEditor.h"

using namespace Gui::Dialog;

/**
 *  Constructs a UndoRedoStack which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
UndoRedoStack::UndoRedoStack( QWidget * parent, const char * name, WFlags f)
  : QListBox(parent, name, f)
{
}

/** Destroys the object and frees any allocated resources */
UndoRedoStack::~UndoRedoStack()
{
}

/** 
 * If the left mouse button is pressed and depending on the moving direction
 * the items are selected or deselected.
 */
void UndoRedoStack::mouseMoveEvent ( QMouseEvent * e )
{
  if (e->state() == NoButton)
    return;

  QListBox::mouseMoveEvent(e);

  QListBoxItem* item = itemAt (e->pos());
  int pos = -1;
  if (item != NULL)
    pos = index(item);
  if (pos != -1)
  {
    int i;
    for (i=pos+1; i < (int)count(); i++)
    {
      setSelected(i, false);
    }
    for (i=0; i<=pos; i++)
    {
      setSelected(i, true);
    }
  }
}

/** Select all items above the selected one */
void UndoRedoStack::mousePressEvent (QMouseEvent* e)
{
  QListBox::mousePressEvent(e);

  int pos = currentItem();
  if (pos != -1)
  {
    int i;
    for (i=pos+1; i < (int)count(); i++)
    {
      setSelected(i, false);
    }
    for (i=0; i<=pos; i++)
    {
      setSelected(i, true);
    }
  }
}

// ------------------------------------------------------------

/* TRANSLATOR Gui::Dialog::UndoRedoDialog */

/**
 *  Constructs a UndoRedoDialog which is a child of 'parent', with the 
 *  name 'name'.' 
 */
UndoRedoDialog::UndoRedoDialog( QWidget* parent,  const char* name, TMode mode )
    : QPopupMenu( parent, name ), tMode(mode)
{
  if ( !name )
    setName( "UndoRedoDialog" );
  resize( 160, 140 ); 

  setFrameStyle( WinPanel | Raised );

  pTextLabel = new QLabel( this, "TextLabel" );
  pTextLabel->setGeometry( QRect( 5, 110, 150, 25 ) ); 
  pTextLabel->setFrameStyle(QFrame::Sunken);
  pTextLabel->setProperty( "focusPolicy", (int)QLabel::NoFocus );
  pTextLabel->setProperty( "frameShape", (int)QLabel::StyledPanel );
  pTextLabel->setProperty( "frameShadow", (int)QLabel::Sunken );


  pListBox = new UndoRedoStack( this, "ListBox" );
  pListBox->setGeometry( QRect( 5, 5, 150, 100 ) ); 
  pListBox->setProperty( "frameShadow", (int)QLabel::Sunken );
  pListBox->setSelectionMode(QListBox::Multi);

  connect(pListBox, SIGNAL( highlighted ( QListBoxItem * )), this, SLOT(onSelChangeUndoRedoStack()));
  connect(pListBox, SIGNAL( returnPressed ( QListBoxItem * )), this, SLOT(close()));
  connect(pListBox, SIGNAL( mouseButtonClicked ( int, QListBoxItem *, const QPoint & )), this, SLOT(onSelected()));
  connect(this, SIGNAL(aboutToShow()), this, SLOT(onFetchInfo()));

  insertItem( pListBox );
  insertItem( pTextLabel );
}

/** 
 *  Destroys the object and frees any allocated resources.
 */
UndoRedoDialog::~UndoRedoDialog()
{
  // no need to delete child widgets, Qt does it all for us
}

/** 
 *  This method fetches the undo / redo information from the 
 *  active document and shows it in the undo / redo dialog.
 */
void UndoRedoDialog::onFetchInfo() 
{
  pTextLabel->clear();
  pListBox->clear();

  std::vector<std::string> vecReUndos;
  Gui::Document* pcDoc = Application::Instance->activeDocument();

  if ( pcDoc )
  {
    if ( tMode == Undo ) 
      vecReUndos = pcDoc->getUndoVector();
    else
      vecReUndos = pcDoc->getRedoVector();

    for (std::vector<std::string>::iterator i=vecReUndos.begin(); i!=vecReUndos.end(); i++)
      pListBox->insertItem((*i).c_str());
    pTextLabel->setProperty( "text", tr( "Cancel" ) );
  }
  else if ( dynamic_cast<PythonEditView*>(getMainWindow()->activeWindow()) )
  {
    pTextLabel->setText( tr( "No info" ) );
  }
  else
  {
    pTextLabel->setText( tr( "No Undo" ) );
  }
}

/** Sets the number of actons to undo/redo. */
void UndoRedoDialog::onSelChangeUndoRedoStack() 
{
  int pos = pListBox->currentItem() + 1;
  QString text;
  if ( tMode == Undo )
    text = tr("Undoes %1 action(s)").arg(pos);
  else
    text = tr("Redoes %1 action(s)").arg(pos);
  pTextLabel->setText(text);
}

/** Switch to the mode \a tMode (either Undo or Redo). */
void UndoRedoDialog::setMode(TMode mode)
{
  tMode = mode;
}

/** Returns the current mode. */
UndoRedoDialog::TMode UndoRedoDialog::getMode() const
{
  return tMode;
}

/** Closes the dialog and emits the @ref clickedListBox() signal. */
void UndoRedoDialog::onSelected()
{
  close();
  emit clickedListBox();
}

#include "moc_DlgUndoRedo.cpp"

