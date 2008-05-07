/***************************************************************************
 *   Copyright (c) 2007 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef GUI_DOCKWINDOWMANAGER_H
#define GUI_DOCKWINDOWMANAGER_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

namespace Gui {

typedef QPair<QString, Qt::DockWidgetArea> DockWindowItem;

class GuiExport DockWindowItems
{
public:
    DockWindowItems();
    ~DockWindowItems();

    void addDockWidget(const QString& name, Qt::DockWidgetArea pos);
    const QList<DockWindowItem>& dockWidgets() const;

private:
    QList<DockWindowItem> _items;
};

/**
 * Class that manages the widgets inside a QDockWidget.
 * \author Werner Mayer
 */
class GuiExport DockWindowManager : QObject
{
    Q_OBJECT

public:
    /** Creates the only instance of the DockWindowManager. */
    static DockWindowManager* instance();
    static void destruct();

    bool registerDockWindow(const QString& name, QWidget* widget);
    void setup(DockWindowItems*);

    QDockWidget* addDockWindow(const QString& name, QWidget* widget,  
                 Qt::DockWidgetArea pos = Qt::AllDockWidgetAreas);
    QWidget* removeDockWindow(const QString& name);
    void removeDockWindow(QWidget* dock);
    QWidget* getDockWindow(const QString& name) const;
    QList<QWidget*> getDockWindows() const;

    void saveState();
    void retranslate();

private Q_SLOTS:
   /**
    * \internal
    */
    void onDockWidgetDestroyed(QObject*);
   /**
    * \internal
    */
    void onWidgetDestroyed(QObject*);

private:
    QDockWidget* findDockWidget(const QList<QDockWidget*>&, const QString&) const;
    
    DockWindowManager();
    ~DockWindowManager();
    static DockWindowManager* _instance;
    struct DockWindowManagerP* d;
};

} // namespace Gui

#endif // GUI_DOCKWINDOWMANAGER_H 
