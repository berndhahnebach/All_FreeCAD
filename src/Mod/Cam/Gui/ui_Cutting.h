/********************************************************************************
** Form generated from reading ui file 'Cutting.ui'
**
** Created: Mon 14. Jan 13:44:17 2008
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CUTTING_H
#define UI_CUTTING_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

namespace CamGui {

class Ui_Cutting
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_4;
    QLabel *label_5;
    QWidget *verticalLayout_3;
    QVBoxLayout *vboxLayout;
    QPushButton *CalculcateZLevel;
    QSpacerItem *spacerItem;
    QPushButton *pushButton_3;
    QSpacerItem *spacerItem1;
    QPushButton *pushButton;
    QSpacerItem *spacerItem2;
    QPushButton *pushButton_4;
    QSpacerItem *spacerItem3;
    QPushButton *pushButton_5;
    QDialogButtonBox *buttonBox;
    QWidget *verticalLayout;
    QVBoxLayout *vboxLayout1;
    QPushButton *toolpath_calculation_highest_level_button;
    QSpacerItem *spacerItem4;
    QPushButton *toolpath_calculation_middle_level_button;
    QSpacerItem *spacerItem5;
    QPushButton *toolpath_calculation_lowest_level_button;
    QPushButton *toolpath_calculation_go_button;
    QWidget *verticalLayout_4;
    QVBoxLayout *vboxLayout2;
    QPushButton *best_fit_cad_button;
    QSpacerItem *spacerItem6;
    QPushButton *best_fit_fe_button;
    QSpacerItem *spacerItem7;
    QPushButton *best_fit_mesh_button;
    QPushButton *best_fit_go_button;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_3;
    QWidget *verticalLayout_2;
    QVBoxLayout *vboxLayout3;
    QSpacerItem *spacerItem8;
    QDoubleSpinBox *doubleSpinBox;
    QDoubleSpinBox *doubleSpinBox_2;
    QDoubleSpinBox *doubleSpinBox_3;
    QDoubleSpinBox *doubleSpinBox_4;
    QDoubleSpinBox *doubleSpinBox_5;
    QSpacerItem *spacerItem9;

    void setupUi(QDialog *CamGui__Cutting)
    {
    CamGui__Cutting->setObjectName(QString::fromUtf8("CamGui__Cutting"));
    label = new QLabel(CamGui__Cutting);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(150, 240, 69, 28));
    label_2 = new QLabel(CamGui__Cutting);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(150, 270, 62, 16));
    label_4 = new QLabel(CamGui__Cutting);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(150, 320, 155, 16));
    label_5 = new QLabel(CamGui__Cutting);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setGeometry(QRect(150, 350, 155, 16));
    verticalLayout_3 = new QWidget(CamGui__Cutting);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    verticalLayout_3->setGeometry(QRect(360, 10, 211, 301));
    vboxLayout = new QVBoxLayout(verticalLayout_3);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    CalculcateZLevel = new QPushButton(verticalLayout_3);
    CalculcateZLevel->setObjectName(QString::fromUtf8("CalculcateZLevel"));

    vboxLayout->addWidget(CalculcateZLevel);

    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem);

    pushButton_3 = new QPushButton(verticalLayout_3);
    pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));

    vboxLayout->addWidget(pushButton_3);

    spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem1);

    pushButton = new QPushButton(verticalLayout_3);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));

    vboxLayout->addWidget(pushButton);

    spacerItem2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem2);

    pushButton_4 = new QPushButton(verticalLayout_3);
    pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));

    vboxLayout->addWidget(pushButton_4);

    spacerItem3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem3);

    pushButton_5 = new QPushButton(verticalLayout_3);
    pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));

    vboxLayout->addWidget(pushButton_5);

    buttonBox = new QDialogButtonBox(CamGui__Cutting);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(200, 380, 341, 32));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);
    verticalLayout = new QWidget(CamGui__Cutting);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setGeometry(QRect(20, 50, 131, 132));
    vboxLayout1 = new QVBoxLayout(verticalLayout);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    toolpath_calculation_highest_level_button = new QPushButton(verticalLayout);
    toolpath_calculation_highest_level_button->setObjectName(QString::fromUtf8("toolpath_calculation_highest_level_button"));
    toolpath_calculation_highest_level_button->setEnabled(false);

    vboxLayout1->addWidget(toolpath_calculation_highest_level_button);

    spacerItem4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout1->addItem(spacerItem4);

    toolpath_calculation_middle_level_button = new QPushButton(verticalLayout);
    toolpath_calculation_middle_level_button->setObjectName(QString::fromUtf8("toolpath_calculation_middle_level_button"));
    toolpath_calculation_middle_level_button->setEnabled(false);

    vboxLayout1->addWidget(toolpath_calculation_middle_level_button);

    spacerItem5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout1->addItem(spacerItem5);

    toolpath_calculation_lowest_level_button = new QPushButton(verticalLayout);
    toolpath_calculation_lowest_level_button->setObjectName(QString::fromUtf8("toolpath_calculation_lowest_level_button"));
    toolpath_calculation_lowest_level_button->setEnabled(false);

    vboxLayout1->addWidget(toolpath_calculation_lowest_level_button);

    toolpath_calculation_go_button = new QPushButton(verticalLayout);
    toolpath_calculation_go_button->setObjectName(QString::fromUtf8("toolpath_calculation_go_button"));
    toolpath_calculation_go_button->setEnabled(false);

    vboxLayout1->addWidget(toolpath_calculation_go_button);

    verticalLayout_4 = new QWidget(CamGui__Cutting);
    verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
    verticalLayout_4->setGeometry(QRect(190, 50, 131, 132));
    vboxLayout2 = new QVBoxLayout(verticalLayout_4);
    vboxLayout2->setSpacing(6);
    vboxLayout2->setMargin(0);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    best_fit_cad_button = new QPushButton(verticalLayout_4);
    best_fit_cad_button->setObjectName(QString::fromUtf8("best_fit_cad_button"));
    best_fit_cad_button->setEnabled(false);

    vboxLayout2->addWidget(best_fit_cad_button);

    spacerItem6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout2->addItem(spacerItem6);

    best_fit_fe_button = new QPushButton(verticalLayout_4);
    best_fit_fe_button->setObjectName(QString::fromUtf8("best_fit_fe_button"));
    best_fit_fe_button->setEnabled(false);

    vboxLayout2->addWidget(best_fit_fe_button);

    spacerItem7 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout2->addItem(spacerItem7);

    best_fit_mesh_button = new QPushButton(verticalLayout_4);
    best_fit_mesh_button->setObjectName(QString::fromUtf8("best_fit_mesh_button"));
    best_fit_mesh_button->setEnabled(false);

    vboxLayout2->addWidget(best_fit_mesh_button);

    best_fit_go_button = new QPushButton(verticalLayout_4);
    best_fit_go_button->setObjectName(QString::fromUtf8("best_fit_go_button"));
    best_fit_go_button->setEnabled(false);

    vboxLayout2->addWidget(best_fit_go_button);

    label_7 = new QLabel(CamGui__Cutting);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    label_7->setGeometry(QRect(220, 20, 61, 20));
    label_8 = new QLabel(CamGui__Cutting);
    label_8->setObjectName(QString::fromUtf8("label_8"));
    label_8->setGeometry(QRect(10, 20, 161, 21));
    label_3 = new QLabel(CamGui__Cutting);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(150, 300, 80, 16));
    verticalLayout_2 = new QWidget(CamGui__Cutting);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    verticalLayout_2->setGeometry(QRect(90, 210, 53, 184));
    vboxLayout3 = new QVBoxLayout(verticalLayout_2);
    vboxLayout3->setSpacing(6);
    vboxLayout3->setMargin(0);
    vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
    spacerItem8 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout3->addItem(spacerItem8);

    doubleSpinBox = new QDoubleSpinBox(verticalLayout_2);
    doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));

    vboxLayout3->addWidget(doubleSpinBox);

    doubleSpinBox_2 = new QDoubleSpinBox(verticalLayout_2);
    doubleSpinBox_2->setObjectName(QString::fromUtf8("doubleSpinBox_2"));

    vboxLayout3->addWidget(doubleSpinBox_2);

    doubleSpinBox_3 = new QDoubleSpinBox(verticalLayout_2);
    doubleSpinBox_3->setObjectName(QString::fromUtf8("doubleSpinBox_3"));

    vboxLayout3->addWidget(doubleSpinBox_3);

    doubleSpinBox_4 = new QDoubleSpinBox(verticalLayout_2);
    doubleSpinBox_4->setObjectName(QString::fromUtf8("doubleSpinBox_4"));

    vboxLayout3->addWidget(doubleSpinBox_4);

    doubleSpinBox_5 = new QDoubleSpinBox(verticalLayout_2);
    doubleSpinBox_5->setObjectName(QString::fromUtf8("doubleSpinBox_5"));

    vboxLayout3->addWidget(doubleSpinBox_5);

    spacerItem9 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout3->addItem(spacerItem9);


    retranslateUi(CamGui__Cutting);

    QSize size(584, 428);
    size = size.expandedTo(CamGui__Cutting->minimumSizeHint());
    CamGui__Cutting->resize(size);

    QObject::connect(buttonBox, SIGNAL(accepted()), CamGui__Cutting, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), CamGui__Cutting, SLOT(reject()));

    QMetaObject::connectSlotsByName(CamGui__Cutting);
    } // setupUi

    void retranslateUi(QDialog *CamGui__Cutting)
    {
    CamGui__Cutting->setWindowTitle(QApplication::translate("CamGui::Cutting", "Dialog", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("CamGui::Cutting", "Master-Radius", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("CamGui::Cutting", "Slave-Radius", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("CamGui::Cutting", "Correction Factor for Springback", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("CamGui::Cutting", "Limit Angle", 0, QApplication::UnicodeUTF8));
    CalculcateZLevel->setText(QApplication::translate("CamGui::Cutting", "Calculate Z-Level Toolpath", 0, QApplication::UnicodeUTF8));
    pushButton_3->setText(QApplication::translate("CamGui::Cutting", "Calculate Feature Based Toolpath", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("CamGui::Cutting", "Make a Best-Fit", 0, QApplication::UnicodeUTF8));
    pushButton_4->setText(QApplication::translate("CamGui::Cutting", "Correct Springback", 0, QApplication::UnicodeUTF8));
    pushButton_5->setText(QApplication::translate("CamGui::Cutting", "Approximate NURB Surface", 0, QApplication::UnicodeUTF8));
    toolpath_calculation_highest_level_button->setText(QApplication::translate("CamGui::Cutting", "Highest Level", 0, QApplication::UnicodeUTF8));
    toolpath_calculation_middle_level_button->setText(QApplication::translate("CamGui::Cutting", "Middle Level", 0, QApplication::UnicodeUTF8));
    toolpath_calculation_lowest_level_button->setText(QApplication::translate("CamGui::Cutting", "Lowest Level", 0, QApplication::UnicodeUTF8));
    toolpath_calculation_go_button->setText(QApplication::translate("CamGui::Cutting", "GO", 0, QApplication::UnicodeUTF8));
    best_fit_cad_button->setText(QApplication::translate("CamGui::Cutting", "CAD-Part", 0, QApplication::UnicodeUTF8));
    best_fit_fe_button->setText(QApplication::translate("CamGui::Cutting", "FE-Result", 0, QApplication::UnicodeUTF8));
    best_fit_mesh_button->setText(QApplication::translate("CamGui::Cutting", "Scanned Mesh", 0, QApplication::UnicodeUTF8));
    best_fit_go_button->setText(QApplication::translate("CamGui::Cutting", "GO", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("CamGui::Cutting", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic;\">Best-Fit</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("CamGui::Cutting", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:8pt;\"><span style=\" font-size:12pt; font-style:italic;\">Tool-Path-Calculation</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("CamGui::Cutting", "Cutting-Distance", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(CamGui__Cutting);
    } // retranslateUi

};

} // namespace CamGui

namespace CamGui {
namespace Ui {
    class Cutting: public Ui_Cutting {};
} // namespace Ui
} // namespace CamGui

#endif // UI_CUTTING_H
