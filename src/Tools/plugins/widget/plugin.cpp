/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "plugin.h"
#include "customwidgets.h"

static const char *filechooser_pixmap[] = {
    "22 22 8 1",
    "  c Gray100",
    ". c Gray97",
    "X c #4f504f",
    "o c #00007f",
    "O c Gray0",
    "+ c none",
    "@ c Gray0",
    "# c Gray0",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "+OOOOOOOOOOOOOOOOOOOO+",
    "OOXXXXXXXXXXXXXXXXXXOO",
    "OXX.          OO OO  O",
    "OX.      oo     O    O",
    "OX.      oo     O   .O",
    "OX  ooo  oooo   O    O",
    "OX    oo oo oo  O    O",
    "OX  oooo oo oo  O    O",
    "OX oo oo oo oo  O    O",
    "OX oo oo oo oo  O    O",
    "OX  oooo oooo   O    O",
    "OX            OO OO  O",
    "OO..................OO",
    "+OOOOOOOOOOOOOOOOOOOO+",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++",
    "++++++++++++++++++++++"
};

/* XPM */
static char *combobox_pixmap[]={
"22 22 8 1",
"a c #000000",
"# c #000080",
"e c #008080",
"f c #800000",
"b c #808080",
"c c #c0c0c0",
"d c #ff0000",
". c #ffffff",
".#aaaaaaaaaaaaaaaaaa#.",
"#bbccccccccccccccccdd#",
"accee#########e.addfaa",
"#c..............a.fa.#",
"e#aaaaaaaaaaaaaaaaaa#e",
"....#c...............#",
"....ac...............a",
"....ac.ccbbbbbbbbeb..a",
"....ac.bbbeeeeeee##c.a",
"....ac.bee########ac.a",
"....ac..cccccccccccc.a",
"....ac.ccccccccccbec.a",
"....ac.cccccccccbbec.a",
"....ac.bcbbbbbbbbbec.a",
"....ac..cccccccccccc.a",
"....ac.cbbeeeeeee#bc.a",
"....ac.bee########ac.a",
"....ab.b##aaaaaaaaacca",
"....#bc.ccccccccccccc#",
".....ab............ca.",
".....eacc.........ca#.",
".......#aaaaaaaaaa#..."};

/* XPM */
static char *lineedit_pixmap[]={
"22 22 6 1",
"a c #000000",
"# c #000080",
"b c #008080",
"c c #808080",
"d c #c0c0c0",
". c #ffffff",
"......................",
"......................",
"......................",
"...#aaaaaaaaaaaaaa#...",
".baccccccccccccccccab.",
".acccddddddddddddddca.",
"#ccd................d#",
"acc.................da",
"acd.......d....ca.ac.a",
"acd......db......a...a",
"acd.dbbb.dbbbd...a...a",
"acd.ccdbddb.db...a...a",
"acd.dbbbddb..b...a...a",
"acd.bd.bddb..b...a...a",
"acd.bbbbddbbbc...a...a",
"acd..d.....dd..ca.acda",
"#cd.................d#",
".ac................da.",
".badd............dda#.",
"...#aaaaaaaaaaaaaa#...",
"......................",
"......................"};


/* XPM */
static char *checkbox_pixmap[]={
"22 22 4 1",
"# c #000000",
"a c #808080",
"b c #c0c0c0",
". c #ffffff",
"......................",
"......................",
"......................",
"......................",
"....###########aaa....",
"....##aaaaaaaaaabb....",
"....#aabbbbbbbbbbb....",
"....#abbbbbbbbaa......",
"....#abbbbbbba#a......",
"....#ababbbba##a......",
"....#ab#abba###a......",
"....#ab##aa###ab......",
"....#ab######abb......",
"....#abb####abbb......",
"....#abbb##abbbb......",
"....aabbbbabbbb.......",
"....abb......b........",
"....abb...............",
"......................",
"......................",
"......................",
"......................"};

