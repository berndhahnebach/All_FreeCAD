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
# include <QHeaderView>
#endif

#include "DlgCommandsImp.h"
#include "Application.h"
#include "Command.h"
#include "BitmapFactory.h"
#include "Widgets.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgCustomCommandsImp */

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

    connect(commandTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), 
            this, SLOT(onDescription(QTreeWidgetItem*)));
    connect(categoryTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), 
            this, SLOT(onGroupActivated(QTreeWidgetItem*)));

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::map<std::string,Command*> sCommands = cCmdMgr.getCommands();

    // do a special sort before adding to the tree view
    QStringList groups;
    groups << QLatin1String("File") << QLatin1String("Edit")
           << QLatin1String("View") << QLatin1String("Standard-View")
           << QLatin1String("Tools") << QLatin1String("Window")
           << QLatin1String("Help") << QLatin1String("Macros");
    for (std::map<std::string,Command*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it) {
        QString group = QLatin1String(it->second->getGroupName());
        if (!groups.contains(group))
            groups << group;
    }

    QStringList labels; labels << tr("Category");
    categoryTreeWidget->setHeaderLabels(labels);
    for ( QStringList::Iterator It = groups.begin(); It != groups.end(); ++It ) {
        QTreeWidgetItem* item = new QTreeWidgetItem(categoryTreeWidget);
        item->setText(0, QObject::tr((*It).toAscii()));
        item->setData(0, Qt::UserRole, QVariant(*It));
    }

    labels.clear();
    labels << tr("Icon") << tr("Command");
    commandTreeWidget->setHeaderLabels(labels);
    commandTreeWidget->header()->hide();

    categoryTreeWidget->setCurrentItem(categoryTreeWidget->topLevelItem(0));
}

/** Destroys the object and frees any allocated resources */
DlgCustomCommandsImp::~DlgCustomCommandsImp()
{
}

/** Shows the description for the corresponding command */
void DlgCustomCommandsImp::onDescription(QTreeWidgetItem *item)
{
    if (item)
        textLabel->setText(item->toolTip(1));
    else
        textLabel->setText(QString());
}

/** Shows all commands of this category */
void DlgCustomCommandsImp::onGroupActivated(QTreeWidgetItem* item)
{
    if (!item) 
        return;

    QVariant data = item->data(0, Qt::UserRole);
    QString group = data.toString();
    commandTreeWidget->clear();

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::vector<Command*> aCmds = cCmdMgr.getGroupCommands(group.toAscii());
    for (std::vector<Command*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it) {
        QTreeWidgetItem* item = new QTreeWidgetItem(commandTreeWidget);
        item->setText(1, QObject::trUtf8((*it)->getMenuText()));
        item->setToolTip(1, QObject::trUtf8((*it)->getToolTipText()));
        item->setData(1, Qt::UserRole, QByteArray((*it)->getName()));
        item->setSizeHint(0, QSize(32, 32));
        item->setBackgroundColor(0, Qt::lightGray);
        if ((*it)->getPixmap())
            item->setIcon(0, BitmapFactory().pixmap((*it)->getPixmap()));
    }

    textLabel->setText(QString());
    commandTreeWidget->resizeColumnToContents(0);
}

void DlgCustomCommandsImp::onAddMacroAction(const QByteArray& macro)
{
    QTreeWidgetItem* item = categoryTreeWidget->currentItem();
    if (!item)
        return;

    QVariant data = item->data(0, Qt::UserRole);
    QString group = data.toString();
    if (group == QLatin1String("Macros"))
    {
        CommandManager & cCmdMgr = Application::Instance->commandManager();
        Command* pCmd = cCmdMgr.getCommandByName(macro);

        QTreeWidgetItem* item = new QTreeWidgetItem(commandTreeWidget);
        item->setText(1, QString::fromUtf8(pCmd->getMenuText()));
        item->setToolTip(1, QString::fromUtf8(pCmd->getToolTipText()));
        item->setData(1, Qt::UserRole, macro);
        item->setSizeHint(0, QSize(32, 32));
        item->setBackgroundColor(0, Qt::lightGray);
        if (pCmd->getPixmap())
            item->setIcon(0, BitmapFactory().pixmap(pCmd->getPixmap()));
    }
}

void DlgCustomCommandsImp::onRemoveMacroAction(const QByteArray& macro)
{
    QTreeWidgetItem* item = categoryTreeWidget->currentItem();
    if (!item)
        return;

    QVariant data = item->data(0, Qt::UserRole);
    QString group = data.toString();
    if (group == QLatin1String("Macros"))
    {
        for (int i=0; i<commandTreeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem* item = commandTreeWidget->topLevelItem(i);
            QByteArray command = item->data(1, Qt::UserRole).toByteArray();
            if (command == macro) {
                commandTreeWidget->takeTopLevelItem(i);
                delete item;
                break;
            }
        }
    }
}

void DlgCustomCommandsImp::onModifyMacroAction(const QByteArray& macro)
{
    QTreeWidgetItem* item = categoryTreeWidget->currentItem();
    if (!item)
        return;

    QVariant data = item->data(0, Qt::UserRole);
    QString group = data.toString();
    if (group == QLatin1String("Macros"))
    {
        CommandManager & cCmdMgr = Application::Instance->commandManager();
        Command* pCmd = cCmdMgr.getCommandByName(macro);
        for (int i=0; i<commandTreeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem* item = commandTreeWidget->topLevelItem(i);
            QByteArray command = item->data(1, Qt::UserRole).toByteArray();
            if (command == macro) {
                item->setText(1, QString::fromUtf8(pCmd->getMenuText()));
                item->setToolTip(1, QString::fromUtf8(pCmd->getToolTipText()));
                item->setData(1, Qt::UserRole, macro);
                item->setSizeHint(0, QSize(32, 32));
                item->setBackgroundColor(0, Qt::lightGray);
                if (pCmd->getPixmap())
                    item->setIcon(0, BitmapFactory().pixmap(pCmd->getPixmap()));
                if (commandTreeWidget->isItemSelected(item))
                    onDescription(item);
                break;
            }
        }
    }
}

#include "moc_DlgCommandsImp.cpp"
