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
# include <qcursor.h>
#	include <q3header.h>
# include <qlayout.h>
# include <q3popupmenu.h>
# include <qstatusbar.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <Q3GridLayout>
#include <QPixmap>
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

TreeModelItem::TreeModelItem(const QString& s, Base::BaseClass* obj)
    : name(s), parentItem(0), object(obj)
{
}

TreeModelItem::~TreeModelItem()
{
    qDeleteAll(childItems);
}

void TreeModelItem::setParent(TreeModelItem* parent)
{
    parentItem = parent;
}

TreeModelItem *TreeModelItem::parent() const
{
    return parentItem;
}

void TreeModelItem::appendChild(TreeModelItem *item)
{
    childItems.append(item);
}

void TreeModelItem::removeChild(TreeModelItem *item)
{
    childItems.removeAll(item);
}

TreeModelItem *TreeModelItem::child(int row)
{
    return childItems.value(row);
}

int TreeModelItem::childCount() const
{
    return childItems.count();
}

int TreeModelItem::columnCount() const
{
    return 1;
}

QVariant TreeModelItem::data(int role) const
{
    if (role == Qt::DisplayRole) {
        return QVariant(name);
    } else if (role == Qt::DecorationRole) {
        return QVariant(icon);
    }

    return QVariant();
}

Qt::ItemFlags TreeModelItem::flags() const
{
    Qt::ItemFlags basicFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (!parentItem) // the root
        return basicFlags;

    // Allow drag for objects and drop for documents
    if (object->isDerivedFrom(App::Document::getClassTypeId())) {
        basicFlags |= Qt::ItemIsDropEnabled;
    } else if (object->isDerivedFrom(App::DocumentObject::getClassTypeId())) {
        basicFlags |= Qt::ItemIsDragEnabled;
    }

    return basicFlags;
}

int TreeModelItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeModelItem*>(this));

    return 0;
}

void TreeModelItem::setIcon(const QPixmap& icon)
{
    this->icon = icon;
}

bool TreeModelItem::hasObject(Base::BaseClass* obj) const
{
    return this->object == obj;
}

// ----------------------------------------------------------------------------

/* TRANSLATOR Gui::TreeModel */

QPixmap* TreeModel::documentPixmap = 0;

TreeModel::TreeModel(QObject* parent)
{
    // The Application item
    rootItem = new TreeModelItem(tr("Application"));
    documentPixmap = new QPixmap(Gui::BitmapFactory().pixmap("Document"));
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount ( const QModelIndex & parent ) const
{
    return rootItem->columnCount();
}

QVariant TreeModel::data ( const QModelIndex & index, int role ) const
{
    if (!index.isValid())
        return QVariant();

    TreeModelItem *item = static_cast<TreeModelItem*>(index.internalPointer());
    return item->data(role);
}

bool TreeModel::setData(const QModelIndex& index, const QVariant & value, int role)
{
    if (!index.isValid())
        return false;

    return false;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    TreeModelItem *item = static_cast<TreeModelItem*>(index.internalPointer());
    return item->flags();
}

QModelIndex TreeModel::index ( int row, int column, const QModelIndex & parent ) const
{
    // The Application root item
    if (!parent.isValid()) {
        return createIndex(row, column, rootItem);
    }

    TreeModelItem *parentItem = static_cast<TreeModelItem*>(parent.internalPointer());
    TreeModelItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent ( const QModelIndex & index ) const
{
    if (!index.isValid() || !index.internalPointer() || index.internalPointer() == rootItem)
        return QModelIndex();

    TreeModelItem *child = static_cast<TreeModelItem*>(index.internalPointer());
    TreeModelItem *parent = child->parent();
    if (!parent)
        return QModelIndex();
    return createIndex(parent->row(), 0, parent);
}

int TreeModel::rowCount ( const QModelIndex & parent ) const
{
    // One children for the Application item
    if (!parent.isValid())
        return 1;

    TreeModelItem *parentItem = static_cast<TreeModelItem*>(parent.internalPointer());
    return parentItem->childCount();
}

QVariant TreeModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole)
            return tr("Labels & Attributes");
    }

    return QVariant();
}

bool TreeModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role )
{
    return false;
}

QModelIndex TreeModel::newDocument(App::Document* doc)
{
    TreeModelItem* child = new TreeModelItem(doc->getName(), doc);
    child->setParent(rootItem);
    rootItem->appendChild(child);
    child->setIcon(*documentPixmap);

    QModelIndex root = this->index(0, 0);
    int row = rootItem->childCount()-1;
    beginInsertRows(root, row, row);
    endInsertRows();
    return this->index(row, 0, root);
}

