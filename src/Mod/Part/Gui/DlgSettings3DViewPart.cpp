/****************************************************************************
** Form implementation generated from reading ui file '.\DlgSettings3DViewPart.ui'
**
** Created: Sa 13. Mai 12:40:17 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgSettings3DViewPart.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a PartGui::DlgSettings3DViewPart as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
PartGui::DlgSettings3DViewPart::DlgSettings3DViewPart( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "PartGui::DlgSettings3DViewPart" );
    DlgSettings3DViewPartLayout = new QVBoxLayout( this, 11, 6, "DlgSettings3DViewPartLayout"); 

    GroupBox12 = new QGroupBox( this, "GroupBox12" );
    GroupBox12->setColumnLayout(0, Qt::Vertical );
    GroupBox12->layout()->setSpacing( 6 );
    GroupBox12->layout()->setMargin( 11 );
    GroupBox12Layout = new QVBoxLayout( GroupBox12->layout() );
    GroupBox12Layout->setAlignment( Qt::AlignTop );

    layout14 = new QHBoxLayout( 0, 0, 6, "layout14"); 

    textLabel1_3 = new QLabel( GroupBox12, "textLabel1_3" );
    layout14->addWidget( textLabel1_3 );

    line2 = new QFrame( GroupBox12, "line2" );
    line2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, line2->sizePolicy().hasHeightForWidth() ) );
    line2->setFrameShape( QFrame::HLine );
    line2->setFrameShadow( QFrame::Sunken );
    line2->setFrameShape( QFrame::HLine );
    layout14->addWidget( line2 );
    GroupBox12Layout->addLayout( layout14 );

    layout1 = new QHBoxLayout( 0, 0, 6, "layout1"); 

    textLabel1 = new QLabel( GroupBox12, "textLabel1" );
    layout1->addWidget( textLabel1 );

    prefFloatSpinBox1 = new Gui::PrefFloatSpinBox( GroupBox12, "prefFloatSpinBox1" );
    prefFloatSpinBox1->setMaxValue( 100 );
    prefFloatSpinBox1->setMinValue( 0.01 );
    prefFloatSpinBox1->setLineStep( 0.01 );
    prefFloatSpinBox1->setValue( 0.5 );
    prefFloatSpinBox1->setPrecision( 2 );
    prefFloatSpinBox1->setProperty( "prefEntry", "MeshDeviation" );
    prefFloatSpinBox1->setProperty( "prefPath", "Mod/Part" );
    layout1->addWidget( prefFloatSpinBox1 );
    GroupBox12Layout->addLayout( layout1 );

    layout14_2_2 = new QHBoxLayout( 0, 0, 6, "layout14_2_2"); 

    textLabel1_3_3_2 = new QLabel( GroupBox12, "textLabel1_3_3_2" );
    layout14_2_2->addWidget( textLabel1_3_3_2 );

    line2_2_2 = new QFrame( GroupBox12, "line2_2_2" );
    line2_2_2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, line2_2_2->sizePolicy().hasHeightForWidth() ) );
    line2_2_2->setFrameShape( QFrame::HLine );
    line2_2_2->setFrameShadow( QFrame::Sunken );
    line2_2_2->setFrameShape( QFrame::HLine );
    layout14_2_2->addWidget( line2_2_2 );
    GroupBox12Layout->addLayout( layout14_2_2 );

    prefCheckBox8 = new Gui::PrefCheckBox( GroupBox12, "prefCheckBox8" );
    prefCheckBox8->setProperty( "prefEntry", "NoPerVertexNormals" );
    prefCheckBox8->setProperty( "prefPath", "Mod/Part" );
    GroupBox12Layout->addWidget( prefCheckBox8 );

    prefCheckBox3 = new Gui::PrefCheckBox( GroupBox12, "prefCheckBox3" );
    prefCheckBox3->setProperty( "prefEntry", "QualityNormals" );
    prefCheckBox3->setProperty( "prefPath", "Mod/Part" );
    GroupBox12Layout->addWidget( prefCheckBox3 );
    DlgSettings3DViewPartLayout->addWidget( GroupBox12 );
    languageChange();
    resize( QSize(544, 520).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PartGui::DlgSettings3DViewPart::~DlgSettings3DViewPart()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PartGui::DlgSettings3DViewPart::languageChange()
{
    setCaption( tr( "Part Viewing Settings" ) );
    GroupBox12->setTitle( tr( "3DViewPart settings" ) );
    textLabel1_3->setText( tr( "View accuracy / Performance" ) );
    textLabel1->setText( tr( "Max deviation of view mesh (smaller means nicer but slower):" ) );
    textLabel1_3_3_2->setText( tr( "View smooting" ) );
    prefCheckBox8->setText( tr( "Not using per vertex normals (using flat shading)" ) );
    prefCheckBox3->setText( tr( "Using high qualiti normals (slower but nicer)" ) );
}

void PartGui::DlgSettings3DViewPart::warnInventor(bool)
{
    qWarning( "PartGui::DlgSettings3DViewPart::warnInventor(bool): Not implemented yet" );
}

void PartGui::DlgSettings3DViewPart::loadSettings()
{
    qWarning( "PartGui::DlgSettings3DViewPart::loadSettings(): Not implemented yet" );
}

void PartGui::DlgSettings3DViewPart::saveSettings()
{
    qWarning( "PartGui::DlgSettings3DViewPart::saveSettings(): Not implemented yet" );
}

