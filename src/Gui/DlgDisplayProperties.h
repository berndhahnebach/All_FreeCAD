/****************************************************************************
** Form interface generated from reading ui file '.\DlgDisplayProperties.ui'
**
** Created: Mi 17. Mai 14:36:08 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGDISPLAYPROPERTIES_H
#define GUI_DIALOG_DLGDISPLAYPROPERTIES_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
class QComboBox;
namespace Gui {
class ColorButton;
}
class QScrollBar;
class QSpinBox;
class QPushButton;

namespace Gui {
namespace Dialog {
class DlgDisplayProperties : public QDialog
{
    Q_OBJECT

public:
    DlgDisplayProperties( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~DlgDisplayProperties();

    QGroupBox* groupBox1;
    QLabel* textLabel1;
    QComboBox* ModeBox;
    QLabel* textLabel1_3;
    QComboBox* PlotCombo;
    QGroupBox* groupBox2;
    QComboBox* MaterialCombo;
    Gui::ColorButton* ColorButton;
    QLabel* textLabel1_2;
    QScrollBar* TransBar;
    QSpinBox* TransSpin;
    QPushButton* OkButton;
    QPushButton* CancelButton;

public slots:
    virtual void onChangeMaterial(const QString&);
    virtual void onChangeMode(const QString&);
    virtual void onChangePlot(const QString&);
    virtual void onOK(void);
    virtual void onColorChange();
    virtual void onCancel();
    virtual void onChangeTrans(int);

protected:
    QVBoxLayout* DlgDisplayPropertiesLayout;
    QVBoxLayout* groupBox1Layout;
    QHBoxLayout* layout3;
    QHBoxLayout* layout2;
    QVBoxLayout* groupBox2Layout;
    QHBoxLayout* layout1;
    QHBoxLayout* layout4;

protected slots:
    virtual void languageChange();

};
}
}

#endif // GUI_DIALOG_DLGDISPLAYPROPERTIES_H