void TreeModel::deleteDocument(App::Document* doc)
{
    int count = rootItem->childCount();
    for (int i=0; i<count; i++) {
        TreeModelItem* item = rootItem->child(i);
        if (item->hasObject(doc)) {
            rootItem->removeChild(item);
            QModelIndex root = this->index(0, 0);
            beginRemoveRows(root, i, i);
            endRemoveRows();
            delete item;
            break;
        }
    }
}

QModelIndex TreeModel::newObject(App::DocumentObject* obj)
{
    int count = rootItem->childCount();
    for (int i=0; i<count; i++) {
        TreeModelItem* item = rootItem->child(i);
        if (item->hasObject(&obj->getDocument())) {
            TreeModelItem* child = new TreeModelItem(obj->name.getValue(), obj);
            child->setParent(item);
            item->appendChild(child);

            Gui::Document* doc = Application::Instance->getDocument(&obj->getDocument());
            Gui::ViewProvider* view = doc->getViewProvider(obj);
            child->setIcon(view->getIcon());

            QModelIndex root = this->index(0, 0);
            QModelIndex index = this->index(i, 0, root);
            int row = item->childCount()-1;
            beginInsertRows(index, row, row);
            endInsertRows();
            return this->index(row, 0, index);
        }
    }

    return QModelIndex();
}

void TreeModel::deleteObject(App::DocumentObject* obj)
{
    QModelIndex index;
    TreeModelItem* document = 0;
    int count = rootItem->childCount();
    for (int i=0; i<count; i++) {
        TreeModelItem* child = rootItem->child(i);
        if (child->hasObject(&obj->getDocument())) {
            document = child;
            QModelIndex root = this->index(0, 0);
            index = this->index(i, 0, root);
            break;
        }
    }

    if (document) {
        int count = document->childCount();
        for (int i=0; i<count; i++) {
            TreeModelItem* child = document->child(i);
            if (child->hasObject(obj)) {
                document->removeChild(child);
                beginRemoveRows(index, i, i);
                endRemoveRows();
                delete child;
            }
        }
    }
}

void TreeModel::changeObject(App::DocumentObject*)
{
}

// ----------------------------------------------------------------------------

TreeWidget::TreeWidget(Gui::Document* pcDocument,QWidget *parent)
  : DockWindow(pcDocument,parent)
{
    setWindowTitle( tr("Tree view" ) );

    this->treeView = new QTreeView(this);
    this->treeModel = new TreeModel(this);
    this->treeView->setModel(this->treeModel);
    this->treeView->setRootIsDecorated(false);
    this->treeView->setExpanded(this->treeModel->index(0, 0), true);

    Gui::Selection().Attach(this);
    App::GetApplication().signalNewDocument.connect(boost::bind(&TreeWidget::slotNewDocument, this, _1));
    App::GetApplication().signalDeletedDocument.connect(boost::bind(&TreeWidget::slotDeletedDocument, this, _1));

    QGridLayout* pLayout = new QGridLayout(this); 
    pLayout->setSpacing(0);
    pLayout->setMargin (0);
    pLayout->addWidget( this->treeView, 0, 0 );
}

TreeWidget::~TreeWidget()
{
    //App::GetApplication().signalNewDocument.disconnect(boost::bind(&TreeWidget::slotNewDocument, this, _1));
    //App::GetApplication().signalDeletedDocument.disconnect(boost::bind(&TreeWidget::slotDeletedDocument, this, _1));
    Gui::Selection().Detach(this);
}

void TreeWidget::slotNewDocument(App::Document& Doc)
{
    QModelIndex item = treeModel->newDocument(&Doc);
    treeView->setExpanded(item, true);
    //Doc.signalNewObject.connect(boost::bind(&TreeWidget::slotNewObject, this, _1));
    //Doc.signalDeletedObject.connect(boost::bind(&TreeWidget::slotDeletedObject, this, _1));
    
    //Doc.signalChangedObject.connect(boost::bind(&TreeWidget::slotChangedObject, this, _1));
}

