/****************************************************************************
** Form interface generated from reading ui file '.\DlgPartImportStep.ui'
**
** Created: Sa 13. Mai 12:40:17 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PARTGUI_DLGPARTIMPORTSTEP_H
#define PARTGUI_DLGPARTIMPORTSTEP_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLineEdit;
class QPushButton;

namespace PartGui {
class DlgPartImportStep : public QDialog
{
    Q_OBJECT

public:
    DlgPartImportStep( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgPartImportStep();

    QGroupBox* GroupBox5;
    QLineEdit* FileName;
    QPushButton* SearchFile;
    QPushButton* CancelButton;
    QPushButton* OKButton;

public slots:
    virtual void OnApply();
    virtual void onChooseFileName();

protected:
    QVBoxLayout* DlgPartImportStepLayout;
    QHBoxLayout* GroupBox5Layout;
    QGridLayout* Layout13;

protected slots:
    virtual void languageChange();

};
}

#endif // PARTGUI_DLGPARTIMPORTSTEP_H