/* XPM */
static char *iconview_pixmap[]={
"22 22 10 1",
"# c #000000",
"h c #000080",
"f c #0000ff",
"d c #008000",
"e c #008080",
"a c #800000",
"b c #808080",
"c c #c0c0c0",
"g c #ff0000",
". c #ffffff",
"...################...",
".a#bbccccccccccccbb#a.",
".#bcc..............b#.",
"#bb......c.....c....c#",
"#bbbbc..cbbc...bbbc.c#",
"#cccdd....bdb..ccdd..#",
"#cbcb#c.cbcbd..bcb#c.#",
"#cbbb#b..bbb#..cbb#c.#",
"#c..c##...cb#c...c##.#",
"#c...................#",
"#ccbbc..c#bbc..cbbcc.#",
"#c...................#",
"#cbbbaa.cb..cc..c.bb.#",
"#cbccca.c#ccb..cecf#.#",
"#cbcgba..c#b...bfbfh.#",
"#cacbba..bb#c..bbhb#.#",
"#caaaaa.bc.bb..bb###.#",
"#b..................c#",
"#b.bbcc..cbbbb.cbbc.c#",
".#b................c#.",
".a#cc............cc##.",
"...################..."};

/* XPM */
static char *listbox_pixmap[]={
"22 22 6 1",
"# c #000000",
"c c #800000",
"d c #808000",
"a c #808080",
"b c #c0c0c0",
". c #ffffff",
".####################.",
"#aabbbbbbbbbbbbb#abb.#",
"#abcccccccccccdb#b#a.#",
"#b..............#....#",
"c####################c",
"#b...............#...#",
"#b...............#...#",
"#b.###########a..#...#",
"#b...............#.#.#",
"#b.cccccccccccd..#.#.#",
"#b...............#.#.#",
"#b.cccccccccccd..#.#.#",
"#b...............#.#.#",
"#b.###########a..#.#.#",
"#b...............#...#",
"#b.###########a..#...#",
"#b...............#...#",
"#a.###########a..#..b#",
"#a...............#..b#",
".#a..............#.b#.",
".c#bb............#b##.",
"...################..."};

/* XPM */
static char *radiobutton_pixmap[]={
"22 22 4 1",
"b c #000000",
"# c #808080",
"a c #c0c0c0",
". c #ffffff",
"......................",
"......................",
"......................",
"......................",
".......########.......",
"......#####aaa##......",
".....#b##a...aaa#.....",
"....###aa.aa....a#....",
"....###a.####a...a....",
"....##a.####bba..a....",
"....##.a###bbb#.......",
"....#a.a##bbbb#.......",
"....#a..bbbbbba.......",
"....#aa.abbbb#...a....",
"....##a..a##a....a....",
".....#a.........a.....",
"......#a.......a......",
".......#aa...aa.......",
"......................",
"......................",
"......................",
"......................"};


/* XPM */
static char *slider_pixmap[]={
"22 22 5 1",
"b c #000000",
"c c #008080",
"# c #808080",
"a c #c0c0c0",
". c #ffffff",
"......................",
"......................",
"......................",
"......................",
"......................",
".........#............",
"........a##...........",
"........a##...........",
"........a##...........",
"..bbbb..a#bbbbbbbbbb..",
".bbbbb..a#bbbbbbbbbbc.",
".bb###..a#b########c#.",
".bbb##..a#b########aa.",
"..cc##..a#b########a..",
"........a##...........",
"........a##...........",
"........a##...........",
"......#####...........",
".......####...........",
"......................",
"......................",
"......................"};

/* XPM */
static char *spinbox_pixmap[]={
"22 22 6 1",
"a c #000000",
"# c #000080",
"b c #008080",
"c c #808080",
"d c #c0c0c0",
". c #ffffff",
"...#aaaaaaaaaaaaaa#...",
".baccccccccccccccccab.",
".acccddddddddddddddca.",
"#ccd................d#",
"acc.............dcd.da",
"acd.............dbd..a",
"acd............dcbbd.a",
"acd.d..dd..d...dbbbc.a",
"acddb.dbbdcbb.dbbb#bda",
"acd.b.d.cc..b.bb###bda",
"acd.b...bd.cb.dddccdda",
"acd.b...b..db...dddd.a",
"acd.b..cd...bdddccbbda",
"acd.b.dbbccdb.ccbbbbda",
"acddd.ddd.dd..dbbb#cda",
"acd............bb##cda",
"acd............db#cd.a",
"acd.............bbcdda",
"#cd.............ddd.d#",
".ac................da.",
".badd............dda#.",
"...#aaaaaaaaaaaaaa#..."};

