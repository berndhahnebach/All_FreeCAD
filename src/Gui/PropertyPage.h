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


#ifndef __PROPERTY_PAGE__H__
#define __PROPERTY_PAGE__H__

#include "CustomWidgets.h"

namespace Gui {
namespace Dialog {

/** Base class for property pages.
 * \author Werner Mayer
 */
class GuiExport PropertyPage
{
protected:
  PropertyPage();
  virtual ~PropertyPage();

public:
  bool isModified();
  void setModified(bool b);
  void onApply();
  void onCancel();
  void onReset();

protected:
  virtual void apply();
  virtual void cancel();
  virtual void reset();

private:
  bool bChanged; /**< for internal use only */
};

} // namespace Dialog
} // namespace Gui

#endif // __PROPERTY_PAGE__H__
