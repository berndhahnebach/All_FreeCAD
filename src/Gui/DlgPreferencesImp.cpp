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

#include <Base/Exception.h>

#include "DlgPreferencesImp.h"
#include "PropertyPage.h"
#include "WidgetFactory.h"
#include "BitmapFactory.h"
#include "MainWindow.h"

#ifdef FC_DEBUG
# include <Base/Console.h> 
#endif

using namespace Gui::Dialog;

// --------------------------------------------------

namespace Gui {
namespace Dialog {

class PrefGroupItem : public Q3ListBoxItem 
{
public:
  PrefGroupItem( Q3ListBox *parent, const QPixmap &p1, const QPixmap &p2, const QString &name, int index);

  virtual int height( const Q3ListBox * ) const;
  virtual int width( const Q3ListBox * )  const;
  virtual int rtti() const { return 10001; }
  int index() const { return stack_index; }

protected:
  virtual void paint( QPainter * );

private:
  QPixmap pm_Sel;
  QPixmap pm_Unsel;
  int stack_index;
};

} // namespace Dialog
} // namespace Gui

PrefGroupItem::PrefGroupItem( Q3ListBox * parent, const QPixmap &p1, const QPixmap &p2, const QString &name, int index )
    : Q3ListBoxItem( parent ), pm_Sel( p1 ), pm_Unsel( p2 ), stack_index(index)
{
  setText( name );
  QPalette pal = parent->palette();
  pal.setInactive( pal.active() );
  parent->setPalette( pal );
}

int PrefGroupItem::height( const Q3ListBox * ) const
{
  return 50;
}

int PrefGroupItem::width( const Q3ListBox * )  const
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

/* TRANSLATOR Gui::Dialog::DlgPreferencesImp */

/**
 *  Constructs a DlgPreferencesImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgPreferencesImp::DlgPreferencesImp( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl ), _pCurTab(0L)
{
  this->setupUi(this);
  connect( buttonHelp,  SIGNAL ( clicked() ), getMainWindow(), SLOT ( whatsThis() ));

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

      addPreferencePage( WidgetFactory().createPreferencePage( className ) );
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
    tab->setCurrentIndex( pos );
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
void DlgPreferencesImp::addPreferencePage( PreferencePage* page )
{
  if ( _pCurTab && page )
  {
    _pCurTab->addTab( page, page->windowTitle() );
    //page->hide();
    page->loadSettings();
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

  QTabWidget* tabWidget = new QTabWidget;
  int index = tabWidgetStack->addWidget(tabWidget);
  _mGroupIDs[ name ] = index;

  PrefGroupItem* item = new PrefGroupItem(listBox, pixSel, pixUnsel, name, index);
  return tabWidget;
}

void DlgPreferencesImp::on_listBox_highlighted(int index)
{
  PrefGroupItem* item = (PrefGroupItem*)listBox->item(index);
  if (item->rtti() != 10001)
    return;
  _pCurTab = getPreferenceGroup(item->index());
  if (!_pCurTab)
    return;
  tabWidgetStack->setCurrentWidget( _pCurTab );
}

void DlgPreferencesImp::accept()
{
  _invalidParameter = false;
  on_buttonApply_clicked();
  if ( _invalidParameter == false )
    QDialog::accept();
}

void DlgPreferencesImp::on_buttonApply_clicked()
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
  		  int index = page->metaObject()->indexOfMethod( "checkSettings()" );
        try{
	  	  if ( index >= 0 )
          page->qt_metacall( QMetaObject::InvokeMetaMethod, index, 0 );
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
      PreferencePage* page = (PreferencePage*)(tab->widget( i )->qt_metacast("Gui::Dialog::PreferencePage"));
	  	if ( page )
        page->saveSettings();
    }
  }
}

void DlgPreferencesImp::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::LanguageChange) {
    retranslateUi(this);
    // update the widget's tabs
    for ( QMap<QString, int>::Iterator it = _mGroupIDs.begin(); it != _mGroupIDs.end(); ++it ) {
      QTabWidget* tab = getPreferenceGroup( it.data() );
      int ct = tab->count();
      for ( int i=0; i<ct; i++ ) {
        QWidget* page = tab->widget( i );
        tab->setTabText( i, page->windowTitle() );
      }
    }
  } else {
    QWidget::changeEvent(e);
  }
}

#include "moc_DlgPreferencesImp.cpp"
