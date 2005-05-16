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
#include "BitmapFactory.h"

#include "../Base/Parameter.h"
#include "../App/Application.h"


using namespace Gui::Dialog;

/**
 *  Constructs a DlgParameterImp which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
DlgParameterImp::DlgParameterImp( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : DlgParameter( parent, name, modal, fl|WStyle_MinMax ),_pcMainLabel(0L)
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
  
  const std::map<std::string,FCParameterManager *> rcList = App::GetApplication().GetParameterSetList();

  for( std::map<std::string,FCParameterManager *>::const_iterator It= rcList.begin();It!=rcList.end();It++)
  {
    if(It->first == "User parameter")
      SetNameComboBox->insertItem(It->first.c_str(),1);
    else
      SetNameComboBox->insertItem(It->first.c_str(),-1);
  }

  QString cStr("User parameter");
  SetNameComboBox->setCurrentText( cStr );
  
  onParameterSetChange(cStr);
  connect( SetNameComboBox, SIGNAL( activated( const QString& ) ), 
           this, SLOT( onParameterSetChange( const QString& ) ) );
  connect( ParamGrp, SIGNAL( currentChanged( QListViewItem* ) ), 
           this, SLOT( onGroupSelected(QListViewItem *) ) );
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
    FCHandle<FCParameterGrp> _hcGrp = reinterpret_cast<ParameterGroupItem*>(item)->_hcGrp;
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
  }
}

/** \todo */
void DlgParameterImp::onLoadParameterSet()
{ 
}

/** \todo */
void DlgParameterImp::onInsertFromFile()
{

}

/** Switches the type of parameters either to user or system parameters. */
void DlgParameterImp::onParameterSetChange(const QString& rcString)
{
  FCParameterManager &rcParMngr = App::GetApplication().GetParameterSet(rcString.latin1());

  if(_pcMainLabel) delete _pcMainLabel;
  // remove all labels
  ParamGrp->clear();
  ParamVal->clear();

  // root label
  _pcMainLabel = new ParameterGroupItem(ParamGrp,rcParMngr.GetGroup("BaseApp"));
  _pcMainLabel->setOpen(true);

  ParamGrp->triggerUpdate(); 
}

// --------------------------------------------------------------------

ParameterGroup::ParameterGroup( QWidget * parent, const char * name, WFlags f )
  : QListView(parent, name, f)
{
  menuEdit = new QPopupMenu(this);
  menuEdit->insertItem( tr("Remove group"), this, SLOT( onDeleteSelectedItem() ) );
}

ParameterGroup::~ParameterGroup()
{
}

void ParameterGroup::contentsContextMenuEvent ( QContextMenuEvent* event )
{
  if ( selectedItem () )
    menuEdit->popup(event->globalPos());
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

    if ( QMessageBox::question(this, tr("Remove group"), 
                               tr("Do really want to remove this parameter group?"),
                               QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes )
    {
      sel->parent()->takeItem( sel );
      setSelected( item, true );
      delete sel;
    }
  }
}

// --------------------------------------------------------------------

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
  menuNew->insertItem( tr("New boolean item"), this, SLOT( onCreateBoolItem() ) );

  connect( this, SIGNAL( itemRenamed ( QListViewItem*, int, const QString& ) ), 
           this, SLOT( onRenamedSelectedItem( QListViewItem*, int, const QString& ) ) );
  connect(this, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(onChangeSelectedItem()));
}

ParameterValue::~ParameterValue()
{
}

void ParameterValue::setCurrentGroup( const FCHandle<FCParameterGrp>& hGrp )
{
  _hcGrp = hGrp;
}

void ParameterValue::takeItem ( QListViewItem * i )
{
  if ( i && i->rtti() == 3000 )
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
  if ( sel && sel->rtti() == 3000 )
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
    sel->setRenameEnabled(0,true);
    sel->startRename(0);
  }
}

void ParameterValue::onRenamedSelectedItem( QListViewItem * item, int col, const QString & text )
{
  if ( item )
  {
    item->setRenameEnabled(0,false);
  }
}

