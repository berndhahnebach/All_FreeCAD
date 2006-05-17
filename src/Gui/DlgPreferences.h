/****************************************************************************
** Form interface generated from reading ui file '.\DlgPreferences.ui'
**
** Created: Mi 17. Mai 14:36:20 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGPREFERENCES_H
#define GUI_DIALOG_DLGPREFERENCES_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QListBox;
class QListBoxItem;
class QWidgetStack;
class QWidget;

namespace Gui {
namespace Dialog {
class DlgPreferences : public QDialog
{
    Q_OBJECT

public:
    DlgPreferences( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgPreferences();

    QPushButton* buttonOk;
    QPushButton* buttonApply;
    QPushButton* buttonCancel;
    QListBox* listBox;
    QWidgetStack* tabWidgetStack;
    QWidget* WStackPage;

protected:
    QGridLayout* DlgPreferencesLayout;
    QHBoxLayout* layout9;
    QHBoxLayout* layout3;

protected slots:
    virtual void languageChange();

    virtual void onPrefPageClicked(int);
    virtual void apply();


};
}
}

#endif // GUI_DIALOG_DLGPREFERENCES_H
