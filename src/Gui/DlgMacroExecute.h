/****************************************************************************
** Form interface generated from reading ui file '.\DlgMacroExecute.ui'
**
** Created: Mi 17. Mai 14:36:07 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGMACROEXECUTE_H
#define GUI_DIALOG_DLGMACROEXECUTE_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QGroupBox;
class QLineEdit;
class QListView;
class QListViewItem;
class QComboBox;

namespace Gui {
namespace Dialog {
class DlgMacroExecute : public QDialog
{
    Q_OBJECT

public:
    DlgMacroExecute( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgMacroExecute();

    QPushButton* CreateButton;
    QPushButton* DeleteButton;
    QPushButton* ExecuteButton;
    QPushButton* EditButton;
    QPushButton* CancelButton;
    QGroupBox* MacroNameGroup;
    QLineEdit* LineEditMacroName;
    QListView* MacrosListView;
    QGroupBox* DestinationGroup;
    QComboBox* ComboBoxPath;
    QPushButton* DestButton;
    QPushButton* DoneButton;

public slots:
    virtual void onCreate();
    virtual void onDelete();
    virtual void onEdit();
    virtual void onExecute();
    virtual void onNewFolder();

protected:
    QVBoxLayout* DlgMacroExecuteLayout;
    QGridLayout* Layout4;
    QGridLayout* Layout3;
    QVBoxLayout* MacroNameGroupLayout;
    QHBoxLayout* DestinationGroupLayout;

protected slots:
    virtual void languageChange();

    virtual void onNewListItemPicked(QListViewItem*);
    virtual void onListDoubleClicked(QListViewItem*);


};
}
}

#endif // GUI_DIALOG_DLGMACROEXECUTE_H
