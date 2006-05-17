/****************************************************************************
** Form interface generated from reading ui file '.\DlgTipOfTheDay.ui'
**
** Created: Mi 17. Mai 14:36:03 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGTIPOFTHEDAYBASE_H
#define GUI_DIALOG_DLGTIPOFTHEDAYBASE_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QPushButton;
class QGroupBox;
class QLabel;

namespace Gui {
namespace Dialog {
class DlgTipOfTheDayBase : public QDialog
{
    Q_OBJECT

public:
    DlgTipOfTheDayBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgTipOfTheDayBase();

    QCheckBox* checkShowTips;
    QPushButton* pushButtonNext;
    QPushButton* pushButtonClose;
    QGroupBox* groupBox1;
    QLabel* textHeader;
    QLabel* textTip;
    QGroupBox* groupBox2;
    QLabel* pixmapLightbulb;

public slots:
    virtual void onNext();

protected:
    QGridLayout* DlgTipOfTheDayBaseLayout;
    QHBoxLayout* layout2;
    QGridLayout* groupBox1Layout;
    QVBoxLayout* layout4;
    QGridLayout* groupBox2Layout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};
}
}

#endif // GUI_DIALOG_DLGTIPOFTHEDAYBASE_H
