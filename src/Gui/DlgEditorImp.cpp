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
# ifdef FC_OS_WIN32
#   include <assert.h>
#	endif
#endif

#include "DlgEditorImp.h"

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
  setParamGrpPath("User parameter:BaseApp/Windows/Editor");
  setEntryName("Editor");

  append(EnableLineNumber->getHandler());
  append(EnableFolding->getHandler());
  append(MyCustomWidget1_2->getHandler());
  append(getHandler());

  connect(ListBox1, SIGNAL(highlighted ( const QString & )), this, SLOT( onDisplayColor( const QString & ) ));
  connect(MyCustomWidget1, SIGNAL(changed ()), this, SLOT( onChosenColor()));
  connect(ComboBox1, SIGNAL(activated ( const QString & ) ), this, SLOT( onChosenFont( const QString & )));

  if (MyCustomWidget1_2->count() == 0)
  {
    MyCustomWidget1_2->insertItem("6");
    MyCustomWidget1_2->insertItem("7");
    MyCustomWidget1_2->insertItem("8");
    MyCustomWidget1_2->insertItem("9");
    MyCustomWidget1_2->insertItem("10");
    MyCustomWidget1_2->insertItem("11");
    MyCustomWidget1_2->insertItem("12");
  }

  int fontsize = hPrefGrp->GetInt("FontSize", 3);
  MyCustomWidget1_2->setCurrentItem(fontsize);

  // fonts
  QFontDatabase fdb;
  QStringList familyNames = fdb.families( FALSE );
  QStringList newList;
  QString s;
  QStringList::Iterator it = familyNames.begin();
  QString item = hPrefGrp->GetASCII("Font", "Verdana").c_str();
  int pos;

  for(int k=0 ; it != familyNames.end() ; it++ ,k++)
  {
    s = *it;
    if ( s.contains('-') )
    {
      int i = s.find('-');
      s = s.right( s.length() - i - 1 ) + " [" + s.left( i ) + "]";
    }

    s[0] = s[0].upper();
    if (s == item)
      pos = k;
    newList.append( s );
  }

  ComboBox1->insertStringList( newList );
  ComboBox1->setCurrentItem(pos);
}

/** Destroys the object and frees any allocated resources */
DlgSettingsEditorImp::~DlgSettingsEditorImp()
{
    // no need to delete child widgets, Qt does it all for us
}

/** No implementation */
void DlgSettingsEditorImp::OnChange(FCSubject<const char*> &rCaller, const char * sReason)
{
  // just do nothing
}

/** Restores the color map */
void DlgSettingsEditorImp::restorePreferences()
{
  std::vector<QString> names = GetDefCol().keys();

  for (std::vector<QString>::iterator it = names.begin(); it!=names.end(); ++it)
  {
    _mColors[*it] = hPrefGrp->GetInt(it->latin1(), GetDefCol().color(*it));
  }

  Languages->setCurrentItem(hPrefGrp->GetInt("Lexer", 0));
}

/** Saves the color map */
void DlgSettingsEditorImp::savePreferences()
{
  for (std::map<QString, long>::iterator it = _mColors.begin(); it!=_mColors.end(); ++it)
  {
    hPrefGrp->SetInt(it->first.latin1(), it->second);
  }

  hPrefGrp->SetInt("Lexer", Languages->currentItem());
}

/** Searches for the corresponding color value to \e name in @ref DefColorMap and
 *  assigns it to the color button
 *  @see Gui::ColorButton
 *  @see DefColorMap
 */
void DlgSettingsEditorImp::onDisplayColor(const QString& name)
{
  long col = _mColors[name];

  // foreground color
  int b = col >> 16;  col -= b << 16;
  int g = col >> 8;   col -= g << 8;
  int r = col;

  MyCustomWidget1->setColor(QColor(r,g,b));
}

/** Updates the color map if a color was changed */
void DlgSettingsEditorImp::onChosenColor()
{
  QString text = ListBox1->currentText();
  if (text.isEmpty())
    return;

  QColor col = MyCustomWidget1->color();
  long lcol = (col.blue() << 16) | (col.green() << 8) | col.red();

  
  _mColors[text] = lcol;
}

/** Saves the chosen font, but it is disabled in this version */
void DlgSettingsEditorImp::onChosenFont(const QString & item)
{
  hPrefGrp->SetASCII("Font", item.latin1());
}

// -------------------------------------------------------------------

DefColorMap *DefColorMap::_pcSingleton = NULL;

/** Construction */
DefColorMap::DefColorMap(void)
{

  QColor col;

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

/** Returns the corresponding color value to the given setting name */ 
long DefColorMap::color(const QString& name)
{
  if (m_clDefColors.find(name) != m_clDefColors.end())
    return m_clDefColors[name];
  else
    return 0;
}

/** Returns the names of all settings */
std::vector<QString> DefColorMap::keys() const
{
  std::vector<QString> keys;

  for (std::map<QString, long>::const_iterator it = m_clDefColors.begin(); it!=m_clDefColors.end(); ++it)
    keys.push_back(it->first);
  return keys;
}

#include "DlgEditor.cpp"
#include "moc_DlgEditor.cpp"
#include "moc_DlgEditorImp.cpp"
