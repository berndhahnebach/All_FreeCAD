/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef __BITMAP_FACTORY_H__
#define __BITMAP_FACTORY_H__

#include "../Base/Factory.h"

#ifndef _PreComp_
# include <qmap.h>
# include <qpixmap.h>
# include <qstringlist.h>
#endif

namespace Gui {

/** The Bitmap Factory
  * the main purpose is to collect all build in Bitmaps and
  * hold all paths for the extern bitmaps (files) to serve
  * as a single point of accessing bitmaps in FreeCAD
  * \author Werner Mayer, Jürgen Riegel
  */
class GuiExport BitmapFactoryInst : public Base::Factory
{
public:
  enum Position
  {
    TopLeft,  /**< Place to the top left corner */
    TopRight, /**< Place to the top right corner */
    BottomLeft, /**< Place to the bottom left corner */
    BottomRight /**< Place to the bottom right corner */
  };

  static BitmapFactoryInst& instance(void);
  static void destruct (void);

  /// Adds a path where pixmaps can be found
  void addPath(const char* sPath);
  /// Removes a path from the list of pixmap paths
  void removePath(const char* sPath);
  /// Adds a build in XPM pixmap under a given name
  void addXPM(const char* sName, const char** pXPM);
  /// Adds a build in XPM pixmap under a given file format
  void addFileFormat(const char* sFormat, const char** pXPM);
  /// Removes a build in pixmap by a given name
  void removeXPM(const char* sName);
  /// Retrieves a pixmap by file format
  QPixmap* fileFormat(const char* sFormat);
  /// Retrieves a pixmap by name
  QPixmap pixmap(const char* sName);
  /** Retrieves a pixmap by name
   * specifying also the name and possition of a smaller pixmap.
   * The the smaller pixmap is drawn into the bigger pixmap.
   */
  QPixmap pixmap(const char* sName, const char* sMask, Position pos = BottomLeft);
  /** Returns the names of all registered pixmaps.
   * To get the appropriate pixmaps call pixmap() for each name.
   */
  QStringList pixmapNames() const;

private:
  static BitmapFactoryInst* _pcSingleton;

  BitmapFactoryInst(){}
  ~BitmapFactoryInst(){}

  QMap<QString,const char**> _mpXPM;
  QStringList _vsPaths;
};

/// Get the global instance
inline GuiExport BitmapFactoryInst& BitmapFactory(void)
{
  return BitmapFactoryInst::instance();
}

} // namespace Gui

#endif // __BITMAP_FACTORY_H__
