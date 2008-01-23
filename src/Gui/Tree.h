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


#ifndef GUI_TREE_H
#define GUI_TREE_H

#ifndef __Qt4All__
# include "Qt4All.h"
#endif

#include <Base/PyExport.h>
#include <App/Document.h>
#include <App/Application.h>

#include "Selection.h"
#include "DockWindow.h"

namespace Gui {

class ViewProviderDocumentObject;
class DocumentObjectItem;

class TreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TreeWidget(QWidget* parent=0);
    ~TreeWidget();

    static const int ObjectType;

protected:
    void drawRow(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const;
};

/** The link between the tree and a document.
 * Every document in the application gets its associated DocumentItem which controls 
 * the visibility and the functions of the document.
 * \author Jürgen Riegel
 */
class DocumentItem : public QTreeWidgetItem
{
public:
    DocumentItem(Gui::Document* doc, QTreeWidgetItem * parent);
    ~DocumentItem();

    void setObjectHighlighted(const char*, bool);
    void setObjectSelected(const char*, bool);
    void clearSelection(void);
    void updateSelection(void);
    void testStatus(void);

protected:
    /** Adds a view provider to the document item.
     * If this view provider is already added nothing happens.
     */
    void slotNewObject(Gui::ViewProviderDocumentObject&);
    /** Removes a view provider from the document item.
     * If this view provider is not added nothing happens.
     */
    void slotDeleteObject(Gui::ViewProviderDocumentObject&);
    void slotChangeObject(Gui::ViewProviderDocumentObject&);
    void slotRenameObject(Gui::ViewProviderDocumentObject&);
    void slotActiveObject(Gui::ViewProviderDocumentObject&);

private:
    Gui::Document* pDocument;
    std::map<std::string,DocumentObjectItem*> ObjectMap;
};

/** The link between the tree and a document object.
 * Every object in the document gets its associated DocumentObjectItem which controls 
 * the visibility and the functions of the object.
 * @author Werner Mayer
 */
class DocumentObjectItem : public QTreeWidgetItem
{
public:
    DocumentObjectItem(Gui::ViewProviderDocumentObject* pcViewProvider, QTreeWidgetItem * parent);
    ~DocumentObjectItem();

    Gui::ViewProviderDocumentObject* object() const;
    void testStatus();
    void displayStatusInfo();

private:
    int previousStatus;
    Gui::ViewProviderDocumentObject* viewObject;

    friend class TreeWidget;
};

/**
 * The dock window containing the tree view.
 * @author Werner Mayer
 */
class TreeDockWidget : public Gui::DockWindow, public Gui::SelectionSingleton::ObserverType
{
    Q_OBJECT

public:
    TreeDockWidget(Gui::Document*  pcDocument,QWidget *parent=0);
    ~TreeDockWidget();

    const char *getName(void) const {return "TreeView";}

    /// Observer message from the Selection
    void OnChange(Gui::SelectionSingleton::SubjectType &,Gui::SelectionSingleton::MessageType);
    void scrollItemToTop(Gui::Document*);

public Q_SLOTS:
    void onItemSelectionChanged(void);
    void onItemEntered(QTreeWidgetItem * item);
    void onTestStatus(void);

protected:
    void slotNewDocument(Gui::Document&);
    void slotDeleteDocument(Gui::Document&);
    void slotRenameDocument(Gui::Document&);
    void slotActiveDocument(Gui::Document&);

    void contextMenuEvent ( QContextMenuEvent * e );
    void changeEvent(QEvent *e);

private:
    QTreeWidgetItem* rootItem;
    QTreeWidget* treeWidget;
    QTimer* statusTimer;
    static QPixmap* documentPixmap;
    std::map<Gui::Document*,DocumentItem*> DocumentMap;
    bool fromOutside;
};

}


#endif // GUI_TREE_H

