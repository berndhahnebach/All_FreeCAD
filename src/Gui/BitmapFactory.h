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


#ifndef GUI_BITMAPFACTORY_H
#define GUI_BITMAPFACTORY_H

#include <Base/Factory.h>

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

namespace Gui {

/** The Bitmap Factory
  * the main purpose is to collect all build in Bitmaps and
  * hold all paths for the extern bitmaps (files) to serve
  * as a single point of accessing bitmaps in FreeCAD
  * \author Werner Mayer, Jürgen Riegel
  */
class BitmapFactoryInstP;
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
    /// Adds a build in XPM pixmap under a given name
    void addPixmapToCache(const QString& name, const QPixmap& px);
    /// Checks whether the pixmap is already registered.
    bool findPixmapInCache(const QString& name, QPixmap& px) const;
    /// Retrieves a pixmap by name
    QPixmap pixmap(const char* sName) const;
    /** Retrieves a pixmap by name
    * specifying also the name and possition of a smaller pixmap.
    * The the smaller pixmap is drawn into the bigger pixmap.
    */
    QPixmap pixmap(const char* sName, const char* sMask, Position pos = BitmapFactoryInst::BottomLeft) const;
    /** Returns the names of all registered pixmaps.
    * To get the appropriate pixmaps call pixmap() for each name.
    */
    QStringList pixmapNames() const;

private:
    static BitmapFactoryInst* _pcSingleton;
    BitmapFactoryInst();
    ~BitmapFactoryInst();

    BitmapFactoryInstP* d;
};

/// Get the global instance
inline GuiExport BitmapFactoryInst& BitmapFactory(void)
{
    return BitmapFactoryInst::instance();
}

} // namespace Gui

#endif // GUI_BITMAPFACTORY_H
