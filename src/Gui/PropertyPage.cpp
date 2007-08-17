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

#include "PropertyPage.h"

using namespace Gui::Dialog;

/** Construction */
PropertyPage::PropertyPage(QWidget* parent) : QWidget(parent) 
{
  bChanged = false;
}

/** Destruction */
PropertyPage::~PropertyPage() 
{
}

/** Applies all changes. Reimplement this in your subclasses. */
void PropertyPage::apply()
{
}

/** Discards all changes. Reimplement this in your subclasses. */
void PropertyPage::cancel()
{
}

/** Resets to the default values. Reimplement this in your subclasses. */
void PropertyPage::reset()
{
}

/** Returns whether the page was modified or not. */
bool PropertyPage::isModified()
{
  return bChanged;
}

/** Sets the page to be modified. */
void PropertyPage::setModified(bool b)
{
  bChanged = b;
}

/** Applies all changes calling @ref apply() and resets the modified state. */
void PropertyPage::onApply()
{
  if (isModified())
    apply();

  setModified(false);
}

/** Discards all changes calling @ref cancel() and resets the modified state. */
void PropertyPage::onCancel()
{
  if (isModified())
  {
    cancel();
    setModified(false);
  }
}

/** Resets to the default values. */
void PropertyPage::onReset()
{
  reset();
}

// ----------------------------------------------------------------

/** Construction */
PreferencePage::PreferencePage(QWidget* parent) : QWidget(parent) 
{
}

/** Destruction */
PreferencePage::~PreferencePage() 
{
}

// ----------------------------------------------------------------

/** Construction */
CustomizeActionPage::CustomizeActionPage(QWidget* parent) : QWidget(parent) 
{
}

/** Destruction */
CustomizeActionPage::~CustomizeActionPage() 
{
}

bool CustomizeActionPage::event(QEvent* e)
{
  bool ok = QWidget::event(e);

  if (e->type() == QEvent::ParentChange || e->type() == QEvent::ParentAboutToChange)
  {
    QWidget* topLevel = this->parentWidget();
    while (topLevel && !topLevel->inherits("QDialog"))
      topLevel = topLevel->parentWidget();
    if ( topLevel )
    {
      int index = topLevel->metaObject()->indexOfSignal( QMetaObject::normalizedSignature("addMacroAction(const QByteArray&)") );
      if ( index >= 0 ) {
        if ( e->type() == QEvent::ParentChange ) {
          connect(topLevel, SIGNAL(addMacroAction( const QByteArray& )), this, SLOT(onAddMacroAction( const QByteArray& )));
          connect(topLevel, SIGNAL(removeMacroAction( const QByteArray& )), this, SLOT(onRemoveMacroAction( const QByteArray& )));
          connect(topLevel, SIGNAL(modifyMacroAction( const QByteArray& )), this, SLOT(onModifyMacroAction( const QByteArray& )));
        } else {
          disconnect(topLevel, SIGNAL(addMacroAction( const QByteArray& )), this, SLOT(onAddMacroAction( const QByteArray& )));
          disconnect(topLevel, SIGNAL(removeMacroAction( const QByteArray& )), this, SLOT(onRemoveMacroAction( const QByteArray& )));
          disconnect(topLevel, SIGNAL(modifyMacroAction( const QByteArray& )), this, SLOT(onModifyMacroAction( const QByteArray& )));
        }
      }
    }
  }

  return ok;
}

#include "moc_PropertyPage.cpp"
