/****************************************************************************
** Form implementation generated from reading ui file '.\DlgDisplayProperties.ui'
**
** Created: Mi 17. Mai 14:36:08 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgDisplayProperties.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <Gui/PrefWidgets.h>
#include <qscrollbar.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgDisplayProperties as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Gui::Dialog::DlgDisplayProperties::DlgDisplayProperties( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgDisplayProperties" );
    DlgDisplayPropertiesLayout = new QVBoxLayout( this, 11, 6, "DlgDisplayPropertiesLayout"); 

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setColumnLayout(0, Qt::Vertical );
    groupBox1->layout()->setSpacing( 6 );
    groupBox1->layout()->setMargin( 11 );
    groupBox1Layout = new QVBoxLayout( groupBox1->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 

    textLabel1 = new QLabel( groupBox1, "textLabel1" );
    layout3->addWidget( textLabel1 );

    ModeBox = new QComboBox( FALSE, groupBox1, "ModeBox" );
    layout3->addWidget( ModeBox );
    groupBox1Layout->addLayout( layout3 );

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    textLabel1_3 = new QLabel( groupBox1, "textLabel1_3" );
    textLabel1_3->setEnabled( FALSE );
    layout2->addWidget( textLabel1_3 );

    PlotCombo = new QComboBox( FALSE, groupBox1, "PlotCombo" );
    PlotCombo->setEnabled( FALSE );
    layout2->addWidget( PlotCombo );
    groupBox1Layout->addLayout( layout2 );
    DlgDisplayPropertiesLayout->addWidget( groupBox1 );

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    MaterialCombo = new QComboBox( FALSE, groupBox2, "MaterialCombo" );
    MaterialCombo->setEnabled( FALSE );
    groupBox2Layout->addWidget( MaterialCombo );

    ColorButton = new Gui::ColorButton( groupBox2, "ColorButton" );
    groupBox2Layout->addWidget( ColorButton );

    textLabel1_2 = new QLabel( groupBox2, "textLabel1_2" );
    groupBox2Layout->addWidget( textLabel1_2 );

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 

    TransBar = new QScrollBar( groupBox2, "TransBar" );
    TransBar->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, TransBar->sizePolicy().hasHeightForWidth() ) );
    TransBar->setMaxValue( 100 );
    TransBar->setOrientation( QScrollBar::Horizontal );
    layout1->addWidget( TransBar );

    TransSpin = new QSpinBox( groupBox2, "TransSpin" );
    TransSpin->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)0, 0, 0, TransSpin->sizePolicy().hasHeightForWidth() ) );
    TransSpin->setMaxValue( 100 );
    TransSpin->setLineStep( 5 );
    layout1->addWidget( TransSpin );
    groupBox2Layout->addLayout( layout1 );
    DlgDisplayPropertiesLayout->addWidget( groupBox2 );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4"); 

    OkButton = new QPushButton( this, "OkButton" );
    layout4->addWidget( OkButton );
    QSpacerItem* spacer = new QSpacerItem( 81, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout4->addItem( spacer );

    CancelButton = new QPushButton( this, "CancelButton" );
    layout4->addWidget( CancelButton );
    DlgDisplayPropertiesLayout->addLayout( layout4 );
    languageChange();
    resize( QSize(248, 266).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( MaterialCombo, SIGNAL( activated(const QString&) ), this, SLOT( onChangeMaterial(const QString&) ) );
    connect( ModeBox, SIGNAL( activated(const QString&) ), this, SLOT( onChangeMode(const QString&) ) );
    connect( PlotCombo, SIGNAL( activated(const QString&) ), this, SLOT( onChangePlot(const QString&) ) );
    connect( TransSpin, SIGNAL( valueChanged(int) ), TransBar, SLOT( setValue(int) ) );
    connect( TransBar, SIGNAL( valueChanged(int) ), TransSpin, SLOT( setValue(int) ) );
    connect( OkButton, SIGNAL( clicked() ), this, SLOT( onOK() ) );
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( onCancel() ) );
    connect( ColorButton, SIGNAL( changed() ), this, SLOT( onColorChange() ) );
    connect( TransBar, SIGNAL( valueChanged(int) ), this, SLOT( onChangeTrans(int) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgDisplayProperties::~DlgDisplayProperties()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgDisplayProperties::languageChange()
{
    setCaption( tr( "Display properties" ) );
    groupBox1->setTitle( tr( "Viewing mode" ) );
    textLabel1->setText( tr( "Visual mode:" ) );
    textLabel1_3->setText( tr( "Plot mode:" ) );
    groupBox2->setTitle( tr( "Material" ) );
    ColorButton->setText( QString::null );
    textLabel1_2->setText( tr( "Transparency:" ) );
    OkButton->setText( tr( "OK" ) );
    CancelButton->setText( tr( "Cancel" ) );
}

void Gui::Dialog::DlgDisplayProperties::onChangeMaterial(const QString&)
{
    qWarning( "Gui::Dialog::DlgDisplayProperties::onChangeMaterial(const QString&): Not implemented yet" );
}

void Gui::Dialog::DlgDisplayProperties::onChangeMode(const QString&)
{
    qWarning( "Gui::Dialog::DlgDisplayProperties::onChangeMode(const QString&): Not implemented yet" );
}

void Gui::Dialog::DlgDisplayProperties::onChangePlot(const QString&)
{
    qWarning( "Gui::Dialog::DlgDisplayProperties::onChangePlot(const QString&): Not implemented yet" );
}

void Gui::Dialog::DlgDisplayProperties::onOK()
{
    qWarning( "Gui::Dialog::DlgDisplayProperties::onOK(): Not implemented yet" );
}

void Gui::Dialog::DlgDisplayProperties::onColorChange()
{
    qWarning( "Gui::Dialog::DlgDisplayProperties::onColorChange(): Not implemented yet" );
}

void Gui::Dialog::DlgDisplayProperties::onCancel()
{
    qWarning( "Gui::Dialog::DlgDisplayProperties::onCancel(): Not implemented yet" );
}

void Gui::Dialog::DlgDisplayProperties::onChangeTrans(int)
{
    qWarning( "Gui::Dialog::DlgDisplayProperties::onChangeTrans(int): Not implemented yet" );
}

