/****************************************************************************
** Form implementation generated from reading ui file '.\DlgAuthorization.ui'
**
** Created: Mi 17. Mai 14:36:09 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgAuthorization.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgAuthorization as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Gui::Dialog::DlgAuthorization::DlgAuthorization( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgAuthorization" );
    setSizeGripEnabled( TRUE );
    DlgAuthorizationLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgAuthorizationLayout"); 

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    layout2->addWidget( buttonOk );
    QSpacerItem* spacer = new QSpacerItem( 140, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    layout2->addWidget( buttonCancel );

    DlgAuthorizationLayout->addLayout( layout2, 5, 0 );

    password = new QLineEdit( this, "password" );
    password->setFrameShape( QLineEdit::LineEditPanel );
    password->setFrameShadow( QLineEdit::Sunken );
    password->setEchoMode( QLineEdit::Password );

    DlgAuthorizationLayout->addWidget( password, 3, 0 );

    textLabel2 = new QLabel( this, "textLabel2" );

    DlgAuthorizationLayout->addWidget( textLabel2, 2, 0 );

    username = new QLineEdit( this, "username" );

    DlgAuthorizationLayout->addWidget( username, 1, 0 );

    textLabel1 = new QLabel( this, "textLabel1" );

    DlgAuthorizationLayout->addWidget( textLabel1, 0, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 21, 41, QSizePolicy::Minimum, QSizePolicy::Expanding );
    DlgAuthorizationLayout->addItem( spacer_2, 4, 0 );
    languageChange();
    resize( QSize(304, 189).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( username, password );
    setTabOrder( password, buttonOk );
    setTabOrder( buttonOk, buttonCancel );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgAuthorization::~DlgAuthorization()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgAuthorization::languageChange()
{
    setCaption( tr( "Authorization" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
    textLabel2->setText( tr( "Password:" ) );
    textLabel1->setText( tr( "Username:" ) );
}

