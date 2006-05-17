/****************************************************************************
** Form interface generated from reading ui file '.\DlgCommands.ui'
**
** Created: Mi 17. Mai 14:36:13 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGCUSTOMCOMMANDSBASE_H
#define GUI_DIALOG_DLGCUSTOMCOMMANDSBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
namespace Gui {
class CommandIconView;
}
class QLabel;
class QListBox;
class QListBoxItem;

namespace Gui {
namespace Dialog {
class DlgCustomCommandsBase : public QWidget
{
    Q_OBJECT

public:
    DlgCustomCommandsBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgCustomCommandsBase();

    QGroupBox* GroupBox2;
    Gui::CommandIconView* IconView1;
    QLabel* TextLabel;
    QLabel* TextLabelCommands;
    QListBox* ComboBoxCategory;
    QLabel* TextLabelCategory;

protected:
    QGridLayout* DlgCustomCommandsBaseLayout;
    QGridLayout* GroupBox2Layout;

protected slots:
    virtual void languageChange();

};
}
}

#endif // GUI_DIALOG_DLGCUSTOMCOMMANDSBASE_H
