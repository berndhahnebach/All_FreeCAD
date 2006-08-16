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
# include <qiconview.h>
# include <qcombobox.h>
# include <qfontmetrics.h>
# include <qheader.h>
# include <qinputdialog.h>
# include <qmessagebox.h>
# include <qpopupmenu.h>
# include <qsplitter.h>
#endif

#include "DlgParameterImp.h"
#include "DlgInputDialogImp.h"
#include "BitmapFactory.h"
#include "FileDialog.h"
#include "SpinBox.h"

#include <Base/Parameter.h>
#include <Base/Exception.h>
#include <App/Application.h>



using namespace Gui::Dialog;

/* TRANSLATOR Gui::Dialog::DlgParameterImp */

/**
 *  Constructs a DlgParameterImp which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgParameterImp::DlgParameterImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgParameter( parent, name, modal, fl|WStyle_MinMax )
{
  ParamGrp = new ParameterGroup( splitter3, "ParameterGroup" );
  ParamGrp->addColumn( tr( "Group" ) );
  ParamGrp->header()->setClickEnabled( FALSE, ParamGrp->header()->count() - 1 );
  ParamGrp->setResizeMode( QListView::AllColumns );

  ParamVal = new ParameterValue( splitter3, "ParameterValue" );
  ParamVal->addColumn( tr( "Name" ) );
  ParamVal->header()->setClickEnabled( FALSE, ParamVal->header()->count() - 1 );
  ParamVal->addColumn( tr( "Type" ) );
  ParamVal->header()->setClickEnabled( FALSE, ParamVal->header()->count() - 1 );
  ParamVal->addColumn( tr( "Value" ) );
  ParamVal->header()->setClickEnabled( FALSE, ParamVal->header()->count() - 1 );
  ParamVal->setResizeMode( QListView::AllColumns );

  const std::map<std::string,ParameterManager *> rcList = App::GetApplication().GetParameterSetList();

  for( std::map<std::string,ParameterManager *>::const_iterator It= rcList.begin();It!=rcList.end();It++)
  {
    SetNameComboBox->insertItem(It->first.c_str());
  }

  QString cStr("User parameter");
  SetNameComboBox->setCurrentText( cStr );
  
  onParameterSetChange(cStr);
  connect( SetNameComboBox, SIGNAL( activated( const QString& ) ), 
           this, SLOT( onParameterSetChange( const QString& ) ) );
  connect( ParamGrp, SIGNAL( currentChanged( QListViewItem* ) ), 
           this, SLOT( onGroupSelected(QListViewItem *) ) );
  onGroupSelected(ParamGrp->currentItem());
}

/** 
 *  Destroys the object and frees any allocated resources
 */
DlgParameterImp::~DlgParameterImp()
{
  // no need to delete child widgets, Qt does it all for us
}

