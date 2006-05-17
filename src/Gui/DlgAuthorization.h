/****************************************************************************
** Form interface generated from reading ui file '.\DlgAuthorization.ui'
**
** Created: Sa 13. Mai 12:35:57 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGAUTHORIZATION_H
#define GUI_DIALOG_DLGAUTHORIZATION_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QLineEdit;
class QLabel;

namespace Gui {
namespace Dialog {
class DlgAuthorization : public QDialog
{
    Q_OBJECT

public:
    DlgAuthorization( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgAuthorization();

    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QLineEdit* password;
    QLabel* textLabel2;
    QLineEdit* username;
    QLabel* textLabel1;

protected:
    QGridLayout* DlgAuthorizationLayout;
    QHBoxLayout* layout2;

protected slots:
    virtual void languageChange();

};
}
}

#endif // GUI_DIALOG_DLGAUTHORIZATION_H