void TreeWidget::slotDeletedDocument(App::Document& Doc)
{
    //Doc.signalNewObject.disconnect(boost::bind(&TreeWidget::slotNewObject, this, _1));
    //Doc.signalDeletedObject.disconnect(boost::bind(&TreeWidget::slotDeletedObject, this, _1));
    //Doc.signalChangedObject.disconnect(boost::bind(&TreeWidget::slotChangedObject, this, _1));
    treeModel->deleteDocument(&Doc);
}

void TreeWidget::slotNewObject(App::DocumentObject& obj)
{
    QModelIndex item = treeModel->newObject(&obj);
    treeView->setExpanded(item, true);
}

void TreeWidget::slotDeletedObject(App::DocumentObject& obj)
{
    treeModel->deleteObject(&obj);
}

void TreeWidget::slotChangedObject(App::DocumentObject& obj)
{
    treeModel->changeObject(&obj);
}

void TreeWidget::contextMenuEvent ( QContextMenuEvent * e )
{
    // ask workbenches and view provider, ...
    MenuItem* view = new MenuItem;
    Gui::Application::Instance->setupContextMenu("Tree", view);

    QMenu ContextMenu;
    MenuManager::getInstance()->setupContextMenu(view,ContextMenu);
    delete view;
    ContextMenu.exec( QCursor::pos() );
}

void TreeWidget::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
    }

    DockWindow::changeEvent(e);
}

void TreeWidget::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,Gui::SelectionSingleton::MessageType Reason)
{
}

// ----------------------------------------------------------------------------

//**************************************************************************
// DocItem
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
DocItem::DocItem( Q3ListViewItem* parent,Q3ListViewItem * after,Gui::Document* doc)
    : Q3ListViewItem( parent, after ), _pcDocument(doc)
{
  setPixmap(0,*TreeView::pcDocumentPixmap);
  setText(0,QString(_pcDocument->getDocument()->getName()));

  bHighlight = false;
  bSelected = false;

  buildUp();
}

DocItem::~DocItem()
{
  //_pcDocument->Detach(this);
}



bool DocItem::testStatus(void)
{
  bool bChanged = false;

  for (std::map<std::string,ObjectItem*>::iterator pos = FeatMap.begin();pos!=FeatMap.end();++pos)
  {
    if(pos->second->testStatus())
      bChanged = true;
  }
  return bChanged;

}

void DocItem::highlightFeature(const char* name, bool bOn)
{
  std::map<std::string,ObjectItem*>::iterator pos;
  pos = FeatMap.find(name);
  
  if(pos != FeatMap.end())
    pos->second->highlightFeature(bOn);

  bHighlight = bOn;
  if (!isOpen() )
    repaint();

}

void DocItem::selectFeature(const char* name, bool bOn)
{
  std::map<std::string,ObjectItem*>::iterator pos;
  pos = FeatMap.find(name);
  
  if(pos != FeatMap.end())
    pos->second->selectFeature(bOn);


}

void DocItem::clearSelection(void)
{
  for (std::map<std::string,ObjectItem*>::iterator pos = FeatMap.begin();pos!=FeatMap.end();++pos)
  {
    pos->second->selectFeature(false);
  }
}

void DocItem::isSelectionUptodate(void)
{
  for (std::map<std::string,ObjectItem*>::iterator pos = FeatMap.begin();pos!=FeatMap.end();++pos)
  {
    if(pos->second->bSelected != pos->second->isSelected())
    {
      if(pos->second->bSelected)
      {
        //Base::Console().Log("Sel : Rmv tree selection (%s,%s)\n",_pcDocument->getDocument()->getName(),pos->first.c_str());
        Gui::Selection().rmvSelection(_pcDocument->getDocument()->getName(),pos->first.c_str());
        //pos->second->bSelected = false;
      }else{

        //Base::Console().Log("Sel : Add tree selection (%s,%s)\n",_pcDocument->getDocument()->getName(),pos->first.c_str());
        Gui::Selection().addSelection(_pcDocument->getDocument()->getName(),pos->first.c_str());
        pos->second->bSelected = true;

      }
    }
    
  }



}

void DocItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
   QColorGroup _cg( cg );
   QColor c = _cg.text();

   if ( bHighlight && !isOpen() )
       _cg.setColor( QColorGroup::Base , QColor (200,200,255));

   Q3ListViewItem::paintCell( p, _cg, column, width, align );

   _cg.setColor( QColorGroup::Text, c );

}