/* XPM */
static char *colorbutton_pixmap[]={
"21 21 7 1",
"d c #000000",
"b c #000080",
"e c #0000ff",
"a c #008080",
"# c #808080",
"c c #c0c0c0",
". c #ffffff",
".....................",
".#abbbbbbbbbbbbbba#c.",
"c#c..............c##.",
"#c................ca.",
"#..................b.",
"#...ddddddddddd....b.",
"#...deeeeeeeeed....b.",
"#...deeeeeeeeed....b.",
"#...deeeeeeeeed....b.",
"#...deeeeeeeeed....b.",
"#...deeeeeeeeed....b.",
"#...deeeeeeeeed....b.",
"#...deeeeeeeeed....b.",
"#...deeeeeeeeed....b.",
"#...deeeeeeeeed....b.",
"#...ddddddddddd....b.",
"#..................b.",
"#.................cb.",
"#cccccccccccccccccca.",
"c#cccccccccccccccc##.",
".cccccccccccccccccc.."};

/* XPM */
static char *buttongroup_pixmap[]={
"22 22 16 1",
"# c #000000",
"i c #10a44a",
"n c #292929",
"a c #3d3d3d",
"b c #4a4a4a",
"j c #626262",
"g c #838383",
"f c #a0a0a0",
"e c #b9b9b9",
"h c #cacaca",
"c c #d6d6d6",
"m c #dedede",
"l c #e7e7e7",
"k c #efefef",
"d c #f7f7f7",
". c #ffffff",
"......................",
"....###########.......",
".#####################",
"#aabbbbbbbbbbbbbbbbaa#",
"#ac................ca#",
"#bddeefgdd##hdddddddb#",
"#bddeiijdddg#dddddddb#",
"#bkkfiijkk#k#kkkkkkkb#",
"#bkkgjjbkkj##gkgkgkkb#",
"#bllllllllllllllllllb#",
"#blleffgll#lllllllllb#",
"#bmmfiijmm##gmmmmmmmb#",
"#bmmfiibmm#m#mmmmmmmb#",
"#bccgjbbcc##gcgcgcgcb#",
"#bccccccccccccccccccb#",
"#bhhfffjhhf#ghhhhhhhb#",
"#bhhfiibhh#hhhhhhhhhb#",
"#bhhgiibhh#hhhhhhhhhb#",
"#beejbbbeef#jejejejeb#",
"#afeeeeeeeeeeeeeeeefa#",
"#aabbbbbbbbbbbbbbbban#",
"######################"};

CustomWidgetPlugin::CustomWidgetPlugin()
{
}

QStringList CustomWidgetPlugin::keys() const
{
  QStringList list;
  list << "Gui::FileChooser" << "Gui::PrefFileChooser" 
       << "Gui::AccelLineEdit" 
       << "Gui::CommandIconView"
       << "Gui::SpinBox" << "Gui::FloatSpinBox" 
       << "Gui::PrefSpinBox" << "Gui::PrefFloatSpinBox"
       << "Gui::PrefLineEdit" 
       << "Gui::PrefComboBox"
       << "Gui::PrefListBox"
       << "Gui::PrefCheckBox"
       << "Gui::PrefRadioButton" << "Gui::PrefButtonGroup"
       << "Gui::PrefSlider"
       << "Gui::ColorButton" << "Gui::PrefColorButton";
  return list;
}

