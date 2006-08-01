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


#include "PreCompiled.h"
#ifndef _PreComp_
# include <qfontdatabase.h>
# include <assert.h>
#endif

#include "DlgEditorImp.h"
#include "PrefWidgets.h"
#include "PythonEditor.h"

using namespace Gui;
using namespace Gui::Dialog;

/**
 *  Constructs a DlgSettingsEditorImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgSettingsEditorImp::DlgSettingsEditorImp( QWidget* parent,  const char* name, WFlags fl )
    : DlgEditorSettingsBase( parent, name, fl )
{
  pythonSyntax = new PythonSyntaxHighlighter(textEdit1);

  connect(ListBox1, SIGNAL(highlighted ( const QString & )), this, SLOT( onDisplayColor( const QString & ) ));
  connect(ColorBtn, SIGNAL(changed ()), this, SLOT( onChosenColor()));
}

/** Destroys the object and frees any allocated resources */
DlgSettingsEditorImp::~DlgSettingsEditorImp()
{
  // no need to delete child widgets, Qt does it all for us
  delete pythonSyntax;
}

/** Searches for the corresponding color value to \e name in @ref DefColorMap and
 *  assigns it to the color button
 *  @see Gui::ColorButton
 *  @see DefColorMap
 */
void DlgSettingsEditorImp::onDisplayColor(const QString& name)
{
  // foreground color
  QString org = _trMap[ name ];
  unsigned long col = _mColors[ org ];
#ifndef OLD_COLOR_STYLE
  ColorBtn->setColor(QColor((col >> 24) & 0xff, (col >> 16) & 0xff, (col >> 8) & 0xff));
#else
  ColorBtn->setColor(QColor(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff));
#endif
}

/** Updates the color map if a color was changed */
void DlgSettingsEditorImp::onChosenColor()
{
  QString text = ListBox1->currentText();

  text = _trMap[ text ];
  if (text.isEmpty())
    return;

  QColor col = ColorBtn->color();
#ifndef OLD_COLOR_STYLE
  unsigned long lcol = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
#else
  unsigned long lcol = (col.blue() << 16) | (col.green() << 8) | col.red();
#endif

  
  _mColors[text] = lcol;
  pythonSyntax->setColor( text, col );
}

void DlgSettingsEditorImp::saveSettings()
{
  EnableLineNumber->onSave();
  EnableFolding->onSave();
  tabSize->onSave();
  indentSize->onSave();
  radioTabs->onSave();
  radioSpaces->onSave();

  // Saves the color map
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Editor");
  for (QMap<QString, unsigned long>::Iterator it = _mColors.begin(); it!=_mColors.end(); ++it)
  {
    hGrp->SetUnsigned(it.key().latin1(), it.data());
  }

  hGrp->SetASCII( "FontSize", FontSize->currentText().latin1() );
  hGrp->SetASCII( "Font", FontDB->currentText().latin1() );
}

void DlgSettingsEditorImp::loadSettings()
{
  EnableLineNumber->onRestore();
  EnableFolding->onRestore();
  tabSize->onRestore();
  indentSize->onRestore();
  radioTabs->onRestore();
  radioSpaces->onRestore();

  // Restores the color map
  ParameterGrp::handle hGrp = WindowParameter::getDefaultParameter()->GetGroup("Editor");
  QStringList names = GetDefCol().types();

  for ( QStringList::Iterator it = names.begin(); it!=names.end(); ++it)
  {
    _mColors[*it] = hGrp->GetUnsigned( (*it).latin1(), GetDefCol().color(*it));
    unsigned long col = GetDefCol().color( *it );
    QColor color;
#ifndef OLD_COLOR_STYLE
    color.setRgb((col >> 24) & 0xff, (col >> 16) & 0xff, (col >> 8) & 0xff);
#else
    color.setRgb(col & 0xff, (col >> 8) & 0xff, (col >> 16) & 0xff);
#endif
    pythonSyntax->setColor( *it, color );
  }

  trToOrig();

  // fill up font styles
  //
  QFontDatabase fdb;
  QStringList familyNames = fdb.families( FALSE );
  FontDB->insertStringList( familyNames );

  FontSize->setCurrentText( hGrp->GetASCII( "FontSize", FontSize->currentText().latin1() ).c_str() );
  FontDB  ->setCurrentText( hGrp->GetASCII( "Font", "Courier" ).c_str() );

  ListBox1->setCurrentItem(0);
}

/**
 * Builds up the internal map that holds the original text to its translation.
 * This is used to keep the syntax highlighting stuff working..
 */ 
void DlgSettingsEditorImp::trToOrig()
{
  // fill up map (tr("text") <-> "text")
  _trMap.clear();
  for ( QMap<QString, unsigned long>::Iterator it = _mColors.begin(); it != _mColors.end(); it++ )
  {
    _trMap[ DlgEditorSettingsBase::tr( it.key() ) ] = it.key();
  }
}

