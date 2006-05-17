/****************************************************************************
** Form implementation generated from reading ui file '.\DlgOnlineHelp.ui'
**
** Created: Mi 17. Mai 14:36:21 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgOnlineHelp.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <Gui/PrefWidgets.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgOnlineHelpBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgOnlineHelpBase::DlgOnlineHelpBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgOnlineHelpBase" );
    DlgOnlineHelpBaseLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgOnlineHelpBaseLayout"); 
    QSpacerItem* spacer = new QSpacerItem( 20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding );
    DlgOnlineHelpBaseLayout->addItem( spacer, 2, 0 );

    groupBox6 = new QGroupBox( this, "groupBox6" );
    groupBox6->setColumnLayout(0, Qt::Vertical );
    groupBox6->layout()->setSpacing( 6 );
    groupBox6->layout()->setMargin( 11 );
    groupBox6Layout = new QGridLayout( groupBox6->layout() );
    groupBox6Layout->setAlignment( Qt::AlignTop );

    PrefLineEdit2 = new Gui::PrefLineEdit( groupBox6, "PrefLineEdit2" );
    PrefLineEdit2->setEnabled( FALSE );
    PrefLineEdit2->setProperty( "prefEntry", "ProxyText" );
    PrefLineEdit2->setProperty( "prefPath", "OnlineHelp" );

    groupBox6Layout->addWidget( PrefLineEdit2, 1, 0 );

    prefAuthorize = new Gui::PrefCheckBox( groupBox6, "prefAuthorize" );
    prefAuthorize->setEnabled( FALSE );
    prefAuthorize->setChecked( FALSE );
    prefAuthorize->setProperty( "prefEntry", "Authorize" );
    prefAuthorize->setProperty( "prefPath", "OnlineHelp" );

    groupBox6Layout->addWidget( prefAuthorize, 3, 0 );

    TextLabel1 = new QLabel( groupBox6, "TextLabel1" );

    groupBox6Layout->addWidget( TextLabel1, 4, 0 );

    textLabel1_2 = new QLabel( groupBox6, "textLabel1_2" );

    groupBox6Layout->addWidget( textLabel1_2, 6, 0 );

    DownloadURL = new Gui::PrefLineEdit( groupBox6, "DownloadURL" );
    DownloadURL->setMinimumSize( QSize( 250, 0 ) );
    DownloadURL->setProperty( "prefEntry", "DownloadURL" );
    DownloadURL->setProperty( "prefPath", "OnlineHelp" );

    groupBox6Layout->addWidget( DownloadURL, 5, 0 );

    DownloadLoc = new Gui::PrefFileChooser( groupBox6, "DownloadLoc" );
    DownloadLoc->setMode( Gui::PrefFileChooser::Directory );
    DownloadLoc->setProperty( "prefEntry", "DownloadLocation" );
    DownloadLoc->setProperty( "prefPath", "OnlineHelp" );

    groupBox6Layout->addWidget( DownloadLoc, 7, 0 );

    textLabel1 = new QLabel( groupBox6, "textLabel1" );

    groupBox6Layout->addWidget( textLabel1, 0, 0 );

    UseProxy = new Gui::PrefCheckBox( groupBox6, "UseProxy" );
    UseProxy->setChecked( FALSE );
    UseProxy->setProperty( "prefEntry", "UseProxy" );
    UseProxy->setProperty( "prefPath", "OnlineHelp" );

    groupBox6Layout->addWidget( UseProxy, 2, 0 );

    DlgOnlineHelpBaseLayout->addWidget( groupBox6, 0, 0 );

    GroupBoxOther = new QGroupBox( this, "GroupBoxOther" );
    GroupBoxOther->setColumnLayout(0, Qt::Vertical );
    GroupBoxOther->layout()->setSpacing( 6 );
    GroupBoxOther->layout()->setMargin( 11 );
    GroupBoxOtherLayout = new QGridLayout( GroupBoxOther->layout() );
    GroupBoxOtherLayout->setAlignment( Qt::AlignTop );

    prefExtBrowser = new Gui::PrefFileChooser( GroupBoxOther, "prefExtBrowser" );
    prefExtBrowser->setProperty( "prefEntry", "ExternalBrowser" );
    prefExtBrowser->setProperty( "prefPath", "OnlineHelp" );

    GroupBoxOtherLayout->addWidget( prefExtBrowser, 3, 0 );

    prefStartPage = new Gui::PrefFileChooser( GroupBoxOther, "prefStartPage" );
    prefStartPage->setProperty( "prefEntry", "Startpage" );
    prefStartPage->setProperty( "prefPath", "OnlineHelp" );

    GroupBoxOtherLayout->addWidget( prefStartPage, 1, 0 );

    TextLabelBrowser = new QLabel( GroupBoxOther, "TextLabelBrowser" );

    GroupBoxOtherLayout->addWidget( TextLabelBrowser, 2, 0 );

    TextLabelURL = new QLabel( GroupBoxOther, "TextLabelURL" );

    GroupBoxOtherLayout->addWidget( TextLabelURL, 0, 0 );

    DlgOnlineHelpBaseLayout->addWidget( GroupBoxOther, 1, 0 );
    languageChange();
    resize( QSize(395, 419).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( UseProxy, SIGNAL( toggled(bool) ), PrefLineEdit2, SLOT( setEnabled(bool) ) );
    connect( UseProxy, SIGNAL( toggled(bool) ), prefAuthorize, SLOT( setEnabled(bool) ) );
    connect( DownloadLoc, SIGNAL( fileNameSelected(const QString&) ), this, SLOT( onCheckLocation(const QString&) ) );

    // tab order
    setTabOrder( PrefLineEdit2, UseProxy );
    setTabOrder( UseProxy, prefAuthorize );
    setTabOrder( prefAuthorize, DownloadURL );
    setTabOrder( DownloadURL, DownloadLoc );
    setTabOrder( DownloadLoc, prefStartPage );
    setTabOrder( prefStartPage, prefExtBrowser );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgOnlineHelpBase::~DlgOnlineHelpBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgOnlineHelpBase::languageChange()
{
    setCaption( tr( "Online help" ) );
    groupBox6->setTitle( tr( "Download" ) );
    prefAuthorize->setText( tr( "Enable user authorization" ) );
    TextLabel1->setText( tr( "Download online help from:" ) );
    textLabel1_2->setText( tr( "To:" ) );
    DownloadURL->setText( tr( "http://free-cad.sourceforge.net/index.html" ) );
    textLabel1->setText( tr( "Proxy server:" ) );
    UseProxy->setText( tr( "Use proxy" ) );
    GroupBoxOther->setTitle( tr( "Help viewer" ) );
    TextLabelBrowser->setText( tr( "External browser" ) );
    TextLabelURL->setText( tr( "Location of start page" ) );
}

void Gui::Dialog::DlgOnlineHelpBase::saveSettings()
{
    qWarning( "Gui::Dialog::DlgOnlineHelpBase::saveSettings(): Not implemented yet" );
}

void Gui::Dialog::DlgOnlineHelpBase::loadSettings()
{
    qWarning( "Gui::Dialog::DlgOnlineHelpBase::loadSettings(): Not implemented yet" );
}

void Gui::Dialog::DlgOnlineHelpBase::onCheckLocation(const QString&)
{
    qWarning( "Gui::Dialog::DlgOnlineHelpBase::onCheckLocation(const QString&): Not implemented yet" );
}

