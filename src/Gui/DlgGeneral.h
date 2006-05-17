/****************************************************************************
** Form interface generated from reading ui file '.\DlgGeneral.ui'
**
** Created: Mi 17. Mai 14:36:22 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGGENERALBASE_H
#define GUI_DIALOG_DLGGENERALBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
class QComboBox;
namespace Gui {
class PrefSpinBox;
}
namespace Gui {
class PrefCheckBox;
}
namespace Gui {
class PrefComboBox;
}

namespace Gui {
namespace Dialog {
class DlgGeneralBase : public QWidget
{
    Q_OBJECT

public:
    DlgGeneralBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgGeneralBase();

    QGroupBox* GroupBox5;
    QGroupBox* GroupBox7;
    QLabel* TextLabel1_4;
    QComboBox* Languages;
    QGroupBox* GroupBox3;
    QLabel* TextLabel1_3;
    Gui::PrefSpinBox* RecentFiles;
    QLabel* TextLabel1;
    QComboBox* WindowStyle;
    Gui::PrefCheckBox* UsesBigPixmaps;
    QGroupBox* GroupBox4;
    Gui::PrefCheckBox* AllowDragMenu;
    QGroupBox* GroupBox9;
    Gui::PrefCheckBox* ShowCmdLine;
    QLabel* TextLabel1_4_2;
    Gui::PrefSpinBox* SizeCmdLine;
    QGroupBox* GroupBox10;
    Gui::PrefCheckBox* SplashScreen;
    QLabel* textLabel1_2;
    Gui::PrefComboBox* AutoloadModuleCombo;
    Gui::PrefComboBox* AutoloadTabCombo;
    QLabel* textLabel1;

protected:
    QGridLayout* DlgGeneralBaseLayout;
    QGridLayout* GroupBox5Layout;
    QGridLayout* GroupBox7Layout;
    QHBoxLayout* layout4;
    QVBoxLayout* GroupBox3Layout;
    QHBoxLayout* layout2;
    QHBoxLayout* layout3;
    QGridLayout* GroupBox4Layout;
    QGridLayout* GroupBox9Layout;
    QHBoxLayout* layout1;
    QGridLayout* GroupBox10Layout;
    QGridLayout* layout7;

protected slots:
    virtual void languageChange();

    virtual void saveSettings();
    virtual void loadSettings();


};
}
}

#endif // GUI_DIALOG_DLGGENERALBASE_H
