/***************************************************************************
 *   Copyright (c) 2002 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <qlayout.h>
# include <qlistbox.h>
# include <qwidgetstack.h>
# include <qtabwidget.h>
# include <qpainter.h>
# include <qmessagebox.h>
# include <qmetaobject.h>
#endif

#include "DlgPreferencesImp.h"
#include "WidgetFactory.h"
#include "BitmapFactory.h"

#include <Base/Exception.h>

#ifdef FC_DEBUG
# include "../Base/Console.h" 
#endif
#define new DEBUG_CLIENTBLOCK
using namespace Gui::Dialog;

// --------------------------------------------------

namespace Gui {
namespace Dialog {

class PrefGroupItem : public QListBoxItem 
{
public:
  PrefGroupItem( QListBox *parent, const QPixmap &p1, const QPixmap &p2, const QString &name);

  virtual int height( const QListBox * ) const;
  virtual int width( const QListBox * )  const;

protected:
  virtual void paint( QPainter * );

private:
  QPixmap pm_Sel;
  QPixmap pm_Unsel;
};

} // namespace Dialog
} // namespace Gui

PrefGroupItem::PrefGroupItem( QListBox * parent, const QPixmap &p1, const QPixmap &p2, const QString &name )
    : QListBoxItem( parent ), pm_Sel( p1 ), pm_Unsel( p2 )
{
  setText( name );
  QPalette pal = parent->palette();
  pal.setInactive( pal.active() );
  parent->setPalette( pal );
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
  if ( isSelected() )
    p->drawPixmap( (w-pm_Sel.width())/2+10, 10, pm_Sel );
  else
    p->drawPixmap( (w-pm_Unsel.width())/2+10, 10, pm_Unsel );
}

/**
 *  Constructs a DlgPreferencesImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgPreferencesImp::DlgPreferencesImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgPreferences( parent, name, modal, fl ), _pCurTab(0L)
{
  tabWidgetStack->removeWidget( WStackPage ); // remove the damned dummy widget

  // make sure that pages are ready to create
  GetWidgetFactorySupplier();
  for ( QStringList::Iterator it = _pages.begin(); it!=_pages.end(); ++it )
  {
    int pos;
    QString s(*it);

    if ( (pos=s.find("|")) > -1 )
    {
      QString group = s.left( pos );
      QString className = s.mid( pos+1 );

      // add group if it does not exist
      if ( _mGroupIDs.find( group ) == _mGroupIDs.end() )
      {
        addPreferenceGroup( group, "PrefTree_GroupOpen", "PrefTree_GroupClosed" );
      }

      addPreferencePage( WidgetFactory().createWidget( className ) );
    }
  }

  // show the first page
  listBox->setCurrentItem(0);
}

/**
 *  Destroys the object and frees any allocated resources.
 */
DlgPreferencesImp::~DlgPreferencesImp()
{
  // no need to delete child widgets, Qt does it all for us
}

QStringList DlgPreferencesImp::_pages;


/**
 * Adds a preference page with its class name \a className and
 * the group \a group it belongs to. To create this page it must
 * be registered in the WidgetFactory.
 * @see WidgetFactory
 * @see PrefPageProducer
 */
void DlgPreferencesImp::addPage( const QString& className, const QString& group )
{
  QString s = QString("%1|%2").arg( group ).arg( className );
  _pages.push_back( s );
}

/**
 * Activates the page at position \a pos of the group with name \a groupName.
 */
void DlgPreferencesImp::activatePageOfGroup( int pos, const char* groupName )
{
  QMap<QString, int>::ConstIterator it = _mGroupIDs.find( groupName );
  if ( it != _mGroupIDs.end() )
  {
    QTabWidget* tab = getPreferenceGroup( it.data() );
    listBox->setCurrentItem( it.data() );
    tab->setCurrentPage( pos );
  }
}

/** Adds a new preference group */
void DlgPreferencesImp::addPreferenceGroup(const QString& name, const char* Pixmap, const char* Pixmap2)
{
  _pCurTab = getOrAddPreferenceGroup(name, Pixmap, Pixmap2);
}

