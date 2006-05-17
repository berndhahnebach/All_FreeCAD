/****************************************************************************
** Form implementation generated from reading ui file '.\DlgActivateWindow.ui'
**
** Created: Sa 13. Mai 12:35:58 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgActivateWindow.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgActivateWindowBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Gui::Dialog::DlgActivateWindowBase::DlgActivateWindowBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgActivateWindowBase" );
    setSizeGripEnabled( TRUE );
    DlgActivateWindowBaseLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgActivateWindowBaseLayout"); 

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    layout2->addWidget( buttonOk );
    QSpacerItem* spacer = new QSpacerItem( 100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
    layout2->addWidget( buttonCancel );

    DlgActivateWindowBaseLayout->addLayout( layout2, 1, 0 );

    listBox2 = new QListBox( this, "listBox2" );

    DlgActivateWindowBaseLayout->addWidget( listBox2, 0, 0 );
    languageChange();
    resize( QSize(341, 267).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( listBox2, buttonOk );
    setTabOrder( buttonOk, buttonCancel );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgActivateWindowBase::~DlgActivateWindowBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgActivateWindowBase::languageChange()
{
    setCaption( tr( "Choose Window" ) );
    buttonOk->setText( tr( "&Activate" ) );
    buttonOk->setAccel( QKeySequence( tr( "Alt+A" ) ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}