/**
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DlgParameterImp::languageChange()
{
  DlgParameter::languageChange();

  ParamGrp->header()->setLabel( 0, tr( "Group" ) );
  ParamVal->header()->setLabel( 0, tr( "Name" ) );
  ParamVal->header()->setLabel( 1, tr( "Type" ) );
  ParamVal->header()->setLabel( 2, tr( "Value" ) );
}

void DlgParameterImp::onGroupSelected( QListViewItem * item )
{
  if ( item && item->rtti() == 2000 )
  {
    ParamVal->clear();
    FCHandle<ParameterGrp> _hcGrp = reinterpret_cast<ParameterGroupItem*>(item)->_hcGrp;
    reinterpret_cast<ParameterValue*>(ParamVal)->setCurrentGroup( _hcGrp );

    // filling up Text nodes
    std::map<std::string,std::string> mcTextMap = _hcGrp->GetASCIIMap();
    for(std::map<std::string,std::string>::iterator It2=mcTextMap.begin();It2!=mcTextMap.end();It2++)
    {
      (void)new ParameterText(ParamVal,It2->first.c_str(),It2->second.c_str(), _hcGrp);
    }

    // filling up Int nodes
    std::map<std::string,long> mcIntMap = _hcGrp->GetIntMap();
    for(std::map<std::string,long>::iterator It3=mcIntMap.begin();It3!=mcIntMap.end();It3++)
    {
      (void)new ParameterInt(ParamVal,It3->first.c_str(),It3->second, _hcGrp);
    }

    // filling up Float nodes
    std::map<std::string,double> mcFloatMap = _hcGrp->GetFloatMap();
    for(std::map<std::string,double>::iterator It4=mcFloatMap.begin();It4!=mcFloatMap.end();It4++)
    {
      (void)new ParameterFloat(ParamVal,It4->first.c_str(),It4->second, _hcGrp);
    }

    // filling up bool nodes
    std::map<std::string,bool> mcBoolMap = _hcGrp->GetBoolMap();
    for(std::map<std::string,bool>::iterator It5=mcBoolMap.begin();It5!=mcBoolMap.end();It5++)
    {
      (void)new ParameterBool(ParamVal,It5->first.c_str(),It5->second, _hcGrp);
    }

    // filling up UInt nodes
    std::map<std::string,unsigned long> mcUIntMap = _hcGrp->GetUnsignedMap();
    for(std::map<std::string,unsigned long>::iterator It6=mcUIntMap.begin();It6!=mcUIntMap.end();It6++)
    {
      (void)new ParameterUInt(ParamVal,It6->first.c_str(),It6->second, _hcGrp);
    }
  }
}

/** Switches the type of parameters either to user or system parameters. */
void DlgParameterImp::onParameterSetChange(const QString& rcString)
{
  ParameterManager* rcParMngr = App::GetApplication().GetParameterSet(rcString.latin1());

  // remove all labels
  ParamGrp->clear();
  ParamVal->clear();

  // root labels
  std::vector<FCHandle<ParameterGrp> > grps = rcParMngr->GetGroups();
  for ( std::vector<FCHandle<ParameterGrp> >::iterator it = grps.begin(); it != grps.end(); ++it ) {
    QListViewItem* item = new ParameterGroupItem(ParamGrp,*it);
    item->setOpen(true);
  }

  ParamGrp->triggerUpdate(); 
}

// --------------------------------------------------------------------

/* TRANSLATOR Gui::Dialog::ParameterGroup */

ParameterGroup::ParameterGroup( QWidget * parent, const char * name, WFlags f )
  : QListView(parent, name, f)
{
#ifdef FC_OS_WIN32
  QPalette pal = palette();
  const QColor& bg = pal.color( QPalette::Inactive, QColorGroup::Base );
  pal.setColor( QPalette::Inactive, QColorGroup::Highlight, bg );
  const QColor& ht = pal.color( QPalette::Inactive, QColorGroup::Text );
  pal.setColor( QPalette::Inactive, QColorGroup::HighlightedText, ht );
  setPalette( pal );
#endif

  menuEdit = new QPopupMenu(this);
  QFont font = menuEdit->font();
  font.setBold( true );
  custom = new CustomMenuItem( tr("Expand"), font );
  _expandId = menuEdit->insertItem( custom );
  menuEdit->connectItem( _expandId, this, SLOT( onToggleSelectedItem() ) );
  menuEdit->insertSeparator();
  menuEdit->insertItem( tr("Add sub-group"), this, SLOT( onCreateSubgroup() ) );
  menuEdit->insertItem( tr("Remove group"), this, SLOT( onDeleteSelectedItem() ) );
  menuEdit->insertItem( tr("Rename group"), this, SLOT( onRenameSelectedItem() ) );
  menuEdit->insertSeparator();
  menuEdit->insertItem( tr("Export parameter"), this, SLOT( onExportToFile() ) );
  _importId = menuEdit->insertItem( tr("Import parameter"), this, SLOT( onImportFromFile() ) );
}

ParameterGroup::~ParameterGroup()
{
}

void ParameterGroup::setSelected ( QListViewItem * item, bool selected )
{
  if ( selected )
    QListView::setSelected( item, selected );
}

void ParameterGroup::contentsContextMenuEvent ( QContextMenuEvent* event )
{
  if ( selectedItem () )
  {
    menuEdit->setItemEnabled( _expandId, selectedItem()->childCount() > 0 );
    // do not allow to import parameters from a non-empty parameter group
    menuEdit->setItemEnabled( _importId, selectedItem()->childCount() == 0 );

    if ( selectedItem()->isOpen() )
      custom->setText( tr("Collapse") );
    else
      custom->setText( tr("Expand") );
    menuEdit->popup(event->globalPos());
  }
}

