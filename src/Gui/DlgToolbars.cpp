/****************************************************************************
** Form implementation generated from reading ui file '.\DlgToolbars.ui'
**
** Created: Sa 13. Mai 12:35:58 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgToolbars.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlistview.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"16 16 54 1",
". c None",
"U c #0024ff",
"Q c #0028ff",
"I c #183ce6",
"R c #2034d5",
"O c #2048ee",
"N c #294cee",
"Y c #3159f6",
"M c #3159ff",
"L c #3165ff",
"S c #3950cd",
"P c #3965ff",
"H c #3969ff",
"Z c #415dcd",
"V c #4a5dbd",
"T c #4a61c5",
"C c #4a75ee",
"J c #5271de",
"K c #5285ff",
"X c #5a79ee",
"w c #5a8dff",
"n c #6275bd",
"g c #6295ff",
"x c #6a7dbd",
"d c #6a85c5",
"D c #6a99f6",
"B c #6a99ff",
"G c #6aa1ff",
"i c #73a5ff",
"a c #7b89ac",
"W c #7b99ee",
"# c #7ba1e6",
"y c #7baaf6",
"c c #7baeff",
"k c #83b6ff",
"j c #8ba1f6",
"F c #8bb6ff",
"o c #8bbaf6",
"l c #8bbeff",
"m c #94c6ff",
"h c #94caff",
"e c #9cceff",
"E c #9cd2ff",
"t c #a4d2f6",
"A c #a4d2ff",
"f c #acdeff",
"z c #ace2ff",
"u c #b4d6f6",
"s c #bddaf6",
"v c #bddaff",
"r c #bde6ff",
"b c #cdf2f6",
"q c #cdf6ff",
"p c #e6ffff",
"................",
".........#a.....",
".........bcd....",
".........efgd...",
".........#heid..",
"aj##d#..akllmgn.",
"opqrsstuvkckklwx",
"yzmmhmeAmcccccBC",
"DEFkciiGccccigHI",
"JGKHLMNOPcciBLQx",
"aIRSTxaanBiBLUV.",
".........WgMUT..",
".........XLQT...",
".........YUT....",
".........Zn.....",
"................"};

static const char* const image1_data[] = { 
"16 16 46 1",
". c None",
"R c #083cff",
"N c #1038ee",
"O c #1044ff",
"F c #183cff",
"H c #1840f6",
"J c #184cff",
"A c #203cd5",
"C c #2044ff",
"y c #2955ff",
"s c #3155de",
"E c #3161ff",
"I c #3950d5",
"G c #3969ff",
"x c #4175ff",
"M c #4a61c5",
"P c #4a65cd",
"Q c #4a7dff",
"L c #526dcd",
"K c #5285ff",
"D c #5a89ff",
"B c #6275bd",
"o c #6295ff",
"z c #6a99f6",
"v c #6a9dff",
"# c #7389c5",
"n c #73a5ff",
"h c #73aaff",
"d c #7b8dbd",
"l c #7ba1ee",
"k c #7baeff",
"j c #7bb2ff",
"w c #83b2f6",
"r c #83b6ff",
"a c #8ba5ee",
"i c #94c6ff",
"u c #94caff",
"f c #9cc6f6",
"g c #9cd6ff",
"p c #bde6ff",
"q c #c5e6ff",
"m c #cddeff",
"b c #dee2ff",
"c c #e6eaff",
"t c #e6faff",
"e c #eeffff",
"................",
".....#a.........",
"....abca........",
"...dcef.........",
"..deeghd........",
".#eeijk#.ada#al#",
"dmeinknolfppqprs",
"ftuknnvkiiwrhnxy",
"zrjnnvvvoovvooxA",
"BCDnvvnEFCyEGxEH",
".IJKovvLddMIANHM",
"..LOxovd......d.",
"...PFQK.........",
"....BRJ#........",
".....BN#........",
"................"};

static const char* const image2_data[] = { 
"16 16 45 1",
". c None",
"P c #1034ee",
"D c #1038ee",
"O c #183cf6",
"G c #1840ff",
"v c #295dff",
"p c #315dff",
"s c #3165ff",
"I c #3955d5",
"C c #3965ff",
"N c #396dff",
"F c #416dee",
"f c #4175ff",
"E c #4a65cd",
"K c #4a79ff",
"Q c #526dcd",
"L c #5271cd",
"M c #5285ff",
"g c #5a71c5",
"B c #5a89f6",
"q c #6279c5",
"J c #628dff",
"A c #6295ff",
"z c #6299ff",
"l c #6a79bd",
"w c #6a89d5",
"H c #6a99f6",
"o c #6a9dff",
"# c #7385bd",
"n c #73a5ff",
"y c #73aaff",
"b c #7ba1f6",
"k c #7baeff",
"j c #8bbeff",
"m c #94c6ff",
"u c #94ceff",
"i c #9ccaff",
"e c #9cd2ff",
"a c #b4c6f6",
"x c #bde6ff",
"t c #d5e2ff",
"d c #defaff",
"c c #e6eaff",
"h c #e6ffff",
"r c #ffffff",
"......#ab#......",
".....#cdefg.....",
"....#chijkfl....",
"...#chmknnopq...",
"..#rhjnnoooosl..",
".#thunnoooooovl.",
".wxiynoooozABCD.",
".#.#.#fooCE.b##.",
"......FyzG#.....",
"......BkAv#.....",
".....#Hkopg.....",
".....#kyACI.....",
".....#jnAfD.....",
".....wkAJKD.....",
".....LMNssO#....",
".....#DDDPQ....."};

static const char* const image3_data[] = { 
"16 16 48 1",
". c None",
"N c #002cff",
"Q c #082cff",
"T c #183cf6",
"S c #2050ff",
"r c #2950e6",
"O c #2959ff",
"k c #3155e6",
"E c #315df6",
"w c #394cbd",
"K c #3965ee",
"B c #3965ff",
"J c #4159c5",
"z c #4175ff",
"R c #4a61c5",
"v c #4a7dff",
"A c #526dcd",
"I c #527dee",
"q c #6295ff",
"C c #6a85bd",
"s c #6a85c5",
"m c #6a91e6",
"P c #6a9dff",
"L c #6aa1ff",
"H c #73a5ff",
"f c #7ba1ee",
"D c #7baeff",
"d c #83aaee",
"y c #83b6ff",
"M c #83baff",
"e c #8b99b4",
"x c #8b9dc5",
"u c #8bbeff",
"p c #94caff",
"# c #9ca1b4",
"t c #9cceff",
"o c #9cd2ff",
"l c #a4b6cd",
"a c #a4baee",
"F c #a4cef6",
"j c #a4d2ff",
"c c #b4caee",
"n c #b4eaff",
"b c #bdcaee",
"G c #bdd6ff",
"i c #defaff",
"h c #e6faff",
"g c #f6ffff",
".....#abcde.....",
".....fghijk#....",
"....lmnopqr.....",
".....sptuvwl....",
".....xypyzA.....",
".....eypyBC.....",
".....lfpDE#.....",
".....ldtDE#.....",
".l...eFtyvCl..l.",
".sFGayuyyyHffIJ.",
".CKLMMyyDyDHBNA.",
"..COPyyyDDHzQR#.",
"...COPDDHHvNwl..",
"....CSqHqzNw#...",
".....CSvvQwl....",
"......srTA#....."};


/*
 *  Constructs a Gui::Dialog::DlgCustomToolbarsBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgCustomToolbarsBase::DlgCustomToolbarsBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl ),
      image0( (const char **) image0_data ),
      image1( (const char **) image1_data ),
      image2( (const char **) image2_data ),
      image3( (const char **) image3_data )
{
    if ( !name )
	setName( "Gui::Dialog::DlgCustomToolbarsBase" );
    DlgCustomToolbarsBaseLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgCustomToolbarsBaseLayout"); 

    GroupBox30_2 = new QGroupBox( this, "GroupBox30_2" );
    GroupBox30_2->setMinimumSize( QSize( 0, 0 ) );
    GroupBox30_2->setMaximumSize( QSize( 32767, 32767 ) );
    GroupBox30_2->setColumnLayout(0, Qt::Vertical );
    GroupBox30_2->layout()->setSpacing( 6 );
    GroupBox30_2->layout()->setMargin( 11 );
    GroupBox30_2Layout = new QGridLayout( GroupBox30_2->layout() );
    GroupBox30_2Layout->setAlignment( Qt::AlignTop );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    GroupBox30_2Layout->addItem( spacer, 1, 1 );

    layout2 = new QVBoxLayout( 0, 0, 6, "layout2"); 

    AvailableActions = new QListView( GroupBox30_2, "AvailableActions" );
    AvailableActions->addColumn( tr( "Available commands" ) );
    AvailableActions->header()->setClickEnabled( FALSE, AvailableActions->header()->count() - 1 );
    AvailableActions->header()->setResizeEnabled( FALSE, AvailableActions->header()->count() - 1 );
    AvailableActions->setSelectionMode( QListView::Extended );
    AvailableActions->setResizeMode( QListView::AllColumns );
    layout2->addWidget( AvailableActions );

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 

    CreateToolbar = new QPushButton( GroupBox30_2, "CreateToolbar" );
    layout1->addWidget( CreateToolbar );

    DeleteToolbar = new QPushButton( GroupBox30_2, "DeleteToolbar" );
    layout1->addWidget( DeleteToolbar );
    layout2->addLayout( layout1 );

    GroupBox30_2Layout->addMultiCellLayout( layout2, 0, 1, 0, 0 );

    layout3 = new QVBoxLayout( 0, 0, 6, "layout3"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout3->addItem( spacer_2 );

    buttonRight = new QPushButton( GroupBox30_2, "buttonRight" );
    buttonRight->setEnabled( FALSE );
    buttonRight->setMinimumSize( QSize( 30, 30 ) );
    buttonRight->setPixmap( image0 );
    layout3->addWidget( buttonRight );

    buttonLeft = new QPushButton( GroupBox30_2, "buttonLeft" );
    buttonLeft->setEnabled( FALSE );
    buttonLeft->setMinimumSize( QSize( 30, 30 ) );
    buttonLeft->setPixmap( image1 );
    buttonLeft->setAutoDefault( TRUE );
    buttonLeft->setDefault( FALSE );
    layout3->addWidget( buttonLeft );

    buttonUp = new QPushButton( GroupBox30_2, "buttonUp" );
    buttonUp->setEnabled( FALSE );
    buttonUp->setMinimumSize( QSize( 30, 30 ) );
    buttonUp->setPixmap( image2 );
    layout3->addWidget( buttonUp );

    buttonDown = new QPushButton( GroupBox30_2, "buttonDown" );
    buttonDown->setEnabled( FALSE );
    buttonDown->setMinimumSize( QSize( 30, 30 ) );
    buttonDown->setPixmap( image3 );
    buttonDown->setAutoDefault( TRUE );
    layout3->addWidget( buttonDown );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout3->addItem( spacer_3 );

    GroupBox30_2Layout->addLayout( layout3, 0, 1 );

    layout4 = new QVBoxLayout( 0, 0, 6, "layout4"); 

    ToolbarActions = new QListView( GroupBox30_2, "ToolbarActions" );
    ToolbarActions->addColumn( tr( "Current commands" ) );
    ToolbarActions->header()->setClickEnabled( FALSE, ToolbarActions->header()->count() - 1 );
    ToolbarActions->header()->setResizeEnabled( FALSE, ToolbarActions->header()->count() - 1 );
    ToolbarActions->setSelectionMode( QListView::Extended );
    ToolbarActions->setResizeMode( QListView::AllColumns );
    layout4->addWidget( ToolbarActions );

    ComboToolbars = new QComboBox( FALSE, GroupBox30_2, "ComboToolbars" );
    ComboToolbars->setMinimumSize( QSize( 140, 0 ) );
    layout4->addWidget( ComboToolbars );

    GroupBox30_2Layout->addMultiCellLayout( layout4, 0, 1, 2, 2 );

    DlgCustomToolbarsBaseLayout->addWidget( GroupBox30_2, 0, 0 );
    languageChange();
    resize( QSize(400, 331).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonUp, SIGNAL( clicked() ), this, SLOT( onMoveUpAction() ) );
    connect( buttonDown, SIGNAL( clicked() ), this, SLOT( onMoveDownAction() ) );
    connect( DeleteToolbar, SIGNAL( clicked() ), this, SLOT( onDeleteToolbar() ) );
    connect( buttonLeft, SIGNAL( clicked() ), this, SLOT( onRemoveAction() ) );
    connect( buttonRight, SIGNAL( clicked() ), this, SLOT( onAddAction() ) );
    connect( CreateToolbar, SIGNAL( clicked() ), this, SLOT( onCreateToolbar() ) );
    connect( ComboToolbars, SIGNAL( activated(const QString&) ), this, SLOT( onItemActivated(const QString&) ) );
    connect( ToolbarActions, SIGNAL( clicked(QListViewItem*,const QPoint&,int) ), this, SLOT( onNewActionChanged(QListViewItem*) ) );
    connect( AvailableActions, SIGNAL( clicked(QListViewItem*) ), this, SLOT( onAllActionsChanged(QListViewItem*) ) );
    connect( AvailableActions, SIGNAL( doubleClicked(QListViewItem*) ), this, SLOT( onDoubleClickedAction(QListViewItem*) ) );

    // tab order
    setTabOrder( AvailableActions, CreateToolbar );
    setTabOrder( CreateToolbar, DeleteToolbar );
    setTabOrder( DeleteToolbar, buttonRight );
    setTabOrder( buttonRight, buttonLeft );
    setTabOrder( buttonLeft, buttonUp );
    setTabOrder( buttonUp, buttonDown );
    setTabOrder( buttonDown, ToolbarActions );
    setTabOrder( ToolbarActions, ComboToolbars );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgCustomToolbarsBase::~DlgCustomToolbarsBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgCustomToolbarsBase::languageChange()
{
    setCaption( tr( "Toolbars" ) );
    GroupBox30_2->setTitle( tr( "Setup Toolbar" ) );
    AvailableActions->header()->setLabel( 0, tr( "Available commands" ) );
    CreateToolbar->setText( tr( "Create bar" ) );
    DeleteToolbar->setText( tr( "Delete bar" ) );
    buttonRight->setText( QString::null );
    QToolTip::add( buttonRight, tr( "Move right" ) );
    QWhatsThis::add( buttonRight, tr( "<b>Move the selected item one level down.</b><p>This will also change the level of the parent item.</p>" ) );
    buttonLeft->setText( QString::null );
    QToolTip::add( buttonLeft, tr( "Move left" ) );
    QWhatsThis::add( buttonLeft, tr( "<b>Move the selected item one level up.</b><p>This will also change the level of the parent item.</p>" ) );
    buttonUp->setText( QString::null );
    QToolTip::add( buttonUp, tr( "Move up" ) );
    QWhatsThis::add( buttonUp, tr( "<b>Move the selected item up.</b><p>The item will be moved within the hierarchy level.</p>" ) );
    buttonDown->setText( QString::null );
    QToolTip::add( buttonDown, tr( "Move down" ) );
    QWhatsThis::add( buttonDown, tr( "<b>Move the selected item down.</b><p>The item will be moved within the hierarchy level.</p>" ) );
    ToolbarActions->header()->setLabel( 0, tr( "Current commands" ) );
}

void Gui::Dialog::DlgCustomToolbarsBase::onAddAction()
{
    qWarning( "Gui::Dialog::DlgCustomToolbarsBase::onAddAction(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomToolbarsBase::onAllActionsChanged(QListViewItem*)
{
    qWarning( "Gui::Dialog::DlgCustomToolbarsBase::onAllActionsChanged(QListViewItem*): Not implemented yet" );
}

void Gui::Dialog::DlgCustomToolbarsBase::onCreateToolbar()
{
    qWarning( "Gui::Dialog::DlgCustomToolbarsBase::onCreateToolbar(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomToolbarsBase::onDeleteToolbar()
{
    qWarning( "Gui::Dialog::DlgCustomToolbarsBase::onDeleteToolbar(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomToolbarsBase::onDoubleClickedAction(QListViewItem*)
{
    qWarning( "Gui::Dialog::DlgCustomToolbarsBase::onDoubleClickedAction(QListViewItem*): Not implemented yet" );
}

void Gui::Dialog::DlgCustomToolbarsBase::onNewActionChanged(QListViewItem*)
{
    qWarning( "Gui::Dialog::DlgCustomToolbarsBase::onNewActionChanged(QListViewItem*): Not implemented yet" );
}

void Gui::Dialog::DlgCustomToolbarsBase::onMoveUpAction()
{
    qWarning( "Gui::Dialog::DlgCustomToolbarsBase::onMoveUpAction(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomToolbarsBase::onRemoveAction()
{
    qWarning( "Gui::Dialog::DlgCustomToolbarsBase::onRemoveAction(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomToolbarsBase::onMoveDownAction()
{
    qWarning( "Gui::Dialog::DlgCustomToolbarsBase::onMoveDownAction(): Not implemented yet" );
}

void Gui::Dialog::DlgCustomToolbarsBase::onItemActivated(const QString&)
{
    qWarning( "Gui::Dialog::DlgCustomToolbarsBase::onItemActivated(const QString&): Not implemented yet" );
}

