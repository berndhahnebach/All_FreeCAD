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


#ifndef PROPERTYEDITORINPUT_H
#define PROPERTYEDITORINPUT_H

#include "propertyeditoritem.h"


#ifndef __Qt4All__
# include <Gui/Qt4All.h>
#endif

namespace Gui {
namespace PropertyEditor {

/**
 * Change a string property.
 * \author Werner Mayer
 */
class GuiExport PropertyStringItem: public PropertyItem
{
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

protected:
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyStringItem();
};

/**
 * Dummy property to separate groups of properties.
 * \author Werner Mayer
 */
class GuiExport PropertySeparatorItem : public PropertyItem
{
    TYPESYSTEM_HEADER();

    bool isSeparator() const { return true; }
    QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
};

/**
 * Change a number.
 * \author Werner Mayer
 */
class GuiExport PropertyIntegerItem: public PropertyItem
{
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

protected:
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyIntegerItem();
};

/**
 * Change a number with constraints.
 * \author Werner Mayer
 */
class GuiExport PropertyIntegerConstraintItem: public PropertyItem
{
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

protected:
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyIntegerConstraintItem();
};

/**
 * Change a floating point number.
 * \author Werner Mayer
 */
class GuiExport PropertyFloatItem: public PropertyItem
{
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

protected:
    virtual QVariant toString(const App::Property*) const;
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyFloatItem();
};

/**
 * Change a floating point number with constraints.
 * \author Werner Mayer
 */
class GuiExport PropertyFloatConstraintItem: public PropertyItem
{
    TYPESYSTEM_HEADER();

    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

protected:
    virtual QVariant toString(const App::Property*) const;
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);

protected:
    PropertyFloatConstraintItem();
};

} // namespace PropertyEditor
} // namespace Gui

#endif
