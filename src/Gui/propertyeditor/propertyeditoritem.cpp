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


#include "PreCompiled.h"

#ifndef _PreComp_
# include <q3header.h>
# include <qpainter.h>
# include <qpushbutton.h>
#endif

#include "propertyeditoritem.h"
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/ViewProviderDocumentObject.h>

using namespace Gui::PropertyEditor;

TYPESYSTEM_SOURCE(Gui::PropertyEditor::PropertyItem, Base::BaseClass);

PropertyItem::PropertyItem() : parentItem(0), readonly(false)
{
}

PropertyItem::~PropertyItem()
{
    qDeleteAll(childItems);
}

void PropertyItem::reset()
{
    qDeleteAll(childItems);
    childItems.clear();
}

void PropertyItem::setPropertyData( const std::vector<App::Property*>& items)
{
    propertyItems = items;
}

const std::vector<App::Property*>& PropertyItem::getPropertyData() const
{
    return propertyItems;
}

void PropertyItem::setParent(PropertyItem* parent)
{
    parentItem = parent;
}

PropertyItem *PropertyItem::parent() const
{
    return parentItem;
}

void PropertyItem::appendChild(PropertyItem *item)
{
    childItems.append(item);
}

PropertyItem *PropertyItem::child(int row)
{
    return childItems.value(row);
}

int PropertyItem::childCount() const
{
    return childItems.count();
}

int PropertyItem::columnCount() const
{
    return 2;
}

void PropertyItem::setReadOnly(bool ro)
{
    readonly = ro;
}

bool PropertyItem::isReadOnly() const
{
    return readonly;
}

QVariant PropertyItem::toolTip(const App::Property* prop) const
{
    return QVariant(QString::fromUtf8(prop->getDocumentation()));
}

QVariant PropertyItem::decoration(const App::Property* /*prop*/) const
{
    return QVariant();
}

QVariant PropertyItem::toString(const App::Property* prop) const
{
    return value(prop);
}

QVariant PropertyItem::value(const App::Property* /*prop*/) const
{
    return QVariant();
}

void PropertyItem::setValue(const QVariant& /*value*/)
{
}

QString PropertyItem::pythonIdentifier(const App::Property* prop) const
{
    App::PropertyContainer* parent = prop->getContainer();
    if (parent->getTypeId() == App::Document::getClassTypeId()) {
        App::Document* doc = static_cast<App::Document*>(parent);
        QString docName = QString::fromAscii(App::GetApplication().getDocumentName(doc));
        QString propName = QString::fromAscii(parent->getName(prop));
        return QString::fromAscii("FreeCAD.getDocument(\"%1\").%2").arg(docName).arg(propName);
    }
    if (parent->getTypeId().isDerivedFrom(App::DocumentObject::getClassTypeId())) {
        App::DocumentObject* obj = static_cast<App::DocumentObject*>(parent);
        App::Document* doc = &(obj->getDocument());
        QString docName = QString::fromAscii(App::GetApplication().getDocumentName(doc));
        QString objName = QString::fromAscii(obj->getNameInDocument());
        QString propName = QString::fromAscii(parent->getName(prop));
        return QString::fromAscii("FreeCAD.getDocument(\"%1\").getObject(\"%2\").%3")
            .arg(docName).arg(objName).arg(propName);
    }
    if (parent->getTypeId().isDerivedFrom(Gui::ViewProviderDocumentObject::getClassTypeId())) {
        App::DocumentObject* obj = static_cast<Gui::ViewProviderDocumentObject*>(parent)->getObject();
        App::Document* doc = &(obj->getDocument());
        QString docName = QString::fromAscii(App::GetApplication().getDocumentName(doc));
        QString objName = QString::fromAscii(obj->getNameInDocument());
        QString propName = QString::fromAscii(parent->getName(prop));
        return QString::fromAscii("FreeCADGui.getDocument(\"%1\").getObject(\"%2\").%3")
            .arg(docName).arg(objName).arg(propName);
    }
    return QString();
}

QWidget* PropertyItem::createEditor(QWidget* /*parent*/, const QObject* /*receiver*/, const char* /*method*/) const
{
    return 0;
}

void PropertyItem::setEditorData(QWidget * /*editor*/, const QVariant& /*data*/) const
{
}

QVariant PropertyItem::editorData(QWidget * /*editor*/) const
{
    return QVariant();
}

QString PropertyItem::propertyName() const
{
    if (propName.isEmpty())
        return QLatin1String("<empty>");
    return propName;
}

void PropertyItem::setPropertyName(const QString& name)
{
    setObjectName(name);
    QString display;
    for (int i=0; i<name.length(); i++) {
        if (name[i].isUpper() && !display.isEmpty()) {
            display += QLatin1String(" ");
        }
        display += name[i];
    }

    propName = display;
}

void PropertyItem::setPropertyValue(const QString& value)
{
    for (std::vector<App::Property*>::const_iterator it = propertyItems.begin();
        it != propertyItems.end(); ++it) {
        QString cmd = pythonIdentifier(*it);
        cmd += QString::fromAscii(" = %1").arg(value);
        Gui::Application::Instance->runPythonCode((const char*)cmd.toAscii());
    }
}

