/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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


#ifndef GUI_CONTROL_H
#define GUI_CONTROL_H

// Std. configurations

#include <QObject>

namespace App
{
  class DocumentObject;
  class Document;
}

namespace Gui
{
namespace TaskView
{
    class TaskDialog;
}


/** The control class
 */
class GuiExport ControlSingelton : public QObject
{
     Q_OBJECT

public:
 
    static ControlSingelton& instance(void);
    static void destruct (void);

    void showDialog(Gui::TaskView::TaskDialog *dlg);


protected:
 
protected:
    /// Construction
    ControlSingelton();
    /// Destruction
    virtual ~ControlSingelton();

    static ControlSingelton* _pcSingleton;


};

/// Get the global instance
inline ControlSingelton& Control(void)
{
    return ControlSingelton::instance();
}

} //namespace Gui

#endif // GUI_CONTROL_H
