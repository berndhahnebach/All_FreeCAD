/****************************************************************************
** Form implementation generated from reading ui file '.\DlgPartImportIges.ui'
**
** Created: Sa 13. Mai 12:40:17 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgPartImportIges.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a PartGui::DlgPartImportIges as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PartGui::DlgPartImportIges::DlgPartImportIges( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "PartGui::DlgPartImportIges" );
    DlgPartImportIgesLayout = new QVBoxLayout( this, 11, 6, "DlgPartImportIgesLayout"); 

    GroupBox5 = new QGroupBox( this, "GroupBox5" );
    GroupBox5->setColumnLayout(0, Qt::Vertical );
    GroupBox5->layout()->setSpacing( 6 );
    GroupBox5->layout()->setMargin( 11 );
    GroupBox5Layout = new QHBoxLayout( GroupBox5->layout() );
    GroupBox5Layout->setAlignment( Qt::AlignTop );

    FileName = new QLineEdit( GroupBox5, "FileName" );
    FileName->setFrameShape( QLineEdit::LineEditPanel );
    FileName->setFrameShadow( QLineEdit::Sunken );
    GroupBox5Layout->addWidget( FileName );

    SearchFile = new QPushButton( GroupBox5, "SearchFile" );
    SearchFile->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, SearchFile->sizePolicy().hasHeightForWidth() ) );
    SearchFile->setMinimumSize( QSize( 30, 0 ) );
    SearchFile->setMaximumSize( QSize( 30, 32767 ) );
    GroupBox5Layout->addWidget( SearchFile );
    DlgPartImportIgesLayout->addWidget( GroupBox5 );

    Layout13 = new QGridLayout( 0, 1, 1, 0, 6, "Layout13"); 

    CancelButton = new QPushButton( this, "CancelButton" );

    Layout13->addWidget( CancelButton, 0, 4 );

    OKButton = new QPushButton( this, "OKButton" );

    Layout13->addWidget( OKButton, 0, 0 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout13->addItem( spacer, 0, 3 );
    DlgPartImportIgesLayout->addLayout( Layout13 );
    languageChange();
    resize( QSize(342, 115).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( OKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( CancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( SearchFile, SIGNAL( clicked() ), this, SLOT( onChooseFileName() ) );

    // tab order
    setTabOrder( FileName, OKButton );
    setTabOrder( OKButton, CancelButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PartGui::DlgPartImportIges::~DlgPartImportIges()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PartGui::DlgPartImportIges::languageChange()
{
    setCaption( tr( "IGES input file" ) );
    GroupBox5->setTitle( tr( "File Name" ) );
    FileName->setText( QString::null );
    SearchFile->setText( tr( "..." ) );
    CancelButton->setText( tr( "Cancel" ) );
    OKButton->setText( tr( "OK" ) );
}

void PartGui::DlgPartImportIges::OnApply()
{
    qWarning( "PartGui::DlgPartImportIges::OnApply(): Not implemented yet" );
}

void PartGui::DlgPartImportIges::onChooseFileName()
{
    qWarning( "PartGui::DlgPartImportIges::onChooseFileName(): Not implemented yet" );
}

