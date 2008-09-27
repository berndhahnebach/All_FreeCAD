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


#ifndef PROPERTYEDITORLIST_H
#define PROPERTYEDITORLIST_H

#include "propertyeditoritem.h"
#include <Base/Vector3D.h>

Q_DECLARE_METATYPE(Base::Vector3f)

namespace Gui {
namespace PropertyEditor {

/**
 * Edit properties of boolean type. 
 * \author Werner Mayer
 */
class GuiExport PropertyBoolItem: public PropertyItem
{
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

protected:
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyBoolItem();
};

/**
 * Edit properties of vector type. 
 * \author Werner Mayer
 */
class PropertyFloatItem;
class GuiExport PropertyVectorItem: public PropertyItem
{
    Q_OBJECT
    Q_PROPERTY(float x READ x WRITE setX DESIGNABLE true USER true)
    Q_PROPERTY(float y READ y WRITE setY DESIGNABLE true USER true)
    Q_PROPERTY(float z READ z WRITE setZ DESIGNABLE true USER true)
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

    float x() const;
    void setX(float x);
    float y() const;
    void setY(float y);
    float z() const;
    void setZ(float z);

protected:
    virtual QVariant toString(const App::Property*) const;
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyVectorItem();

private:
    PropertyFloatItem* m_x;
    PropertyFloatItem* m_y;
    PropertyFloatItem* m_z;
};

/**
 * Edit properties of enum type. 
 * \author Werner Mayer
 */
class GuiExport PropertyEnumItem: public PropertyItem
{
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

protected:
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyEnumItem();
};

/**
 * Edit properties of enum type. 
 * \author Werner Mayer
 */
class GuiExport PropertyStringListItem: public PropertyItem
{
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

protected:
    QVariant toString(const App::Property* prop) const;
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyStringListItem();
};

} //namespace PropertyEditor
} //namespace Gui


#endif //PROPERTYEDITORLIST_H