void ParameterValue::onCreateTextItem()
{
  bool ok;
  QString name = QInputDialog::getText(QObject::tr("New text"), QObject::tr("Enter the name:"), 
                                      QLineEdit::Normal, QString::null, &ok, this);
  if ( !ok || name.isEmpty() )
    return;

  QString val = QInputDialog::getText(QObject::tr("New text"), QObject::tr("Enter your text:"), 
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
  QString name = QInputDialog::getText(QObject::tr("New integer"), QObject::tr("Enter the name:"), 
                                      QLineEdit::Normal, QString::null, &ok, this);
  if ( !ok || name.isEmpty() )
    return;

  int val = QInputDialog::getInteger(QObject::tr("New integer"), QObject::tr("Enter your number:"), 
                                     0, -2147483647, 2147483647, 1, &ok, this);

  if ( ok )
  {
    ParameterValueItem *pcItem;
    pcItem = new ParameterInt(this,name,(long)val, _hcGrp);
    pcItem->appendToGroup();
  }
}

void ParameterValue::onCreateFloatItem()
{
  bool ok;
  QString name = QInputDialog::getText(QObject::tr("New float"), QObject::tr("Enter the name:"), 
                                      QLineEdit::Normal, QString::null, &ok, this);
  if ( !ok || name.isEmpty() )
    return;

  double val = QInputDialog::getDouble(QObject::tr("New float"), QObject::tr("Enter your number:"), 
                                       0, -2147483647, 2147483647, 1, &ok, this);
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
  QString name = QInputDialog::getText(QObject::tr("New boolean"), QObject::tr("Enter the name:"), 
                                      QLineEdit::Normal, QString::null, &ok, this);
  if ( !ok || name.isEmpty() )
    return;

  QStringList list; list << "true" << "false";
  QString val = QInputDialog::getItem (QObject::tr("New boolean"), QObject::tr("Choose an item:"),
                                       list, 0, false, &ok, this);
  if ( ok )
  {
    ParameterValueItem *pcItem;
    pcItem = new ParameterBool(this,name,(val == list[0] ? true : false), _hcGrp);
    pcItem->appendToGroup();
  }
}

// ---------------------------------------------------------------------------

ParameterGroupItem::ParameterGroupItem( ParameterGroupItem * parent, const FCHandle<FCParameterGrp> &hcGrp )
    : QListViewItem( parent ), _hcGrp(hcGrp)
{
  fillUp();
}

ParameterGroupItem::ParameterGroupItem( QListView* parent, const FCHandle<FCParameterGrp> &hcGrp)
    : QListViewItem( parent ), _hcGrp(hcGrp)
{
  fillUp();
}

ParameterGroupItem::~ParameterGroupItem()
{
}

void ParameterGroupItem::fillUp(void)
{
  //setPixmap( folderOpen );
  // filing up groups
  std::vector<FCHandle<FCParameterGrp> > vhcParamGrp = _hcGrp->GetGroups();

  setText(0,_hcGrp->GetGroupName());
  setPixmap(0,Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));

  for(std::vector<FCHandle<FCParameterGrp> >::iterator It=vhcParamGrp.begin();It!=vhcParamGrp.end();It++)
    (void)new ParameterGroupItem(this,*It);
}

void ParameterGroupItem::setOpen ( bool o )
{
  if( o )
  {
    setPixmap(0,Gui::BitmapFactory().pixmap("RawTree_LabelOpen"));
  }else{
    setPixmap(0,Gui::BitmapFactory().pixmap("RawTree_LabelClosed"));
  }
  QListViewItem::setOpen ( o );
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

// --------------------------------------------------------------------

ParameterValueItem::ParameterValueItem ( QListView * parent, QString label1, const FCHandle<FCParameterGrp> &hcGrp)
  : QListViewItem( parent, label1 ), _hcGrp(hcGrp)
{
}

ParameterValueItem::~ParameterValueItem()
{
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
    QString oldName = text(0).latin1();
    QListViewItem::okRename( 0 );
    QString newName = text(0).latin1();

    // name has changed
    if ( oldName != newName )
    {
      replace( oldName, newName );
    }
  }
}

// --------------------------------------------------------------------

ParameterText::ParameterText ( QListView * parent, QString label1, QString value, const FCHandle<FCParameterGrp> &hcGrp)
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

ParameterInt::ParameterInt ( QListView * parent, QString label1, long value, const FCHandle<FCParameterGrp> &hcGrp)
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

ParameterFloat::ParameterFloat ( QListView * parent, QString label1, double value, const FCHandle<FCParameterGrp> &hcGrp)
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
                                       text(2).toDouble(), -2147483647, 2147483647, 1, &ok, listView());
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

ParameterBool::ParameterBool ( QListView * parent, QString label1, bool value, const FCHandle<FCParameterGrp> &hcGrp)
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

#include "DlgParameter.cpp"
#include "moc_DlgParameter.cpp"
#include "moc_DlgParameterImp.cpp"
