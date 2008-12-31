/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer@users.sourceforge.net>        *
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

#ifndef GUI_INPUTVECTOR_H
#define GUI_INPUTVECTOR_H

#include <cfloat>
#include <QDialog>
#include <QMessageBox>

#include <Gui/propertyeditor/PropertyItem.h>

namespace Gui {

/** This is the base dialog class that defines the interface for
 * specifying a direction vector by the user.
 * @author Werner Mayer
 */
class GuiExport LocationDialog : public QDialog
{
    Q_OBJECT

protected:
    LocationDialog(QWidget* parent = 0, Qt::WFlags fl = 0);
    virtual ~LocationDialog();

private Q_SLOTS:
    void on_direction_activated(int);

public:
    Base::Vector3f getUserDirection(bool* ok=0) const;

private:
    virtual void directionActivated(int) = 0;
};

/** This is the template class that implements the interface of LocationDialog.
 * The template argument can be the Ui interface class built by uic out of a
 * .ui file.
 * This class might be very useful for dialogs where a combo box is used to
 * define a direction vector by the user. For such classes the programmer don't
 * to write a subclass to implement the appropriate singals/slots. Instead it's
 * possible to omit this further class and use LocationInterface parametrized
 * with the generated Ui class.
 * @author Werner Mayer
 */
template <class Ui>
class LocationInterface : public LocationDialog, public Ui
{
public:
    LocationInterface(QWidget* parent = 0, Qt::WFlags fl = 0)  : LocationDialog(parent, fl)
    {
        this->setupUi(this);
        // Vector3f declared to use with QVariant see Gui/propertyeditor/PropertyItem.h
        this->direction->setItemData(0, QVariant::fromValue<Base::Vector3f>(Base::Vector3f(1,0,0)));
        this->direction->setItemData(1, QVariant::fromValue<Base::Vector3f>(Base::Vector3f(0,1,0)));
        this->direction->setItemData(2, QVariant::fromValue<Base::Vector3f>(Base::Vector3f(0,0,1)));
    }
    virtual ~LocationInterface(){}

    Base::Vector3f getPosition() const
    {
        return Base::Vector3f((float)xPos->value(),
                              (float)yPos->value(),
                              (float)zPos->value());
    }

    Base::Vector3f getDirection() const
    {
        QVariant data = this->direction->itemData (this->direction->currentIndex());
        if (data.canConvert<Base::Vector3f>()) {
            return data.value<Base::Vector3f>();
        }
        else {
            return Base::Vector3f(0,0,1);
        }
    }

private:
    void directionActivated(int index)
    {
        // last item is selected to define direction by user
        if (index+1 == this->direction->count()) {
            bool ok;
            Base::Vector3f dir = this->getUserDirection(&ok);
            if (ok) {
                if (dir.Length() < FLT_EPSILON) {
                    QMessageBox::critical(this, LocationDialog::tr("Wrong direction"),
                        LocationDialog::tr("Direction must not be the null vector"));
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
};

/** This template class does basically the same as LocationInterface unless
 * that the Ui class is used as composition not as further base class.
 * @author Werner Mayer
 */
template <class Ui>
class LocationInterfaceComp : public Ui
{
public:
    LocationInterfaceComp()
    {
    }
    ~LocationInterfaceComp()
    {
    }

    void setupUi(QDialog *dlg)
    {
        Ui::setupUi(dlg);
        // Vector3f declared to use with QVariant see Gui/propertyeditor/PropertyItem.h
        this->direction->setItemData(0, QVariant::fromValue<Base::Vector3f>(Base::Vector3f(1,0,0)));
        this->direction->setItemData(1, QVariant::fromValue<Base::Vector3f>(Base::Vector3f(0,1,0)));
        this->direction->setItemData(2, QVariant::fromValue<Base::Vector3f>(Base::Vector3f(0,0,1)));
    }

    Base::Vector3f getPosition() const
    {
        return Base::Vector3f((float)xPos->value(),
                              (float)yPos->value(),
                              (float)zPos->value());
    }

    Base::Vector3f getDirection() const
    {
        QVariant data = this->direction->itemData (this->direction->currentIndex());
        if (data.canConvert<Base::Vector3f>()) {
            return data.value<Base::Vector3f>();
        }
        else {
            return Base::Vector3f(0,0,1);
        }
    }

public:
    void directionActivated(LocationDialog* dlg, int index)
    {
        // last item is selected to define direction by user
        if (index+1 == this->direction->count()) {
            bool ok;
            Base::Vector3f dir = dlg->getUserDirection(&ok);
            if (ok) {
                if (dir.Length() < FLT_EPSILON) {
                    QMessageBox::critical(dlg, LocationDialog::tr("Wrong direction"),
                        LocationDialog::tr("Direction must not be the null vector"));
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
};

/** This template class is the parent dialog using LocationInterfaceComp and
 * implements the pure virtual method directionActivated().
 * @author Werner Mayer
 */
template <class Ui>
class LocationDialogComp : public LocationDialog
{
public:
    LocationDialogComp(QWidget* parent = 0, Qt::WFlags fl = 0)
      : LocationDialog(parent, fl)
    {
        ui.setupUi(this);
    }
    virtual ~LocationDialogComp()
    {
        // no need to delete child widgets, Qt does it all for us
    }

    Base::Vector3f getDirection() const
    {
        return ui.getDirection();
    }

private:
    void directionActivated(int index)
    {
        ui.directionActivated(this,index);
    }

protected:
    LocationInterfaceComp<Ui> ui;
};

} // namespace Gui

#endif // GUI_INPUTVECTOR_H
