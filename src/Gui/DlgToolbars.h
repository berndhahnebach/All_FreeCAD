/****************************************************************************
** Form interface generated from reading ui file '.\DlgToolbars.ui'
**
** Created: Sa 13. Mai 12:35:58 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGCUSTOMTOOLBARSBASE_H
#define GUI_DIALOG_DLGCUSTOMTOOLBARSBASE_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QListView;
class QListViewItem;
class QPushButton;
class QComboBox;

namespace Gui {
namespace Dialog {
class DlgCustomToolbarsBase : public QWidget
{
    Q_OBJECT

public:
    DlgCustomToolbarsBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgCustomToolbarsBase();

    QGroupBox* GroupBox30_2;
    QListView* AvailableActions;
    QPushButton* CreateToolbar;
    QPushButton* DeleteToolbar;
    QPushButton* buttonRight;
    QPushButton* buttonLeft;
    QPushButton* buttonUp;
    QPushButton* buttonDown;
    QListView* ToolbarActions;
    QComboBox* ComboToolbars;

protected:
    QGridLayout* DlgCustomToolbarsBaseLayout;
    QGridLayout* GroupBox30_2Layout;
    QVBoxLayout* layout2;
    QHBoxLayout* layout1;
    QVBoxLayout* layout3;
    QVBoxLayout* layout4;

protected slots:
    virtual void languageChange();

    virtual void onAddAction();
    virtual void onAllActionsChanged( QListViewItem * );
    virtual void onCreateToolbar();
    virtual void onDeleteToolbar();
    virtual void onDoubleClickedAction(QListViewItem*);
    virtual void onNewActionChanged( QListViewItem * );
    virtual void onMoveUpAction();
    virtual void onRemoveAction();
    virtual void onMoveDownAction();
    virtual void onItemActivated(const QString &);


private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;
    QPixmap image3;

};
}
}

#endif // GUI_DIALOG_DLGCUSTOMTOOLBARSBASE_H
