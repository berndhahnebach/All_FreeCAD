/****************************************************************************
** Form interface generated from reading ui file '.\DlgPointsRead.ui'
**
** Created: Mi 17. Mai 14:50:08 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef POINTSGUI_DLGPOINTSREAD_H
#define POINTSGUI_DLGPOINTSREAD_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QComboBox;
class QGroupBox;
class QButtonGroup;
class QRadioButton;
class QLineEdit;
class QTextEdit;

namespace PointsGui {
class DlgPointsRead : public QDialog
{
    Q_OBJECT

public:
    DlgPointsRead( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgPointsRead();

    QLabel* textLabel1;
    QComboBox* comboBoxTrmplate;
    QGroupBox* groupBox1;
    QLabel* textLabel2;
    QButtonGroup* buttonGroup2;
    QRadioButton* radioButtonIgnore;
    QRadioButton* radioButtonNbrOfPoints;
    QLabel* textLabel3;
    QLabel* textLabel4;
    QLineEdit* lineEditClusterStart;
    QLineEdit* lineEditComments;
    QGroupBox* groupBox2;
    QLabel* textLabel6;
    QLabel* textLabel7;
    QLabel* textLabel8;
    QLabel* textLabel8_2;
    QLabel* textLabel8_2_2;
    QComboBox* comboBoxSeperator;
    QComboBox* comboBoxPointsFormat;
    QComboBox* comboBoxNextBlock1;
    QComboBox* comboBoxNextBlock2;
    QComboBox* comboBoxNextBlock3;
    QGroupBox* groupBox3;
    QLineEdit* lineEditFileName;
    QLabel* textLabel5;
    QLineEdit* lineEditPreviewLines;
    QTextEdit* textEditPreview;

protected:
    QVBoxLayout* DlgPointsReadLayout;
    QHBoxLayout* layout5;
    QVBoxLayout* groupBox1Layout;
    QHBoxLayout* layout1;
    QHBoxLayout* buttonGroup2Layout;
    QHBoxLayout* layout4;
    QVBoxLayout* layout3;
    QVBoxLayout* layout2;
    QVBoxLayout* groupBox2Layout;
    QHBoxLayout* layout9;
    QVBoxLayout* layout8;
    QVBoxLayout* layout7;
    QVBoxLayout* groupBox3Layout;
    QHBoxLayout* layout6;

protected slots:
    virtual void languageChange();

};
}

#endif // POINTSGUI_DLGPOINTSREAD_H
