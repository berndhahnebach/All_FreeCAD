/****************************************************************************
** Form interface generated from reading ui file '.\DlgSettingsMacro.ui'
**
** Created: Mi 17. Mai 14:36:15 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGSETTINGSMACRO_H
#define GUI_DIALOG_DLGSETTINGSMACRO_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
namespace Gui {
class PrefFileChooser;
}
namespace Gui {
class PrefCheckBox;
}

namespace Gui {
namespace Dialog {
class DlgSettingsMacro : public QWidget
{
    Q_OBJECT

public:
    DlgSettingsMacro( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgSettingsMacro();

    QGroupBox* GroupBox6;
    QGroupBox* GroupBox8;
    Gui::PrefFileChooser* MacroPath;
    QGroupBox* GroupBox7;
    Gui::PrefCheckBox* PrefCheckBox_RecordGui;
    Gui::PrefCheckBox* PrefCheckBox_GuiAsComment;
    QGroupBox* groupBox4;
    Gui::PrefCheckBox* PConsoleCheckBox;
    Gui::PrefCheckBox* FileLogCheckBox;
    Gui::PrefFileChooser* MacroPath_2;

protected:
    QVBoxLayout* DlgSettingsMacroLayout;
    QVBoxLayout* GroupBox6Layout;
    QGridLayout* GroupBox8Layout;
    QGridLayout* GroupBox7Layout;
    QVBoxLayout* groupBox4Layout;

protected slots:
    virtual void languageChange();

    virtual void loadSettings();
    virtual void saveSettings();


};
}
}

#endif // GUI_DIALOG_DLGSETTINGSMACRO_H
