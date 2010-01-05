/***************************************************************************
 *   Copyright (c) 2010 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <QApplication>
# include <boost/signals.hpp>
# include <boost/bind.hpp>
#endif

#include "DocumentModel.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "Document.h"
#include "ViewProviderDocumentObject.h"
#include <App/Document.h>
#include <App/DocumentObject.h>

using namespace Gui;


QIcon* DocumentModel::documentIcon = 0;

DocumentModel::DocumentModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    // Setup connections
    Application::Instance->signalNewDocument.connect(boost::bind(&DocumentModel::slotNewDocument, this, _1));
    Application::Instance->signalDeleteDocument.connect(boost::bind(&DocumentModel::slotDeleteDocument, this, _1));
    Application::Instance->signalRenameDocument.connect(boost::bind(&DocumentModel::slotRenameDocument, this, _1));
    Application::Instance->signalActiveDocument.connect(boost::bind(&DocumentModel::slotActiveDocument, this, _1));
    Application::Instance->signalRelabelDocument.connect(boost::bind(&DocumentModel::slotRelabelDocument, this, _1));
    if (!documentIcon)
        documentIcon = new QIcon(Gui::BitmapFactory().pixmap("Document"));
}

DocumentModel::~DocumentModel()
{
}

void DocumentModel::slotNewDocument(const Gui::Document& Doc)
{
    Doc.signalNewObject.connect(boost::bind(&DocumentModel::slotNewObject, this, _1));
    Doc.signalDeletedObject.connect(boost::bind(&DocumentModel::slotDeleteObject, this, _1));
    Doc.signalChangedObject.connect(boost::bind(&DocumentModel::slotChangeObject, this, _1));
    Doc.signalRenamedObject.connect(boost::bind(&DocumentModel::slotRenameObject, this, _1));
    Doc.signalActivatedObject.connect(boost::bind(&DocumentModel::slotActiveObject, this, _1));
    Doc.signalInEdit.connect(boost::bind(&DocumentModel::slotInEdit, this, _1));
    Doc.signalResetEdit.connect(boost::bind(&DocumentModel::slotResetEdit, this, _1));

    QModelIndex parent = createIndex(0,0,Application::Instance);
    int count_docs = (int)this->docs.size();
    beginInsertRows(parent, count_docs, count_docs+1);
    this->docs.push_back(&Doc);
    endInsertRows();
}

void DocumentModel::slotDeleteDocument(const Gui::Document& Doc)
{
}

void DocumentModel::slotRenameDocument(const Gui::Document& Doc)
{
    // do nothing here
}

void DocumentModel::slotRelabelDocument(const Gui::Document& Doc)
{
}

void DocumentModel::slotActiveDocument(const Gui::Document& Doc)
{
}

void DocumentModel::slotInEdit(const Gui::ViewProviderDocumentObject& v)
{
}

void DocumentModel::slotResetEdit(const Gui::ViewProviderDocumentObject& v)
{
}

void DocumentModel::slotNewObject(const Gui::ViewProviderDocumentObject& obj)
{
}

void DocumentModel::slotDeleteObject(const Gui::ViewProviderDocumentObject& obj)
{
}

void DocumentModel::slotChangeObject(const Gui::ViewProviderDocumentObject& view)
{
}

void DocumentModel::slotRenameObject(const Gui::ViewProviderDocumentObject& obj)
{
}

void DocumentModel::slotActiveObject(const Gui::ViewProviderDocumentObject& obj)
{
}

int DocumentModel::columnCount (const QModelIndex & /*parent*/) const
{
    return 1;
}

QVariant DocumentModel::data (const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Qt::DecorationRole) {
        // the root item
        if (index.internalPointer() == Application::Instance) {
            return qApp->windowIcon();
        }
        Base::BaseClass* item = 0;
        item = static_cast<Base::BaseClass*>(index.internalPointer());
        if (item->getTypeId() == Document::getClassTypeId()) {
            return *documentIcon;
        }
    }
    else if (role == Qt::DisplayRole) {
        // the root item
        if (index.internalPointer() == Application::Instance)
            return tr("Application");
        Base::BaseClass* item = 0;
        item = static_cast<Base::BaseClass*>(index.internalPointer());
        if (item->getTypeId() == Document::getClassTypeId()) {
            App::Document* doc = static_cast<Document*>(item)->getDocument();
            return QString::fromUtf8(doc->Label.getValue());
        }
        else if (item->getTypeId().isDerivedFrom(Gui::ViewProviderDocumentObject::getClassTypeId())) {
            App::DocumentObject* obj = static_cast<Gui::ViewProviderDocumentObject*>(item)->getObject();
            return QString::fromUtf8(obj->Label.getValue());
        }
    }

    return QVariant();
}

bool DocumentModel::setData(const QModelIndex& index, const QVariant & value, int role)
{
    if (!index.isValid())
        return false;

    if (role == Qt::EditRole) {
        return true;
    }

    return true;
}

Qt::ItemFlags DocumentModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

QModelIndex DocumentModel::index (int row, int column, const QModelIndex & parent) const
{
    if (!parent.isValid())
        return createIndex(row, column, Application::Instance);
    if (parent.internalPointer() == Application::Instance) {
        if (row >= (int)this->docs.size())
            return QModelIndex();
        Gui::Document* doc = const_cast<Gui::Document*>(this->docs[row]);
        return createIndex(row, column, doc);
    }

    Base::BaseClass* item = 0;
    item = static_cast<Base::BaseClass*>(parent.internalPointer());
    if (item->getTypeId() == Document::getClassTypeId()) {
        App::Document* doc = static_cast<Document*>(item)->getDocument();
        std::vector<App::DocumentObject*> objs = doc->getObjects();
        if (objs.empty()) return QModelIndex();
        App::DocumentObject* obj = objs[row];
        return createIndex(row, column, obj);
    }

    return QModelIndex();
}

QModelIndex DocumentModel::parent (const QModelIndex & index) const
{
    if (!index.isValid())
        return QModelIndex();
    if (index.internalPointer() == Application::Instance)
        return QModelIndex();
    Base::BaseClass* item = 0;
    item = static_cast<Base::BaseClass*>(index.internalPointer());
    if (item->getTypeId() == Document::getClassTypeId()) {
        return createIndex(0, 0, Application::Instance);
    }

    return QModelIndex();
}

int DocumentModel::rowCount (const QModelIndex & parent) const
{
    if (!parent.isValid())
        return 1; // the root item
    if (parent.internalPointer() == Application::Instance)
        return (int)this->docs.size();

    Base::BaseClass* item = 0;
    item = static_cast<Base::BaseClass*>(parent.internalPointer());
    if (item->getTypeId() == Document::getClassTypeId()) {
        App::Document* doc = static_cast<Document*>(item)->getDocument();
        std::vector<App::DocumentObject*> objs = doc->getObjects();
        return (int)objs.size();
    }

    return 0;
}

QVariant DocumentModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role != Qt::DisplayRole)
            return QVariant();
        return tr("Labels & Attributes");
    }

    return QVariant();
}

bool DocumentModel::setHeaderData (int, Qt::Orientation, const QVariant &, int)
{
    return false;
}