void ParameterGroup::keyPressEvent (QKeyEvent* event)
{
  switch ( tolower(event->key()) ) 
  {
    case Qt::Key_Delete:
    {
      onDeleteSelectedItem();
    } break;
		default:
			QListView::keyPressEvent(event);
  }
}

void ParameterGroup::onDeleteSelectedItem()
{
  QListViewItem* sel = selectedItem();
  if ( sel && sel->parent() )
  {
    QListViewItem* item = sel->itemBelow();
    if ( !item || item->parent() != sel->parent() )
      item = sel->itemAbove();
    if ( !item )
      item = sel->parent();

    if ( QMessageBox::question(this, tr("Remove group"), tr("Do really want to remove this parameter group?"),
                               QMessageBox::Yes, QMessageBox::No|QMessageBox::Default|QMessageBox::Escape) == 
                               QMessageBox::Yes )
    {
      QListViewItem* parent = sel->parent();
      parent->takeItem( sel );
      setSelected( item, true );
      delete sel;
      if ( parent->childCount() == 0 )
        parent->setOpen( false );
    }
  }
}

void ParameterGroup::onToggleSelectedItem()
{
  if ( selectedItem () )
  {
    if ( selectedItem()->isOpen() )
      selectedItem()->setOpen( false );
    else if ( selectedItem()->childCount() > 0 )
      selectedItem()->setOpen( true );
  }
}

void ParameterGroup::onCreateSubgroup()
{
  bool ok;
  QString name = QInputDialog::getText(QObject::tr("New sub-group"), QObject::tr("Enter the name:"),
                                      QLineEdit::Normal, QString::null, &ok, this);

  if ( ok && !name.isEmpty() )
  {
    QListViewItem* item = selectedItem();
    if ( item && item->rtti() == 2000 )
    {
      ParameterGroupItem* para = reinterpret_cast<ParameterGroupItem*>(item);
      FCHandle<ParameterGrp> hGrp = para->_hcGrp;

      if ( hGrp->HasGroup( name.latin1() ) )
      {
        QMessageBox::critical( this, tr("Existing sub-group"),
          tr("The sub-group '%1' already exists.").arg( name ) );
        return;
      }

      hGrp = hGrp->GetGroup( name.latin1() );
      (void)new ParameterGroupItem(para,hGrp);
      para->setOpen( true );
    }
  }
}

void ParameterGroup::onExportToFile()
{
  bool ok;
  QString file = FileDialog::getSaveFileName( QString::null, "XML (*.FCParam)", this, "Parameter",
                                              tr("Export parameter to file"), 0, true, tr("Export"), &ok);
  if ( ok )
  {
    QListViewItem* item = selectedItem();
    if ( item && item->rtti() == 2000 )
    {
      ParameterGroupItem* para = reinterpret_cast<ParameterGroupItem*>(item);
      FCHandle<ParameterGrp> hGrp = para->_hcGrp;
      hGrp->exportTo( file.latin1() );
    }
  }
}

void ParameterGroup::onImportFromFile()
{
  bool ok;
  QString file = FileDialog::getOpenFileName( QString::null, "XML (*.FCParam)", this, "Parameter",
                                              tr("Import parameter from file"), 0, true, tr("Import"), &ok);
  if ( ok )
  {
    QFileInfo fi(file);
    QListViewItem* item = selectedItem();
    if ( item && item->rtti() == 2000 )
    {
      ParameterGroupItem* para = reinterpret_cast<ParameterGroupItem*>(item);
      FCHandle<ParameterGrp> hGrp = para->_hcGrp;

      QListViewItem* child;
      QPtrList<QListViewItem> remChild;
      if ( (child=para->firstChild()) != 0 )
      {
        remChild.append( child );
        child = child->nextSibling();
        while ( child )
        {
          remChild.append( child );
          child = child->nextSibling();
        }
      }

      // remove the items and internal parameter values
      for ( child = remChild.first(); child; child = remChild.next() )
      {
        para->takeItem( child );
        delete child;
      }

      try
      {
        hGrp->importFrom( file.latin1() );
        std::vector<FCHandle<ParameterGrp> > cSubGrps = hGrp->GetGroups();
        for ( std::vector<FCHandle<ParameterGrp> >::iterator it = cSubGrps.begin(); it != cSubGrps.end(); ++it )
        {
          new ParameterGroupItem(para,*it);
        }

        para->setOpen( para->childCount() > 0 );
      }
      catch( const Base::Exception& )
      {
        QMessageBox::critical(this, tr("Import Error"),tr("Reading of '%1' failed.").arg( file ));
      }
    }
  }
}

