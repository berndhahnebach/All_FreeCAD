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
#include "PreCompiled.h"

#ifndef _PreComp_
#endif
#include <qwidgetstack.h>
#include <qtabwidget.h>
#if QT_VERSION >= 230
# include <qlistview.h>
#endif
#include "WidgetFactory.h"
#include "DlgPreferencesImp.h"
#include "PropertyPage.h"
#include "Application.h"
#include "Tools.h"
#include "Widgets.h"
#include "BitmapFactory.h"

// --------------------------------------------------

class PrefGroupItem : public QListBoxItem 
{
  public:
    PrefGroupItem( QListBox *parent, const QPixmap &p1, const QPixmap &p2, const QString &name);

    virtual int height( const QListBox * ) const;
    virtual int width( const QListBox * )  const;

  protected:
    virtual void paint( QPainter * );

  private:
    QPixmap pm_Unsel;
    QPixmap pm_Sel;
};

PrefGroupItem::PrefGroupItem( QListBox * parent, const QPixmap &p1, const QPixmap &p2, const QString &name )
    : QListBoxItem( parent ), pm_Unsel( p1 ), pm_Sel( p2 )
{
  setText( name );
}

int PrefGroupItem::height( const QListBox * ) const
{
  return 50;
}

int PrefGroupItem::width( const QListBox * )  const
{
  return 75;
}

void PrefGroupItem::paint( QPainter *p )
{
  int w = width( listBox() );
  int tx = (w-p->fontMetrics().boundingRect(text()).width())/2+10;
  QFont f = p->font();
  f.setBold(true);
  p->setFont(f);
  p->drawText( tx, 40, text() );
  if ( selected() )
	  p->drawPixmap( (w-pm_Sel.width())/2+10, 10, pm_Sel );
  else
    p->drawPixmap( (w-pm_Unsel.width())/2+10, 10, pm_Unsel );
}

/* 
 *  Constructs a FCDlgPreferencesImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
FCDlgPreferencesImp::FCDlgPreferencesImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ),FCWindowParameter(name), m_pCurTab(NULL)
{
  if ( !name )
  	setName( "DlgPreferences" );
  resize( 574, 457 ); 
  setProperty( "caption", tr( "Preferences" ) );
  DlgPreferencesLayout = new QGridLayout( this ); 
  DlgPreferencesLayout->setSpacing( 6 );
  DlgPreferencesLayout->setMargin( 11 );

  Layout6 = new QGridLayout; 
  Layout6->setSpacing( 6 );
  Layout6->setMargin( 0 );

  PushButton14 = new QPushButton( this, "PushButton14" );
  PushButton14->setProperty( "text", tr( "Apply" ) );

  Layout6->addWidget( PushButton14, 0, 3 );
  QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout6->addItem( spacer, 0, 5 );

  PushButton15 = new QPushButton( this, "PushButton15" );
  PushButton15->setProperty( "text", tr( "Cancel" ) );

  Layout6->addWidget( PushButton15, 0, 4 );

  PushButton13 = new QPushButton( this, "PushButton13" );
  PushButton13->setProperty( "text", tr( "OK" ) );

  Layout6->addWidget( PushButton13, 0, 2 );
  QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout6->addItem( spacer_2, 0, 1 );
  QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
  Layout6->addItem( spacer_3, 0, 0 );

  DlgPreferencesLayout->addMultiCellLayout( Layout6, 1, 1, 0, 1 );

  ListBox = new QListBox( this, "ListBox" );
  ListBox->setProperty( "sizePolicy", QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)5, ListBox->sizePolicy().hasHeightForWidth() ) );
  ListBox->setFixedWidth(120);

  DlgPreferencesLayout->addWidget( ListBox, 0, 0 );

  tabWidgetStack = new QWidgetStack( this, "tabWidgetStack" );

  DlgPreferencesLayout->addWidget( tabWidgetStack, 0, 1 );

  // signals and slots connections
  connect( PushButton13, SIGNAL( clicked() ), this, SLOT( accept  () ) );
  connect( PushButton15, SIGNAL( clicked() ), this, SLOT( accept  () ) );
  connect( PushButton13, SIGNAL( clicked() ), this, SLOT( onOK    () ) );
  connect( PushButton14, SIGNAL( clicked() ), this, SLOT( onApply () ) );
  connect( PushButton15, SIGNAL( clicked() ), this, SLOT( onCancel() ) );

  // tab order
  setTabOrder( ListBox, PushButton13 );
  setTabOrder( PushButton13, PushButton14 );
  setTabOrder( PushButton14, PushButton15 );

  connect(ListBox, SIGNAL ( highlighted ( int ) ), this, SLOT( prefPageClicked( int ) ));

	// make sure that pages are ready to create
	GetWidgetFactorySupplier();
	for (std::vector<QString>::iterator it = aclPages.begin(); it!=aclPages.end(); ++it)
	{
		if ((*it).startsWith("Group_"))
		{
			QString s = (*it).mid(6);
		  addPreferenceGroup(s, "PrefTree_GroupOpen", "PrefTree_GroupClosed");
		}
		else
		{
			addPreferencePage(GetWidgetFactory().ProduceWidget(*it), tr(*it));
		}
	}

  // show the first page
  ListBox->setCurrentItem(0);
}

/*  
 *  Destroys the object and frees any allocated resources
 */
