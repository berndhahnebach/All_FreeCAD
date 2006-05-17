/****************************************************************************
** Form implementation generated from reading ui file '.\DlgSettingsColorGradient.ui'
**
** Created: Sa 13. Mai 12:36:02 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgSettingsColorGradient.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgSettingsColorGradientBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Gui::Dialog::DlgSettingsColorGradientBase::DlgSettingsColorGradientBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgSettingsColorGradientBase" );
    setSizeGripEnabled( TRUE );
    DlgSettingsColorGradientBaseLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgSettingsColorGradientBaseLayout"); 

    groupBoxVisible = new QGroupBox( this, "groupBoxVisible" );
    groupBoxVisible->setColumnLayout(0, Qt::Vertical );
    groupBoxVisible->layout()->setSpacing( 6 );
    groupBoxVisible->layout()->setMargin( 11 );
    groupBoxVisibleLayout = new QHBoxLayout( groupBoxVisible->layout() );
    groupBoxVisibleLayout->setAlignment( Qt::AlignTop );

    checkBoxGrayed = new QCheckBox( groupBoxVisible, "checkBoxGrayed" );
    groupBoxVisibleLayout->addWidget( checkBoxGrayed );

    checkBoxInvisible = new QCheckBox( groupBoxVisible, "checkBoxInvisible" );
    groupBoxVisibleLayout->addWidget( checkBoxInvisible );

    DlgSettingsColorGradientBaseLayout->addWidget( groupBoxVisible, 2, 0 );

    buttonGroupStyle = new QButtonGroup( this, "buttonGroupStyle" );
    buttonGroupStyle->setColumnLayout(0, Qt::Vertical );
    buttonGroupStyle->layout()->setSpacing( 6 );
    buttonGroupStyle->layout()->setMargin( 11 );
    buttonGroupStyleLayout = new QHBoxLayout( buttonGroupStyle->layout() );
    buttonGroupStyleLayout->setAlignment( Qt::AlignTop );

    radioButtonFlow = new QRadioButton( buttonGroupStyle, "radioButtonFlow" );
    radioButtonFlow->setChecked( TRUE );
    buttonGroupStyleLayout->addWidget( radioButtonFlow );

    radioButtonZero = new QRadioButton( buttonGroupStyle, "radioButtonZero" );
    buttonGroupStyleLayout->addWidget( radioButtonZero );

    DlgSettingsColorGradientBaseLayout->addWidget( buttonGroupStyle, 1, 0 );

    groupBoxModel = new QGroupBox( this, "groupBoxModel" );
    groupBoxModel->setColumnLayout(0, Qt::Vertical );
    groupBoxModel->layout()->setSpacing( 6 );
    groupBoxModel->layout()->setMargin( 11 );
    groupBoxModelLayout = new QHBoxLayout( groupBoxModel->layout() );
    groupBoxModelLayout->setAlignment( Qt::AlignTop );

    layout12 = new QHBoxLayout( 0, 0, 6, "layout12"); 

    textLabel5 = new QLabel( groupBoxModel, "textLabel5" );
    layout12->addWidget( textLabel5 );

    comboBoxModel = new QComboBox( FALSE, groupBoxModel, "comboBoxModel" );
    layout12->addWidget( comboBoxModel );
    QSpacerItem* spacer = new QSpacerItem( 100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout12->addItem( spacer );
    groupBoxModelLayout->addLayout( layout12 );

    DlgSettingsColorGradientBaseLayout->addWidget( groupBoxModel, 0, 0 );

    layout9 = new QHBoxLayout( 0, 0, 6, "layout9"); 

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    layout9->addWidget( buttonOk );
    QSpacerItem* spacer_2 = new QSpacerItem( 170, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout9->addItem( spacer_2 );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    layout9->addWidget( buttonCancel );

    DlgSettingsColorGradientBaseLayout->addLayout( layout9, 4, 0 );

    groupBoxRange = new QGroupBox( this, "groupBoxRange" );
    groupBoxRange->setColumnLayout(0, Qt::Vertical );
    groupBoxRange->layout()->setSpacing( 6 );
    groupBoxRange->layout()->setMargin( 11 );
    groupBoxRangeLayout = new QGridLayout( groupBoxRange->layout() );
    groupBoxRangeLayout->setAlignment( Qt::AlignTop );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4"); 

    textLabelMax = new QLabel( groupBoxRange, "textLabelMax" );
    layout4->addWidget( textLabelMax );

    floatSpinBoxMax = new Gui::FloatSpinBox( groupBoxRange, "floatSpinBoxMax" );
    floatSpinBoxMax->setMaxValue( 1000 );
    floatSpinBoxMax->setMinValue( -1000 );
    floatSpinBoxMax->setPrecision( 2 );
    layout4->addWidget( floatSpinBoxMax );
    QSpacerItem* spacer_3 = new QSpacerItem( 16, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem( spacer_3 );

    textLabelMin = new QLabel( groupBoxRange, "textLabelMin" );
    layout4->addWidget( textLabelMin );

    floatSpinBoxMin = new Gui::FloatSpinBox( groupBoxRange, "floatSpinBoxMin" );
    floatSpinBoxMin->setMaxValue( 1000 );
    floatSpinBoxMin->setMinValue( -1000 );
    floatSpinBoxMin->setPrecision( 2 );
    layout4->addWidget( floatSpinBoxMin );

    groupBoxRangeLayout->addMultiCellLayout( layout4, 0, 0, 0, 1 );

    textLabel1 = new QLabel( groupBoxRange, "textLabel1" );

    groupBoxRangeLayout->addWidget( textLabel1, 1, 0 );

    spinBoxLabel = new Gui::SpinBox( groupBoxRange, "spinBoxLabel" );
    spinBoxLabel->setMaxValue( 30 );

    groupBoxRangeLayout->addWidget( spinBoxLabel, 1, 1 );

    DlgSettingsColorGradientBaseLayout->addWidget( groupBoxRange, 3, 0 );
    languageChange();
    resize( QSize(327, 332).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( floatSpinBoxMax, SIGNAL( valueChanged(double) ), this, SLOT( onValidateMaximum() ) );
    connect( floatSpinBoxMin, SIGNAL( valueChanged(double) ), this, SLOT( onValidateMinimum() ) );

    // tab order
    setTabOrder( comboBoxModel, radioButtonFlow );
    setTabOrder( radioButtonFlow, checkBoxGrayed );
    setTabOrder( checkBoxGrayed, checkBoxInvisible );
    setTabOrder( checkBoxInvisible, floatSpinBoxMax );
    setTabOrder( floatSpinBoxMax, floatSpinBoxMin );
    setTabOrder( floatSpinBoxMin, buttonOk );
    setTabOrder( buttonOk, buttonCancel );

    // buddies
    textLabel5->setBuddy( comboBoxModel );
    textLabelMax->setBuddy( floatSpinBoxMax );
    textLabelMin->setBuddy( floatSpinBoxMin );
    textLabel1->setBuddy( spinBoxLabel );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgSettingsColorGradientBase::~DlgSettingsColorGradientBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgSettingsColorGradientBase::languageChange()
{
    setCaption( tr( "Colorgradient settings" ) );
    groupBoxVisible->setTitle( tr( "Visibility" ) );
    checkBoxGrayed->setText( tr( "Out &grayed" ) );
    checkBoxGrayed->setAccel( QKeySequence( tr( "Alt+G" ) ) );
    checkBoxInvisible->setText( tr( "Out &invisible" ) );
    checkBoxInvisible->setAccel( QKeySequence( tr( "Alt+I" ) ) );
    buttonGroupStyle->setTitle( tr( "Style" ) );
    radioButtonFlow->setText( tr( "&Flow" ) );
    radioButtonFlow->setAccel( QKeySequence( tr( "Alt+F" ) ) );
    radioButtonZero->setText( tr( "&Zero" ) );
    radioButtonZero->setAccel( QKeySequence( tr( "Alt+Z" ) ) );
    groupBoxModel->setTitle( tr( "Color model" ) );
    textLabel5->setText( tr( "&Gradient:" ) );
    comboBoxModel->clear();
    comboBoxModel->insertItem( tr( "red-yellow-green-cyan-blue" ) );
    comboBoxModel->insertItem( tr( "blue-cyan-green-yellow-red" ) );
    comboBoxModel->insertItem( tr( "white-black" ) );
    comboBoxModel->insertItem( tr( "black-white" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    groupBoxRange->setTitle( tr( "Parameter range" ) );
    textLabelMax->setText( tr( "Ma&ximum:" ) );
    textLabelMin->setText( tr( "Mi&nimum:" ) );
    textLabel1->setText( tr( "&Number of labels:" ) );
}

void Gui::Dialog::DlgSettingsColorGradientBase::onValidateMaximum()
{
    qWarning( "Gui::Dialog::DlgSettingsColorGradientBase::onValidateMaximum(): Not implemented yet" );
}

void Gui::Dialog::DlgSettingsColorGradientBase::onValidateMinimum()
{
    qWarning( "Gui::Dialog::DlgSettingsColorGradientBase::onValidateMinimum(): Not implemented yet" );
}

