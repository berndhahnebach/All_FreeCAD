/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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

#include <Base/Parameter.h>

#include "DlgKeyboardImp.h"
#include "Action.h"
#include "Application.h"
#include "BitmapFactory.h"
#include "Command.h"
#include "Tools.h"
#include "Widgets.h"
#include "Window.h"

using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgCustomKeyboardImp */

/**
 *  Constructs a DlgCustomKeyboardImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgCustomKeyboardImp::DlgCustomKeyboardImp( QWidget* parent  )
  : CustomizeActionPage(parent), firstShow(true)
{
    this->setupUi(this);

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::map<std::string,Command*> sCommands = cCmdMgr.getCommands();

    // do a special sort before adding to the tree view
    QStringList items; items << "File" << "Edit" << "View" << "Standard-View" << "Tools" << "Window" << "Help" << "Macros";
    for (std::map<std::string,Command*>::iterator it = sCommands.begin(); it != sCommands.end(); ++it) {
        QString group = it->second->getGroupName();
        if (!items.contains(group))
            items << group;
    }

    int index = 0;
    for ( QStringList::Iterator It = items.begin(); It != items.end(); ++It, ++index ) {
        categoryBox->addItem(QObject::tr((*It).toAscii()));
        categoryBox->setItemData(index, QVariant(*It), Qt::UserRole);
    }

    QStringList labels; 
    labels << "Icon" << "Command";
    commandTreeWidget->setHeaderLabels(labels);
    commandTreeWidget->header()->hide();
    assignedTreeWidget->setHeaderLabels(labels);
    assignedTreeWidget->header()->hide();
}

/** Destroys the object and frees any allocated resources */
DlgCustomKeyboardImp::~DlgCustomKeyboardImp()
{
}

void DlgCustomKeyboardImp::showEvent(QShowEvent* e)
{
    // If we did this already in the constructor we wouldn't get the vertical scrollbar if needed.
    // The problem was noticed with Qt 4.1.4 but may arise with any later version.
    if (firstShow) {
        on_categoryBox_activated(categoryBox->currentIndex());
        firstShow = false;
    }
}

/** Shows the description for the corresponding command */
void DlgCustomKeyboardImp::on_commandTreeWidget_currentItemChanged(QTreeWidgetItem* item)
{
    if (!item)
        return;

    QVariant data = item->data(1, Qt::UserRole);
    QString name = data.toString(); // command name

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    Command* cmd = cCmdMgr.getCommandByName(name.toAscii());
    if (cmd) {
        if (cmd->getAction()) {
            QKeySequence ks = cmd->getAction()->shortcut();
            QKeySequence ks2 = cmd->getAccel();
            QKeySequence ks3 = editShortcut->text();

            if (ks.isEmpty())
                accelLineEditShortcut->setText( tr("none") );
            else
                accelLineEditShortcut->setText(ks);

            buttonAssign->setEnabled(!editShortcut->text().isEmpty() && (ks != ks3));
            buttonReset->setEnabled((ks != ks2));
        } else {
            QKeySequence ks = cmd->getAccel();
            if (ks.isEmpty())
                accelLineEditShortcut->setText( tr("none") );
            else
                accelLineEditShortcut->setText(ks);
            buttonAssign->setEnabled(false);
            buttonReset->setEnabled(false);
        }
    }

    textLabelDescription->setText(item->toolTip(1));
}

/** Shows all commands of this category */
void DlgCustomKeyboardImp::on_categoryBox_activated(int index)
{
    QVariant data = categoryBox->itemData(index, Qt::UserRole);
    QString group = data.toString();
    commandTreeWidget->clear();
    buttonAssign->setEnabled(false);
    buttonReset->setEnabled(false);
    accelLineEditShortcut->clear();
    editShortcut->clear();

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::vector<Command*> aCmds = cCmdMgr.getGroupCommands( group.toAscii() );
    for (std::vector<Command*>::iterator it = aCmds.begin(); it != aCmds.end(); ++it) {
        QTreeWidgetItem* item = new QTreeWidgetItem(commandTreeWidget);
        item->setText(1, QObject::tr((*it)->getMenuText()));
        item->setToolTip(1, QObject::tr((*it)->getToolTipText()));
        item->setData(1, Qt::UserRole, QString((*it)->getName()));
        item->setSizeHint(0, QSize(32, 32));
        item->setBackgroundColor(0, Qt::lightGray);
        if ((*it)->getPixmap())
            item->setIcon(0, BitmapFactory().pixmap((*it)->getPixmap()));
    }

    commandTreeWidget->resizeColumnToContents(0);
}

/** Assigns a new accelerator to the selected command. */
void DlgCustomKeyboardImp::on_buttonAssign_clicked()
{
    QTreeWidgetItem* item = commandTreeWidget->currentItem();
    if (!item)
        return;

    QVariant data = item->data(1, Qt::UserRole);
    QByteArray name = data.toString().toAscii(); // command name

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    Command* cmd = cCmdMgr.getCommandByName(name.constData());
    if (cmd && cmd->getAction()) {
        QKeySequence shortcut = editShortcut->text();
        cmd->getAction()->setShortcut(shortcut);
        accelLineEditShortcut->setText(editShortcut->text());
        editShortcut->clear();

        ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Shortcut");
        hGrp->SetASCII(name.constData(), editShortcut->text().toAscii());
        buttonAssign->setEnabled(false);
        buttonReset->setEnabled(true);
    }
}

