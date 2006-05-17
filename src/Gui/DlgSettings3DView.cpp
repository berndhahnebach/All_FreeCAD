/****************************************************************************
** Form implementation generated from reading ui file '.\DlgSettings3DView.ui'
**
** Created: Sa 13. Mai 12:36:03 2006
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.2.1   edited May 19 14:22 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "DlgSettings3DView.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Gui::Dialog::DlgSettings3DView as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Gui::Dialog::DlgSettings3DView::DlgSettings3DView( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Gui::Dialog::DlgSettings3DView" );
    DlgSettings3DViewLayout = new QGridLayout( this, 1, 1, 11, 6, "DlgSettings3DViewLayout"); 

    GroupBox12 = new QGroupBox( this, "GroupBox12" );
    GroupBox12->setColumnLayout(0, Qt::Vertical );
    GroupBox12->layout()->setSpacing( 6 );
    GroupBox12->layout()->setMargin( 11 );
    GroupBox12Layout = new QGridLayout( GroupBox12->layout() );
    GroupBox12Layout->setAlignment( Qt::AlignTop );

    CheckBox_ShowFPS = new Gui::PrefCheckBox( GroupBox12, "CheckBox_ShowFPS" );
    CheckBox_ShowFPS->setEnabled( FALSE );
    CheckBox_ShowFPS->setProperty( "prefEntry", "ShowFPS" );
    CheckBox_ShowFPS->setProperty( "prefPath", "View" );

    GroupBox12Layout->addWidget( CheckBox_ShowFPS, 1, 0 );

    prefCheckBox8 = new Gui::PrefCheckBox( GroupBox12, "prefCheckBox8" );
    prefCheckBox8->setProperty( "prefEntry", "UseAntialiasing" );
    prefCheckBox8->setProperty( "prefPath", "View" );

    GroupBox12Layout->addWidget( prefCheckBox8, 3, 0 );

    line1 = new QFrame( GroupBox12, "line1" );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );

    GroupBox12Layout->addWidget( line1, 4, 0 );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4"); 

    textLabel1 = new QLabel( GroupBox12, "textLabel1" );
    layout4->addWidget( textLabel1 );

    FloatSpinBox_EyeDistance = new Gui::PrefFloatSpinBox( GroupBox12, "FloatSpinBox_EyeDistance" );
    FloatSpinBox_EyeDistance->setMaxValue( 1000 );
    FloatSpinBox_EyeDistance->setMinValue( 0.1 );
    FloatSpinBox_EyeDistance->setLineStep( 2 );
    FloatSpinBox_EyeDistance->setValue( 65 );
    FloatSpinBox_EyeDistance->setProperty( "prefEntry", "EyeDistance" );
    FloatSpinBox_EyeDistance->setProperty( "prefPath", "View" );
    layout4->addWidget( FloatSpinBox_EyeDistance );

    GroupBox12Layout->addLayout( layout4, 5, 0 );

    CheckBox_CornerCoordSystem = new Gui::PrefCheckBox( GroupBox12, "CheckBox_CornerCoordSystem" );
    CheckBox_CornerCoordSystem->setChecked( TRUE );
    CheckBox_CornerCoordSystem->setProperty( "prefEntry", "CornerCoordSystem" );
    CheckBox_CornerCoordSystem->setProperty( "prefPath", "View" );

    GroupBox12Layout->addWidget( CheckBox_CornerCoordSystem, 0, 0 );

    CheckBox_UseAutoRotation = new Gui::PrefCheckBox( GroupBox12, "CheckBox_UseAutoRotation" );
    CheckBox_UseAutoRotation->setEnabled( TRUE );
    CheckBox_UseAutoRotation->setChecked( TRUE );
    CheckBox_UseAutoRotation->setProperty( "prefEntry", "UseAutoRotation" );
    CheckBox_UseAutoRotation->setProperty( "prefPath", "View" );

    GroupBox12Layout->addWidget( CheckBox_UseAutoRotation, 2, 0 );

    DlgSettings3DViewLayout->addWidget( GroupBox12, 0, 0 );

    selectionGroup = new QButtonGroup( this, "selectionGroup" );
    selectionGroup->setColumnLayout(0, Qt::Vertical );
    selectionGroup->layout()->setSpacing( 6 );
    selectionGroup->layout()->setMargin( 11 );
    selectionGroupLayout = new QGridLayout( selectionGroup->layout() );
    selectionGroupLayout->setAlignment( Qt::AlignTop );

    checkBoxPreselection = new Gui::PrefCheckBox( selectionGroup, "checkBoxPreselection" );
    checkBoxPreselection->setProperty( "prefEntry", "DisablePreselection" );
    checkBoxPreselection->setProperty( "prefPath", "View" );

    selectionGroupLayout->addWidget( checkBoxPreselection, 0, 0 );

    checkBoxSelection = new Gui::PrefCheckBox( selectionGroup, "checkBoxSelection" );
    checkBoxSelection->setProperty( "prefEntry", "DisableSelection" );
    checkBoxSelection->setProperty( "prefPath", "View" );

    selectionGroupLayout->addWidget( checkBoxSelection, 1, 0 );

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 

    textLabel1_2 = new QLabel( selectionGroup, "textLabel1_2" );
    layout6->addWidget( textLabel1_2 );

    HighlightColor = new Gui::PrefColorButton( selectionGroup, "HighlightColor" );
    HighlightColor->setColor( QColor( 26, 26, 204 ) );
    HighlightColor->setProperty( "prefEntry", "HighlightColor" );
    HighlightColor->setProperty( "prefPath", "View" );
    layout6->addWidget( HighlightColor );

    selectionGroupLayout->addLayout( layout6, 2, 0 );

    layout7 = new QHBoxLayout( 0, 0, 6, "layout7"); 

    textLabel2 = new QLabel( selectionGroup, "textLabel2" );
    layout7->addWidget( textLabel2 );

    SelectionColor = new Gui::PrefColorButton( selectionGroup, "SelectionColor" );
    SelectionColor->setColor( QColor( 26, 128, 26 ) );
    SelectionColor->setProperty( "prefEntry", "SelectionColor" );
    SelectionColor->setProperty( "prefPath", "View" );
    layout7->addWidget( SelectionColor );

    selectionGroupLayout->addLayout( layout7, 3, 0 );

    DlgSettings3DViewLayout->addWidget( selectionGroup, 1, 0 );

    backgroundColorGroup = new QButtonGroup( this, "backgroundColorGroup" );
    backgroundColorGroup->setColumnLayout(0, Qt::Vertical );
    backgroundColorGroup->layout()->setSpacing( 6 );
    backgroundColorGroup->layout()->setMargin( 11 );
    backgroundColorGroupLayout = new QGridLayout( backgroundColorGroup->layout() );
    backgroundColorGroupLayout->setAlignment( Qt::AlignTop );

    radioButtonSimple = new Gui::PrefRadioButton( backgroundColorGroup, "radioButtonSimple" );
    radioButtonSimple->setProperty( "prefEntry", "Simple" );
    radioButtonSimple->setProperty( "prefPath", "View" );
    backgroundColorGroup->insert( radioButtonSimple, 0 );

    backgroundColorGroupLayout->addWidget( radioButtonSimple, 0, 0 );

    radioButtonGradient = new Gui::PrefRadioButton( backgroundColorGroup, "radioButtonGradient" );
    radioButtonGradient->setChecked( TRUE );
    radioButtonGradient->setProperty( "prefEntry", "Gradient" );
    radioButtonGradient->setProperty( "prefPath", "View" );
    backgroundColorGroup->insert( radioButtonGradient, 1 );

    backgroundColorGroupLayout->addWidget( radioButtonGradient, 1, 0 );

    SelectionColor_Background = new Gui::PrefColorButton( backgroundColorGroup, "SelectionColor_Background" );
    SelectionColor_Background->setEnabled( FALSE );
    SelectionColor_Background->setProperty( "prefEntry", "BackgroundColor" );
    SelectionColor_Background->setProperty( "prefPath", "View" );

    backgroundColorGroupLayout->addWidget( SelectionColor_Background, 0, 1 );

    backgroundColorFrom = new Gui::PrefColorButton( backgroundColorGroup, "backgroundColorFrom" );
    backgroundColorFrom->setColor( QColor( 127, 127, 205 ) );
    backgroundColorFrom->setProperty( "prefEntry", "BackgroundColor2" );
    backgroundColorFrom->setProperty( "prefPath", "View" );

    backgroundColorGroupLayout->addWidget( backgroundColorFrom, 1, 1 );

    backgroundColorTo = new Gui::PrefColorButton( backgroundColorGroup, "backgroundColorTo" );
    backgroundColorTo->setColor( QColor( 255, 255, 255 ) );
    backgroundColorTo->setProperty( "prefEntry", "BackgroundColor3" );
    backgroundColorTo->setProperty( "prefPath", "View" );

    backgroundColorGroupLayout->addWidget( backgroundColorTo, 2, 1 );

    DlgSettings3DViewLayout->addWidget( backgroundColorGroup, 3, 0 );

    cameraColorGroup = new QButtonGroup( this, "cameraColorGroup" );
    cameraColorGroup->setColumnLayout(0, Qt::Vertical );
    cameraColorGroup->layout()->setSpacing( 6 );
    cameraColorGroup->layout()->setMargin( 11 );
    cameraColorGroupLayout = new QGridLayout( cameraColorGroup->layout() );
    cameraColorGroupLayout->setAlignment( Qt::AlignTop );

    radioPerspective = new Gui::PrefRadioButton( cameraColorGroup, "radioPerspective" );
    radioPerspective->setChecked( TRUE );
    radioPerspective->setProperty( "prefEntry", "Perspective" );
    radioPerspective->setProperty( "prefPath", "View" );
    cameraColorGroup->insert( radioPerspective, 0 );

    cameraColorGroupLayout->addWidget( radioPerspective, 0, 0 );

    radioOrthographic = new Gui::PrefRadioButton( cameraColorGroup, "radioOrthographic" );
    radioOrthographic->setProperty( "prefEntry", "Orthographic" );
    radioOrthographic->setProperty( "prefPath", "View" );
    cameraColorGroup->insert( radioOrthographic, 1 );

    cameraColorGroupLayout->addWidget( radioOrthographic, 1, 0 );

    DlgSettings3DViewLayout->addWidget( cameraColorGroup, 2, 0 );
    languageChange();
    resize( QSize(456, 577).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( radioButtonSimple, SIGNAL( toggled(bool) ), SelectionColor_Background, SLOT( setEnabled(bool) ) );
    connect( radioButtonGradient, SIGNAL( toggled(bool) ), backgroundColorFrom, SLOT( setEnabled(bool) ) );
    connect( radioButtonGradient, SIGNAL( toggled(bool) ), backgroundColorTo, SLOT( setEnabled(bool) ) );
    connect( radioButtonGradient, SIGNAL( toggled(bool) ), SelectionColor_Background, SLOT( setDisabled(bool) ) );
    connect( checkBoxSelection, SIGNAL( toggled(bool) ), SelectionColor, SLOT( setDisabled(bool) ) );
    connect( checkBoxPreselection, SIGNAL( toggled(bool) ), HighlightColor, SLOT( setDisabled(bool) ) );

    // tab order
    setTabOrder( CheckBox_CornerCoordSystem, CheckBox_ShowFPS );
    setTabOrder( CheckBox_ShowFPS, CheckBox_UseAutoRotation );
    setTabOrder( CheckBox_UseAutoRotation, prefCheckBox8 );
    setTabOrder( prefCheckBox8, FloatSpinBox_EyeDistance );
    setTabOrder( FloatSpinBox_EyeDistance, checkBoxPreselection );
    setTabOrder( checkBoxPreselection, HighlightColor );
    setTabOrder( HighlightColor, SelectionColor );
    setTabOrder( SelectionColor, radioPerspective );
    setTabOrder( radioPerspective, radioButtonGradient );
    setTabOrder( radioButtonGradient, SelectionColor_Background );
    setTabOrder( SelectionColor_Background, backgroundColorFrom );
    setTabOrder( backgroundColorFrom, backgroundColorTo );
    setTabOrder( backgroundColorTo, radioButtonSimple );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Gui::Dialog::DlgSettings3DView::~DlgSettings3DView()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Gui::Dialog::DlgSettings3DView::languageChange()
{
    setCaption( tr( "3D View" ) );
    GroupBox12->setTitle( tr( "3DView settings" ) );
    CheckBox_ShowFPS->setText( tr( "Show FPS Count" ) );
    prefCheckBox8->setText( tr( "Enable antialiasing (slower)" ) );
    textLabel1->setText( tr( "Eye to eye distance for stereo modes:" ) );
    CheckBox_CornerCoordSystem->setText( tr( "Show coordinate system in the corner" ) );
    CheckBox_UseAutoRotation->setText( tr( "Use spin rotation" ) );
    selectionGroup->setTitle( tr( "Selection" ) );
    checkBoxPreselection->setText( tr( "Disable preselection highlighting" ) );
    checkBoxSelection->setText( tr( "Disable selection highlighting" ) );
    textLabel1_2->setText( tr( "Highlight color:" ) );
    HighlightColor->setText( QString::null );
    textLabel2->setText( tr( "Selection color:" ) );
    SelectionColor->setText( QString::null );
    backgroundColorGroup->setTitle( tr( "Background color" ) );
    radioButtonSimple->setText( tr( "Simple color" ) );
    radioButtonGradient->setText( tr( "Color gradient" ) );
    SelectionColor_Background->setText( QString::null );
    backgroundColorFrom->setText( QString::null );
    backgroundColorTo->setText( QString::null );
    cameraColorGroup->setTitle( tr( "Camera type" ) );
    radioPerspective->setText( tr( "Perspective rendering" ) );
    radioOrthographic->setText( tr( "Orthographic rendering" ) );
}

void Gui::Dialog::DlgSettings3DView::loadSettings()
{
    qWarning( "Gui::Dialog::DlgSettings3DView::loadSettings(): Not implemented yet" );
}

void Gui::Dialog::DlgSettings3DView::saveSettings()
{
    qWarning( "Gui::Dialog::DlgSettings3DView::saveSettings(): Not implemented yet" );
}