void ParameterGroup::onRenameSelectedItem()
{
  QListViewItem* sel = selectedItem();
  if ( sel && sel->rtti() == 2000 )
  {
     sel->startRename(0);
  }
}

// --------------------------------------------------------------------

/* TRANSLATOR Gui::Dialog::ParameterValue */

ParameterValue::ParameterValue( QWidget * parent, const char * name, WFlags f )
  : QListView(parent, name, f)
{
  menuEdit = new QPopupMenu(this);
  QFont font = menuEdit->font();
  font.setBold( true );
  int id = menuEdit->insertItem( new CustomMenuItem( tr("Change value"), font ) );
  menuEdit->connectItem( id, this, SLOT( onChangeSelectedItem() ) );
  menuEdit->insertSeparator();
  menuEdit->insertItem( tr("Remove key"), this, SLOT( onDeleteSelectedItem() ) );
  menuEdit->insertItem( tr("Rename key"), this, SLOT( onRenameSelectedItem() ) );
  menuNew = new QPopupMenu(this);
  menuNew->insertItem( tr("New string item"), this, SLOT( onCreateTextItem() ) );
  menuNew->insertItem( tr("New float item"), this, SLOT( onCreateFloatItem() ) );
  menuNew->insertItem( tr("New integer item"), this, SLOT( onCreateIntItem() ) );
  menuNew->insertItem( tr("New unsigned item"), this, SLOT( onCreateUIntItem() ) );
  menuNew->insertItem( tr("New boolean item"), this, SLOT( onCreateBoolItem() ) );

  connect(this, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(onChangeSelectedItem()));
}

ParameterValue::~ParameterValue()
{
}

void ParameterValue::setCurrentGroup( const FCHandle<ParameterGrp>& hGrp )
{
  _hcGrp = hGrp;
}

void ParameterValue::takeItem ( QListViewItem * i )
{
  if ( i && i->rtti() == 2100 )
  {
    reinterpret_cast<ParameterValueItem*>(i)->removeFromGroup();
  }

  QListView::takeItem( i );
}

void ParameterValue::contentsContextMenuEvent ( QContextMenuEvent* event )
{
  if ( selectedItem () )
    menuEdit->popup(event->globalPos());
  else
    menuNew->popup(event->globalPos());
}

void ParameterValue::keyPressEvent (QKeyEvent* event)
{
  switch ( tolower(event->key()) ) 
  {
    case Qt::Key_Delete:
    {
      onDeleteSelectedItem();
    } break;
		default:
			QListView::keyPressEvent(event);
  }
}

void ParameterValue::onChangeSelectedItem()
{
  QListViewItem* sel = selectedItem();
  if ( sel && sel->rtti() == 2100 )
  {
    reinterpret_cast<ParameterValueItem*>(sel)->changeValue();
  }
}

void ParameterValue::onDeleteSelectedItem()
{
  QListViewItem* sel = selectedItem();
  if ( sel )
  {
    QListViewItem* next = sel->itemBelow();
    if ( !next )
      next = sel->itemAbove();
    takeItem( sel );
    delete sel;
    if ( next )
      setSelected(next, true);
  }
}

void ParameterValue::onRenameSelectedItem()
{
  QListViewItem* sel = selectedItem();
  if ( sel )
  {
    sel->startRename(0);
  }
}

void ParameterValue::onCreateTextItem()
{
  bool ok;
  QString name = QInputDialog::getText(QObject::tr("New text item"), QObject::tr("Enter the name:"), 
                                      QLineEdit::Normal, QString::null, &ok, this);
  if ( !ok || name.isEmpty() )
    return;

  std::map<std::string,std::string> smap = _hcGrp->GetASCIIMap();
  if ( smap.find( name.latin1() ) != smap.end() )
  {
    QMessageBox::critical( this, tr("Existing item"),
      tr("The item '%1' already exists.").arg( name ) );
    return;
  }

  QString val = QInputDialog::getText(QObject::tr("New text item"), QObject::tr("Enter your text:"), 
                                      QLineEdit::Normal, QString::null, &ok, this);
  if ( ok && !val.isEmpty() )
  {
    ParameterValueItem *pcItem;
    pcItem = new ParameterText(this,name,val, _hcGrp);
    pcItem->appendToGroup();
  }
}

