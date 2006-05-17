/****************************************************************************
** Form interface generated from reading ui file '.\DlgActivateWindow.ui'
**
** Created: Mi 17. Mai 14:36:10 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGACTIVATEWINDOWBASE_H
#define GUI_DIALOG_DLGACTIVATEWINDOWBASE_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QListBox;
class QListBoxItem;

namespace Gui {
namespace Dialog {
class DlgActivateWindowBase : public QDialog
{
    Q_OBJECT

public:
    DlgActivateWindowBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgActivateWindowBase();

    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QListBox* listBox2;

protected:
    QGridLayout* DlgActivateWindowBaseLayout;
    QHBoxLayout* layout2;

protected slots:
    virtual void languageChange();

};
}
}

#endif // GUI_DIALOG_DLGACTIVATEWINDOWBASE_H