/**
 * Sets the strings of the subwidgets using the current language.
 */
void DlgSettingsEditorImp::languageChange()
{
  int pos = FontSize->currentItem();
  DlgEditorSettingsBase::languageChange();
  FontSize->setCurrentItem( pos );

  trToOrig();
}

// -------------------------------------------------------------------

DefColorMap *DefColorMap::_pcSingleton = 0;

/** Construction */
DefColorMap::DefColorMap(void)
{
  QColor col;

#ifndef OLD_COLOR_STYLE
  col = Qt::black; 
  unsigned long lText             = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Text"]           = lText;
  col = Qt::gray; 
  unsigned long lTextSel          = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Text Selection"] = lTextSel;
  col = Qt::cyan; 
  unsigned long lBookmarks        = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Bookmark"]       = lBookmarks;
  col = Qt::red; 
  unsigned long lBreakpnts        = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Breakpoint"]     = lBreakpnts;
  col = Qt::blue; 
  unsigned long lKeywords         = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Keyword"]        = lKeywords;
  col.setRgb(0, 170, 0); 
  unsigned long lComments         = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Comment"]        = lComments;
  col.setRgb(160, 160, 164); 
  unsigned long lBlockCom         = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Block comment"]  = lBlockCom;
  col = Qt::red; 
  unsigned long lCharacter        = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Character"]      = lCharacter;
  col.setRgb(255, 170, 0); 
  unsigned long lClass            = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Class name"]     = lClass;
  col.setRgb(255, 170, 0); 
  unsigned long lDefine           = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Define name"]    = lDefine;
  col.setRgb(160, 160, 164); 
  unsigned long lOperat           = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Operator"]       = lOperat;
  col = Qt::blue; 
  unsigned long lNumbers          = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Number"]         = lNumbers;
  col = Qt::red; 
  unsigned long lStrings          = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["String"]         = lStrings;
  col.setRgb(170, 170, 127); 
  unsigned long lPyOutput         = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Python output"]  = lPyOutput;
  col = Qt::red; 
  unsigned long lPyError          = (col.red() << 24) | (col.green() << 16) | (col.blue() << 8);
  m_clDefColors["Python error"]   = lPyError;
#else
  col = Qt::black; long lText     = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Text"]           = lText;

  col = Qt::gray; long lTextSel   = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Text Selection"] = lTextSel;

  col = Qt::cyan; long lBookmarks = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Bookmark"]       = lBookmarks;

  col = Qt::red; long lBreakpnts  = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Breakpoint"]     = lBreakpnts;

  col = Qt::blue; long lKeywords  = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Keyword"]        = lKeywords;

  col.setRgb(0, 170, 0); long lComments = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Comment"]        = lComments;

  col.setRgb(160, 160, 164); long lBlockCom  = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Block comment"]   = lBlockCom;

  col = Qt::red; long lCharacter  = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Character"]   = lCharacter;

  col.setRgb(255, 170, 0); long lClass  = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Class name"]  = lClass;

  col.setRgb(255, 170, 0); long lDefine = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Define name"]  = lDefine;

  col.setRgb(160, 160, 164); long lOperat = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Operator"]    = lOperat;

  col = Qt::blue; long lNumbers   = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Number"]         = lNumbers;

  col = Qt::red; long lStrings    = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["String"]         = lStrings;

  col.setRgb(170, 170, 127); long lPyOutput = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Python output"] = lPyOutput;

  col = Qt::red; long lPyError = (col.blue() << 16) | (col.green() << 8) | col.red();
  m_clDefColors["Python error"] = lPyError;
#endif
}

/** Destruction */
DefColorMap::~DefColorMap(void)
{
}

/** Destroys the DefColorMap singleton */
void DefColorMap::Destruct(void)
{
  // not initialized or double destruct!
  assert(_pcSingleton);
  delete _pcSingleton;
}

/** Creates the DefColorMap singleton */
DefColorMap &DefColorMap::Instance(void)
{
  // not initialized?
  if(!_pcSingleton)
  {
    _pcSingleton = new DefColorMap;
  }

  return *_pcSingleton;
}

/** Returns the corresponding color value to the given type name */ 
unsigned long DefColorMap::color( const QString& name )
{
  if ( m_clDefColors.find( name ) != m_clDefColors.end() )
    return m_clDefColors[ name ];
  else
    return 0;
}

/** Returns the names of all types */
QStringList DefColorMap::types() const
{
  QStringList keys;

  for ( QMap<QString, unsigned long>::const_iterator it = m_clDefColors.begin(); it!=m_clDefColors.end(); it++ )
    keys.push_back( it.key() );
  return keys;
}

#include "DlgEditor.cpp"
#include "moc_DlgEditor.cpp"
#include "moc_DlgEditorImp.cpp"