void ParameterValue::onCreateIntItem()
{
  bool ok;
  QString name = QInputDialog::getText(QObject::tr("New integer item"), QObject::tr("Enter the name:"), 
                                      QLineEdit::Normal, QString::null, &ok, this);
  if ( !ok || name.isEmpty() )
    return;

  std::map<std::string,long> lmap = _hcGrp->GetIntMap();
  if ( lmap.find( name.latin1() ) != lmap.end() )
  {
    QMessageBox::critical( this, tr("Existing item"),
      tr("The item '%1' already exists.").arg( name ) );
    return;
  }

  int val = QInputDialog::getInteger(QObject::tr("New integer item"), QObject::tr("Enter your number:"), 
                                     0, -2147483647, 2147483647, 1, &ok, this);

  if ( ok )
  {
    ParameterValueItem *pcItem;
    pcItem = new ParameterInt(this,name,(long)val, _hcGrp);
    pcItem->appendToGroup();
  }
}

void ParameterValue::onCreateUIntItem()
{
  bool ok;
  QString name = QInputDialog::getText(QObject::tr("New unsigned item"), QObject::tr("Enter the name:"), 
                                      QLineEdit::Normal, QString::null, &ok, this);
  if ( !ok || name.isEmpty() )
    return;

  std::map<std::string,unsigned long> lmap = _hcGrp->GetUnsignedMap();
  if ( lmap.find( name.latin1() ) != lmap.end() )
  {
    QMessageBox::critical( this, tr("Existing item"),
      tr("The item '%1' already exists.").arg( name ) );
    return;
  }

  DlgInputDialogImp dlg(QObject::tr("Enter your number:"),this, "Input", true, DlgInputDialogImp::UIntBox);
  dlg.setCaption(QObject::tr("New unsigned item"));
  UIntSpinBox* edit = dlg.getUIntBox();
  edit->setRange(0,UINT_MAX);
  if (dlg.exec() == QDialog::Accepted ) {
    QString value = edit->text();
    unsigned long val = value.toULong(&ok);

    if ( ok )
    {
      ParameterValueItem *pcItem;
      pcItem = new ParameterUInt(this,name, val, _hcGrp);
      pcItem->appendToGroup();
    }
  }
}

void ParameterValue::onCreateFloatItem()
{
  bool ok;
  QString name = QInputDialog::getText(QObject::tr("New float item"), QObject::tr("Enter the name:"), 
                                      QLineEdit::Normal, QString::null, &ok, this);
  if ( !ok || name.isEmpty() )
    return;

  std::map<std::string,double> fmap = _hcGrp->GetFloatMap();
  if ( fmap.find( name.latin1() ) != fmap.end() )
  {
    QMessageBox::critical( this, tr("Existing item"),
      tr("The item '%1' already exists.").arg( name ) );
    return;
  }

  double val = QInputDialog::getDouble(QObject::tr("New float item"), QObject::tr("Enter your number:"), 
                                       0, -2147483647, 2147483647, 12, &ok, this);
  if ( ok )
  {
    ParameterValueItem *pcItem;
    pcItem = new ParameterFloat(this,name,val, _hcGrp);
    pcItem->appendToGroup();
  }
}

void ParameterValue::onCreateBoolItem()
{
  bool ok;
  QString name = QInputDialog::getText(QObject::tr("New boolean item"), QObject::tr("Enter the name:"), 
                                      QLineEdit::Normal, QString::null, &ok, this);
  if ( !ok || name.isEmpty() )
    return;

  std::map<std::string,bool> bmap = _hcGrp->GetBoolMap();
  if ( bmap.find( name.latin1() ) != bmap.end() )
  {
    QMessageBox::critical( this, tr("Existing item"),
      tr("The item '%1' already exists.").arg( name ) );
    return;
  }

  QStringList list; list << "true" << "false";
  QString val = QInputDialog::getItem (QObject::tr("New boolean item"), QObject::tr("Choose an item:"),
                                       list, 0, false, &ok, this);
  if ( ok )
  {
    ParameterValueItem *pcItem;
    pcItem = new ParameterBool(this,name,(val == list[0] ? true : false), _hcGrp);
    pcItem->appendToGroup();
  }
}

