/****************************************************************************
** Form interface generated from reading ui file '.\DlgSettings3DViewPart.ui'
**
** Created: Sa 13. Mai 12:40:16 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PARTGUI_DLGSETTINGS3DVIEWPART_H
#define PARTGUI_DLGSETTINGS3DVIEWPART_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
class QFrame;
namespace Gui {
class PrefFloatSpinBox;
}
namespace Gui {
class PrefCheckBox;
}

namespace PartGui {
class DlgSettings3DViewPart : public QWidget
{
    Q_OBJECT

public:
    DlgSettings3DViewPart( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgSettings3DViewPart();

    QGroupBox* GroupBox12;
    QLabel* textLabel1_3;
    QFrame* line2;
    QLabel* textLabel1;
    Gui::PrefFloatSpinBox* prefFloatSpinBox1;
    QLabel* textLabel1_3_3_2;
    QFrame* line2_2_2;
    Gui::PrefCheckBox* prefCheckBox8;
    Gui::PrefCheckBox* prefCheckBox3;

public slots:
    virtual void warnInventor(bool);

protected:
    QVBoxLayout* DlgSettings3DViewPartLayout;
    QVBoxLayout* GroupBox12Layout;
    QHBoxLayout* layout14;
    QHBoxLayout* layout1;
    QHBoxLayout* layout14_2_2;

protected slots:
    virtual void languageChange();

    virtual void loadSettings();
    virtual void saveSettings();


};
}

#endif // PARTGUI_DLGSETTINGS3DVIEWPART_H
