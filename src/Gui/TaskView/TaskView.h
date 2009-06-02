/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
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


#ifndef GUI_TASKVIEW_TASKVIEW_H
#define GUI_TASKVIEW_TASKVIEW_H

#include <map>
#include <string>
#include <vector>

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include <Gui/iisTaskPanel/include/iisTaskPanel>

namespace App {
class Property;
}

namespace Gui {
namespace TaskView {

/// Father class of all content in TaskView
class GuiExport TaskContent 
{

public:
    //TaskContent();
    //~TaskContent();
};

/// Father class of content with header and Icon
class GuiExport TaskBox : public iisTaskBox, public TaskContent
{
    Q_OBJECT

public:
    TaskBox(const QPixmap &icon, const QString &title, bool expandable, QWidget *parent);
    ~TaskBox();
};

/// Father class of content of a Free widget (without header and Icon), shut be a exception!
class GuiExport TaskWidget : public QWidget, public TaskContent
{
    Q_OBJECT

public:
    TaskWidget(QWidget *parent=0);
    ~TaskWidget();
};

/** TaskView class
  * handles the FreeCAD task view panel. Keeps track of the inserted content elements.
  * This elements get injected mostly by the ViewProvider classes of the selected
  * DocumentObjects. 
  */
class GuiExport TaskView : public iisTaskPanel
{
    Q_OBJECT

public:
    TaskView(QWidget *parent = 0);
    ~TaskView();

protected:

private:
  
};

} //namespace TaskView
} //namespace Gui

#endif // GUI_TASKVIEW_TASKVIEW_H
