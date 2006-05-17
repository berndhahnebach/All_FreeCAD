/****************************************************************************
** Form implementation generated from reading ui file '.\DlgTipOfTheDay.ui'
**
** Created: Mi 17. Mai 14:36:04 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgTipOfTheDay.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"26 32 8 1",
". c None",
"d c #000000",
"e c #00ffff",
"# c #808000",
"c c #808080",
"f c #c0c0c0",
"a c #ffff00",
"b c #ffffff",
"..........#####...........",
"........##aaaaa##.........",
".......#ababababa#........",
"......#abbbcdcbaaad.......",
".....#abbbbdddababad......",
".....#bbbbbdddbabaad......",
"....#abbbbbdddababa#d.....",
"....#abbbbbcdcbabaa#d.....",
"....#babbbbbdbababa#d.....",
"....#aaababadababaa#d.....",
"....#aababababababa#d.....",
".....daaaabcdcbaaa#d......",
".....d#aabadddabab#d......",
"......d#aaacdcaaa#d.......",
"......ed#aababab#ded......",
".....ebd#aaaaaaa#dbed.....",
"....ebbbd##aaa##dbbbed....",
"...ebbbbcdd###dddbbbbed...",
".debbbbbcbbdddffdbbbbbed..",
"..debbbbccdbffdddbbbbed...",
"...debbbbcbdddfdbbbbed....",
"....debbbccbffddbbbed.....",
".....debbbdddddbbbed......",
"......debbbdddbbbed.......",
".......debbbbbbbed........",
"........debbbbbed.........",
".........debbbed..........",
"..........debed...........",
"...........ded............",
"............d.............",
"..........................",
".........................."};


/*
 *  Constructs a Gui::Dialog::DlgTipOfTheDayBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Gui::Dialog::DlgTipOfTheDayBase::DlgTipOfTheDayBase( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ),
      image0( (const char **) image0_data )
{
    if ( !name )
	setName( "Gui::Dialog::DlgTipOfTheDayBase" );
    DlgTipOfTheDayBaseLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgTipOfTheDayBaseLayout"); 

    layout2 = new QHBoxLayout( 0, 0, 6, "layout2"); 

    checkShowTips = new QCheckBox( this, "checkShowTips" );
    layout2->addWidget( checkShowTips );
    QSpacerItem* spacer = new QSpacerItem( 40, 21, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout2->addItem( spacer );

    pushButtonNext = new QPushButton( this, "pushButtonNext" );
    pushButtonNext->setDefault( TRUE );
    layout2->addWidget( pushButtonNext );

    pushButtonClose = new QPushButton( this, "pushButtonClose" );
    layout2->addWidget( pushButtonClose );

    DlgTipOfTheDayBaseLayout->addLayout( layout2, 1, 0 );

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setPaletteBackgroundColor( QColor( 134, 132, 127 ) );
    groupBox1->setFrameShape( QGroupBox::WinPanel );
    groupBox1->setColumnLayout(0, Qt::Vertical );
    groupBox1->layout()->setSpacing( 0 );
    groupBox1->layout()->setMargin( 0 );
    groupBox1Layout = new QGridLayout( groupBox1->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    layout4 = new QVBoxLayout( 0, 0, 1, "layout4"); 

    textHeader = new QLabel( groupBox1, "textHeader" );
    textHeader->setMaximumSize( QSize( 32767, 50 ) );
    textHeader->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    textHeader->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    layout4->addWidget( textHeader );

    textTip = new QLabel( groupBox1, "textTip" );
    textTip->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    QFont textTip_font(  textTip->font() );
    textTip_font.setPointSize( 10 );
    textTip->setFont( textTip_font ); 
    textTip->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    textTip->setIndent( 5 );
    layout4->addWidget( textTip );

    groupBox1Layout->addLayout( layout4, 0, 1 );

    groupBox2 = new QGroupBox( groupBox1, "groupBox2" );
    groupBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)5, 0, 1, groupBox2->sizePolicy().hasHeightForWidth() ) );
    groupBox2->setMaximumSize( QSize( 50, 32767 ) );
    groupBox2->setPaletteBackgroundColor( QColor( 134, 132, 127 ) );
    groupBox2->setFrameShape( QGroupBox::NoFrame );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    groupBox2Layout = new QGridLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    pixmapLightbulb = new QLabel( groupBox2, "pixmapLightbulb" );
    pixmapLightbulb->setMinimumSize( QSize( 0, 46 ) );
    pixmapLightbulb->setMaximumSize( QSize( 40, 32767 ) );
    pixmapLightbulb->setPaletteBackgroundColor( QColor( 134, 132, 127 ) );
    pixmapLightbulb->setPixmap( image0 );
    pixmapLightbulb->setScaledContents( FALSE );
    pixmapLightbulb->setAlignment( int( QLabel::AlignBottom | QLabel::AlignHCenter ) );

    groupBox2Layout->addWidget( pixmapLightbulb, 0, 0 );

    groupBox1Layout->addWidget( groupBox2, 0, 0 );

    DlgTipOfTheDayBaseLayout->addWidget( groupBox1, 0, 0 );
    languageChange();
    resize( QSize(415, 270).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButtonClose, SIGNAL( clicked() ), this, SLOT( close() ) );
    connect( pushButtonNext, SIGNAL( clicked() ), this, SLOT( onNext() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgTipOfTheDayBase::~DlgTipOfTheDayBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgTipOfTheDayBase::languageChange()
{
    setCaption( tr( "Tip of the day" ) );
    checkShowTips->setText( tr( "&Show tips at startup" ) );
    checkShowTips->setAccel( QKeySequence( tr( "Alt+S" ) ) );
    pushButtonNext->setText( tr( "&Next Tip" ) );
    pushButtonNext->setAccel( QKeySequence( tr( "Alt+N" ) ) );
    pushButtonClose->setText( tr( "&Close" ) );
    pushButtonClose->setAccel( QKeySequence( tr( "Alt+C" ) ) );
    groupBox1->setTitle( QString::null );
    textHeader->setText( tr( "<b><font face=\"Times New Roman\"><font size=\"+3\">Did you know...</font></font></b>" ) );
    textTip->setText( tr( "Tip of the day" ) );
    groupBox2->setTitle( QString::null );
}

void Gui::Dialog::DlgTipOfTheDayBase::onNext()
{
    qWarning( "Gui::Dialog::DlgTipOfTheDayBase::onNext(): Not implemented yet" );
}

