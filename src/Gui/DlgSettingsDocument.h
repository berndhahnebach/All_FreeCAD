/****************************************************************************
** Form interface generated from reading ui file '.\DlgSettingsDocument.ui'
**
** Created: Sa 13. Mai 12:36:01 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGSETTINGSDOCUMENT_H
#define GUI_DIALOG_DLGSETTINGSDOCUMENT_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
namespace Gui {
class PrefSpinBox;
}
class QFrame;
namespace Gui {
class PrefCheckBox;
}

namespace Gui {
namespace Dialog {
class DlgSettingsDocument : public QWidget
{
    Q_OBJECT

public:
    DlgSettingsDocument( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgSettingsDocument();

    QGroupBox* GroupBox5;
    QLabel* textLabel1;
    Gui::PrefSpinBox* SpinBoxCompression;
    QFrame* line1;
    Gui::PrefCheckBox* prefCheckNewDoc;

protected:
    QGridLayout* DlgSettingsDocumentLayout;
    QGridLayout* GroupBox5Layout;
    QHBoxLayout* layout8;

protected slots:
    virtual void languageChange();

    virtual void saveSettings();
    virtual void loadSettings();


};
}
}

#endif // GUI_DIALOG_DLGSETTINGSDOCUMENT_H
