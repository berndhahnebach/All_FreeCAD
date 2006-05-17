/****************************************************************************
** Form interface generated from reading ui file '.\DlgReportView.ui'
**
** Created: Sa 13. Mai 12:36:04 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGREPORTVIEWBASE_H
#define GUI_DIALOG_DLGREPORTVIEWBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
namespace Gui {
class PrefCheckBox;
}
class QLabel;
namespace Gui {
class PrefColorButton;
}

namespace Gui {
namespace Dialog {
class DlgReportViewBase : public QWidget
{
    Q_OBJECT

public:
    DlgReportViewBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgReportViewBase();

    QButtonGroup* buttonGroup2;
    Gui::PrefCheckBox* checkLogging;
    Gui::PrefCheckBox* checkWarning;
    Gui::PrefCheckBox* checkError;
    QButtonGroup* buttonGroup1;
    QLabel* textLabel1;
    Gui::PrefColorButton* colorText;
    QLabel* textLabel2;
    Gui::PrefColorButton* colorLogging;
    QLabel* textLabel3;
    Gui::PrefColorButton* colorWarning;
    QLabel* textLabel4;
    Gui::PrefColorButton* colorError;

protected:
    QVBoxLayout* DlgReportViewBaseLayout;
    QVBoxLayout* buttonGroup2Layout;
    QVBoxLayout* buttonGroup1Layout;
    QHBoxLayout* layout5;
    QHBoxLayout* layout6;
    QHBoxLayout* layout7;
    QHBoxLayout* layout8;

protected slots:
    virtual void languageChange();

    virtual void loadSettings();
    virtual void saveSettings();


};
}
}

#endif // GUI_DIALOG_DLGREPORTVIEWBASE_H