QVariant PropertyItem::data(int column, int role) const
{
    // property name
    if (column == 0) {
        if (role == Qt::DisplayRole)
            return propertyName();
        // no properties set
        if (propertyItems.empty())
            return QVariant();
        else if (role == Qt::ToolTipRole)
            return toolTip(propertyItems[0]);
        else
            return QVariant();
    }
    else {
        // no properties set
        if (propertyItems.empty()) {
            PropertyItem* parent = this->parent();
            if (!parent || !parent->parent())
                return QVariant();
            if (role == Qt::EditRole)
                return parent->property(qPrintable(objectName()));
            else if (role == Qt::DisplayRole)
                return parent->property(qPrintable(objectName()));
            else
                return QVariant();
        }
        if (role == Qt::EditRole)
            return value(propertyItems[0]);
        else if (role == Qt::DecorationRole)
            return decoration(propertyItems[0]);
        else if (role == Qt::DisplayRole)
            return toString(propertyItems[0]);
        else if (role == Qt::ToolTipRole)
            return toolTip(propertyItems[0]);
        else
            return QVariant();
    }
}

bool PropertyItem::setData (const QVariant& value)
{
    if (propertyItems.empty()) {
        PropertyItem* parent = this->parent();
        if (!parent || !parent->parent())
            return false;
        parent->setProperty(qPrintable(objectName()),value);
        return true;
    }
    setValue(value);
    return true;
}

Qt::ItemFlags PropertyItem::flags(int column) const
{
    Qt::ItemFlags basicFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (column == 1 && !isReadOnly())
        return basicFlags | Qt::ItemIsEditable;
    else
        return basicFlags;
}

int PropertyItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<PropertyItem*>(this));

    return 0;
}

// --------------------------------------------------------------------

PropertyItemEditorFactory::PropertyItemEditorFactory()
{
}

PropertyItemEditorFactory::~PropertyItemEditorFactory()
{
}

QWidget * PropertyItemEditorFactory::createEditor (QVariant::Type /*type*/, QWidget * /*parent*/) const
{
    // do not allow to create any editor widgets because we do that in subclasses of PropertyItem
    return 0;
}

QByteArray PropertyItemEditorFactory::valuePropertyName (QVariant::Type /*type*/) const
{
    // do not allow to set properties because we do that in subclasses of PropertyItem
    return "";
}

// --------------------------------------------------------------------

PropertyItemDelegate::PropertyItemDelegate(QObject* parent)
    : QItemDelegate(parent)
{
}

PropertyItemDelegate::~PropertyItemDelegate()
{
}

void PropertyItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    QStyleOptionViewItem option = opt;

    PropertyItem *property = static_cast<PropertyItem*>(index.internalPointer());

    if (property && property->isSeparator()) {
        option.palette.setColor(QPalette::Text, option.palette.color(QPalette::BrightText));
        option.font.setBold(true);
        option.state &= ~QStyle::State_Selected;
    }

    if (index.column() == 1) {
        option.state &= ~QStyle::State_Selected;
    }

    option.state &= ~QStyle::State_HasFocus;

    if (property && property->isSeparator()) {
        QBrush bg = option.palette.dark();
        painter->fillRect(option.rect, bg);
    }

    QPen savedPen = painter->pen();

    QItemDelegate::paint(painter, option, index);

    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &option));
    painter->setPen(QPen(color));
    if (index.column() == 1 || !(property && property->isSeparator())) {
        int right = (option.direction == Qt::LeftToRight) ? option.rect.right() : option.rect.left();
        painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
    }
    painter->drawLine(option.rect.x(), option.rect.bottom(),
            option.rect.right(), option.rect.bottom());
    painter->setPen(savedPen);
}

QWidget * PropertyItemDelegate::createEditor (QWidget * parent, const QStyleOptionViewItem & /*option*/, 
                                              const QModelIndex & index ) const
{
    if (!index.isValid())
        return 0;

    PropertyItem *childItem = static_cast<PropertyItem*>(index.internalPointer());
    if (!childItem)
        return 0;
    return childItem->createEditor(parent, this, SLOT(valueChanged()));
}

void PropertyItemDelegate::valueChanged()
{
    QWidget* editor = qobject_cast<QWidget*>(sender());
    if (editor)
        commitData(editor);
}

void PropertyItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!index.isValid())
        return;
    QVariant data = index.data(Qt::EditRole);
    PropertyItem *childItem = static_cast<PropertyItem*>(index.internalPointer());
    editor->blockSignals(true);
    childItem->setEditorData(editor, data);
    editor->blockSignals(false);
    return;
}

void PropertyItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (!index.isValid())
        return;
    PropertyItem *childItem = static_cast<PropertyItem*>(index.internalPointer());
    QVariant data = childItem->editorData(editor);
    model->setData(index, data, Qt::EditRole);
}

#include "moc_propertyeditoritem.cpp"

