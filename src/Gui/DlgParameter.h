/****************************************************************************
** Form interface generated from reading ui file '.\DlgParameter.ui'
**
** Created: Sa 13. Mai 12:35:56 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGPARAMETER_H
#define GUI_DIALOG_DLGPARAMETER_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QSplitter;
class QComboBox;

namespace Gui {
namespace Dialog {
class DlgParameter : public QDialog
{
    Q_OBJECT

public:
    DlgParameter( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgParameter();

    QPushButton* buttonOk;
    QSplitter* splitter3;
    QComboBox* SetNameComboBox;

protected:
    QGridLayout* DlgParameterLayout;
    QHBoxLayout* Layout1;

protected slots:
    virtual void languageChange();

};
}
}

#endif // GUI_DIALOG_DLGPARAMETER_H
