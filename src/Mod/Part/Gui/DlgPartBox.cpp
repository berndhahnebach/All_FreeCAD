/****************************************************************************
** Form implementation generated from reading ui file '.\DlgPartBox.ui'
**
** Created: Mi 17. Mai 14:44:50 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgPartBox.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a PartGui::DlgPartBox as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PartGui::DlgPartBox::DlgPartBox( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PartGui::DlgPartBox" );
    DlgPartBoxLayout = new QVBoxLayout( this, 11, 6, "DlgPartBoxLayout"); 

    GroupBox5 = new QGroupBox( this, "GroupBox5" );
    GroupBox5->setColumnLayout(0, Qt::Vertical );
    GroupBox5->layout()->setSpacing( 6 );
    GroupBox5->layout()->setMargin( 11 );
    GroupBox5Layout = new QGridLayout( GroupBox5->layout() );
    GroupBox5Layout->setAlignment( Qt::AlignTop );

    TextLabel1 = new QLabel( GroupBox5, "TextLabel1" );

    GroupBox5Layout->addWidget( TextLabel1, 0, 0 );

    XLineEdit = new QLineEdit( GroupBox5, "XLineEdit" );
    XLineEdit->setFrameShape( QLineEdit::LineEditPanel );
    XLineEdit->setFrameShadow( QLineEdit::Sunken );

    GroupBox5Layout->addWidget( XLineEdit, 0, 1 );

    YLineEdit = new QLineEdit( GroupBox5, "YLineEdit" );

    GroupBox5Layout->addWidget( YLineEdit, 1, 1 );

    ZLineEdit = new QLineEdit( GroupBox5, "ZLineEdit" );

    GroupBox5Layout->addWidget( ZLineEdit, 2, 1 );

    TextLabel3 = new QLabel( GroupBox5, "TextLabel3" );

    GroupBox5Layout->addWidget( TextLabel3, 2, 0 );

    TextLabel2 = new QLabel( GroupBox5, "TextLabel2" );

    GroupBox5Layout->addWidget( TextLabel2, 1, 0 );
    DlgPartBoxLayout->addWidget( GroupBox5 );

    GroupBox5_2 = new QGroupBox( this, "GroupBox5_2" );
    GroupBox5_2->setColumnLayout(0, Qt::Vertical );
    GroupBox5_2->layout()->setSpacing( 6 );
    GroupBox5_2->layout()->setMargin( 11 );
    GroupBox5_2Layout = new QGridLayout( GroupBox5_2->layout() );
    GroupBox5_2Layout->setAlignment( Qt::AlignTop );

    ULineEdit = new QLineEdit( GroupBox5_2, "ULineEdit" );

    GroupBox5_2Layout->addWidget( ULineEdit, 0, 1 );

    VLineEdit = new QLineEdit( GroupBox5_2, "VLineEdit" );

    GroupBox5_2Layout->addWidget( VLineEdit, 1, 1 );

    WLineEdit = new QLineEdit( GroupBox5_2, "WLineEdit" );

    GroupBox5_2Layout->addWidget( WLineEdit, 2, 1 );

    TextLabel1_2 = new QLabel( GroupBox5_2, "TextLabel1_2" );

    GroupBox5_2Layout->addWidget( TextLabel1_2, 0, 0 );

    TextLabel2_2 = new QLabel( GroupBox5_2, "TextLabel2_2" );

    GroupBox5_2Layout->addWidget( TextLabel2_2, 1, 0 );

    TextLabel3_2 = new QLabel( GroupBox5_2, "TextLabel3_2" );

    GroupBox5_2Layout->addWidget( TextLabel3_2, 2, 0 );
    DlgPartBoxLayout->addWidget( GroupBox5_2 );

    Layout13 = new QGridLayout( 0, 1, 1, 0, 6, "Layout13"); 

    ApplyButton = new QPushButton( this, "ApplyButton" );

    Layout13->addWidget( ApplyButton, 0, 2 );

    CancelButton = new QPushButton( this, "CancelButton" );

    Layout13->addWidget( CancelButton, 0, 4 );

    OKButton = new QPushButton( this, "OKButton" );

    Layout13->addWidget( OKButton, 0, 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout13->addItem( spacer, 0, 3 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout13->addItem( spacer_2, 0, 1 );
    DlgPartBoxLayout->addLayout( Layout13 );
    languageChange();
    resize( QSize(296, 275).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( OKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( ApplyButton, SIGNAL( clicked() ), this, SLOT( OnApply() ) );
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( XLineEdit, YLineEdit );
    setTabOrder( YLineEdit, ZLineEdit );
    setTabOrder( ZLineEdit, ULineEdit );
    setTabOrder( ULineEdit, VLineEdit );
    setTabOrder( VLineEdit, WLineEdit );
    setTabOrder( WLineEdit, OKButton );
    setTabOrder( OKButton, ApplyButton );
    setTabOrder( ApplyButton, CancelButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PartGui::DlgPartBox::~DlgPartBox()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PartGui::DlgPartBox::languageChange()
{
    setCaption( tr( "Box definition" ) );
    GroupBox5->setTitle( tr( "Position:" ) );
    TextLabel1->setText( tr( "X:" ) );
    XLineEdit->setText( tr( "0" ) );
    YLineEdit->setText( tr( "0" ) );
    ZLineEdit->setText( tr( "0" ) );
    TextLabel3->setText( tr( "Z:" ) );
    TextLabel2->setText( tr( "Y:" ) );
    GroupBox5_2->setTitle( tr( "Size:" ) );
    ULineEdit->setText( tr( "100" ) );
    VLineEdit->setText( tr( "100" ) );
    WLineEdit->setText( tr( "100" ) );
    TextLabel1_2->setText( tr( "U:" ) );
    TextLabel2_2->setText( tr( "V:" ) );
    TextLabel3_2->setText( tr( "W:" ) );
    ApplyButton->setText( tr( "Apply" ) );
    CancelButton->setText( tr( "Cancel" ) );
    OKButton->setText( tr( "OK" ) );
}

void PartGui::DlgPartBox::OnApply()
{
    qWarning( "PartGui::DlgPartBox::OnApply(): Not implemented yet" );
}