void DocItem::addViewProviderDocumentObject(ViewProviderDocumentObject* Provider)
{
  std::string name = Provider->getObject()->name.getValue();
  std::map<std::string,ObjectItem*>::iterator it = FeatMap.find( name );

  if ( it == FeatMap.end() ) {
    
    // get the associated document object
    App::DocumentObject* obj = Provider->getObject();
    
    // is the object part of a group?
    ObjectItem* groupItem=0;
    App::DocumentObjectGroup* grp = App::DocumentObjectGroup::getGroupOfObject( obj );
    if ( grp ) {
      std::string grpname = grp->name.getValue();
      std::map<std::string,ObjectItem*>::iterator jt = FeatMap.find( grpname );

      if ( jt != FeatMap.end() )
        groupItem = jt->second;

#ifdef FC_DEBUG
      else
        Base::Console().Warning("DocItem::addViewProviderDocumentObject: try to insert a group object before the group itself is inserted\n");
#endif
    }

    ObjectItem* item = Provider->getTreeItem(this);
    Q3ListViewItem* child=0;
    Q3ListViewItem* sibling=0;
    if ( groupItem )
    {
      // set the newly created item at the end of the group
      groupItem->setOpen(true);
      groupItem->insertItem(item);
      child = groupItem->firstChild();
    }
    else
    {
      // set the newly created item at the end of the document item
      child = firstChild();
    }

    // get the last item
    while (child)
    {
      sibling = child;
      child = child->nextSibling();
    }

    if ( sibling )
      item->moveItem(sibling);

    FeatMap[ name ] = item;

    if(FeatMap.size() == 1)
      setOpen(true);
  } else {
#ifdef FC_DEBUG
    Base::Console().Warning("DocItem::addViewProviderDocumentObject: cannot add view provider twice\n");
#endif
  }
}

void DocItem::removeViewProviderDocumentObject(ViewProviderDocumentObject* Provider)
{
  //FIXME: 
#if 1
  // If we remove an item from the list view all its children would get removed, too. Thus we must
  // move all children items one level up. 
  QString name = Provider->getObject()->name.getValue();
  std::map<std::string,ObjectItem*>::iterator it = FeatMap.find(name.latin1());
  if ( it != FeatMap.end() )
  {
    ObjectItem* item = it->second;
    FeatMap.erase(it);

    ObjectItem *child = (ObjectItem*)item->firstChild();
    Q3ListViewItem* parent = item->parent();
    while ( child ) {
      // move the items to the parent item
      ObjectItem* sibling = (ObjectItem*)child->nextSibling();
      item->takeItem(child);
      parent->insertItem(child);
      child = sibling;
    }

    delete item;
  }
#else
  // If we remove an item from the list view all its children get removed, too. Thus we must
  // make sure to´remove properly all concerning elements from our map, too. 
  QString name = Provider->getObject()->name.getValue();
  std::map<std::string,ObjectItem*>::iterator it = FeatMap.find(name.latin1());
  if ( it != FeatMap.end() )
  {
    ObjectItem* item = it->second;
    FeatMap.erase(it);

    ObjectItem *child = (ObjectItem*)item->firstChild();
    while ( child ) {
      ObjectItem* sibling = (ObjectItem*)child->nextSibling();
      // call recursively
      removeViewProviderDocumentObject( child->_pcViewProvider );
      child = sibling;
    }

    delete item;
  }
#endif
}

void DocItem::update(void)
{
  //puts("Updtate");

}

void DocItem::buildUp(void)
{

}

void DocItem::setOpen( bool o )
{
  Q3ListViewItem::setOpen ( o );
}

void DocItem::setup()
{
  //setExpandable( TRUE );
  Q3ListViewItem::setup();
}

void DocItem::activate ()
{
  //puts("Activated");
}

void DocItem::rename(void)
{
  setText(0,QString(_pcDocument->getDocument()->getName()));

}

//**************************************************************************
// ObjectItem
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


/** Constructor
 *  Instanciate the class with his parent (in the tree) and the
 *  acociated FCLabel.
 *  @return Const string with the date/time
 */
ObjectItem::ObjectItem( Q3ListViewItem* parent,Gui::ViewProviderDocumentObject* pcViewProvider)
    : Q3ListViewItem( parent ),
  _pcViewProvider(pcViewProvider)
{
  BaseColor = Qt::white;
  TextColor = Qt::black;
  HighlightColor = QColor (200,200,255);

//  setPixmap(0,pcViewProvider->getIcon());
//  setText(0,QString(pcViewProvider->getObject()->getName()));


  bHighlight = false;
  bSelected = false;

  buildUp();
}

ObjectItem::~ObjectItem()
{
}

