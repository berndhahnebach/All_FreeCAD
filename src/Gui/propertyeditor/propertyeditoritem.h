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


#ifndef PROPERTYEDITORITEM_H
#define PROPERTYEDITORITEM_H

#ifndef __Qt4All__
# include <Gui/Qt4All.h>
#endif

#include <vector>

#include <Base/Type.h>
#include <App/PropertyStandard.h>

namespace Gui {
namespace PropertyEditor {

class GuiExport PropertyItem : public Base::BaseClass
{
    TYPESYSTEM_HEADER();

public:
    ~PropertyItem();

    /** Sets the current property objects. */
    void setProperty( const std::vector<App::Property*>& );
    const std::vector<App::Property*>& getProperty() const;

    /** Creates the appropriate editor for this item and sets the editor to the value of overrideValue(). */
    virtual QWidget* createEditor(QWidget* parent, const QObject* receiver, const char* method) const;
    virtual void setEditorData(QWidget *editor, const QVariant& data) const;
    virtual QVariant editorData(QWidget *editor) const;

    void setParent(PropertyItem* parent);
    PropertyItem *parent() const;
    void appendChild(PropertyItem *child);

    void setReadOnly(bool);
    bool isReadOnly() const;

    PropertyItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QString propertyName() const;
    void setPropertyName(const QString&);
    QVariant data(int column, int role) const;
    bool setData (const QVariant& value);
    Qt::ItemFlags flags(int column) const;
    int row() const;
    void reset();

protected:
    PropertyItem();

    virtual QVariant decoration(const App::Property*) const;
    virtual QVariant toolTip(const App::Property*) const;
    virtual QVariant toString(const App::Property*) const;
    virtual QVariant value(const App::Property*) const;
    virtual void setValue(const QVariant&);
    QString pythonIdentifier(const App::Property*) const;

private:
    QString propName;
    QVariant propData;
    std::vector<App::Property*> propertyItems;
    PropertyItem *parentItem;
    QList<PropertyItem*> childItems;
    bool readonly;
};

class PropertyItemEditorFactory : public QItemEditorFactory
{
public:
    PropertyItemEditorFactory();
    virtual ~PropertyItemEditorFactory();

    virtual QWidget * createEditor ( QVariant::Type type, QWidget * parent ) const;
    virtual QByteArray valuePropertyName ( QVariant::Type type ) const;
};

class PropertyItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    PropertyItemDelegate(QObject* parent);
    ~PropertyItemDelegate();

    virtual QWidget * createEditor (QWidget *, const QStyleOptionViewItem&, const QModelIndex&) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData (QWidget *editor, QAbstractItemModel *model, const QModelIndex& index ) const;

public Q_SLOTS:
    void valueChanged();
};

} // namespace PropertyEditor
} // namespace Gui

#endif // PROPERTYEDITORITEM_H