FCDlgPreferencesImp::~FCDlgPreferencesImp()
{
    // no need to delete child widgets, Qt does it all for us
}

std::vector<QString> FCDlgPreferencesImp::aclPages;
void FCDlgPreferencesImp::addPage(const QString& name)
{
	aclPages.push_back(name);
}

void FCDlgPreferencesImp::addGroup(const QString& name)
{
	QString s = "Group_";
	s += name;
	aclPages.push_back(s);
}

void FCDlgPreferencesImp::addPreferenceGroup(const QString& name, const char* Pixmap, const char* Pixmap2)
{
  m_pCurTab = getOrAddPreferenceGroup(name, Pixmap, Pixmap2);
}

void FCDlgPreferencesImp::addPreferencePage(QWidget* page, const QString& name)
{
	if (m_pCurTab && page)
	{
		m_pCurTab->addTab(page, name);

		page->hide();

		connectWidget(page);
	}
}

QTabWidget* FCDlgPreferencesImp::getPreferenceGroup(int id)
{
  return (QTabWidget*)tabWidgetStack->widget(id);
}

QTabWidget* FCDlgPreferencesImp::getOrAddPreferenceGroup(const QString& name, const char* Pixmap, const char* Pixmap2)
{
  // already inside
  if (m_mGroupIDs.find(name) != m_mGroupIDs.end())
  {
    return (QTabWidget*)tabWidgetStack->widget(m_mGroupIDs[name]);
  }

  QPixmap pixSel   = GetBitmapFactory().GetPixmap(Pixmap);
  QPixmap pixUnsel = GetBitmapFactory().GetPixmap(Pixmap2);

  int iSize = m_mGroupIDs.size();
  m_mGroupIDs[name] = iSize;
  QTabWidget* tabWidget = new QTabWidget;
  tabWidgetStack->addWidget(tabWidget, iSize);

  (void) new PrefGroupItem(ListBox, pixSel, pixUnsel, name);

  return tabWidget;
}

void FCDlgPreferencesImp::prefPageClicked(int item)
{
  m_pCurTab = getPreferenceGroup(item);

  if (!m_pCurTab)
    return;

  tabWidgetStack->raiseWidget(m_pCurTab);
}

void FCDlgPreferencesImp::connectWidget(QWidget* page) const
{
  if (dynamic_cast<FCPreferencePage*>(page) != NULL)
  {
    // and its preference widgets
    std::vector<FCWidgetPrefsHandler*> aHandlers = dynamic_cast<FCPreferencePage*>(page)->getHandlers();
    for (std::vector<FCWidgetPrefsHandler*>::iterator it = aHandlers.begin(); it != aHandlers.end(); ++it)
    {
      connect(PushButton13, SIGNAL(clicked()), *it, SLOT(onSave()));//OK
      connect(PushButton14, SIGNAL(clicked()), *it, SLOT(onSave()));//Apply
    }
  }
}

void FCDlgPreferencesImp::onOK()
{
  onApply();
}

void FCDlgPreferencesImp::onApply()
{
  QWidget* page = m_pCurTab->currentPage();
  if (dynamic_cast<FCPreferencePage*>(page) != NULL)
     (dynamic_cast<FCPreferencePage*>(page))->onApply();

# ifdef FC_DEBUG
  else
    GetConsole().Warning("Added page does not inherit from class FCPreferencePage");
#endif
}

void FCDlgPreferencesImp::onCancel()
{
  // not yet implemented
}

// compiling the mocs and the Dlg
#include "DlgPreferences.cpp"
#include "moc_DlgPreferences.cpp"
#include "moc_DlgPreferencesImp.cpp"