/**
 * Adds a new preference page to the current group
 * Before the first call of this method @ref addPreferenceGroup must be called
 * otherwise the preference pages will be lost.
 */
void DlgPreferencesImp::addPreferencePage( QWidget* page )
{
  if ( _pCurTab && page )
  {
    _pCurTab->addTab( page, page->caption() );

    page->hide();

  	int index = page->metaObject()->findSlot( "loadSettings()", TRUE );
	  if ( index >= 0 )
		  page->qt_invoke( index, 0 );
  }
}

QTabWidget* DlgPreferencesImp::getPreferenceGroup(int id)
{
  return (QTabWidget*)tabWidgetStack->widget(id);
}

QTabWidget* DlgPreferencesImp::getOrAddPreferenceGroup(const QString& name, const char* Pixmap, const char* Pixmap2)
{
  // already inside
  if ( _mGroupIDs.find( name ) != _mGroupIDs.end() )
  {
    return (QTabWidget*)tabWidgetStack->widget( _mGroupIDs[ name ] );
  }

  QPixmap pixSel   = Gui::BitmapFactory().pixmap(Pixmap);
  QPixmap pixUnsel = Gui::BitmapFactory().pixmap(Pixmap2);

  int iSize = _mGroupIDs.size();
  _mGroupIDs[ name ] = iSize;
  QTabWidget* tabWidget = new QTabWidget;
  tabWidgetStack->addWidget(tabWidget, iSize);

  (void) new PrefGroupItem(listBox, pixSel, pixUnsel, name);

  return tabWidget;
}

void DlgPreferencesImp::onPrefPageClicked(int item)
{
  _pCurTab = getPreferenceGroup(item);

  if (!_pCurTab)
    return;

  tabWidgetStack->raiseWidget( _pCurTab );
}

void DlgPreferencesImp::accept()
{
  _invalidParameter = false;
  apply();
  if ( _invalidParameter == false )
    DlgPreferences::accept();
}

void DlgPreferencesImp::apply()
{
  try
  {
    // check for valid input first
    for ( QMap<QString, int>::Iterator it = _mGroupIDs.begin(); it != _mGroupIDs.end(); ++it )
    {
      QTabWidget* tab = getPreferenceGroup( it.data() );
      int ct = tab->count();

      for ( int i=0; i<ct; i++ )
      {
        QWidget* page = tab->page( i );
  		  int index = page->metaObject()->findSlot( "checkSettings()", TRUE );
        try{
	  	  if ( index >= 0 )
		      page->qt_invoke( index, 0 );
        } catch ( const Base::Exception& e ) {
          listBox->setCurrentItem( it.data() );
          tab->setCurrentPage( i );
          QMessageBox::warning( this, tr("Wrong parameter"), e.what() );
          throw e;
        }
      }
    }
  } catch ( const Base::Exception& ) {
    _invalidParameter = true;
    return;
  }

  for ( QMap<QString, int>::Iterator it = _mGroupIDs.begin(); it != _mGroupIDs.end(); ++it )
  {
    QTabWidget* tab = getPreferenceGroup( it.data() );
    int ct = tab->count();

    for ( int i=0; i<ct; i++ )
    {
      QWidget* page = tab->page( i );
  		int index = page->metaObject()->findSlot( "saveSettings()", TRUE );
	  	if ( index >= 0 )
		    page->qt_invoke( index, 0 );
    }
  }
}

void DlgPreferencesImp::languageChange()
{
  DlgPreferences::languageChange();

  // update the widget's tabs
  for ( QMap<QString, int>::Iterator it = _mGroupIDs.begin(); it != _mGroupIDs.end(); ++it )
  {
    QTabWidget* tab = getPreferenceGroup( it.data() );
    int ct = tab->count();

    for ( int i=0; i<ct; i++ )
    {
      QWidget* page = tab->page( i );
      tab->changeTab( page, page->caption() );
    }
  }
}

#include "DlgPreferences.cpp"
#include "moc_DlgPreferences.cpp"
#include "moc_DlgPreferencesImp.cpp"