// ---------------------------------------------------------------------------

ParameterGroupItem::ParameterGroupItem( ParameterGroupItem * parent, const FCHandle<ParameterGrp> &hcGrp )
    : QListViewItem( parent ), _hcGrp(hcGrp)
{
  setRenameEnabled(0,true);
  fillUp();
}

ParameterGroupItem::ParameterGroupItem( QListView* parent, const FCHandle<ParameterGrp> &hcGrp)
    : QListViewItem( parent ), _hcGrp(hcGrp)
{
  setRenameEnabled(0,true);
  fillUp();
}

ParameterGroupItem::~ParameterGroupItem()
{
  // if the group has already been removed from the parameters then clear the observer list
  // as we cannot notify the attached observers here
  if ( _hcGrp.IsLastRef() )
    _hcGrp->ClearObserver();
}

void ParameterGroupItem::fillUp(void)
{
  //setPixmap( folderOpen );
  // filing up groups
  std::vector<FCHandle<ParameterGrp> > vhcParamGrp = _hcGrp->GetGroups();

  setText(0,_hcGrp->GetGroupName());
  setPixmap(0,Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));

  for(std::vector<FCHandle<ParameterGrp> >::iterator It=vhcParamGrp.begin();It!=vhcParamGrp.end();It++)
    (void)new ParameterGroupItem(this,*It);
}

void ParameterGroupItem::setSelected ( bool o )
{
  if( o )
  {
    setPixmap(0,Gui::BitmapFactory().pixmap("RawTree_LabelOpen"));
  }else{
    setPixmap(0,Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));
  }
  QListViewItem::setSelected ( o );
}

void ParameterGroupItem::takeItem ( QListViewItem * item )
{
  if ( item && item->rtti() == 2000 )
  {
    ParameterGroupItem* child = reinterpret_cast<ParameterGroupItem*>(item);
    _hcGrp->RemoveGrp(child->text(0).latin1());
  }

  QListViewItem::takeItem( item );
}

void ParameterGroupItem::startRename ( int col )
{
  QListViewItem* par = this->parent();
  if ( par && par->rtti() == 2000 )
  {
    QListViewItem::startRename( col );
  }
  else
  {
    QMessageBox::information( listView(), QObject::tr("Rename group"), QObject::tr("Cannot rename this group.") );
  }
}

void ParameterGroupItem::okRename ( int col )
{
  QListViewItem* item = parent();
  if ( item && item->rtti() == 2000 )
  {
    QString oldName = text(0);
    QListViewItem::okRename( 0 );
    QString newName = text(0);

    // name has changed
    if ( oldName != newName )
    {
      ParameterGroupItem* par = reinterpret_cast<ParameterGroupItem*>(item);

      // first check if there is already a group with name "newName"
      if ( par->_hcGrp->HasGroup( newName.latin1() ) )
      {
        QMessageBox::critical( listView(), QObject::tr("Existing group"),
          QObject::tr("The group '%1' already exists.").arg( newName ) );
        setText( 0, oldName );
      }
      else
      {
        // rename the group by adding a new group, copy the content and remove the old group
        FCHandle<ParameterGrp> hOldGrp = par->_hcGrp->GetGroup( oldName.latin1() );
        FCHandle<ParameterGrp> hNewGrp = par->_hcGrp->GetGroup( newName.latin1() );
        hOldGrp->copyTo( hNewGrp );
        par->_hcGrp->RemoveGrp( oldName.latin1() );
      }
    }
  }
}

// --------------------------------------------------------------------

ParameterValueItem::ParameterValueItem ( QListView * parent, QString label1, const FCHandle<ParameterGrp> &hcGrp)
  : QListViewItem( parent, label1 ), _hcGrp(hcGrp)
{
  setRenameEnabled(0,true);
}

ParameterValueItem::~ParameterValueItem()
{
}

void ParameterValueItem::startRename ( int col )
{
  QListViewItem::startRename( col );
}

void ParameterValueItem::okRename ( int col )
{
  if ( col != 0 )
  {
    QListViewItem::okRename( col );
    return;
  }
  else
  {
    QString oldName = text(0);
    QListViewItem::okRename( 0 );
    QString newName = text(0);

    // name has changed
    if ( oldName != newName )
    {
      replace( oldName, newName );
    }
  }
}

