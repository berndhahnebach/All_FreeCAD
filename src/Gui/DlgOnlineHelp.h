/****************************************************************************
** Form interface generated from reading ui file '.\DlgOnlineHelp.ui'
**
** Created: Sa 13. Mai 12:36:05 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGONLINEHELPBASE_H
#define GUI_DIALOG_DLGONLINEHELPBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
namespace Gui {
class PrefLineEdit;
}
namespace Gui {
class PrefCheckBox;
}
class QLabel;
namespace Gui {
class PrefFileChooser;
}

namespace Gui {
namespace Dialog {
class DlgOnlineHelpBase : public QWidget
{
    Q_OBJECT

public:
    DlgOnlineHelpBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgOnlineHelpBase();

    QGroupBox* groupBox6;
    Gui::PrefLineEdit* PrefLineEdit2;
    Gui::PrefCheckBox* prefAuthorize;
    QLabel* TextLabel1;
    QLabel* textLabel1_2;
    Gui::PrefLineEdit* DownloadURL;
    Gui::PrefFileChooser* DownloadLoc;
    QLabel* textLabel1;
    Gui::PrefCheckBox* UseProxy;
    QGroupBox* GroupBoxOther;
    Gui::PrefFileChooser* prefExtBrowser;
    Gui::PrefFileChooser* prefStartPage;
    QLabel* TextLabelBrowser;
    QLabel* TextLabelURL;

protected:
    QGridLayout* DlgOnlineHelpBaseLayout;
    QGridLayout* groupBox6Layout;
    QGridLayout* GroupBoxOtherLayout;

protected slots:
    virtual void languageChange();

    virtual void saveSettings();
    virtual void loadSettings();
    virtual void onCheckLocation(const QString&);


};
}
}

#endif // GUI_DIALOG_DLGONLINEHELPBASE_H
