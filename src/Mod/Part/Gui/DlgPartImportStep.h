/****************************************************************************
** Form interface generated from reading ui file '.\DlgPartImportStep.ui'
**
** Created: Do 9. Dez 23:03:05 2004
**      by: The User Interface Compiler ($Id$)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DLGPARTIMPORTSTEP_H
#define DLGPARTIMPORTSTEP_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLineEdit;
class QPushButton;

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

#endif // DLGPARTIMPORTSTEP_H
