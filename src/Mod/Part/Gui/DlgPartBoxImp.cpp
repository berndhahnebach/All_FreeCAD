/***************************************************************************
 *   Copyright (c) Jürgen Riegel          (juergen.riegel@web.de) 2002     *
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
# include <cfloat>
# include <QMessageBox>
#endif

#include "DlgPartBoxImp.h"
#include <Gui/propertyeditor/PropertyItem.h>
#include <Gui/ui_InputVector.h>

using namespace PartGui;

/* 
 *  Constructs a DlgPartBox which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgPartBoxImp::DlgPartBoxImp(QWidget* parent, Qt::WFlags fl)
  : QDialog(parent, fl)
{
    this->setupUi(this);
    // Vector3f declared to use with QVariant see Gui/propertyeditor/PropertyItem.h
    this->direction->setItemData(0, QVariant::fromValue<Base::Vector3f>(Base::Vector3f(1,0,0)));
    this->direction->setItemData(1, QVariant::fromValue<Base::Vector3f>(Base::Vector3f(0,1,0)));
    this->direction->setItemData(2, QVariant::fromValue<Base::Vector3f>(Base::Vector3f(0,0,1)));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgPartBoxImp::~DlgPartBoxImp()
{
    // no need to delete child widgets, Qt does it all for us
}

Base::Vector3f DlgPartBoxImp::getDirection() const
{
    QVariant data = this->direction->itemData (this->direction->currentIndex());
    if (data.canConvert<Base::Vector3f>()) {
        return data.value<Base::Vector3f>();
    }
    else {
        return Base::Vector3f(0,0,1);
    }
}

void DlgPartBoxImp::on_direction_activated(int index)
{
    // last item is selected to define direction by user
    if (index+1 == this->direction->count()) {
        Gui::Dialog::Ui_InputVector iv;
        QDialog dlg(this);
        iv.setupUi(&dlg);
        if (dlg.exec()) {
            Base::Vector3f dir;
            dir.x = (float)iv.vectorX->value();
            dir.y = (float)iv.vectorY->value();
            dir.z = (float)iv.vectorZ->value();
            if (dir.Length() < FLT_EPSILON) {
                QMessageBox::critical(this, tr("Wrong direction"),
                    tr("Direction must not be the null vector"));
                return;
            }

            // check if the user-defined direction is already there
            for (int i=0; i<this->direction->count()-1; i++) {
                QVariant data = this->direction->itemData (i);
                if (data.canConvert<Base::Vector3f>()) {
                    const Base::Vector3f val = data.value<Base::Vector3f>();
                    if (val == dir) {
                        this->direction->setCurrentIndex(i);
                        return;
                    }
                }
            }

            // add a new item before the very last item
            QString display = QString::fromAscii("(%1,%2,%3)")
                .arg(dir.x)
                .arg(dir.y)
                .arg(dir.z);
            this->direction->insertItem(this->direction->count()-1, display,
                QVariant::fromValue<Base::Vector3f>(dir));
            this->direction->setCurrentIndex(this->direction->count()-2);
        }
    }
}

#include "moc_DlgPartBoxImp.cpp"
