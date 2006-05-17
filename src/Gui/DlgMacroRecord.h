/****************************************************************************
** Form interface generated from reading ui file '.\DlgMacroRecord.ui'
**
** Created: Mi 17. Mai 14:36:06 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGMACRORECORD_H
#define GUI_DIALOG_DLGMACRORECORD_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QLineEdit;
class QGroupBox;
class QPushButton;

namespace Gui {
namespace Dialog {
class DlgMacroRecord : public QDialog
{
    Q_OBJECT

public:
    DlgMacroRecord( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgMacroRecord();

    QLabel* TextLabel1;
    QLineEdit* LineEditPath;
    QGroupBox* GroupBox4;
    QPushButton* PushButton9;
    QPushButton* PushButton10;
    QPushButton* PushButton9_2;
    QLabel* TextLabel4;
    QLabel* TextLabel5;
    QLabel* TextLabel6;
    QPushButton* PushButtonStart;
    QPushButton* PushButtonStop;
    QPushButton* PushButtonCancel;

public slots:
    virtual void cancel();
    virtual void onTieCommandBar();
    virtual void onTieKeyboard();
    virtual void onTieToolBar();
    virtual void start();
    virtual void stop();
    virtual void onSaveMacro();

protected:
    QHBoxLayout* DlgMacroRecordLayout;
    QVBoxLayout* Layout9;
    QHBoxLayout* Layout8;
    QGridLayout* GroupBox4Layout;
    QVBoxLayout* Layout5;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;

};
}
}

#endif // GUI_DIALOG_DLGMACRORECORD_H
