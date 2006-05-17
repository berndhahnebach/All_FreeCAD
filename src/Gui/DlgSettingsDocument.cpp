/****************************************************************************
** Form implementation generated from reading ui file '.\DlgSettingsDocument.ui'
**
** Created: Mi 17. Mai 14:36:17 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgSettingsDocument.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <Gui/PrefWidgets.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgSettingsDocument as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgSettingsDocument::DlgSettingsDocument( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgSettingsDocument" );
    DlgSettingsDocumentLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgSettingsDocumentLayout"); 

    GroupBox5 = new QGroupBox( this, "GroupBox5" );
    GroupBox5->setColumnLayout(0, Qt::Vertical );
    GroupBox5->layout()->setSpacing( 6 );
    GroupBox5->layout()->setMargin( 11 );
    GroupBox5Layout = new QGridLayout( GroupBox5->layout() );
    GroupBox5Layout->setAlignment( Qt::AlignTop );

    layout8 = new QHBoxLayout( 0, 0, 6, "layout8"); 

    textLabel1 = new QLabel( GroupBox5, "textLabel1" );
    layout8->addWidget( textLabel1 );

    SpinBoxCompression = new Gui::PrefSpinBox( GroupBox5, "SpinBoxCompression" );
    SpinBoxCompression->setMaxValue( 9 );
    SpinBoxCompression->setValue( 3 );
    SpinBoxCompression->setProperty( "prefEntry", "CompressionLevel" );
    SpinBoxCompression->setProperty( "prefPath", "Document" );
    layout8->addWidget( SpinBoxCompression );

    GroupBox5Layout->addLayout( layout8, 2, 0 );

    line1 = new QFrame( GroupBox5, "line1" );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );

    GroupBox5Layout->addWidget( line1, 1, 0 );

    prefCheckNewDoc = new Gui::PrefCheckBox( GroupBox5, "prefCheckNewDoc" );
    prefCheckNewDoc->setProperty( "prefEntry", "CreateNewDoc" );
    prefCheckNewDoc->setProperty( "prefPath", "Document" );

    GroupBox5Layout->addWidget( prefCheckNewDoc, 0, 0 );

    DlgSettingsDocumentLayout->addWidget( GroupBox5, 0, 0 );
    languageChange();
    resize( QSize(390, 497).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgSettingsDocument::~DlgSettingsDocument()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgSettingsDocument::languageChange()
{
    setCaption( tr( "Document" ) );
    GroupBox5->setTitle( tr( "General" ) );
    textLabel1->setText( tr( "Document save compression level\n"
"(0 = none, 9 = highest, 3 = default)" ) );
    prefCheckNewDoc->setText( tr( "Create new document at startup" ) );
}

void Gui::Dialog::DlgSettingsDocument::saveSettings()
{
    qWarning( "Gui::Dialog::DlgSettingsDocument::saveSettings(): Not implemented yet" );
}

void Gui::Dialog::DlgSettingsDocument::loadSettings()
{
    qWarning( "Gui::Dialog::DlgSettingsDocument::loadSettings(): Not implemented yet" );
}

