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
#include "ViewProviderDocumentObject.h"
#include "MenuManager.h"
#include "Application.h"
#include "MainWindow.h"


using namespace Gui;

QPixmap* TreeDockWidget::documentPixmap = 0;
const int TreeWidget::DocumentType = 1000;
const int TreeWidget::ObjectType = 1001;

TreeWidget::TreeWidget(QWidget* parent)
    : QTreeWidget(parent)
{
    this->setDragEnabled(true);
    this->setAcceptDrops(true);
    this->setDropIndicatorShown(false);

    this->createGroupAction = new QAction(this);
    this->createGroupAction->setText(tr("Create group..."));
    this->createGroupAction->setStatusTip(tr("Create a group"));
    connect(this->createGroupAction, SIGNAL(triggered()),
            this, SLOT(onCreateGroup()));
}

TreeWidget::~TreeWidget()
{
}

void TreeWidget::contextMenuEvent (QContextMenuEvent * e)
{
    // ask workbenches and view provider, ...
    MenuItem* view = new MenuItem;
    Gui::Application::Instance->setupContextMenu("Tree", view);

    QMenu contextMenu;
    MenuManager::getInstance()->setupContextMenu(view, contextMenu);

    // get the current item
    this->contextItem = itemAt(e->pos());
    if (this->contextItem && this->contextItem->type() == DocumentType) {
        contextMenu.addAction(this->createGroupAction);
    }
    else if (this->contextItem && this->contextItem->type() == ObjectType) {
        DocumentObjectItem* objitem = static_cast<DocumentObjectItem*>
            (this->contextItem);
        if (objitem->object()->getObject()->isDerivedFrom(App::DocumentObjectGroup
            ::getClassTypeId())) {
            QList<QAction*> acts = contextMenu.actions();
            if (!acts.isEmpty()) {
                QAction* first = acts.front();
                QAction* sep = contextMenu.insertSeparator(first);
                contextMenu.insertAction(sep, this->createGroupAction);
            }
            else
                contextMenu.addAction(this->createGroupAction);
        }
    }
    delete view;
    if (contextMenu.actions().count() > 0)
        contextMenu.exec(QCursor::pos());
}

void TreeWidget::onCreateGroup()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Create group"), tr("Name of the group:"),
                                         QLineEdit::Normal, tr("Group"),&ok);
    if (ok && !name.isEmpty()) {
        if (this->contextItem->type() == DocumentType) {
            DocumentItem* docitem = static_cast<DocumentItem*>(this->contextItem);
            App::Document* doc = docitem->document()->getDocument();
            QString cmd = QString("App.getDocument(\"%1\").addObject"
                                  "(\"App::DocumentObjectGroup\",\"%2\")")
                                 .arg(doc->getName()).arg(name);
            Gui::Document* gui = Gui::Application::Instance->getDocument(doc);
            gui->openCommand("Create group");
            Gui::Application::Instance->runPythonCode(cmd.toUtf8());
            gui->commitCommand();
        }
        else if (this->contextItem->type() == ObjectType) {
            DocumentObjectItem* objitem = static_cast<DocumentObjectItem*>
                (this->contextItem);
            App::DocumentObject* obj = objitem->object()->getObject();
            App::Document& doc = obj->getDocument();
            QString cmd = QString("App.getDocument(\"%1\").getObject(\"%2\")"
                                  ".newObject(\"App::DocumentObjectGroup\",\"%3\")")
                                  .arg(doc.getName())
                                  .arg(obj->getNameInDocument())
                                  .arg(name);
            Gui::Document* gui = Gui::Application::Instance->getDocument(&doc);
            gui->openCommand("Create group");
            Gui::Application::Instance->runPythonCode(cmd.toUtf8());
            gui->commitCommand();
        }
    }
}

bool TreeWidget::dropMimeData(QTreeWidgetItem *parent, int index,
                              const QMimeData *data, Qt::DropAction action)
{
    return QTreeWidget::dropMimeData(parent, index, data, action);
}

