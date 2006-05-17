/****************************************************************************
** Form interface generated from reading ui file '.\DlgPartBox.ui'
**
** Created: Mi 17. Mai 14:44:49 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PARTGUI_DLGPARTBOX_H
#define PARTGUI_DLGPARTBOX_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace PartGui {
class DlgPartBox : public QDialog
{
    Q_OBJECT

public:
    DlgPartBox( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgPartBox();

    QGroupBox* GroupBox5;
    QLabel* TextLabel1;
    QLineEdit* XLineEdit;
    QLineEdit* YLineEdit;
    QLineEdit* ZLineEdit;
    QLabel* TextLabel3;
    QLabel* TextLabel2;
    QGroupBox* GroupBox5_2;
    QLineEdit* ULineEdit;
    QLineEdit* VLineEdit;
    QLineEdit* WLineEdit;
    QLabel* TextLabel1_2;
    QLabel* TextLabel2_2;
    QLabel* TextLabel3_2;
    QPushButton* ApplyButton;
    QPushButton* CancelButton;
    QPushButton* OKButton;

public slots:
    virtual void OnApply();

protected:
    QVBoxLayout* DlgPartBoxLayout;
    QGridLayout* GroupBox5Layout;
    QGridLayout* GroupBox5_2Layout;
    QGridLayout* Layout13;

protected slots:
    virtual void languageChange();

};
}

#endif // PARTGUI_DLGPARTBOX_H
