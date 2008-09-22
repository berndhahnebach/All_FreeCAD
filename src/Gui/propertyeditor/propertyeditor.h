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


#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <map>
#include <string>
#include <vector>

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include "propertyeditoritem.h"

namespace App {
class Property;
}

namespace Gui {
namespace PropertyEditor {

class PropertyModel;
class PropertyEditor : public QTreeView
{
    Q_OBJECT

public:
    PropertyEditor(QWidget *parent = 0);
    ~PropertyEditor();

    /** Builds up the list view with the properties. */
    void buildUp(const std::map<std::pair<std::string, int>, std::vector<App::Property*> >& props, size_t ct);

protected:
    virtual void currentChanged (const QModelIndex & current, const QModelIndex & previous);
    virtual void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;
    virtual QStyleOptionViewItem viewOptions() const;

private:
    PropertyModel* propertyModel;
};

class PropertyModel : public QAbstractItemModel
{
public:
    PropertyModel(QObject* parent);
    virtual ~PropertyModel();

    QModelIndex buddy ( const QModelIndex & index ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData ( const QModelIndex & idx, const QVariant & value, int role );
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    QModelIndex parent ( const QModelIndex & index ) const;
    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    bool setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole );
    void buildUp( const std::map<std::pair<std::string, int>, std::vector<App::Property*> >& props, size_t ct );

private:
    PropertyItem *rootItem;
};

} //namespace PropertyEditor
} //namespace Gui

#endif // PROPERTYEDITOR_H