Qt::DropActions TreeWidget::supportedDropActions () const
{
    return QTreeWidget::supportedDropActions();
}

QMimeData * TreeWidget::mimeData (const QList<QTreeWidgetItem *> items) const
{
    // all selected items must reference an object from the same document
    App::Document* doc=0;
    for (QList<QTreeWidgetItem *>::ConstIterator it = items.begin(); it != items.end(); ++it) {
        if ((*it)->type() != TreeWidget::ObjectType)
            return 0;
        App::DocumentObject* obj = static_cast<DocumentObjectItem *>(*it)->object()->getObject();
        if (!doc)
            doc = &obj->getDocument();
        else if (doc != &obj->getDocument())
            return 0;
    }
    return QTreeWidget::mimeData(items);
}

void TreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QTreeWidget::dragMoveEvent(event);
    if (!event->isAccepted())
        return;

    QTreeWidgetItem* item = itemAt(event->pos());
    if (!item) {
        event->ignore();
    }
    else if (item->type() == TreeWidget::DocumentType) {
        QList<QModelIndex> idxs = selectedIndexes();
        App::Document* doc = static_cast<DocumentItem*>(item)->document()->getDocument();
        for (QList<QModelIndex>::Iterator it = idxs.begin(); it != idxs.end(); ++it) {
            QTreeWidgetItem* item = itemFromIndex(*it);
            if (item->type() != TreeWidget::ObjectType) {
                event->ignore();
                return;
            }
            App::DocumentObject* obj = static_cast<DocumentObjectItem*>(item)->
                object()->getObject();
            if (doc != &obj->getDocument()) {
                event->ignore();
                return;
            }
        }
    }
    else if (item->type() == TreeWidget::ObjectType) {
        App::DocumentObject* grp = static_cast<DocumentObjectItem*>(item)->
            object()->getObject();
        if (!grp->getTypeId().isDerivedFrom(App::DocumentObjectGroup::
            getClassTypeId()))
            event->ignore();
        App::Document* doc = &grp->getDocument();
        QList<QModelIndex> idxs = selectedIndexes();
        for (QList<QModelIndex>::Iterator it = idxs.begin(); it != idxs.end(); ++it) {
            QTreeWidgetItem* item = itemFromIndex(*it);
            if (item->type() != TreeWidget::ObjectType) {
                event->ignore();
                return;
            }
            App::DocumentObject* obj = static_cast<DocumentObjectItem*>(item)->
                object()->getObject();
            if (doc != &obj->getDocument()) {
                event->ignore();
                return;
            }
            if (obj->getTypeId().isDerivedFrom(App::DocumentObjectGroup::getClassTypeId())) {
                if (static_cast<App::DocumentObjectGroup*>(grp)->isChildOf(
                    static_cast<App::DocumentObjectGroup*>(obj))) {
                    event->ignore();
                    return;
                }
            }
        }
    }
    else {
        event->ignore();
    }
}

void TreeWidget::dropEvent(QDropEvent *event)
{
    return;
    QTreeWidgetItem* targetitem = itemAt(event->pos());
    if (targetitem->type() == TreeWidget::ObjectType) {
        // add object to group
        App::DocumentObject* grp = static_cast<DocumentObjectItem*>(targetitem)
            ->object()->getObject();
        if (!grp->getTypeId().isDerivedFrom(App::DocumentObjectGroup::getClassTypeId()))
            return; // no group object
        App::Document* doc = &grp->getDocument();
        QList<QModelIndex> idxs = selectedIndexes();

        // Open command
        Gui::Document* gui = Gui::Application::Instance->getDocument(doc);
        gui->openCommand("Move object");
        for (QList<QModelIndex>::Iterator it = idxs.begin(); it != idxs.end(); ++it) {
            // get document object
            QTreeWidgetItem* item = itemFromIndex(*it);
            App::DocumentObject* obj = static_cast<DocumentObjectItem*>(item)
                ->object()->getObject();

            // build Python command for execution
            QString cmd = QString("App.getDocument(\"%1\").getObject(\"%2\").addObject("
                                  "App.getDocument(\"%1\").getObject(\"%3\"))")
                                  .arg(doc->getName())
                                  .arg(grp->getNameInDocument())
                                  .arg(obj->getNameInDocument());
            Gui::Application::Instance->runPythonCode(cmd.toUtf8());
        }
        gui->commitCommand();
    }
    else if (targetitem->type() == TreeWidget::DocumentType) {
    }
}