// --------------------------------------------------------------------

ParameterText::ParameterText ( QListView * parent, QString label1, QString value, const FCHandle<ParameterGrp> &hcGrp)
  :ParameterValueItem( parent, label1, hcGrp)
{
  setPixmap(0,BitmapFactory().pixmap("Param_Text") );
  setText(1, "Text");
  setText(2, value);
}

ParameterText::~ParameterText()
{
}

void ParameterText::changeValue()
{
  const char* name = text(0).latin1();
  bool ok;

  QString txt = QInputDialog::getText(QObject::tr("Change value"), QObject::tr("Enter your text:"), 
                                      QLineEdit::Normal, text(2), &ok, listView());
  if ( ok )
  {
    setText( 2, txt );
    _hcGrp->SetASCII(name, txt.latin1() );
  }
}

void ParameterText::removeFromGroup ()
{
  _hcGrp->RemoveASCII(text(0).latin1());
}

void ParameterText::replace( const QString& oldName, const QString& newName )
{
  std::string val = _hcGrp->GetASCII( oldName.latin1() );
  _hcGrp->RemoveASCII( oldName.latin1() );
  _hcGrp->SetASCII( newName.latin1(), val.c_str() );
}

void ParameterText::appendToGroup()
{
  const char* name = text(0).latin1();
  const char* val =  text(2).latin1();
  _hcGrp->SetASCII(name, val );
}

// --------------------------------------------------------------------

ParameterInt::ParameterInt ( QListView * parent, QString label1, long value, const FCHandle<ParameterGrp> &hcGrp)
  :ParameterValueItem( parent, label1, hcGrp)
{
  setPixmap(0,BitmapFactory().pixmap("Param_Int") );
  setText(1, "Integer");
  setText(2, QString("%1").arg(value));
}

ParameterInt::~ParameterInt()
{
}

void ParameterInt::changeValue()
{
  const char* name = text(0).latin1();
  bool ok;

  int num = QInputDialog::getInteger(QObject::tr("Change value"), QObject::tr("Enter your number:"), 
                                     text(2).toInt(), -2147483647, 2147483647, 1, &ok, listView());
  if ( ok )
  {
    setText(2, QString("%1").arg(num));
    _hcGrp->SetInt(name, (long)num);
  }
}

void ParameterInt::removeFromGroup ()
{
  _hcGrp->RemoveInt(text(0).latin1());
}

void ParameterInt::replace( const QString& oldName, const QString& newName )
{
  long val = _hcGrp->GetInt( oldName.latin1() );
  _hcGrp->RemoveInt( oldName.latin1() );
  _hcGrp->SetInt( newName.latin1(), val );
}

void ParameterInt::appendToGroup()
{
  const char* name = text(0).latin1();
  long val =  text(2).toLong();
  _hcGrp->SetInt(name, val );
}

// --------------------------------------------------------------------

ParameterUInt::ParameterUInt ( QListView * parent, QString label1, unsigned long value, const FCHandle<ParameterGrp> &hcGrp)
  :ParameterValueItem( parent, label1, hcGrp)
{
  setPixmap(0,BitmapFactory().pixmap("Param_UInt") );
  setText(1, "Unsigned");
  setText(2, QString("%1").arg(value));
}

ParameterUInt::~ParameterUInt()
{
}

void ParameterUInt::changeValue()
{
  const char* name = text(0).latin1();
  bool ok;

  DlgInputDialogImp dlg(QObject::tr("Enter your number:"),listView(), "Input", true, DlgInputDialogImp::UIntBox);
  dlg.setCaption(QObject::tr("Change value"));
  UIntSpinBox* edit = dlg.getUIntBox();
  edit->setRange(0,UINT_MAX);
  edit->setValue(text(2).toULong());
  if (dlg.exec() == QDialog::Accepted)
  {
    QString value = edit->text();
    unsigned long num = value.toULong(&ok);

    if ( ok )
    {
      setText(2, QString("%1").arg(num));
      _hcGrp->SetUnsigned(name, (unsigned long)num);
    }
  }
}

void ParameterUInt::removeFromGroup ()
{
  _hcGrp->RemoveUnsigned(text(0).latin1());
}