bool ObjectItem::testStatus(void)
{
  bool ret = _pcViewProvider->testStatus();
  if ( ret )
    repaint();
  return ret;
}

void ObjectItem::displayStatusInfo()
{
  App::DocumentObject* Obj = _pcViewProvider->getObject();

  if(Obj->getTypeId().isDerivedFrom(App::AbstractFeature::getClassTypeId()) )
  {
    App::AbstractFeature *feat = dynamic_cast<App::AbstractFeature *>(Obj);
    QString info = feat->getStatusString();
    if ( feat->getStatus() == App::AbstractFeature::Error )
      info += QString(" (%1)").arg(feat->getErrorString());
    else if ( feat->mustExecute() )
      info += QString(" (but must be executed)");
    getMainWindow()->statusBar()->message( info );
  }
}

void ObjectItem::paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
{
   QColorGroup _cg( cg );
   QColor c = _cg.text();

   if ( bHighlight )
     _cg.setColor( QColorGroup::Base , HighlightColor);
   //else 
   //  _cg.setColor( QColorGroup::Base , BaseColor);

   //_cg.setColor( QColorGroup::Text , TextColor);
   
   Q3ListViewItem::paintCell( p, _cg, column, width, align );

   //_cg.setColor( QColorGroup::Text, c );

}

void ObjectItem::highlightFeature(bool bOn)
{
  bHighlight = bOn;
  repaint();
}

void ObjectItem::selectFeature(bool bOn)
{
  setSelected(bOn);
  bSelected = bOn;
  repaint();
}

void ObjectItem::update(void)
{
  //puts("Updtate");

}

void ObjectItem::buildUp(void)
{

}

void ObjectItem::setOpen( bool o )
{

  Q3ListViewItem::setOpen ( o );
}


void ObjectItem::setup()
{
  //setExpandable( TRUE );
  Q3ListViewItem::setup();
}

void ObjectItem::activate ()
{
  //puts("Activated");
}




QPixmap* TreeView::pcDocumentPixmap=0;

/* TRANSLATOR Gui::TreeView */
TreeView::TreeView(Gui::Document* pcDocument,QWidget *parent)
  : DockWindow(pcDocument,parent),bFromOutside(false), _lastDocItem(0)
{
  Q3GridLayout* layout = new Q3GridLayout( this );
  _pcListView = new Q3ListView(this);
  layout->addWidget( _pcListView, 0, 0 );

  // set defaults and the colums
  _pcListView->setSortColumn(-1);
  _pcListView->addColumn(tr("Labels & Attributes"));
  _pcListView->setColumnWidthMode(0,Q3ListView::Maximum);
  //_pcListView->setSorting(0,true);
//  _pcListView->addColumn(tr("Value"));
//  _pcListView->setColumnWidthMode(1,QListView::Manual );

  // retreive the Pixmaps
  pcDocumentPixmap   = new QPixmap(Gui::BitmapFactory().pixmap("Document"));
  //pcLabelClosed = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));
  //pcAttribute   = new QPixmap(Gui::BitmapFactory().pixmap("RawTree_Attr"));


  // Add the first main label
  _pcMainItem = new Q3ListViewItem(_pcListView,"Application");
  _pcMainItem->setOpen(true);

  //_pcListView->setRootIsDecorated(true);

  _pcListView->setSelectionMode(Q3ListView::Extended );
  connect( _pcListView, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
  connect( _pcListView, SIGNAL( onItem(Q3ListViewItem*) ), this, SLOT( onItem(Q3ListViewItem*) ) );


  QPalette pal = _pcListView->palette();
  pal.setInactive( pal.active() );
  _pcListView->setPalette( pal );


  //_pcListView->setSize(200,400);
  resize( 200, 400 );

  Gui::Selection().Attach(this);


}

TreeView::~TreeView()
{
  Gui::Selection().Detach(this);
  delete pcDocumentPixmap;
}

void TreeView::contextMenuEvent ( QContextMenuEvent * e )
{
  // ask workbenches and view provider, ...
  MenuItem* view = new MenuItem;
  Gui::Application::Instance->setupContextMenu("Tree", view);

  Q3PopupMenu ContextMenu;
  MenuManager::getInstance()->setupContextMenu(view,ContextMenu);
  delete view;
  ContextMenu.exec( QCursor::pos() );
}

void TreeView::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::LanguageChange) {
    _pcListView->setColumnText(0, tr("Labels & Attributes"));
  }

  DockWindow::changeEvent(e);
}

