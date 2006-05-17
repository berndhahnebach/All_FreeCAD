/****************************************************************************
** Form implementation generated from reading ui file '.\DlgReportView.ui'
**
** Created: Sa 13. Mai 12:36:04 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgReportView.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgReportViewBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgReportViewBase::DlgReportViewBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgReportViewBase" );
    DlgReportViewBaseLayout = new QVBoxLayout( this, 11, 6, "DlgReportViewBaseLayout"); 

    buttonGroup2 = new QButtonGroup( this, "buttonGroup2" );
    buttonGroup2->setColumnLayout(0, Qt::Vertical );
    buttonGroup2->layout()->setSpacing( 6 );
    buttonGroup2->layout()->setMargin( 11 );
    buttonGroup2Layout = new QVBoxLayout( buttonGroup2->layout() );
    buttonGroup2Layout->setAlignment( Qt::AlignTop );

    checkLogging = new Gui::PrefCheckBox( buttonGroup2, "checkLogging" );
    checkLogging->setProperty( "prefEntry", "checkLogging" );
    checkLogging->setProperty( "prefPath", "OutputWindow" );
    buttonGroup2Layout->addWidget( checkLogging );

    checkWarning = new Gui::PrefCheckBox( buttonGroup2, "checkWarning" );
    checkWarning->setChecked( TRUE );
    checkWarning->setProperty( "prefEntry", "checkWarning" );
    checkWarning->setProperty( "prefPath", "OutputWindow" );
    buttonGroup2Layout->addWidget( checkWarning );

    checkError = new Gui::PrefCheckBox( buttonGroup2, "checkError" );
    checkError->setChecked( TRUE );
    checkError->setProperty( "prefEntry", "checkError" );
    checkError->setProperty( "prefPath", "OutputWindow" );
    buttonGroup2Layout->addWidget( checkError );
    DlgReportViewBaseLayout->addWidget( buttonGroup2 );

    buttonGroup1 = new QButtonGroup( this, "buttonGroup1" );
    buttonGroup1->setColumnLayout(0, Qt::Vertical );
    buttonGroup1->layout()->setSpacing( 6 );
    buttonGroup1->layout()->setMargin( 11 );
    buttonGroup1Layout = new QVBoxLayout( buttonGroup1->layout() );
    buttonGroup1Layout->setAlignment( Qt::AlignTop );

    layout5 = new QHBoxLayout( 0, 0, 6, "layout5"); 

    textLabel1 = new QLabel( buttonGroup1, "textLabel1" );
    layout5->addWidget( textLabel1 );
    QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout5->addItem( spacer );

    colorText = new Gui::PrefColorButton( buttonGroup1, "colorText" );
    colorText->setProperty( "prefEntry", "colorText" );
    colorText->setProperty( "prefPath", "OutputWindow" );
    layout5->addWidget( colorText );
    buttonGroup1Layout->addLayout( layout5 );

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 

    textLabel2 = new QLabel( buttonGroup1, "textLabel2" );
    layout6->addWidget( textLabel2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout6->addItem( spacer_2 );

    colorLogging = new Gui::PrefColorButton( buttonGroup1, "colorLogging" );
    colorLogging->setColor( QColor( 0, 0, 255 ) );
    colorLogging->setProperty( "prefEntry", "colorLogging" );
    colorLogging->setProperty( "prefPath", "OutputWindow" );
    layout6->addWidget( colorLogging );
    buttonGroup1Layout->addLayout( layout6 );

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 

    textLabel3 = new QLabel( buttonGroup1, "textLabel3" );
    layout7->addWidget( textLabel3 );
    QSpacerItem* spacer_3 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout7->addItem( spacer_3 );

    colorWarning = new Gui::PrefColorButton( buttonGroup1, "colorWarning" );
    colorWarning->setColor( QColor( 255, 170, 0 ) );
    colorWarning->setProperty( "prefEntry", "colorWarning" );
    colorWarning->setProperty( "prefPath", "OutputWindow" );
    layout7->addWidget( colorWarning );
    buttonGroup1Layout->addLayout( layout7 );

    layout8 = new QHBoxLayout( 0, 0, 6, "layout8"); 

    textLabel4 = new QLabel( buttonGroup1, "textLabel4" );
    layout8->addWidget( textLabel4 );
    QSpacerItem* spacer_4 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout8->addItem( spacer_4 );

    colorError = new Gui::PrefColorButton( buttonGroup1, "colorError" );
    colorError->setColor( QColor( 255, 0, 0 ) );
    colorError->setProperty( "prefEntry", "colorError" );
    colorError->setProperty( "prefPath", "OutputWindow" );
    layout8->addWidget( colorError );
    buttonGroup1Layout->addLayout( layout8 );
    DlgReportViewBaseLayout->addWidget( buttonGroup1 );
    QSpacerItem* spacer_5 = new QSpacerItem( 21, 111, QSizePolicy::Minimum, QSizePolicy::Expanding );
    DlgReportViewBaseLayout->addItem( spacer_5 );
    languageChange();
    resize( QSize(432, 404).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgReportViewBase::~DlgReportViewBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgReportViewBase::languageChange()
{
    setCaption( tr( "Output window" ) );
    buttonGroup2->setTitle( tr( "Output" ) );
    checkLogging->setText( tr( "Record log messages" ) );
    checkWarning->setText( tr( "Record warnings" ) );
    checkError->setText( tr( "Record error messages" ) );
    buttonGroup1->setTitle( tr( "Colors" ) );
    textLabel1->setText( tr( "Normal messages:" ) );
    colorText->setText( QString::null );
    textLabel2->setText( tr( "Log messages:" ) );
    colorLogging->setText( QString::null );
    textLabel3->setText( tr( "Warnings:" ) );
    colorWarning->setText( QString::null );
    textLabel4->setText( tr( "Errors:" ) );
    colorError->setText( QString::null );
}

void Gui::Dialog::DlgReportViewBase::loadSettings()
{
    qWarning( "Gui::Dialog::DlgReportViewBase::loadSettings(): Not implemented yet" );
}

void Gui::Dialog::DlgReportViewBase::saveSettings()
{
    qWarning( "Gui::Dialog::DlgReportViewBase::saveSettings(): Not implemented yet" );
}