void ParameterUInt::replace( const QString& oldName, const QString& newName )
{
  unsigned long val = _hcGrp->GetUnsigned( oldName.latin1() );
  _hcGrp->RemoveUnsigned( oldName.latin1() );
  _hcGrp->SetUnsigned( newName.latin1(), val );
}

void ParameterUInt::appendToGroup()
{
  const char* name = text(0).latin1();
  unsigned long val =  text(2).toULong();
  _hcGrp->SetUnsigned(name, val );
}

// --------------------------------------------------------------------

ParameterFloat::ParameterFloat ( QListView * parent, QString label1, double value, const FCHandle<ParameterGrp> &hcGrp)
  :ParameterValueItem( parent, label1, hcGrp)
{
  setPixmap(0,BitmapFactory().pixmap("Param_Float") );
  setText(1, "Float");
  setText(2, QString("%1").arg(value));
}

ParameterFloat::~ParameterFloat()
{
}

void ParameterFloat::changeValue()
{
  const char* name = text(0).latin1();
  bool ok;

  double num = QInputDialog::getDouble(QObject::tr("Change value"), QObject::tr("Enter your number:"), 
                                       text(2).toDouble(), -2147483647, 2147483647, 12, &ok, listView());
  if ( ok )
  {
    setText(2, QString("%1").arg(num));
    _hcGrp->SetFloat(name, num);
  }
}

void ParameterFloat::removeFromGroup ()
{
  _hcGrp->RemoveFloat(text(0).latin1());
}

void ParameterFloat::replace( const QString& oldName, const QString& newName )
{
  double val = _hcGrp->GetFloat( oldName.latin1() );
  _hcGrp->RemoveFloat( oldName.latin1() );
  _hcGrp->SetFloat( newName.latin1(), val );
}

void ParameterFloat::appendToGroup()
{
  const char* name = text(0).latin1();
  double val =  text(2).toDouble();
  _hcGrp->SetFloat(name, val );
}

// --------------------------------------------------------------------

ParameterBool::ParameterBool ( QListView * parent, QString label1, bool value, const FCHandle<ParameterGrp> &hcGrp)
  :ParameterValueItem( parent, label1, hcGrp)
{
  setPixmap(0,BitmapFactory().pixmap("Param_Bool") );
  setText(1, "Boolean");
  setText(2, (value ? "true" : "false"));
}

ParameterBool::~ParameterBool()
{
}

void ParameterBool::changeValue()
{
  const char* name = text(0).latin1();
  bool ok;

  QStringList list; list << "true" << "false";
  int pos = (text(2) == list[0] ? 0 : 1);

  QString txt = QInputDialog::getItem (QObject::tr("Change value"), QObject::tr("Choose an item:"),
                                       list, pos, false, &ok, listView());
  if ( ok )
  {
    setText( 2, txt );
    _hcGrp->SetBool(name, (txt == list[0] ? true : false) );
  }
}

void ParameterBool::removeFromGroup ()
{
  _hcGrp->RemoveBool(text(0));
}

void ParameterBool::replace( const QString& oldName, const QString& newName )
{
  bool val = _hcGrp->GetBool( oldName.latin1() );
  _hcGrp->RemoveBool( oldName.latin1() );
  _hcGrp->SetBool( newName.latin1(), val );
}

void ParameterBool::appendToGroup()
{
  const char* name = text(0).latin1();
  bool val = (text(2) == "true" ? true : false);
  _hcGrp->SetBool(name, val );
}

// --------------------------------------------------------------------

CustomMenuItem::CustomMenuItem( const QString& s, const QFont& f )
	: string( s ), font( f )
{
}

CustomMenuItem::~CustomMenuItem()
{
}

void CustomMenuItem::paint( QPainter* p, const QColorGroup&, bool, bool, int x, int y, int w, int h )
{
  p->setFont ( font );
  p->drawText( x, y, w, h, AlignLeft | AlignVCenter | DontClip | ShowPrefix, string );
}

QSize CustomMenuItem::sizeHint()
{
  return QFontMetrics( font ).size( AlignLeft | AlignVCenter | ShowPrefix | DontClip,  string );
}

void CustomMenuItem::setText( const QString& text )
{
  string = text;
}

#include "DlgParameter.cpp"
#include "moc_DlgParameter.cpp"
#include "moc_DlgParameterImp.cpp"
