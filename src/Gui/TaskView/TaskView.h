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


#ifndef TASKVIEW_H
#define TASKVIEW_H

#include <map>
#include <string>
#include <vector>

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include "iisTaskPanel/include/iisTaskPanel"

namespace App {
class Property;
}

namespace Gui {
namespace TaskView {

class TaskBox:public iisTaskBox
{
    Q_OBJECT

public:
    TaskBox(const QPixmap &icon, const QString &title, bool expandable, QWidget *parent);
    ~TaskBox();

};

class TaskView : public iisTaskPanel
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

#endif // TASKVIEW_H
