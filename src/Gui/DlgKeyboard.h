/****************************************************************************
** Form interface generated from reading ui file '.\DlgKeyboard.ui'
**
** Created: Mi 17. Mai 14:36:12 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGCUSTOMKEYBOARDBASE_H
#define GUI_DIALOG_DLGCUSTOMKEYBOARDBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QComboBox;
class QListBox;
class QListBoxItem;
namespace Gui {
class AccelLineEdit;
}
class QPushButton;

namespace Gui {
namespace Dialog {
class DlgCustomKeyboardBase : public QWidget
{
    Q_OBJECT

public:
    DlgCustomKeyboardBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgCustomKeyboardBase();

    QLabel* textLabelAssignedShortcut;
    QLabel* TextLabelCategory;
    QComboBox* comboBoxCategory;
    QLabel* TextLabelCommands;
    QListBox* listBoxCommands;
    QLabel* textLabelShortcut;
    Gui::AccelLineEdit* accelLineEditShortcut;
    QLabel* textLabelNewShortcut;
    Gui::AccelLineEdit* accelLineEdit1NewShortcut;
    QLabel* textLabelAssigned;
    QListBox* listBoxAssigned;
    QPushButton* pushButtonAssign;
    QPushButton* pushButtonReset;
    QPushButton* pushButtonResetAll;
    QLabel* textLabelDescriptionHeader;
    QLabel* textLabelDescription;

protected:
    QGridLayout* DlgCustomKeyboardBaseLayout;
    QVBoxLayout* layout4;
    QVBoxLayout* layout4_2;
    QVBoxLayout* layout4_3;
    QVBoxLayout* layout7;

protected slots:
    virtual void languageChange();

    virtual void onGroupSelected( const QString & );
    virtual void onDescription( const QString & );
    virtual void onAssign();
    virtual void onReset();
    virtual void onResetAll();
    virtual void onShortcutPressed(const QString&);


};
}
}

#endif // GUI_DIALOG_DLGCUSTOMKEYBOARDBASE_H
