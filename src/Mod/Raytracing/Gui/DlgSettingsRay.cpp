/****************************************************************************
** Form implementation generated from reading ui file '.\DlgSettingsRay.ui'
**
** Created: Sa 13. Mai 12:42:03 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgSettingsRay.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a RaytracingGui::DlgSettingsRay as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
RaytracingGui::DlgSettingsRay::DlgSettingsRay( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "RaytracingGui::DlgSettingsRay" );
    DlgSettingsRayLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgSettingsRayLayout"); 

    GroupBox12 = new QGroupBox( this, "GroupBox12" );
    GroupBox12->setColumnLayout(0, Qt::Vertical );
    GroupBox12->layout()->setSpacing( 6 );
    GroupBox12->layout()->setMargin( 11 );
    GroupBox12Layout = new QVBoxLayout( GroupBox12->layout() );
    GroupBox12Layout->setAlignment( Qt::AlignTop );

    groupBox4 = new QGroupBox( GroupBox12, "groupBox4" );
    groupBox4->setColumnLayout(0, Qt::Vertical );
    groupBox4->layout()->setSpacing( 6 );
    groupBox4->layout()->setMargin( 11 );
    groupBox4Layout = new QHBoxLayout( groupBox4->layout() );
    groupBox4Layout->setAlignment( Qt::AlignTop );

    layout3 = new QVBoxLayout( 0, 0, 6, "layout3"); 

    textLabel1 = new QLabel( groupBox4, "textLabel1" );
    layout3->addWidget( textLabel1 );

    textLabel1_2 = new QLabel( groupBox4, "textLabel1_2" );
    layout3->addWidget( textLabel1_2 );

    textLabel1_3 = new QLabel( groupBox4, "textLabel1_3" );
    layout3->addWidget( textLabel1_3 );
    groupBox4Layout->addLayout( layout3 );

    layout4 = new QVBoxLayout( 0, 0, 6, "layout4"); 

    prefFileChooser1 = new Gui::PrefFileChooser( groupBox4, "prefFileChooser1" );
    prefFileChooser1->setMode( Gui::PrefFileChooser::Directory );
    prefFileChooser1->setProperty( "prefEntry", "ProjectPath" );
    prefFileChooser1->setProperty( "prefPath", "Mod/Raytracing" );
    layout4->addWidget( prefFileChooser1 );

    prefLineEdit2 = new Gui::PrefLineEdit( groupBox4, "prefLineEdit2" );
    prefLineEdit2->setProperty( "prefEntry", "CameraName" );
    prefLineEdit2->setProperty( "prefPath", "Mod/Raytracing" );
    layout4->addWidget( prefLineEdit2 );

    prefLineEdit3 = new Gui::PrefLineEdit( groupBox4, "prefLineEdit3" );
    prefLineEdit3->setProperty( "prefEntry", "PartName" );
    prefLineEdit3->setProperty( "prefPath", "Mod/Raytracing" );
    layout4->addWidget( prefLineEdit3 );
    groupBox4Layout->addLayout( layout4 );
    GroupBox12Layout->addWidget( groupBox4 );

    groupBox5 = new QGroupBox( GroupBox12, "groupBox5" );
    groupBox5->setColumnLayout(0, Qt::Vertical );
    groupBox5->layout()->setSpacing( 6 );
    groupBox5->layout()->setMargin( 11 );
    groupBox5Layout = new QVBoxLayout( groupBox5->layout() );
    groupBox5Layout->setAlignment( Qt::AlignTop );

    layout4_2 = new QHBoxLayout( 0, 0, 6, "layout4_2"); 

    textLabel2 = new QLabel( groupBox5, "textLabel2" );
    textLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)5, 0, 0, textLabel2->sizePolicy().hasHeightForWidth() ) );
    layout4_2->addWidget( textLabel2 );

    prefFloatSpinBox1 = new Gui::PrefFloatSpinBox( groupBox5, "prefFloatSpinBox1" );
    prefFloatSpinBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, prefFloatSpinBox1->sizePolicy().hasHeightForWidth() ) );
    prefFloatSpinBox1->setMaxValue( 100 );
    prefFloatSpinBox1->setMinValue( 0.001 );
    prefFloatSpinBox1->setLineStep( 0.1 );
    prefFloatSpinBox1->setValue( 0.1 );
    prefFloatSpinBox1->setPrecision( 3 );
    prefFloatSpinBox1->setProperty( "prefEntry", "MeshDeviation" );
    prefFloatSpinBox1->setProperty( "prefPath", "Mod/Raytracing" );
    layout4_2->addWidget( prefFloatSpinBox1 );
    groupBox5Layout->addLayout( layout4_2 );

    prefCheckBox8 = new Gui::PrefCheckBox( groupBox5, "prefCheckBox8" );
    prefCheckBox8->setProperty( "prefEntry", "NotWriteVertexNormals" );
    prefCheckBox8->setProperty( "prefPath", "Mod/Raytracing" );
    groupBox5Layout->addWidget( prefCheckBox8 );

    prefCheckBox9 = new Gui::PrefCheckBox( groupBox5, "prefCheckBox9" );
    prefCheckBox9->setProperty( "prefEntry", "WriteUVCoordinates" );
    prefCheckBox9->setProperty( "prefPath", "Mod/Raytracing" );
    groupBox5Layout->addWidget( prefCheckBox9 );
    GroupBox12Layout->addWidget( groupBox5 );

    DlgSettingsRayLayout->addWidget( GroupBox12, 0, 0 );
    languageChange();
    resize( QSize(555, 267).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
RaytracingGui::DlgSettingsRay::~DlgSettingsRay()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void RaytracingGui::DlgSettingsRay::languageChange()
{
    setCaption( tr( "Raytracing" ) );
    GroupBox12->setTitle( tr( "Raytracing" ) );
    groupBox4->setTitle( tr( "Directories" ) );
    textLabel1->setText( tr( "Default Project dir:" ) );
    textLabel1_2->setText( tr( "Camera file name:" ) );
    textLabel1_3->setText( tr( "Part file name:" ) );
    prefLineEdit2->setText( tr( "TempCamera.inc" ) );
    prefLineEdit3->setText( tr( "TempPart.inc" ) );
    groupBox5->setTitle( tr( "Mesh export settings" ) );
    textLabel2->setText( tr( "max mesh deviation:" ) );
    prefCheckBox8->setText( tr( "Do not calculate vertex normals" ) );
    prefCheckBox9->setText( tr( "Write u,v coordinates" ) );
}

void RaytracingGui::DlgSettingsRay::warnInventor(bool)
{
    qWarning( "RaytracingGui::DlgSettingsRay::warnInventor(bool): Not implemented yet" );
}

void RaytracingGui::DlgSettingsRay::loadSettings()
{
    qWarning( "RaytracingGui::DlgSettingsRay::loadSettings(): Not implemented yet" );
}

void RaytracingGui::DlgSettingsRay::saveSettings()
{
    qWarning( "RaytracingGui::DlgSettingsRay::saveSettings(): Not implemented yet" );
}