QWidget* CustomWidgetPlugin::create( const QString &key, QWidget* parent, const char* name )
{
  if ( key == "Gui::FileChooser" )
  	return new Gui::FileChooser( parent, name );
  else if ( key == "Gui::PrefFileChooser" )
  	return new Gui::PrefFileChooser( parent, name );
  else if ( key == "Gui::AccelLineEdit" )
    return new Gui::AccelLineEdit( parent, name );
  else if ( key == "Gui::CommandIconView" )
    return new Gui::CommandIconView( parent, name );
  else if ( key == "Gui::SpinBox" )
    return new Gui::SpinBox( parent, name );
  else if ( key == "Gui::FloatSpinBox" )
    return new Gui::FloatSpinBox( parent, name );
  else if ( key == "Gui::PrefSpinBox" )
    return new Gui::PrefSpinBox( parent, name );
  else if ( key == "Gui::PrefFloatSpinBox" )
    return new Gui::PrefFloatSpinBox( parent, name );
  else if ( key == "Gui::PrefLineEdit" )
    return new Gui::PrefLineEdit( parent, name );
  else if ( key == "Gui::PrefComboBox" )
    return new Gui::PrefComboBox( parent, name );
  else if ( key == "Gui::PrefListBox" )
    return new Gui::PrefListBox( parent, name );
  else if ( key == "Gui::PrefCheckBox" )
    return new Gui::PrefCheckBox( parent, name );
  else if ( key == "Gui::PrefRadioButton" )
    return new Gui::PrefRadioButton( parent, name );
  else if ( key == "Gui::PrefButtonGroup" )
    return new Gui::PrefButtonGroup( parent, name );
  else if ( key == "Gui::PrefSlider" )
    return new Gui::PrefSlider( parent, name );
  else if ( key == "Gui::ColorButton" )
    return new Gui::ColorButton( parent, name );
  else if ( key == "Gui::PrefColorButton" )
    return new Gui::PrefColorButton( parent, name );

  return 0;
}

QString CustomWidgetPlugin::group( const QString& feature ) const
{
  if ( feature.startsWith( "Gui::Pref" ) )
    return "Preference";
  else if ( feature == "Gui::FileChooser" )
  	return "Input";
  else if ( feature == "Gui::AccelLineEdit" )
    return "Input";
  else if ( feature == "Gui::CommandIconView" )
    return "Views";
  else if ( feature == "Gui::SpinBox" )
    return "Input";
  else if ( feature == "Gui::FloatSpinBox" )
    return "Input";
  else if ( feature == "Gui::ColorButton" )
    return "Buttons";

  return QString::null;
}

QIconSet CustomWidgetPlugin::iconSet( const QString& feature ) const
{
  if ( feature == "Gui::FileChooser" )
    return QIconSet( QPixmap( filechooser_pixmap ) );
  else if ( feature == "Gui::PrefFileChooser" )
    return QIconSet( QPixmap( filechooser_pixmap ) );
  else if ( feature == "Gui::AccelLineEdit" )
    return QIconSet( QPixmap( lineedit_pixmap ) );
  else if ( feature == "Gui::CommandIconView" )
    return QIconSet( QPixmap( iconview_pixmap ) );
  else if ( feature == "Gui::SpinBox" )
    return QIconSet( QPixmap( spinbox_pixmap ) );
  else if ( feature == "Gui::FloatSpinBox" )
    return QIconSet( QPixmap( spinbox_pixmap ) );
  else if ( feature == "Gui::PrefSpinBox" )
    return QIconSet( QPixmap( spinbox_pixmap ) );
  else if ( feature == "Gui::PrefFloatSpinBox" )
    return QIconSet( QPixmap( spinbox_pixmap ) );
  else if ( feature == "Gui::PrefLineEdit" )
    return QIconSet( QPixmap( lineedit_pixmap ) );
  else if ( feature == "Gui::PrefComboBox" )
    return QIconSet( QPixmap( combobox_pixmap ) );
  else if ( feature == "Gui::PrefListBox" )
    return QIconSet( QPixmap( listbox_pixmap ) );
  else if ( feature == "Gui::PrefCheckBox" )
    return QIconSet( QPixmap( checkbox_pixmap ) );
  else if ( feature == "Gui::PrefRadioButton" )
    return QIconSet( QPixmap( radiobutton_pixmap ) );
  else if ( feature == "Gui::PrefButtonGroup" )
    return QIconSet( QPixmap( buttongroup_pixmap ) );
  else if ( feature == "Gui::PrefSlider" )
    return QIconSet( QPixmap( slider_pixmap ) );
  else if ( feature == "Gui::ColorButton" )
    return QIconSet( QPixmap( colorbutton_pixmap ) );
  else if ( feature == "Gui::PrefColorButton" )
    return QIconSet( QPixmap( colorbutton_pixmap ) );

  return QIconSet();
}

