/****************************************************************************
** Form interface generated from reading ui file '.\DlgEditor.ui'
**
** Created: Sa 13. Mai 12:36:06 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGEDITORSETTINGSBASE_H
#define GUI_DIALOG_DLGEDITORSETTINGSBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QTextEdit;
class QLabel;
class QComboBox;
namespace Gui {
class PrefCheckBox;
}
namespace Gui {
class ColorButton;
}
class QListBox;
class QListBoxItem;
class QButtonGroup;
namespace Gui {
class PrefSpinBox;
}
namespace Gui {
class PrefRadioButton;
}

namespace Gui {
namespace Dialog {
class DlgEditorSettingsBase : public QWidget
{
    Q_OBJECT

public:
    DlgEditorSettingsBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgEditorSettingsBase();

    QGroupBox* GroupBox2_2;
    QTextEdit* textEdit1;
    QGroupBox* GroupBox5;
    QLabel* TextLabel4;
    QLabel* TextLabel3;
    QComboBox* FontSize;
    QComboBox* FontDB;
    QGroupBox* GroupBox2;
    Gui::PrefCheckBox* EnableLineNumber;
    Gui::PrefCheckBox* EnableFolding;
    QGroupBox* GroupBox1;
    QLabel* TextLabel1;
    Gui::ColorButton* ColorBtn;
    QListBox* ListBox1;
    QButtonGroup* tabIndent;
    Gui::PrefSpinBox* tabSize;
    Gui::PrefSpinBox* indentSize;
    QLabel* textLabel1;
    QLabel* textLabel2;
    Gui::PrefRadioButton* radioTabs;
    Gui::PrefRadioButton* radioSpaces;

protected:
    QGridLayout* DlgEditorSettingsBaseLayout;
    QGridLayout* GroupBox2_2Layout;
    QGridLayout* GroupBox5Layout;
    QGridLayout* GroupBox2Layout;
    QGridLayout* GroupBox1Layout;
    QGridLayout* tabIndentLayout;
    QGridLayout* layout2;

protected slots:
    virtual void languageChange();

    virtual void saveSettings();
    virtual void loadSettings();


};
}
}

#endif // GUI_DIALOG_DLGEDITORSETTINGSBASE_H