void TreeWidget::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
    QTreeWidget::drawRow(painter, options, index);
    // Set the text and highlighted text color of a hidden object to a dark
    //QTreeWidgetItem * item = itemFromIndex(index);
    //if (item->type() == ObjectType && !(static_cast<DocumentObjectItem*>(item)->previousStatus & 1)) {
    //    QStyleOptionViewItem opt(options);
    //    opt.state ^= QStyle::State_Enabled;
    //    QColor c = opt.palette.color(QPalette::Inactive, QPalette::Dark);
    //    opt.palette.setColor(QPalette::Inactive, QPalette::Text, c);
    //    opt.palette.setColor(QPalette::Inactive, QPalette::HighlightedText, c);
    //    QTreeWidget::drawRow(painter, opt, index);
    //}
    //else {
    //    QTreeWidget::drawRow(painter, options, index);
    //}
}

// ----------------------------------------------------------------------------

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
    Application::Instance->signalRelabelDocument.connect(boost::bind(&TreeDockWidget::slotRelabelDocument, this, _1));

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
    this->rootItem->setFlags(Qt::ItemIsEnabled);
    this->treeWidget->expandItem(this->rootItem);
    this->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->treeWidget->setMouseTracking(true); // needed for itemEntered() to work

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
    item->setText(0, QString(Doc.getDocument()->Label.getValue()));
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