void TreeView::testStatus(void)
{
  bool bChanged = false;

  for (std::map<Gui::Document*,DocItem*>::iterator pos = DocMap.begin();pos!=DocMap.end();++pos)
  {
    if(pos->second->testStatus())
      bChanged = true;
  }

  if ( bChanged )
    repaint();
}

// Observer message from the Selection
void TreeView::OnChange(Gui::SelectionSingleton::SubjectType &rCaller,Gui::SelectionSingleton::MessageType Reason)
{
  bFromOutside = true;
  if(Reason.Type != SelectionChanges::ClearSelection)
  {
    Gui::Document* pDoc = Application::Instance->getDocument( Reason.pDocName );
    std::map<Gui::Document*, DocItem*>::iterator it = DocMap.find( pDoc );

    if(it!= DocMap.end())
    {
  
      if(Reason.Type == SelectionChanges::SetPreselect)
       it->second->highlightFeature(Reason.pObjectName,true);
      else  if(Reason.Type == SelectionChanges::RmvPreselect)
       it->second->highlightFeature(Reason.pObjectName,false);
      else if(Reason.Type == SelectionChanges::AddSelection)
       it->second->selectFeature(Reason.pObjectName,true);
      else  if(Reason.Type == SelectionChanges::RmvSelection)
       it->second->selectFeature(Reason.pObjectName,false);
    }
  }else{
    // clears the complete selection
    if ( strcmp(Reason.pDocName,"") == 0 ) {
      for (std::map<Gui::Document*,DocItem*>::iterator pos = DocMap.begin();pos!=DocMap.end();++pos)
      {
        pos->second->clearSelection();
      }

      _pcListView->clearSelection ();
    } else {
      // clears the selection of the given document
      Gui::Document* pDoc = Application::Instance->getDocument(Reason.pDocName);
      std::map<Gui::Document*, DocItem*>::iterator pos = DocMap.find(pDoc);
      if ( pos != DocMap.end() ) {
        pos->second->clearSelection();
      }
    }

    _pcListView->update();
  }

  bFromOutside = false;
}



void TreeView::onSelectionChanged ()
{
  if(bFromOutside) return;

  //Base::Console().Log("Sel : QListView::selectionChanged()");

  for (std::map<Gui::Document*,DocItem*>::iterator pos = DocMap.begin();pos!=DocMap.end();++pos)
  {
    pos->second->isSelectionUptodate();
  }

}

void TreeView::onItem(Q3ListViewItem* item)
{
  // feature item selected
  if ( item && item->rtti() == 3100 )
  {
    ObjectItem* feat = reinterpret_cast<ObjectItem*>(item);
    feat->displayStatusInfo();
  }
}

void TreeView::onUpdate(void)
{
#ifdef FC_LOGUPDATECHAIN
  Base::Console().Log("Acti: Gui::TreeView::onUpdate()");
#endif

  // quick and dirty so far
//  delete _pcMainItem;
//  _pcMainItem = new TreeLabel(this);
}

void TreeView::onRename(Gui::Document *pDoc)
{
  std::map<Gui::Document*,DocItem*>::iterator pos = DocMap.find( pDoc );
  if ( pos != DocMap.end() ) {
    pos->second->rename();
  }
}

bool TreeView::onMsg(const char* pMsg)
{
  //printf("MsgTree: %s View: %p\n",pMsg,this);

  // no messages yet
  return false;
}

DocItem * TreeView::NewDoc( Gui::Document* pDoc )
{
  // set the new ctreated item at the end
  DocItem *item = new DocItem(_pcMainItem,_lastDocItem,pDoc);
  _lastDocItem = item;
  DocMap[ pDoc ] = item;
  return item;
}

void TreeView::DeleteDoc( Gui::Document* pDoc )
{
  std::map<Gui::Document*,DocItem*>::iterator it = DocMap.find( pDoc );
  if ( it != DocMap.end() )
  {
    // now we must search for the matching listview item
    Q3ListViewItem* item = _pcMainItem->firstChild();
    Q3ListViewItem* sibling=0;
    while ( item ) {
      // make sure that we have a ObjectItem
      if ( item == it->second )
      {
        // in case we remove the last item
        if ( _lastDocItem == item )
          _lastDocItem = sibling;
        delete item;
        DocMap.erase(it);
        return;
      }

      sibling = item;
      item = item->nextSibling();  
    }
  }
}


#include "moc_Tree.cpp"

