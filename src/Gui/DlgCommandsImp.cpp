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

#include "DlgCommandsImp.h"
#include "Application.h"
#include "Command.h"
#include "BitmapFactory.h"
#include "Widgets.h"

using namespace Gui::Dialog;

/**
 *  Constructs a DlgCustomCommandsImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomCommandsImp::DlgCustomCommandsImp( QWidget* parent  )
  : CustomizeActionPage(parent)
{
    this->setupUi(this);
  
    // paints for active and inactive the same color
    QPalette pal = categoryTreeWidget->palette();
    pal.setColor(QPalette::Inactive, QPalette::Highlight, pal.color(QPalette::Active, QPalette::Highlight));
    pal.setColor(QPalette::Inactive, QPalette::HighlightedText, pal.color(QPalette::Active, QPalette::HighlightedText));
    categoryTreeWidget->setPalette( pal );

    connect(iconView, SIGNAL(emitSelectionChanged(const QString &)), 
            this, SLOT(onDescription(const QString &)));
    connect(categoryTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), 
            this, SLOT(onGroupActivated(QTreeWidgetItem*)));

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::map<std::string,Command*> sCommands = cCmdMgr.getCommands();

    // do a special sort before adding to the tree view
    QStringList items; items << "File" << "Edit" << "View" << "Standard-View" << "Tools" << "Window" << "Help" << "Macros";
    for (std::map<std::string,Command*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it) {
        QString group = it->second->getGroupName();
        if (!items.contains(group))
            items << group;
    }

    QStringList labels; labels << tr("Category");
    categoryTreeWidget->setHeaderLabels(labels);
    for ( QStringList::Iterator It = items.begin(); It != items.end(); ++It ) {
        QTreeWidgetItem* item = new QTreeWidgetItem(categoryTreeWidget);
        item->setText(0, QObject::tr((*It).toAscii()));
        item->setData(0, Qt::UserRole, QVariant(*It));
    }

    categoryTreeWidget->setCurrentItem(categoryTreeWidget->topLevelItem(0));
}

/** Destroys the object and frees any allocated resources */
DlgCustomCommandsImp::~DlgCustomCommandsImp()
{
}

/** Shows the description for the corresponding command */
void DlgCustomCommandsImp::onDescription(const QString& txt)
{
    textLabel->setText( txt );
}

/** Shows all commands of this category */
void DlgCustomCommandsImp::onGroupActivated( QTreeWidgetItem* item)
{
    if (!item) 
        return;

    QVariant data = item->data(0, Qt::UserRole);
    QString group = data.toString();
    iconView->clear();

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::vector<Command*> aCmds = cCmdMgr.getGroupCommands( group.toAscii() );
    for (std::vector<Command*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it) {
        QPixmap pix;
        if ( (*it)->getPixmap() )
            pix = BitmapFactory().pixmap( (*it)->getPixmap() );
        else 
            pix = BitmapFactory().pixmap( "px" );
        QListWidgetItem* item = new QListWidgetItem(iconView);
        item->setText((*it)->getName());
        item->setToolTip(QObject::tr((*it)->getToolTipText()));
        item->setIcon(pix);
    }
}

void DlgCustomCommandsImp::onAddMacroAction(const QString& macro)
{
    QTreeWidgetItem* item = categoryTreeWidget->currentItem();
    if (!item)
        return;

    QVariant data = item->data(0, Qt::UserRole);
    QString group = data.toString();
    if (group == "Macros")
    {
        CommandManager & cCmdMgr = Application::Instance->commandManager();
        Command* pCmd = cCmdMgr.getCommandByName(macro.toAscii());
        QPixmap pix;
        if ( pCmd->getPixmap() )
            pix = BitmapFactory().pixmap( pCmd->getPixmap() );
        else 
            pix = BitmapFactory().pixmap( "px" );
        QListWidgetItem* viewItem = new QListWidgetItem(iconView);
        viewItem->setText(pCmd->getName());
        viewItem->setToolTip(QObject::tr(pCmd->getToolTipText()));
        viewItem->setIcon(pix);
        iconView->sortItems();
    }
}

void DlgCustomCommandsImp::onRemoveMacroAction(const QString& macro)
{
    QTreeWidgetItem* item = categoryTreeWidget->currentItem();
    if (!item)
        return;

    QVariant data = item->data(0, Qt::UserRole);
    QString group = data.toString();
    if (group == "Macros")
    {
        QList<QListWidgetItem*> items = iconView->findItems(macro, Qt::MatchExactly);
        for (QList<QListWidgetItem*>::iterator it = items.begin(); it != items.end(); ++it) {
            int row = iconView->row(*it);
            iconView->takeItem(row);
            delete *it;
        }
        iconView->sortItems();
    }
}

#include "moc_DlgCommandsImp.cpp"
