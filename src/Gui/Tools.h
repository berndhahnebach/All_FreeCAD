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


#ifndef __FC_TOOLS_H__
#define __FC_TOOLS_H__

#ifndef _PreComp_
# include <qstring.h>
# include <qpixmap.h>
# include <qtoolbar.h>
#endif

/**
 * A helper class
 */
class FCTools
{
public:
  /** Resizes the area of a pixmap
   * If the size is greater than the old one the pixmap
   * will be placed in the center. The border area will be filled
   * up with current style's base color. 
   */
  static QPixmap fillUp(int w, int h, QPixmap p);

  /** Creates an opaque area in a pixmap
   * This is useful for drawing a pixmap into 
   * another pixmap. But therefore this area must be
   * opaque in every pixel, otherwise it is not 
   * possible to draw into.
   */
  static QPixmap fillOpaqueRect(int x, int y, int w, int h, QPixmap p);

  /** Creates a transparent area in a pixmap
   * This is useful for resizing a pixmap. The
   * new space of the pixmap can be made transparent
   * by making transparent the whole area and by making 
   * opaque the area of the original pixmap.
   */
  static QPixmap fillTransparentRect(int x, int y, int w, int h, QPixmap p);

  /** Does the same as @ref fillUp() but does not move the pixmap
   * to the center.
   */
  static QPixmap resize(int w, int h, QPixmap p);

  /** Clears only the tool buttons and the separators of a toolbar */
  static void clearToolButtons(QToolBar* tb);

  /** Returns whether the given url is a:
   * 0: an existing file
   * 1: a non-existing file but valid file name
   * 2: something else
   */
  static int getURLType(const QString& url);

  static QString i18n(const QString& s);
};

template <class T>
const T& FCmax (const T& a, const T& b)
{
  return (a > b ? a : b);
}

template <class T>
const T& FCmin (const T& a, const T& b)
{
  return (a < b ? a : b);
}

#endif // __FC_TOOLS_H__
