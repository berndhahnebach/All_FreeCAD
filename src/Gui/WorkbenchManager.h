/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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


#ifndef GUI_WORKBENCHMANAGER_H
#define GUI_WORKBENCHMANAGER_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

namespace Gui {

class Workbench;

/**
 * The WorkbenchManager class manages all workbench objects.
 * @author Werner Mayer
 */
class GuiExport WorkbenchManager  
{
public:
    /** Creates the only instance of the WorkbenchManager. */
    static WorkbenchManager* instance();
    static void destruct();

    /** Searches for and returns an existing workbench object with name \a name. If no
     * such workbench exists then a workbench of class \a className gets created, if possible.
     * If the workbench cannot be created 0 is returned.
     */
    Workbench* createWorkbench ( const QString& name, const QString& className );
    /** Returns an instance of the workbench with name \a name. If there is
     * no such workbench 0 is returned. 
     */
    Workbench* getWorkbench ( const QString& name );
    /** Activates the workbench with name \a name. */
    bool activate( const QString& name, const QString& className );
    /** Returns the active workbench. */
    Workbench* active() const;
    /** Returns a list of all created workbench objects. */
    QStringList workbenches() const;

protected:
    WorkbenchManager();
    ~WorkbenchManager();

private:
    static WorkbenchManager* _instance;
    Workbench*  _activeWorkbench;
    QMap<QString, Workbench*> _workbenches;
};

} // namespace Gui


#endif // GUI_WORKBENCHMANAGER_H 