QString CustomWidgetPlugin::includeFile( const QString& feature ) const
{
  if ( feature.startsWith( "Gui::Pref" ) )
    return "Gui/PrefWidgets.h";
  else if ( feature == "Gui::FileChooser" )
  	return "Gui/FileDialog.h";
  else if ( feature == "Gui::AccelLineEdit" )
    return "Gui/Widgets.h";
  else if ( feature == "Gui::CommandIconView" )
  	return "Gui/Widgets.h";
  else if ( feature == "Gui::SpinBox" )
  	return "Gui/SpinBox.h";
  else if ( feature == "Gui::FloatSpinBox" )
  	return "Gui/SpinBox.h";
  else if ( feature == "Gui::ColorButton" )
  	return "Gui/Widgets.h";

  return QString::null;
}

QString CustomWidgetPlugin::toolTip( const QString& feature ) const
{
  if ( feature == "Gui::FileChooser" )
  	return "File Chooser";
  else if ( feature == "Gui::PrefFileChooser" )
  	return "File Chooser";
  else if ( feature == "Gui::AccelLineEdit" )
  	return "Accelerator Line Edit";
  else if ( feature == "Gui::CommandIconView" )
  	return "Command View";
  else if ( feature == "Gui::SpinBox" )
  	return "Spin Box";
  else if ( feature == "Gui::FloatSpinBox" )
  	return "Double Spin Box";
  else if ( feature == "Gui::PrefSpinBox" )
  	return "Spin Box";
  else if ( feature == "Gui::PrefFloatSpinBox" )
    return "Double Spin Box";
  else if ( feature == "Gui::PrefLineEdit" )
  	return "Line Edit";
  else if ( feature == "Gui::PrefComboBox" )
  	return "Combo Box";
  else if ( feature == "Gui::PrefListBox" )
  	return "List Box";
  else if ( feature == "Gui::PrefCheckBox" )
  	return "Check Box";
  else if ( feature == "Gui::PrefRadioButton" )
  	return "Radio Button";
  else if ( feature == "Gui::PrefButtonGroup" )
  	return "Button Group";
  else if ( feature == "Gui::PrefSlider" )
  	return "Slider";
  else if ( feature == "Gui::ColorButton" )
  	return "Color Button";
  else if ( feature == "Gui::PrefColorButton" )
  	return "Color Button";

  return QString::null;
}

QString CustomWidgetPlugin::whatsThis( const QString& feature ) const
{
  if ( feature == "Gui::FileChooser" )
  	return "A widget to choose a file or directory.";
  else if ( feature == "Gui::PrefFileChooser" )
  	return "A widget to choose a file or directory.";
  else if ( feature == "Gui::AccelLineEdit" )
  	return "A widget to specify accelerator keys.";
  else if ( feature == "Gui::CommandIconView" )
  	return "Area with movable and labeled icons.";
  else if ( feature == "Gui::SpinBox" )
  	return "Spin box widget (spin button).";
  else if ( feature == "Gui::FloatSpinBox" )
  	return "Spin box widget that can work with doubles.";
  else if ( feature == "Gui::PrefSpinBox" )
    return "Spin box widget (spin button).";
  else if ( feature == "Gui::PrefFloatSpinBox" )
  	return "Spin box widget that can work with doubles.";
  else if ( feature == "Gui::PrefLineEdit" )
  	return "One-line text editor.";
  else if ( feature == "Gui::PrefComboBox" )
  	return "Combined button and popup list.";
  else if ( feature == "Gui::PrefListBox" )
  	return "List of selectable, read-only items.";
  else if ( feature == "Gui::PrefCheckBox" )
  	return "Checkbox with a text label.";
  else if ( feature == "Gui::PrefRadioButton" )
  	return "Radio button with a text or pixmap label.";
  else if ( feature == "Gui::PrefButtonGroup" )
  	return "Organizes QButton widgets in a group.";
  else if ( feature == "Gui::PrefSlider" )
  	return "Vertical or horizontal slider.";
  else if ( feature == "Gui::ColorButton" )
  	return "A button to choose a color.";
  else if ( feature == "Gui::PrefColorButton" )
  	return "A button to choose a color.";

  return QString::null;
}

bool CustomWidgetPlugin::isContainer( const QString& feature ) const
{
  if ( feature == "Gui::PrefButtonGroup" )
  	return TRUE;
  return FALSE;
}


Q_EXPORT_PLUGIN( CustomWidgetPlugin ) 
