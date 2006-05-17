/****************************************************************************
** Form interface generated from reading ui file '.\DlgSettingsImage.ui'
**
** Created: Mi 17. Mai 14:36:16 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GUI_DIALOG_DLGSETTINGSIMAGE_H
#define GUI_DIALOG_DLGSETTINGSIMAGE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QGroupBox;
class QLabel;
class QComboBox;
class QCheckBox;
class QTextEdit;
class QPushButton;
class QSpinBox;
namespace Gui {
class FloatSpinBox;
}

namespace Gui {
namespace Dialog {
class DlgSettingsImage : public QWidget
{
    Q_OBJECT

public:
    DlgSettingsImage( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~DlgSettingsImage();

    QGroupBox* groupBoxProp;
    QLabel* textLabelImageType;
    QLabel* textLabelColor;
    QComboBox* comboColor;
    QComboBox* comboImageType;
    QCheckBox* checkTransparent;
    QGroupBox* groupBoxComment;
    QTextEdit* textEditComment;
    QPushButton* pushButtonInsertTimeDate;
    QPushButton* pushButtonInsertMatrix;
    QPushButton* pushButtonInsertMIBA;
    QGroupBox* groupBoxDim;
    QLabel* textLabelWidth;
    QSpinBox* spinWidth;
    QLabel* textLabelHeight;
    QSpinBox* spinHeight;
    QLabel* textLabelResolution;
    Gui::FloatSpinBox* spinResolution;
    QLabel* textLabel1;
    QPushButton* pushButtonRatioScreen;
    QPushButton* pushButtonRatio4x3;
    QPushButton* pushButtonRatio16x9;
    QPushButton* pushButtonRatio1x1;

public slots:
    virtual void onInsertMIBA();
    virtual void onInsertViewMatrix();
    virtual void onInsertDateTime();
    virtual void onSelectedFilter( const QString& );

protected:
    QGridLayout* DlgSettingsImageLayout;
    QGridLayout* groupBoxPropLayout;
    QVBoxLayout* groupBoxCommentLayout;
    QHBoxLayout* layout2;
    QGridLayout* groupBoxDimLayout;
    QHBoxLayout* layout4;
    QHBoxLayout* layout5;
    QHBoxLayout* layout6;

protected slots:
    virtual void languageChange();

    virtual void onRatioScreen();
    virtual void onRatio4x3();
    virtual void onRatio16x9();
    virtual void onRatio1x1();


};
}
}

#endif // GUI_DIALOG_DLGSETTINGSIMAGE_H