/** Resets the accelerator of the selected command to the default. */
void DlgCustomKeyboardImp::on_buttonReset_clicked()
{
    QTreeWidgetItem* item = commandTreeWidget->currentItem();
    if (!item)
        return;

    QVariant data = item->data(1, Qt::UserRole);
    QByteArray name = data.toString().toAscii(); // command name

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    Command* cmd = cCmdMgr.getCommandByName(name.constData());
    if (cmd && cmd->getAction()) {
        cmd->getAction()->setShortcut(cmd->getAccel());
        QString txt = cmd->getAction()->shortcut();
        accelLineEditShortcut->setText((txt.isEmpty() ? tr("none") : txt));
        ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Shortcut");
        hGrp->RemoveASCII(name.constData());
    }

    buttonReset->setEnabled( false );
}

/** Resets the accelerator of all commands to the default. */
void DlgCustomKeyboardImp::on_buttonResetAll_clicked()
{
    CommandManager & cCmdMgr = Application::Instance->commandManager();
    std::vector<Command*> cmds = cCmdMgr.getAllCommands();
    for (std::vector<Command*>::iterator it = cmds.begin(); it != cmds.end(); ++it) {
        if ((*it)->getAction()) {
            (*it)->getAction()->setShortcut(QKeySequence((*it)->getAccel()));
        }
    }

    WindowParameter::getDefaultParameter()->RemoveGrp("Shortcut");
    buttonReset->setEnabled(false);
}

/** Checks for an already occupied shortcut. */
void DlgCustomKeyboardImp::on_editShortcut_textChanged(const QString& sc)
{
    assignedTreeWidget->clear();
    QTreeWidgetItem* item = commandTreeWidget->currentItem();
    if (!item)
        return;
    QVariant data = item->data(1, Qt::UserRole);
    QString name = data.toString(); // command name

    CommandManager & cCmdMgr = Application::Instance->commandManager();
    Command* cmd = cCmdMgr.getCommandByName(name.toAscii().constData());
    if (cmd && !cmd->getAction()) {
        buttonAssign->setEnabled(false); // command not in use
        return;
    }

    buttonAssign->setEnabled(true);
    QKeySequence ks(sc);
    if (!ks.isEmpty()) {
        int countAmbiguous = 0;
        QString ambiguousCommand;
        QString ambiguousMenu;

        CommandManager & cCmdMgr = Application::Instance->commandManager();
        std::vector<Command*> cmds = cCmdMgr.getAllCommands();
        for (std::vector<Command*>::iterator it = cmds.begin(); it != cmds.end(); ++it) {
            if ((*it)->getAction() && (*it)->getAction()->shortcut() == ks) {
                ++countAmbiguous;
                ambiguousCommand = (*it)->getName(); // store the last one
                ambiguousMenu = QObject::tr((*it)->getMenuText());
        
                QTreeWidgetItem* item = new QTreeWidgetItem(assignedTreeWidget);
                item->setText(1, QObject::tr((*it)->getMenuText()));
                item->setToolTip(1, QObject::tr((*it)->getToolTipText()));
                item->setData(1, Qt::UserRole, QString((*it)->getName()));
                item->setSizeHint(0, QSize(32, 32));
                item->setBackgroundColor(0, Qt::lightGray);
                if ((*it)->getPixmap())
                    item->setIcon(0, BitmapFactory().pixmap((*it)->getPixmap()));
            }
        }

        if (countAmbiguous > 0)
            assignedTreeWidget->resizeColumnToContents(0);
        
        if (countAmbiguous > 1) {
            QMessageBox::warning(this, tr("Multiple defined shortcut"),
                                 tr("The shortcut '%1' is defined more than once. This could result into unexpected behaviour.").arg(sc) );
            editShortcut->setFocus();
            buttonAssign->setEnabled(false);
        } else if (countAmbiguous == 1 && ambiguousCommand != name) {
            QMessageBox::warning(this, tr("Already defined shortcut"),
                                 tr("The shortcut '%1' is already assigned to '%2'.\n\nPlease define another shortcut.").arg(sc).arg(ambiguousMenu) );
            editShortcut->setFocus();
            buttonAssign->setEnabled(false);
        } else {
            if (cmd && cmd->getAction() && cmd->getAction()->shortcut() == ks)
                buttonAssign->setEnabled(false);
        }
    } else {
        if (cmd && cmd->getAction() && cmd->getAction()->shortcut().isEmpty())
            buttonAssign->setEnabled(false); // both key sequences are empty
    }
}

void DlgCustomKeyboardImp::onAddMacroAction(const QString& macro)
{
    QVariant data = categoryBox->itemData(categoryBox->currentIndex(), Qt::UserRole);
    QString group = data.toString();
    if (group == "Macros")
    {
        CommandManager & cCmdMgr = Application::Instance->commandManager();
        Command* pCmd = cCmdMgr.getCommandByName(macro.toAscii());

        QTreeWidgetItem* item = new QTreeWidgetItem(commandTreeWidget);
        item->setText(1, pCmd->getMenuText());
        item->setToolTip(1, pCmd->getToolTipText());
        item->setData(1, Qt::UserRole, QString(pCmd->getName()));
        item->setSizeHint(0, QSize(32, 32));
        item->setBackgroundColor(0, Qt::lightGray);
        if (pCmd->getPixmap())
            item->setIcon(0, BitmapFactory().pixmap(pCmd->getPixmap()));
    }
}

void DlgCustomKeyboardImp::onRemoveMacroAction(const QString& macro)
{
    QVariant data = categoryBox->itemData(categoryBox->currentIndex(), Qt::UserRole);
    QString group = data.toString();
    if (group == "Macros")
    {
        for (int i=0; i<commandTreeWidget->topLevelItemCount(); i++) {
            QTreeWidgetItem* item = commandTreeWidget->topLevelItem(i);
            QString command = item->data(1, Qt::UserRole).toString();
            if (command == macro) {
                commandTreeWidget->takeTopLevelItem(i);
                delete item;
                break;
            }
        }
    }
}

#include "moc_DlgKeyboardImp.cpp"
