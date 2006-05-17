/****************************************************************************
** Form interface generated from reading ui file '.\DlgActions.ui'
**
** Created: Mi 17. Mai 14:36:14 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGCUSTOMACTIONSBASE_H
#define GUI_DIALOG_DLGCUSTOMACTIONSBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QPushButton;
class QListView;
class QListViewItem;
class QToolButton;
class QLabel;
namespace Gui {
class AccelLineEdit;
}
class QLineEdit;
class QComboBox;

namespace Gui {
namespace Dialog {
class DlgCustomActionsBase : public QWidget
{
    Q_OBJECT

public:
    DlgCustomActionsBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgCustomActionsBase();

    QGroupBox* GroupBox7;
    QPushButton* buttonNew;
    QPushButton* buttonDelete;
    QListView* CustomActions;
    QToolButton* actionPixmapButton;
    QLabel* TextLabel1;
    QLabel* TextLabel2;
    QLabel* TextLabel3;
    Gui::AccelLineEdit* actionAccel;
    QLabel* TextLabel3_2;
    QLineEdit* actionMenu;
    QLineEdit* actionToolTip;
    QLabel* TextLabel2_2;
    QLabel* TextLabel5_2;
    QLineEdit* actionWhatsThis;
    QLabel* TextLabel1_2;
    QLineEdit* actionName;
    QLabel* TextLabel1_3;
    QLabel* PixmapLabel;
    QComboBox* actionMacros;
    QLineEdit* actionStatus;

protected:
    QGridLayout* DlgCustomActionsBaseLayout;
    QGridLayout* GroupBox7Layout;

protected slots:
    virtual void languageChange();

    virtual void onCustomActionsCanDelete( QListViewItem* );
    virtual void onCustomActionsDoubleClicked( QListViewItem* );
    virtual void onAddCustomAction();
    virtual void onDelCustomAction();
    virtual void onCustomActionPixmap();


};
}
}

#endif // GUI_DIALOG_DLGCUSTOMACTIONSBASE_H
