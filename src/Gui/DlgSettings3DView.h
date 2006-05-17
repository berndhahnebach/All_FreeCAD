/****************************************************************************
** Form interface generated from reading ui file '.\DlgSettings3DView.ui'
**
** Created: Sa 13. Mai 12:36:03 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGSETTINGS3DVIEW_H
#define GUI_DIALOG_DLGSETTINGS3DVIEW_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
namespace Gui {
class PrefCheckBox;
}
class QFrame;
class QLabel;
namespace Gui {
class PrefFloatSpinBox;
}
class QButtonGroup;
namespace Gui {
class PrefColorButton;
}
namespace Gui {
class PrefRadioButton;
}

namespace Gui {
namespace Dialog {
class DlgSettings3DView : public QWidget
{
    Q_OBJECT

public:
    DlgSettings3DView( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgSettings3DView();

    QGroupBox* GroupBox12;
    Gui::PrefCheckBox* CheckBox_ShowFPS;
    Gui::PrefCheckBox* prefCheckBox8;
    QFrame* line1;
    QLabel* textLabel1;
    Gui::PrefFloatSpinBox* FloatSpinBox_EyeDistance;
    Gui::PrefCheckBox* CheckBox_CornerCoordSystem;
    Gui::PrefCheckBox* CheckBox_UseAutoRotation;
    QButtonGroup* selectionGroup;
    Gui::PrefCheckBox* checkBoxPreselection;
    Gui::PrefCheckBox* checkBoxSelection;
    QLabel* textLabel1_2;
    Gui::PrefColorButton* HighlightColor;
    QLabel* textLabel2;
    Gui::PrefColorButton* SelectionColor;
    QButtonGroup* backgroundColorGroup;
    Gui::PrefRadioButton* radioButtonSimple;
    Gui::PrefRadioButton* radioButtonGradient;
    Gui::PrefColorButton* SelectionColor_Background;
    Gui::PrefColorButton* backgroundColorFrom;
    Gui::PrefColorButton* backgroundColorTo;
    QButtonGroup* cameraColorGroup;
    Gui::PrefRadioButton* radioPerspective;
    Gui::PrefRadioButton* radioOrthographic;

protected:
    QGridLayout* DlgSettings3DViewLayout;
    QGridLayout* GroupBox12Layout;
    QHBoxLayout* layout4;
    QGridLayout* selectionGroupLayout;
    QHBoxLayout* layout6;
    QHBoxLayout* layout7;
    QGridLayout* backgroundColorGroupLayout;
    QGridLayout* cameraColorGroupLayout;

protected slots:
    virtual void languageChange();

    virtual void loadSettings();
    virtual void saveSettings();


};
}
}

#endif // GUI_DIALOG_DLGSETTINGS3DVIEW_H
