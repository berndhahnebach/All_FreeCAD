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
#endif

#include "PropertyPage.h"
#define new DEBUG_CLIENTBLOCK
using namespace Gui::Dialog;

/** Construction */
PropertyPage::PropertyPage() 
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
