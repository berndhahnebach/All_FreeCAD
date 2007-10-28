/***************************************************************************
 *   Copyright (c) 2004 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <boost/signals.hpp>
# include <boost/bind.hpp>
# include <qcursor.h>
# include <qlayout.h>
# include <qstatusbar.h>
# include <QContextMenuEvent>
# include <QPixmap>
#endif

#include <Base/Console.h>
#include <App/Document.h>
#include <App/Feature.h>
#include <App/DocumentObjectGroup.h>

#include "Tree.h"
#include "Document.h"
#include "BitmapFactory.h"
#include "ViewProviderFeature.h"
#include "MenuManager.h"
#include "Application.h"
#include "MainWindow.h"


using namespace Gui;

QPixmap* TreeDockWidget::documentPixmap = 0;

TreeDockWidget::TreeDockWidget(Gui::Document* pcDocument,QWidget *parent)
  : DockWindow(pcDocument,parent), fromOutside(false)
{
    setWindowTitle( tr("Tree view" ) );
    this->treeWidget = new TreeWidget(this);
    this->treeWidget->setRootIsDecorated(false);

    Gui::Selection().Attach(this);
    // Setup connections
    Application::Instance->signalNewDocument.connect(boost::bind(&TreeDockWidget::slotNewDocument, this, _1));
    Application::Instance->signalDeleteDocument.connect(boost::bind(&TreeDockWidget::slotDeleteDocument, this, _1));
    Application::Instance->signalRenameDocument.connect(boost::bind(&TreeDockWidget::slotRenameDocument, this, _1));
    Application::Instance->signalActiveDocument.connect(boost::bind(&TreeDockWidget::slotActiveDocument, this, _1));

    QGridLayout* pLayout = new QGridLayout(this); 
    pLayout->setSpacing(0);
    pLayout->setMargin (0);
    pLayout->addWidget( this->treeWidget, 0, 0 );

    QStringList labels;
    labels << tr("Labels & Attributes");
    this->treeWidget->setHeaderLabels(labels);

    // Add the first main label
    this->rootItem = new QTreeWidgetItem(this->treeWidget);
    this->rootItem->setText(0, tr("Application"));
    this->treeWidget->expandItem(this->rootItem);
    this->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    this->statusTimer = new QTimer(this);

    connect(this->statusTimer, SIGNAL(timeout()), 
            this, SLOT(onTestStatus()));
    connect(this->treeWidget, SIGNAL(itemEntered(QTreeWidgetItem*, int)),
            this, SLOT(onItemEntered(QTreeWidgetItem*)));
    connect(this->treeWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(onItemSelectionChanged()));

    this->statusTimer->setSingleShot(true);
    this->statusTimer->start(300);
    documentPixmap = new QPixmap(Gui::BitmapFactory().pixmap("Document"));
}

TreeDockWidget::~TreeDockWidget()
{
    Gui::Selection().Detach(this);
}

void TreeDockWidget::slotNewDocument(Gui::Document& Doc)
{
    DocumentItem* item = new DocumentItem(&Doc, this->rootItem);
    this->treeWidget->expandItem(item);
    item->setIcon(0, *documentPixmap);
    item->setText(0, QString(Doc.getDocument()->getName()));
    DocumentMap[ &Doc ] = item;
}

void TreeDockWidget::slotDeleteDocument(Gui::Document& Doc)
{
    std::map<Gui::Document*, DocumentItem*>::iterator it = DocumentMap.find(&Doc);
    if (it != DocumentMap.end()) {
        this->rootItem->takeChild(this->rootItem->indexOfChild(it->second));
        delete it->second;
        DocumentMap.erase(it);
    }
}

void TreeDockWidget::slotRenameDocument(Gui::Document& Doc)
{
    // do nothing here
}

void TreeDockWidget::slotActiveDocument(Gui::Document& Doc)
{
    std::map<Gui::Document*, DocumentItem*>::iterator jt = DocumentMap.find(&Doc);
    if (jt == DocumentMap.end())
        return; // signal is emitted before the item gets created
    for (std::map<Gui::Document*, DocumentItem*>::iterator it = DocumentMap.begin();
         it != DocumentMap.end(); ++it)
    {
        QFont f = it->second->font(0);
        f.setBold(it == jt);
        it->second->setFont(0,f);
    }
}

void TreeDockWidget::onTestStatus(void)
{
    if (isVisible()) {
        for (std::map<Gui::Document*,DocumentItem*>::iterator pos = DocumentMap.begin();pos!=DocumentMap.end();++pos) {
            pos->second->testStatus();
        }
    }

    this->statusTimer->setSingleShot(true);
    this->statusTimer->start(300);	
}

void TreeDockWidget::onItemEntered(QTreeWidgetItem * item)
{
    // object item selected
    if ( item && item->type() == QTreeWidgetItem::UserType + 1 ) {
        DocumentObjectItem* obj = static_cast<DocumentObjectItem*>(item);
        obj->displayStatusInfo();
    }
}

void TreeDockWidget::onItemSelectionChanged ()
{
    if (fromOutside)
        return;

    for (std::map<Gui::Document*,DocumentItem*>::iterator pos = DocumentMap.begin();pos!=DocumentMap.end();++pos) {
        pos->second->updateSelection();
    }
}

void TreeDockWidget::contextMenuEvent ( QContextMenuEvent * e )
{
    // ask workbenches and view provider, ...
    MenuItem* view = new MenuItem;
    Gui::Application::Instance->setupContextMenu("Tree", view);

    QMenu contextMenu;
    MenuManager::getInstance()->setupContextMenu(view, contextMenu);
    delete view;
    if (contextMenu.actions().count() > 0)
        contextMenu.exec(QCursor::pos());
}

void TreeDockWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        this->treeWidget->headerItem()->setText( 0, tr( "Labels & Attributes" ) );
    }

    DockWindow::changeEvent(e);
}

void TreeDockWidget::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,Gui::SelectionSingleton::MessageType Reason)
{
    fromOutside = true;
    if (Reason.Type != SelectionChanges::ClearSelection) {
        Gui::Document* pDoc = Application::Instance->getDocument( Reason.pDocName );
        std::map<Gui::Document*, DocumentItem*>::iterator it = DocumentMap.find( pDoc );

        if (it!= DocumentMap.end()) {
            if (Reason.Type == SelectionChanges::SetPreselect)
                it->second->setObjectHighlighted(Reason.pObjectName,true);
            else if (Reason.Type == SelectionChanges::RmvPreselect)
                it->second->setObjectHighlighted(Reason.pObjectName,false);
            else if (Reason.Type == SelectionChanges::AddSelection)
                it->second->setObjectSelected(Reason.pObjectName,true);
            else  if (Reason.Type == SelectionChanges::RmvSelection)
                it->second->setObjectSelected(Reason.pObjectName,false);
        }
    } else {
        // clears the complete selection
        if (strcmp(Reason.pDocName,"") == 0) {
            for (std::map<Gui::Document*,DocumentItem*>::iterator pos = DocumentMap.begin();pos!=DocumentMap.end();++pos) {
                pos->second->clearSelection();
            }

            this->treeWidget->clearSelection ();
        } else {
            // clears the selection of the given document
            Gui::Document* pDoc = Application::Instance->getDocument(Reason.pDocName);
            std::map<Gui::Document*, DocumentItem*>::iterator pos = DocumentMap.find(pDoc);
            if (pos != DocumentMap.end()) {
                pos->second->clearSelection();
            }
        }

        this->treeWidget->update();
    }

    fromOutside = false;
}

// ----------------------------------------------------------------------------

TreeWidget::TreeWidget(QWidget* parent)
    : QTreeWidget(parent)
{
}

TreeWidget::~TreeWidget()
{
}

// ----------------------------------------------------------------------------

DocumentItem::DocumentItem(Gui::Document* doc, QTreeWidgetItem * parent)
    : QTreeWidgetItem(parent), pDocument(doc)
{
    // Setup connections
    doc->signalNewObject.connect(boost::bind(&DocumentItem::slotNewObject, this, _1));
    doc->signalDeletedObject.connect(boost::bind(&DocumentItem::slotDeleteObject, this, _1));
    doc->signalChangedObject.connect(boost::bind(&DocumentItem::slotChangeObject, this, _1));
    doc->signalRenamedObject.connect(boost::bind(&DocumentItem::slotRenameObject, this, _1));
    doc->signalActivatedObject.connect(boost::bind(&DocumentItem::slotActiveObject, this, _1));
}

DocumentItem::~DocumentItem()
{
}

void DocumentItem::slotNewObject(Gui::ViewProviderDocumentObject& obj)
{
    std::string displayName = obj.getObject()->Label.getValue();
    std::string objectName = obj.getObject()->getNameInDocument();
    std::map<std::string, DocumentObjectItem*>::iterator it = ObjectMap.find(objectName);
    if (it == ObjectMap.end()) {
        DocumentObjectItem* item = new DocumentObjectItem(&obj, this);
        item->setIcon(0, obj.getIcon());
        item->setText(0, QString::fromUtf8(displayName.c_str()));
        ObjectMap[objectName] = item;
    } else {
        Base::Console().Warning("DocumentItem::slotNewObject: Cannot add view provider twice.\n");
    }
}

void DocumentItem::slotDeleteObject(Gui::ViewProviderDocumentObject& obj)
{
    std::string objectName = obj.getObject()->getNameInDocument();
    std::map<std::string, DocumentObjectItem*>::iterator it = ObjectMap.find(objectName);
    if (it != ObjectMap.end()) {
        // FIXME: We must check in App::Document::remObject() whether a group should be removed.
        // If so we must remove all its children first and then the group itself.
        QTreeWidgetItem* parent = it->second->parent();
        if (it->second->childCount() > 0) {
            QList<QTreeWidgetItem*> children = it->second->takeChildren();
            parent->addChildren(children);
        }

        parent->takeChild(parent->indexOfChild(it->second));
        delete it->second;
        ObjectMap.erase(it);
    }
}

void DocumentItem::slotChangeObject(Gui::ViewProviderDocumentObject& view)
{
    // As we immediately add a newly created object to the tree we check here which
    // item (this or a DocumentObjectItem) is the parent of the associated item of 'view'
    App::DocumentObject* obj = view.getObject();
    std::string objectName = obj->getNameInDocument();
    std::map<std::string, DocumentObjectItem*>::iterator it = ObjectMap.find(objectName);
    if (it != ObjectMap.end()) {

        // is the object a group?
        if (obj->getTypeId().isDerivedFrom(App::DocumentObjectGroup::getClassTypeId())) {
            std::set<QTreeWidgetItem*> children;
            std::vector<App::DocumentObject*> group = static_cast<App::DocumentObjectGroup*>(obj)->Group.getValues();
            for (std::vector<App::DocumentObject*>::iterator jt = group.begin(); jt != group.end(); ++jt) {
                const char* internalName = (*jt)->getNameInDocument();
                if (internalName) {
                    std::map<std::string, DocumentObjectItem*>::iterator kt = ObjectMap.find(internalName);
                    if (kt != ObjectMap.end()) {
                        children.insert(kt->second);
                        QTreeWidgetItem* parent = kt->second->parent();
                        if (parent && parent != it->second) {
                            int index = parent->indexOfChild(kt->second);
                            parent->takeChild(index);
                            it->second->addChild(kt->second);
                        }
                    }
                    else {
                        Base::Console().Warning("DocumentItem::slotChangedObject: Cannot reparent unknown object.\n");
                    }
                }
                else {
                    Base::Console().Warning("DocumentItem::slotChangedObject: Group references unknown object.\n");
                }
            }

            // move all children which are not part of the group anymore to this item
            int count = it->second->childCount();
            for (int i=0; i < count; i++) {
                QTreeWidgetItem* child = it->second->child(i);
                if (children.find(child) == children.end()) {
                    it->second->takeChild(i);
                    this->addChild(child);
                }
            }
            this->treeWidget()->expandItem(it->second);
        }

        // set the text label
        std::string displayName = obj->Label.getValue();
        it->second->setText(0, QString::fromUtf8(displayName.c_str()));
    } else {
        Base::Console().Warning("DocumentItem::slotChangedObject: Cannot change unknown object.\n");
    }
}

void DocumentItem::slotRenameObject(Gui::ViewProviderDocumentObject& obj)
{
    for (std::map<std::string,DocumentObjectItem*>::iterator it = ObjectMap.begin(); it != ObjectMap.end(); ++it) {
        if (it->second->object() == &obj) {
            DocumentObjectItem* item = it->second;
            ObjectMap.erase(it);
            std::string objectName = obj.getObject()->getNameInDocument();
            ObjectMap[objectName] = item;
            return;
        }
    }

    // no such object found
    Base::Console().Warning("DocumentItem::slotRenamedObject: Cannot rename unknown object.\n");
}

void DocumentItem::slotActiveObject(Gui::ViewProviderDocumentObject& obj)
{
    std::string objectName = obj.getObject()->getNameInDocument();
    std::map<std::string, DocumentObjectItem*>::iterator jt = ObjectMap.find(objectName);
    if (jt == ObjectMap.end())
        return; // signal is emitted before the item gets created
    for (std::map<std::string, DocumentObjectItem*>::iterator it = ObjectMap.begin();
         it != ObjectMap.end(); ++it)
    {
        QFont f = it->second->font(0);
        f.setBold(it == jt);
        it->second->setFont(0,f);
    }
}

void DocumentItem::testStatus(void)
{
    for (std::map<std::string,DocumentObjectItem*>::iterator pos = ObjectMap.begin();pos!=ObjectMap.end();++pos) {
        pos->second->testStatus();
    }
}

void DocumentItem::setObjectHighlighted(const char* name, bool select)
{
    std::map<std::string,DocumentObjectItem*>::iterator pos;
    pos = ObjectMap.find(name);
    if (pos != ObjectMap.end()) {
        //pos->second->setData(0, Qt::TextColorRole, QVariant(Qt::red));
        //treeWidget()->setItemSelected(pos->second, select);
    }
}

void DocumentItem::setObjectSelected(const char* name, bool select)
{
    std::map<std::string,DocumentObjectItem*>::iterator pos;
    pos = ObjectMap.find(name);
    if (pos != ObjectMap.end()) {
        treeWidget()->setItemSelected(pos->second, select);
    }
}

void DocumentItem::clearSelection(void)
{
    for (std::map<std::string,DocumentObjectItem*>::iterator pos = ObjectMap.begin();pos!=ObjectMap.end();++pos) {
        treeWidget()->setItemSelected(pos->second, false);
    }
}

void DocumentItem::updateSelection(void)
{
    for (std::map<std::string,DocumentObjectItem*>::iterator pos = ObjectMap.begin();pos!=ObjectMap.end();++pos)
    {
        bool selectedItem = treeWidget()->isItemSelected(pos->second);
        bool selectedObject = Gui::Selection().isSelected(pos->second->object()->getObject());
        if (selectedItem != selectedObject) {
            if (selectedItem)
                Gui::Selection().addSelection(pDocument->getDocument()->getName(), pos->first.c_str());
            else
                Gui::Selection().rmvSelection(pDocument->getDocument()->getName(), pos->first.c_str());
        }
    }
}

// ----------------------------------------------------------------------------

DocumentObjectItem::DocumentObjectItem(Gui::ViewProviderDocumentObject* pcViewProvider, QTreeWidgetItem* parent)
    : QTreeWidgetItem( parent ), previousStatus(-1), viewObject(pcViewProvider)
{
}

DocumentObjectItem::~DocumentObjectItem()
{
}

Gui::ViewProviderDocumentObject* DocumentObjectItem::object() const
{
    return viewObject;
}

void DocumentObjectItem::testStatus()
{
    App::DocumentObject* pObject = viewObject->getObject();

    // if status has changed then continue
    int currentStatus = (((int)(pObject->StatusBits.to_ulong())) << 1) | (viewObject->isShow()?1:0);
    if (previousStatus == currentStatus)
        return;
    previousStatus = currentStatus;

    QPixmap *px=0;
    if (pObject->StatusBits.test(0)) {
        // object has been touched
        //const char *feature_warning_xpm[]={
        //    "7 7 3 1",
        //    ". c None",
        //    "a c #000000",
        //    "# c #c8c800",
        //    "...#...",
        //    "..#a...",
        //    "..#a#..",
        //    ".##a#..",
        //    ".##a##.",
        //    "######.",
        //    "###a###"};
        //px = new QPixmap(feature_warning_xpm);
        const char *feature_recompute_xpm[]={
            "7 7 3 1",
            ". c None",
            "# c #ffffff",
            "a c #0000ff",
            "..#aa..",
            ".##aa#.",
            "###aa##",
            "###aa##",
            "#######",
            ".##aa#.",
            "..#aa.."};
        px = new QPixmap(feature_recompute_xpm);
        this->setTextColor(0, Qt::red);
    } else if (pObject->StatusBits.test(1)) {
        // object is in error state
        const char *feature_error_xpm[]={
            "7 7 3 1",
            ". c None",
            "# c #ff0000",
            "a c #ffffff",
            "..#aa..",
            ".##aa#.",
            "###aa##",
            "###aa##",
            "#######",
            ".##aa#.",
            "..#aa.."};
        px = new QPixmap(feature_error_xpm);
        this->setTextColor(0, Qt::red);
    } else if (pObject->StatusBits.test(2)) {
        // object is new
        this->setTextColor(0, Qt::blue);
    } else {
        // object is valid
    }

    if (!viewObject->isShow()) {
        this->setTextColor(0, Qt::gray);
        int w = QApplication::style()->pixelMetric(QStyle::PM_ListViewIconSize);
        QPixmap icon = viewObject->getIcon().pixmap(w,w);
        QPixmap hidden = BitmapFactory().disabled(icon);
        if (px) {
            this->setIcon(0, BitmapFactory().merge(hidden,*px,false));
        } else {
            this->setIcon(0, hidden);
        }
    } else { // visible
        this->setTextColor(0, QColor());
        int w = QApplication::style()->pixelMetric(QStyle::PM_ListViewIconSize);
        QPixmap icon = viewObject->getIcon().pixmap(w,w);
        if (px) {
            this->setIcon(0, BitmapFactory().merge(icon,*px,false));
        } else {
            this->setIcon(0, viewObject->getIcon());
        }
    }

    delete px;
}

void DocumentObjectItem::displayStatusInfo()
{
    App::DocumentObject* Obj = viewObject->getObject();

    if (Obj->getTypeId().isDerivedFrom(App::AbstractFeature::getClassTypeId())) {
        App::AbstractFeature *feat = dynamic_cast<App::AbstractFeature *>(Obj);
        QString info = feat->getStatusString();
        if ( feat->getStatus() == App::AbstractFeature::Error )
            info += QString(" (%1)").arg(feat->getErrorString());
        else if ( feat->mustExecute() )
            info += QString(" (but must be executed)");
        getMainWindow()->statusBar()->showMessage( info );
    }
}


#include "moc_Tree.cpp"

