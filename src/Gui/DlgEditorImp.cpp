/***************************************************************************
                          DlgEditorImp.cpp  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Do the editor settings
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
 */


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Werner Mayer 2002                                                     *
 *                                                                         *
 ***************************************************************************/




#include "PreCompiled.h"
#ifndef _PreComp_
#	include <qbutton.h>
#	include <qfontdatabase.h>
#	include <qlabel.h>
#	include <qstringlist.h>
#endif

#include "DlgEditorImp.h"
#include "Widgets.h"

/*
 *  Constructs a FCDlgEditorSettings which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FCDlgEditorSettings::FCDlgEditorSettings( QWidget* parent,  const char* name, WFlags fl )
    : FCDlgEditorSettingsBase( parent, name, fl )
{
  setParamGrpPath("User parameter:BaseApp/Windows/Editor");
  setEntryName("Editor");

  append(EnableLineNumber->getHandler());
  append(EnableFolding->getHandler());
	append(MyCustomWidget1_2->getHandler());
  append(getHandler());

  connect(ListBox1, SIGNAL(highlighted ( const QString & )), this, SLOT( onAssignColor( const QString & ) ));
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

/*  
 *  Destroys the object and frees any allocated resources
 */
FCDlgEditorSettings::~FCDlgEditorSettings()
{
    // no need to delete child widgets, Qt does it all for us
}

void FCDlgEditorSettings::OnChange(FCSubject<const char*> &rCaller, const char * sReason)
{
  // just do nothing
}

void FCDlgEditorSettings::restorePreferences()
{
  std::vector<QString> names = GetDefCol().GetKeys();

  for (std::vector<QString>::iterator it = names.begin(); it!=names.end(); ++it)
  {
    m_clColors[*it] = hPrefGrp->GetInt(it->latin1(), GetDefCol().GetColor(*it));
  }

  Languages->setCurrentItem(hPrefGrp->GetInt("Lexer", 0));
}

void FCDlgEditorSettings::savePreferences()
{
  for (std::map<QString, long>::iterator it = m_clColors.begin(); it!=m_clColors.end(); ++it)
  {
    hPrefGrp->SetInt(it->first.latin1(), it->second);
  }

  hPrefGrp->SetInt("Lexer", Languages->currentItem());
}

void FCDlgEditorSettings::onAssignColor(const QString& name)
{
  long col = m_clColors[name];

  // foreground color
  int b = col >> 16;  col -= b << 16;
  int g = col >> 8;   col -= g << 8;
  int r = col;

  MyCustomWidget1->setColor(QColor(r,g,b));
}

void FCDlgEditorSettings::onChosenColor()
{
  QString text = ListBox1->currentText();
  if (text.isEmpty())
    return;

  QColor col = MyCustomWidget1->color();
 	long lcol = (col.blue() << 16) | (col.green() << 8) | col.red();

  
  m_clColors[text] = lcol;
}

void FCDlgEditorSettings::onChosenFont(const QString & item)
{
  hPrefGrp->SetASCII("Font", item.latin1());
}

///////////////////////////////////////////////////////////////////////////////////////

FCDefColorMap *FCDefColorMap::_pcSingleton = NULL;

FCDefColorMap::FCDefColorMap(void)
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

FCDefColorMap::~FCDefColorMap(void)
{
}

void FCDefColorMap::Destruct(void)
{
	// not initialized or double destruct!
  assert(_pcSingleton);
	delete _pcSingleton;
}

FCDefColorMap &FCDefColorMap::Instance(void)
{
	// not initialized?
	if(!_pcSingleton)
	{
		_pcSingleton = new FCDefColorMap;
	}

  return *_pcSingleton;
}

long FCDefColorMap::GetColor(const QString& name)
{
	if (m_clDefColors.find(name) != m_clDefColors.end())
		return m_clDefColors[name];
	else
		return 0;
}

std::vector<QString> FCDefColorMap::GetKeys() const
{
  std::vector<QString> keys;

  for (std::map<QString, long>::const_iterator it = m_clDefColors.begin(); it!=m_clDefColors.end(); ++it)
    keys.push_back(it->first);
  return keys;
}

#include "DlgEditor.cpp"
#include "moc_DlgEditor.cpp"
#include "moc_DlgEditorImp.cpp"