void TreeDockWidget::slotRelabelDocument(Gui::Document& Doc)
{
    std::map<Gui::Document*, DocumentItem*>::iterator it = DocumentMap.find(&Doc);
    if (it != DocumentMap.end()) {
        it->second->setText(0, QString::fromUtf8(Doc.getDocument()->Label.getValue()));
    }
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
    if ( item && item->type() == TreeWidget::ObjectType ) {
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

void TreeDockWidget::scrollItemToTop(Gui::Document* doc)
{
    std::map<Gui::Document*,DocumentItem*>::iterator it = DocumentMap.find(doc);
    if (it != DocumentMap.end()) {
        DocumentItem* root = it->second;
        QTreeWidgetItemIterator it(root, QTreeWidgetItemIterator::Selected);
        for (; *it; ++it) {
            if ((*it)->type() == TreeWidget::ObjectType) {
                this->treeWidget->scrollToItem(*it, QAbstractItemView::PositionAtTop);
                break;
            }
        }
    }
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

DocumentItem::DocumentItem(Gui::Document* doc, QTreeWidgetItem * parent)
    : QTreeWidgetItem(parent, TreeWidget::DocumentType), pDocument(doc)
{
    // Setup connections
    doc->signalNewObject.connect(boost::bind(&DocumentItem::slotNewObject, this, _1));
    doc->signalDeletedObject.connect(boost::bind(&DocumentItem::slotDeleteObject, this, _1));
    doc->signalChangedObject.connect(boost::bind(&DocumentItem::slotChangeObject, this, _1));
    doc->signalRenamedObject.connect(boost::bind(&DocumentItem::slotRenameObject, this, _1));
    doc->signalActivatedObject.connect(boost::bind(&DocumentItem::slotActiveObject, this, _1));

    setFlags(Qt::ItemIsEnabled/*|Qt::ItemIsEditable*/);
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

Gui::Document* DocumentItem::document() const
{
    return this->pDocument;
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
    : QTreeWidgetItem(parent, TreeWidget::ObjectType), previousStatus(-1), viewObject(pcViewProvider)
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
    int currentStatus = 
        ((pObject->isError()          ? 1 : 0) << 2) |
        ((pObject->mustExecute() == 1 ? 1 : 0) << 1) |
        (viewObject->isShow()         ? 1 : 0);
    if (previousStatus == currentStatus)
        return;
    previousStatus = currentStatus;

    QPixmap px;
    if (currentStatus & 4) {
        // object is in error state
        static const char * const feature_error_xpm[]={
            "9 9 3 1",
            ". c None",
            "# c #ff0000",
            "a c #ffffff",
            "...###...",
            ".##aaa##.",
            ".##aaa##.",
            "###aaa###",
            "###aaa###",
            "#########",
            ".##aaa##.",
            ".##aaa##.",
            "...###..."};
        px = QPixmap(feature_error_xpm);
    }
    else if (currentStatus & 2) {
        // object must be recomputed
        static const char * const feature_recompute_xpm[]={
            "9 9 3 1",
            ". c None",
            "# c #0000ff",
            "a c #ffffff",
            "...###...",
            ".######aa",
            ".#####aa.",
            "#####aa##",
            "#aa#aa###",
            "#aaaa####",
            ".#aa####.",
            ".#######.",
            "...###..."};
        px = QPixmap(feature_recompute_xpm);
    }

    QIcon::Mode mode = QIcon::Normal;
    if (currentStatus & 1) { // visible
        // Note: By default the foreground, i.e. text color is invalid
        // to make use of the default color of the tree widget's palette.
        // If we temporarily set this color to dark and reset to an invalid
        // color again we cannot do it with setTextColor() or setForeground(),
        // respectively, because for any reason the color would always switch
        // to black which will lead to unreadable text if the system background
        // hss already a dark color.
        // However, it works if we set the appropriate role to an empty QVariant().
#if QT_VERSION >= 0x040200
        this->setData(0, Qt::ForegroundRole,QVariant());
#else
        this->setData(0, Qt::TextColorRole,QVariant());
#endif
    }
    else { // invisible
        QStyleOptionViewItem opt;
        opt.initFrom(this->treeWidget());
#if QT_VERSION >= 0x040200
        this->setForeground(0, opt.palette.color(QPalette::Disabled,QPalette::Dark));
#else
        this->setTextColor(0, opt.palette.color(QPalette::Disabled,QPalette::Dark));
#endif
        mode = QIcon::Disabled;
    }

    // get the original icon set
    QIcon icon_org = viewObject->getIcon();
    QIcon icon_mod;
    int w = QApplication::style()->pixelMetric(QStyle::PM_ListViewIconSize);

    // if needed show small pixmap inside
    if (!px.isNull()) {
        icon_mod.addPixmap(BitmapFactory().merge(icon_org.pixmap(w, w, mode, QIcon::Off),
            px,BitmapFactoryInst::TopRight), QIcon::Normal, QIcon::Off);
        icon_mod.addPixmap(BitmapFactory().merge(icon_org.pixmap(w, w, mode, QIcon::On ),
            px,BitmapFactoryInst::TopRight), QIcon::Normal, QIcon::Off);
    }
    else {
        icon_mod.addPixmap(icon_org.pixmap(w, w, mode, QIcon::Off), QIcon::Normal, QIcon::Off);
        icon_mod.addPixmap(icon_org.pixmap(w, w, mode, QIcon::On ), QIcon::Normal, QIcon::On );
    }

    this->setIcon(0, icon_mod);
}

void DocumentObjectItem::displayStatusInfo()
{
    App::DocumentObject* Obj = viewObject->getObject();

    QString info = Obj->getStatusString();
    if ( Obj->mustExecute() == 1 )
        info += QString(" (but must be executed)");
    getMainWindow()->statusBar()->showMessage( info );
   
}


#include "moc_Tree.cpp"

