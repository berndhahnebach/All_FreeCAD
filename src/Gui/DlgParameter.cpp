/****************************************************************************
** Form implementation generated from reading ui file '.\DlgParameter.ui'
**
** Created: Sa 13. Mai 12:35:56 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgParameter.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgParameter as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Gui::Dialog::DlgParameter::DlgParameter( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgParameter" );
    setSizeGripEnabled( TRUE );
    DlgParameterLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgParameterLayout"); 

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    DlgParameterLayout->addLayout( Layout1, 2, 0 );

    splitter3 = new QSplitter( this, "splitter3" );
    splitter3->setOrientation( QSplitter::Horizontal );

    DlgParameterLayout->addWidget( splitter3, 1, 0 );

    SetNameComboBox = new QComboBox( FALSE, this, "SetNameComboBox" );

    DlgParameterLayout->addWidget( SetNameComboBox, 0, 0 );
    languageChange();
    resize( QSize(657, 558).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgParameter::~DlgParameter()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgParameter::languageChange()
{
    setCaption( tr( "Parameter Editor" ) );
    buttonOk->setText( tr( "&Close" ) );
    buttonOk->setAccel( QKeySequence( tr( "Alt+C" ) ) );
}

