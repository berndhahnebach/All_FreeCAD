/****************************************************************************
** Form interface generated from reading ui file '.\DlgSettingsColorGradient.ui'
**
** Created: Sa 13. Mai 12:36:02 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGSETTINGSCOLORGRADIENTBASE_H
#define GUI_DIALOG_DLGSETTINGSCOLORGRADIENTBASE_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QCheckBox;
class QButtonGroup;
class QRadioButton;
class QLabel;
class QComboBox;
class QPushButton;
namespace Gui {
class FloatSpinBox;
}
namespace Gui {
class SpinBox;
}

namespace Gui {
namespace Dialog {
class DlgSettingsColorGradientBase : public QDialog
{
    Q_OBJECT

public:
    DlgSettingsColorGradientBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgSettingsColorGradientBase();

    QGroupBox* groupBoxVisible;
    QCheckBox* checkBoxGrayed;
    QCheckBox* checkBoxInvisible;
    QButtonGroup* buttonGroupStyle;
    QRadioButton* radioButtonFlow;
    QRadioButton* radioButtonZero;
    QGroupBox* groupBoxModel;
    QLabel* textLabel5;
    QComboBox* comboBoxModel;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QGroupBox* groupBoxRange;
    QLabel* textLabelMax;
    Gui::FloatSpinBox* floatSpinBoxMax;
    QLabel* textLabelMin;
    Gui::FloatSpinBox* floatSpinBoxMin;
    QLabel* textLabel1;
    Gui::SpinBox* spinBoxLabel;

protected:
    QGridLayout* DlgSettingsColorGradientBaseLayout;
    QHBoxLayout* groupBoxVisibleLayout;
    QHBoxLayout* buttonGroupStyleLayout;
    QHBoxLayout* groupBoxModelLayout;
    QHBoxLayout* layout12;
    QHBoxLayout* layout9;
    QGridLayout* groupBoxRangeLayout;
    QHBoxLayout* layout4;

protected slots:
    virtual void languageChange();

    virtual void onValidateMaximum();
    virtual void onValidateMinimum();


};
}
}

#endif // GUI_DIALOG_DLGSETTINGSCOLORGRADIENTBASE_H
