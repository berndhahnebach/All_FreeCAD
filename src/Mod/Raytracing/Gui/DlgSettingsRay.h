/****************************************************************************
** Form interface generated from reading ui file '.\DlgSettingsRay.ui'
**
** Created: Mi 17. Mai 14:48:41 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef RAYTRACINGGUI_DLGSETTINGSRAY_H
#define RAYTRACINGGUI_DLGSETTINGSRAY_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
namespace Gui {
class PrefFileChooser;
}
namespace Gui {
class PrefLineEdit;
}
namespace Gui {
class PrefFloatSpinBox;
}
namespace Gui {
class PrefCheckBox;
}

namespace RaytracingGui {
class DlgSettingsRay : public QWidget
{
    Q_OBJECT

public:
    DlgSettingsRay( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgSettingsRay();

    QGroupBox* GroupBox12;
    QGroupBox* groupBox4;
    QLabel* textLabel1;
    QLabel* textLabel1_2;
    QLabel* textLabel1_3;
    Gui::PrefFileChooser* prefFileChooser1;
    Gui::PrefLineEdit* prefLineEdit2;
    Gui::PrefLineEdit* prefLineEdit3;
    QGroupBox* groupBox5;
    QLabel* textLabel2;
    Gui::PrefFloatSpinBox* prefFloatSpinBox1;
    Gui::PrefCheckBox* prefCheckBox8;
    Gui::PrefCheckBox* prefCheckBox9;

public slots:
    virtual void warnInventor(bool);

protected:
    QGridLayout* DlgSettingsRayLayout;
    QVBoxLayout* GroupBox12Layout;
    QHBoxLayout* groupBox4Layout;
    QVBoxLayout* layout3;
    QVBoxLayout* layout4;
    QVBoxLayout* groupBox5Layout;
    QHBoxLayout* layout4_2;

protected slots:
    virtual void languageChange();

    virtual void loadSettings();
    virtual void saveSettings();


};
}

#endif // RAYTRACINGGUI_DLGSETTINGSRAY_H
