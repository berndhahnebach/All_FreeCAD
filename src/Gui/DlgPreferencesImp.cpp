/** \file DlgPreferncesImp.cpp
 *  \brief  
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *   
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


/** Precompiled header stuff
 *  on some compilers the precompiled header option gain significant compile 
 *  time! So every external header (libs and system) should included in 
 *  Precompiled.h. For systems without precompilation the header needed are
 *  included in the else fork.
 */
#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#endif
#include <qwidgetstack.h>
#include <qtabwidget.h>
#if QT_VERSION >= 230
# include <qlistview.h>
#endif
#include "DlgPreferencesImp.h"
#include "DlgSettingsImp.h"
#include "Application.h"

/* 
 *  Constructs a DlgPreferencesImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgPreferencesImp::DlgPreferencesImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgPreferences( parent, name, modal, fl ),FCWindowParameter(name), m_pCurPage(NULL), m_pCurTab(NULL)
{
  connect(ListView2, SIGNAL ( pressed ( QListViewItem * ) ), this, SLOT( prefPageClicked(QListViewItem * ) ));

  FCBmpFactory& rclBF = ApplicationWindow::Instance->GetBmpFactory();
  QPixmap pix = rclBF.GetPixmap("function");
  addPreferenceGroup("FreeCAD", pix);
  addPreferencePage(new QWidget, "General");
  addPreferencePage(new FCDlgSettings, "Help Viewer", pix);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
DlgPreferencesImp::~DlgPreferencesImp()
{
    // no need to delete child widgets, Qt does it all for us
}

void DlgPreferencesImp::addPreferenceGroup(const char* name)
{
  m_pCurTab = getOrAddPreferenceGroup(name);
}

void DlgPreferencesImp::addPreferenceGroup(const char* name, const QPixmap& p)
{
  QPixmap pix = p;
  m_pCurTab = getOrAddPreferenceGroup(name);
  pix.resize(16, 16);
  m_mGroupsItem[name]->setPixmap(0, pix);
}

void DlgPreferencesImp::addPreferencePage(QWidget* page, const char* name)
{
  if (m_pCurTab)
  {
    m_pCurTab->addTab(page, name);

    QListViewItem * item = new QListViewItem( m_mGroupsItem[getGroupName()], 0 );
    item->setText( 0, tr( name ) );

    // concatenate the two strings (to an unique one)
    std::string cname = getGroupName() + std::string(name);
    m_mPages[cname] = std::make_pair<QListViewItem*, QWidget*>(item, page);
    page->hide();

    connectWidget(page);
  }
}

void DlgPreferencesImp::addPreferencePage(QWidget* page, const char* name, const QPixmap& p)
{
  if (m_pCurTab)
  {
    m_pCurTab->addTab(page, name);

    QPixmap pix = p;
    QListViewItem * item = new QListViewItem( m_mGroupsItem[getGroupName()], 0 );
    item->setText( 0, tr( name ) );
    pix.resize(16, 16);
    item->setPixmap( 0, pix );

    // concatenate the two strings (to an unique one)
    std::string cname = getGroupName() + std::string(name);
    m_mPages[cname] = std::make_pair<QListViewItem*, QWidget*>(item, page);
    page->hide();

    connectWidget(page);
  }
}

void DlgPreferencesImp::addPreferencePage(QWidget* page, const char* name, const char* group)
{
  m_pCurTab = getOrAddPreferenceGroup(group);
  m_pCurTab->addTab(page, name);

  QListViewItem * item = new QListViewItem( m_mGroupsItem[group], 0 );
  item->setText( 0, tr( name ) );

  // concatenate the two strings (to an unique one)
  std::string cname = std::string(group) + std::string(name);
  m_mPages[cname] = std::make_pair<QListViewItem*, QWidget*>(item, page);
  page->hide();

  connectWidget(page);
}

QTabWidget* DlgPreferencesImp::getPreferenceGroup(const char* name)
{
  // already inside
  if (m_mGroupsID.find(name) != m_mGroupsID.end())
  {
    return (QTabWidget*)tabWidgetStack->widget(m_mGroupsID[name]);
  }

  return NULL;
}

QTabWidget* DlgPreferencesImp::getOrAddPreferenceGroup(const char* name)
{
  // already inside
  if (m_mGroupsID.find(name) != m_mGroupsID.end())
  {
    return (QTabWidget*)tabWidgetStack->widget(m_mGroupsID[name]);
  }

  int iSize = m_mGroupsID.size();
  m_mGroupsID[name] = iSize;
  m_mGroupsItem[name] = new QListViewItem(ListView2, 0);
  m_mGroupsItem[name]->setText(0, tr(name));
  QTabWidget* tabWidget = new QTabWidget;
  tabWidgetStack->addWidget(tabWidget, iSize);
  connect(tabWidget, SIGNAL (  currentChanged ( QWidget * ) ), this, SLOT( prefPageClicked(QWidget * ) ));

  return tabWidget;
}

void DlgPreferencesImp::prefPageClicked(QListViewItem * item )
{
  if (m_pCurPage)
    m_pCurPage->hide();

  if (!item) return;

  // if subitem get the parent
  QListViewItem* parent;
  QListViewItem* child;
  std::string str;
  if ((parent = item->parent()) != NULL)
  {
    m_pCurTab = getPreferenceGroup(parent->text(0).latin1());
    str = (parent->text(0) + item->text(0)).latin1();
  }
  else if((child = item->firstChild()) != NULL)
  {
    prefPageClicked(child);
    return;
  }
  else return;

  if (!m_pCurTab)
    return;

  if (m_mPages.find(str) != m_mPages.end())
  {
    tabWidgetStack->raiseWidget(m_pCurTab);
    m_pCurPage = m_mPages[str].second;
    m_pCurTab->showPage(m_pCurPage);
  }
}

void DlgPreferencesImp::prefPageClicked(QWidget * widget )
{
  for (std::map <std::string, std::pair<QListViewItem*, QWidget*> >::iterator it = m_mPages.begin(); 
       it != m_mPages.end(); ++it)
       {
         if (it->second.second == widget)
         {
           ListView2->setSelected(it->second.first, true);
           return;
         }
       }
}

std::string DlgPreferencesImp::getGroupName() const
{
  int id = tabWidgetStack->id(m_pCurTab);
  for (std::map<std::string, int>::const_iterator it = m_mGroupsID.begin(); it != m_mGroupsID.end(); ++it)
  {
    if (it->second == id)
      return it->first;
  }

  return "";
}

std::string DlgPreferencesImp::getPageName() const
{
  return m_pCurTab->tabLabel(m_pCurTab->currentPage());
}

void DlgPreferencesImp::connectWidget(QWidget* page) const
{
  // the dialog itself
//  connect(PushButton13, SIGNAL(clicked()), page, SLOT(save()));//OK
//  connect(PushButton14, SIGNAL(clicked()), page, SLOT(save()));//Apply

  if (dynamic_cast<FCWidgetPrefsManager*>(page) != NULL)
  {
    // and its preference widgets
    std::vector<FCWidgetPrefsHandler*> aHandlers = dynamic_cast<FCWidgetPrefsManager*>(page)->getHandlers();
    for (std::vector<FCWidgetPrefsHandler*>::iterator it = aHandlers.begin(); it != aHandlers.end(); ++it)
    {
      connect(PushButton13, SIGNAL(clicked()), *it, SLOT(save()));//OK
      connect(PushButton14, SIGNAL(clicked()), *it, SLOT(save()));//Apply
    }
  }
}

// compiling the mocs and the Dlg
#include "DlgPreferences.cpp"
#include "moc_DlgPreferences.cpp"
#include "moc_DlgPreferencesImp.cpp"
