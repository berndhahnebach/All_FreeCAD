/***************************************************************************
                          PrefWidgets.cpp  -  description
                             -------------------
    begin                : 2002/12/20 10:47:44
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file PrefWidgets.cpp
 *  \brief A set of FreeCAD widgets
 *  \author Werner Mayer
 *  \version 0.1
 *  \date    2003/01/06
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
#	include <qaction.h>
#	include <qbutton.h>
#	include <qbuttongroup.h>
#	include <qcombobox.h>
#	include <qcursor.h>
#	include <qlabel.h>
#	include <qlayout.h>
#	include <qobjectlist.h>
#	include <qmessagebox.h>
#	include <qprocess.h>
#	include <qstring.h>
#	include <qtextbrowser.h>
#	include <qthread.h>
#	include <qurl.h>
#	include <qvalidator.h>
#	include <qwhatsthis.h>
#	include <ctype.h>
#	include <cmath>
#endif

#include "PrefWidgets.h"
#include "Application.h"
#include "ButtonGroup.h"
#include "Command.h"
#include "HtmlView.h"
#include "WidgetFactory.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"



FCWidgetPrefs::FCWidgetPrefs(const char * name, bool bInstall) : pHandler(NULL)
{
  if (bInstall)
  {
    // install a handler for automation stuff
    pHandler = new FCWidgetPrefsHandler(this);
  }
}

FCWidgetPrefs::~FCWidgetPrefs()
{
  if (hPrefGrp.IsValid())
    hPrefGrp->Detach(this);
#ifdef FC_DEBUG
  if (m_sPrefName.isNull() || m_sPrefName.isEmpty())
  {
    GetConsole().Warning("No valid widget name set!\n");
    throw;
  }
#endif
  delete pHandler;
}

void FCWidgetPrefs::setPrefName(QString pref)
{
  m_sPrefName = pref; 
}

void FCWidgetPrefs::setEntryName(QString name)
{
  setPrefName(name);
}

QString FCWidgetPrefs::getEntryName() const
{
#ifdef FC_DEBUG
  if (m_sPrefName.isNull() || m_sPrefName.isEmpty())
  {
    GetConsole().Warning("No valid widget name set!\n");
    throw;
  }
#endif

  return m_sPrefName;
}

void FCWidgetPrefs::setParamGrpPath(QString name)
{
  m_sPrefGrp = name;

#ifdef FC_DEBUG
	if (hPrefGrp.IsValid())
	{
		GetConsole().Warning("Widget already attached\n");
		hPrefGrp->Detach(this);
	}
#endif

	hPrefGrp = GetApplication().GetParameterGroupByPath(m_sPrefGrp.latin1());
	assert(hPrefGrp.IsValid());
	hPrefGrp->Attach(this);
}

QString FCWidgetPrefs::getParamGrpPath() const
{
  return m_sPrefGrp; 
}

FCWidgetPrefsHandler* FCWidgetPrefs::getHandler()
{
  return pHandler;
}

void FCWidgetPrefs::installHandler(FCWidgetPrefsHandler* h)
{
  delete pHandler;
  pHandler = h;
}

FCParameterGrp::handle FCWidgetPrefs::getParamGrp()
{
  return hPrefGrp;
}

void FCWidgetPrefs::OnChange(FCSubject<const char*> &rCaller, const char * sReason)
{
  if (QString(sReason) == m_sPrefName)
    restorePreferences();
}

FCParameterGrp::handle FCWidgetPrefs::getRootParamGrp()
{
  return GetApplication().GetUserParameter().GetGroup("BaseApp");
}

///////////////////////////////////////////////////////////////////////////////////

FCWidgetPrefsHandler::FCWidgetPrefsHandler(FCWidgetPrefs* p) : pPref(p)
{
}

void FCWidgetPrefsHandler::onSave()
{
  pPref->savePreferences();
  if (pPref->getParamGrp().IsValid())
    pPref->getParamGrp()->Notify(pPref->getEntryName().latin1());

  emit saved();
}

void FCWidgetPrefsHandler::onRestore()
{
  pPref->restorePreferences();
  emit restored();
}

///////////////////////////////////////////////////////////////////////////////////

FCPrefSpinBox::FCPrefSpinBox ( QWidget * parent, const char * name )
: FCFloatSpinBox(parent, name), FCWidgetPrefs(name)
{
}

FCPrefSpinBox::~FCPrefSpinBox() 
{
}

void FCPrefSpinBox::restorePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot restore!\n");
    return;
  }

  double fVal;
  if (decimals() == 0)
    fVal = (double)hPrefGrp->GetInt(getEntryName().latin1(), value());
  else
    fVal = (double)hPrefGrp->GetFloat(getEntryName().latin1(), valueFloat());

  setValueFloat(fVal);
}

void FCPrefSpinBox::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  if (decimals() == 0)
    hPrefGrp->SetInt(getEntryName().latin1(), (int)valueFloat());
  else
    hPrefGrp->SetFloat(getEntryName().latin1(), valueFloat());
}

QString FCPrefSpinBox::getEntryName () const
{
  return FCWidgetPrefs::getEntryName();
}

QString FCPrefSpinBox::getParamGrpPath () const
{
  return FCWidgetPrefs::getParamGrpPath();
}

void FCPrefSpinBox::setEntryName (QString name)
{
  FCWidgetPrefs::setEntryName(name);
}

void FCPrefSpinBox::setParamGrpPath (QString name)
{
  FCWidgetPrefs::setParamGrpPath(name);
}

///////////////////////////////////////////////////////////////////////////////////

FCLineEdit::FCLineEdit ( QWidget * parent, const char * name )
: QLineEdit(parent, name), FCWidgetPrefs(name)
{
}

FCLineEdit::~FCLineEdit()
{
}

void FCLineEdit::restorePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot restore!\n");
    return;
  }

  std::string txt = hPrefGrp->GetASCII(getEntryName().latin1(), text().latin1());
  setText(txt.c_str());
}

void FCLineEdit::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  hPrefGrp->SetASCII(getEntryName().latin1(), text().latin1());
}

QString FCLineEdit::getEntryName () const
{
  return FCWidgetPrefs::getEntryName();
}

QString FCLineEdit::getParamGrpPath () const
{
  return FCWidgetPrefs::getParamGrpPath();
}

void FCLineEdit::setEntryName (QString name)
{
  FCWidgetPrefs::setEntryName(name);
}

void FCLineEdit::setParamGrpPath (QString name)
{
  FCWidgetPrefs::setParamGrpPath(name);
}

///////////////////////////////////////////////////////////////////////////////////

FCComboBox::FCComboBox ( QWidget * parent, const char * name )
: QComboBox(parent, name), FCWidgetPrefs(name)
{
}

FCComboBox::~FCComboBox()
{
}

void FCComboBox::restorePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot restore!\n");
    return;
  }

  FCParameterGrp::handle  hPGrp = hPrefGrp->GetGroup(getEntryName().latin1());
  std::vector<std::string> items = hPGrp->GetASCIIs("Item");
  
  if (items.size() > 0) 
    clear();

  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    insertItem(it->c_str());

  int item = hPGrp->GetInt("currentItem", currentItem());
  setCurrentItem(item);
}

void FCComboBox::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  FCParameterGrp::handle  hPGrp = hPrefGrp->GetGroup(getEntryName().latin1());
  hPGrp->Clear();

  int size = int(count());
  for (int i = 0; i < size; i++)
  {
    char szBuf[200];
    sprintf(szBuf, "Item%d", i);
    hPGrp->SetASCII(szBuf, text(i).latin1());
  }

  hPGrp->SetInt("currentItem", currentItem());
}

QString FCComboBox::getEntryName () const
{
  return FCWidgetPrefs::getEntryName();
}

QString FCComboBox::getParamGrpPath () const
{
  return FCWidgetPrefs::getParamGrpPath();
}

void FCComboBox::setEntryName (QString name)
{
  FCWidgetPrefs::setEntryName(name);
}

void FCComboBox::setParamGrpPath (QString name)
{
  FCWidgetPrefs::setParamGrpPath(name);
}

///////////////////////////////////////////////////////////////////////////////////

FCListBox::FCListBox ( QWidget * parent, const char * name, WFlags f )
: QListBox(parent, name, f), FCWidgetPrefs(name)
{
}

FCListBox::~FCListBox()
{
}

void FCListBox::restorePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot restore!\n");
    return;
  }

  FCParameterGrp::handle  hPGrp = hPrefGrp->GetGroup(getEntryName().latin1());
  std::vector<std::string> items = hPGrp->GetASCIIs("Item");
  
  if (items.size() > 0) 
    clear();

  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    insertItem(it->c_str());

  int item = hPGrp->GetInt("currentItem", currentItem());
  setCurrentItem(item);
}

void FCListBox::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  FCParameterGrp::handle  hPGrp = hPrefGrp->GetGroup(getEntryName().latin1());
  hPGrp->Clear();

  int size = int(count());
  for (int i = 0; i < size; i++)
  {
    char szBuf[200];
    sprintf(szBuf, "Item%d", i);
    hPGrp->SetASCII(szBuf, text(i).latin1());
  }

  hPGrp->SetInt("currentItem", currentItem());
}

QString FCListBox::getEntryName () const
{
  return FCWidgetPrefs::getEntryName();
}

QString FCListBox::getParamGrpPath () const
{
  return FCWidgetPrefs::getParamGrpPath();
}

void FCListBox::setEntryName (QString name)
{
  FCWidgetPrefs::setEntryName(name);
}

void FCListBox::setParamGrpPath (QString name)
{
  FCWidgetPrefs::setParamGrpPath(name);
}

///////////////////////////////////////////////////////////////////////////////////

FCCheckBox::FCCheckBox ( QWidget * parent, const char * name )
: QCheckBox(parent, name), FCWidgetPrefs(name)
{
}

FCCheckBox::~FCCheckBox()
{
}

void FCCheckBox::restorePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot restore!\n");
    return;
  }

  bool enable = hPrefGrp->GetBool(getEntryName().latin1(), isChecked());
  setChecked(enable);
}

void FCCheckBox::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  hPrefGrp->SetBool(getEntryName().latin1(), isChecked());
}

QString FCCheckBox::getEntryName () const
{
  return FCWidgetPrefs::getEntryName();
}

QString FCCheckBox::getParamGrpPath () const
{
  return FCWidgetPrefs::getParamGrpPath();
}

void FCCheckBox::setEntryName (QString name)
{
  FCWidgetPrefs::setEntryName(name);
}

void FCCheckBox::setParamGrpPath (QString name)
{
  FCWidgetPrefs::setParamGrpPath(name);
}

///////////////////////////////////////////////////////////////////////////////////

FCRadioButton::FCRadioButton ( QWidget * parent, const char * name )
: QRadioButton(parent, name), FCWidgetPrefs(name)
{
}

FCRadioButton::~FCRadioButton()
{
}

void FCRadioButton::restorePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot restore!\n");
    return;
  }

  bool enable = hPrefGrp->GetBool(getEntryName().latin1(), isChecked());
  setChecked(enable);
}

void FCRadioButton::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  hPrefGrp->SetBool(getEntryName().latin1(), isChecked());
}

QString FCRadioButton::getEntryName () const
{
  return FCWidgetPrefs::getEntryName();
}

QString FCRadioButton::getParamGrpPath () const
{
  return FCWidgetPrefs::getParamGrpPath();
}

void FCRadioButton::setEntryName (QString name)
{
  FCWidgetPrefs::setEntryName(name);
}

void FCRadioButton::setParamGrpPath (QString name)
{
  FCWidgetPrefs::setParamGrpPath(name);
}

///////////////////////////////////////////////////////////////////////////////////

FCSlider::FCSlider ( QWidget * parent, const char * name )
: QSlider(parent, name), FCWidgetPrefs(name)
{
}

FCSlider::~FCSlider()
{
}

void FCSlider::restorePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot restore!\n");
    return;
  }

  FCParameterGrp::handle hPrefs = hPrefGrp->GetGroup(getEntryName().latin1());
  int o = hPrefs->GetInt("Orientation", orientation());
  setOrientation(Qt::Orientation(o));
  int min = hPrefs->GetInt("MinValue", minValue());
  int max = hPrefs->GetInt("MaxValue", maxValue());
  int val = hPrefs->GetInt("Value", value());
  setMinValue(min);
  setMaxValue(max);
  setValue(val);
}

void FCSlider::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  FCParameterGrp::handle hPrefs = hPrefGrp->GetGroup(getEntryName().latin1());
  hPrefs->SetInt("Orientation", int(orientation()));
  hPrefs->SetInt("MinValue", minValue());
  hPrefs->SetInt("MaxValue", maxValue());
  hPrefs->SetInt("Value", value());
}

QString FCSlider::getEntryName () const
{
  return FCWidgetPrefs::getEntryName();
}

QString FCSlider::getParamGrpPath () const
{
  return FCWidgetPrefs::getParamGrpPath();
}

void FCSlider::setEntryName (QString name)
{
  FCWidgetPrefs::setEntryName(name);
}

void FCSlider::setParamGrpPath (QString name)
{
  FCWidgetPrefs::setParamGrpPath(name);
}

///////////////////////////////////////////////////////////////////////////////////

std::vector<QString> FCActionDrag::actions;

FCActionDrag::FCActionDrag ( QString action, QWidget * dragSource , const char * name  )
: QStoredDrag("FCActionDrag", dragSource, name)
{
  // store the QAction name
  actions.push_back(action);
}

FCActionDrag::~FCActionDrag ()
{
}

bool FCActionDrag::canDecode ( const QMimeSource * e )
{
  return e->provides( "FCActionDrag" );
}

bool FCActionDrag::decode ( const QMimeSource * e, QString&  action )
{
  if (actions.size() > 0)
  {
    action = *actions.begin();
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////////

FCCustomWidget::FCCustomWidget(const char* grp, const char * name)
: FCWidgetPrefs(name, false), _bCanModify(true), _bCanRemovable(true)
{
  init(grp, name);
}

FCCustomWidget::~FCCustomWidget()
{
}

void FCCustomWidget::restorePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot restore!\n");
    return;
  }

  if (hPrefGrp->HasGroup("Items"))
  {
    if (!hPrefGrp->GetGroup("Items")->IsEmpty())
    {
      _clItems = hPrefGrp->GetGroup("Items")->GetASCIIs();
    }
  }
}

void FCCustomWidget::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  int i=0;
  FCParameterGrp::handle  hPGrp = hPrefGrp->GetGroup("Items");
  hPGrp->Clear();
  for (std::vector<std::string>::iterator it = _clItems.begin(); it != _clItems.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getEntryName().latin1(), i);
    hPGrp->SetASCII(szBuf, it->c_str());
  }
}

void FCCustomWidget::init(const char* grp, const char* name)
{
  _clWorkbench = ApplicationWindow::Instance->GetActiveWorkbench();
  setPrefName(_clWorkbench);
  hPrefGrp = getRootParamGrp()->GetGroup("Workbenches")->GetGroup(_clWorkbench.latin1());
  hPrefGrp = hPrefGrp->GetGroup(grp);
  hPrefGrp = hPrefGrp->GetGroup(name);
  hPrefGrp->Attach(this);
}

bool FCCustomWidget::hasCustomItems()
{
  return _clItems.size() > 0;
}

QString FCCustomWidget::getWorkbench()
{
  return _clWorkbench;
}

std::vector<std::string> FCCustomWidget::getItems()
{
  return _clItems;
}

void FCCustomWidget::setItems(const std::vector<std::string>& items)
{
  _clItems = items;
}

void FCCustomWidget::appendItems(FCParameterGrp::handle hGrp, const std::vector<std::string>& items)
{
  if (hGrp.IsNull()) return;

  if (hPrefGrp == hGrp)
  {
    for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
      _clItems.push_back(*it);
  }
  else
  {
    if (_clWbItems.find(hGrp) == _clWbItems.end())
    {
      hGrp->Attach(this);
      _clWbItems[hGrp] = items;
    }
  }
}

void FCCustomWidget::removeItems(FCParameterGrp::handle hGrp, const std::vector<std::string>& items)
{
  if (hGrp.IsNull()) return;

  if (hPrefGrp == hGrp)
  {
    unsigned long startPos=0;
    for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    {
      if (_clItems.size() > items.size())
        startPos = _clItems.size() - items.size();
      std::vector<std::string>::iterator pos;
      pos = std::find(_clItems.begin() + startPos, _clItems.end(), *it);
      if (pos != _clItems.end())
        _clItems.erase(pos);
    }
  }
  else
  {
    WorkbenchItems::iterator it = _clWbItems.find(hGrp);
    if (it != _clWbItems.end())
    {
      hGrp->Detach(this);
      it->second.clear();
      _clWbItems.erase(it);
    }
  }
}

void FCCustomWidget::setRemovable(bool b)
{
  _bCanRemovable = b;
}

bool FCCustomWidget::isRemovable() const
{
  return _bCanRemovable;
}

void FCCustomWidget::setCanModify(bool b)
{
  _bCanModify = b;
}

bool FCCustomWidget::canModify() const
{
  return _bCanModify;
}

void FCCustomWidget::loadXML()
{
  restorePreferences();
}

void FCCustomWidget::saveXML()
{
  savePreferences();
}

///////////////////////////////////////////////////////////////////////////////////

FCToolBar::FCToolBar ( const QString & label, QMainWindow *parent, QWidget *w, bool newLine, 
                       const char * name, WFlags f, const char* type )
: QToolBar(label, parent, w, newLine, name, f), FCCustomWidget(type, name)
{
  // allow drag and drop
  setAcceptDrops(true);
  bSaveColor = false;
}

FCToolBar::FCToolBar ( QMainWindow * parent, const char * name, const char* type )
: QToolBar(parent, name), FCCustomWidget(type, name)
{
  // allow drag and drop
  setAcceptDrops(true);
  bSaveColor = false;
}

FCToolBar::~FCToolBar()
{
}

void FCToolBar::update(FCCommandManager& rclMgr)
{
  if (!canModify())
    return; // no need to read again

  clearAll();
  for (std::vector<std::string>::iterator it = _clItems.begin(); it != _clItems.end(); ++it)
  {
    if (*it == "Separator")
      addSeparator();
    else
      rclMgr.AddTo(it->c_str(), this);
  }

  // get also the commands from other workbenches
  std::vector<std::string> items;
  for (WorkbenchItems::iterator it2 = _clWbItems.begin(); it2 != _clWbItems.end(); ++it2)
  {
    items = it2->second;
    for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it)
    {
      if (*it == "Separator")
        addSeparator();
      else
        rclMgr.AddTo(it->c_str(), this);
    }
  }
}

void FCToolBar::setCanModify(bool b)
{
  _bCanModify = b;
  setAcceptDrops(b);
}

bool FCToolBar::isAllowed(QWidget* w)
{
  if (!w)
    return false; // no valid widget

  // this is extensible of course ;-)
  //
  if (w->inherits("QToolButton") )
    return true;
  else if (w->inherits("QComboBox"))
    return true;
  else if (w->inherits("QSpinBox"))
    return true;
  else if (w->inherits("QLineEdit"))
    return true;
  else if (w->inherits("QToolBarSeparator"))
    return true;
  else if (w->inherits("QHBox")) // drop down buttons
    return true;

  // all other types of widget are not allowed
  return false;
}

void FCToolBar::clearAll()
{
  clear();
}

void FCToolBar::dropEvent ( QDropEvent * e)
{
  // store all widgets in STL vector 
  // because of the familiar handling ;-)
  std::vector<QWidget*> childs;
  if ( children() )
  {
    QObjectListIt it( *children() );
    QObject * obj;
    while( (obj=it.current()) != 0 ) 
    {
  	  ++it;
	    if ( obj->isWidgetType() )
      {
        // check if widget type is OK
        if (isAllowed((QWidget*)obj))
          childs.push_back((QWidget*)obj);
      }
    }
  }

  // check if the number of found (filtered) widgets 
  // is the same as the number of items
  //
  // different sizes ->just append at the end
  if (childs.size() != _clItems.size())
  {
/*    // create a new button
    //
    FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
    FCCommand* pCom = NULL;
    std::vector<QString> actions = FCActionDrag::actions;
    for (std::vector<QString>::iterator it = actions.begin(); it != actions.end(); ++it)
    {
      pCom = cCmdMgr.GetCommandByName(it->latin1());
      if (pCom != NULL)
      {
        if (pCom->addTo(this))
          _clItems.push_back(it->latin1());
      }
    }
*/
    GetConsole().Log("Cannot drop item(s). The size of found items"
                     " is different to the size of stored items\n");
    FCActionDrag::actions.clear();

    return;
  }

  // assume 0: horizontal
  //        1: vertical
  Qt::Orientation orient;

#if QT_VERSION <= 230
  orient = orientation();
#else
  // Note: orientation() seems to work not properly in Qt 3.x
  QBoxLayout* layout = boxLayout ();
  if (!layout)
    return;

  if (layout->direction() == QBoxLayout::Up || 
      layout->direction() == QBoxLayout::Down)
  {
    // this is vertical
    orient = Qt::Vertical;
  }
  else
  {
    orient = Qt::Horizontal;
  }
#endif

  // find right position for the new widget(s)
  std::vector<std::string> items;
  std::vector<std::string>::iterator it2 = _clItems.begin();
  QPoint pt = e->pos();
  int pos=0;
  for (std::vector<QWidget*>::iterator it = childs.begin(); it != childs.end(); ++it, ++it2)
  {
    if (orient == Qt::Horizontal)
    {
      pos += (*it)->width();
      if (pos >= pt.x())
        break;
      else
        items.push_back(*it2);
    }
    else
    {
      pos += (*it)->height();
      if (pos >= pt.y())
        break;
      else
        items.push_back(*it2);
    }
  }

  // append the dropped items
  std::vector<QString> actions = FCActionDrag::actions;
  for (std::vector<QString>::iterator it3 = actions.begin(); it3 != actions.end(); ++it3)
  {
    items.push_back(it3->latin1());
  }

  FCActionDrag::actions.clear();

  // and now append the rest of the old items
  for (;it2 != _clItems.end(); ++it2)
    items.push_back(*it2);

  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();

  _clItems = items;

  // clear all and rebuild it again 
  //
  update(cCmdMgr);
}

void FCToolBar::dragEnterEvent ( QDragEnterEvent * e)
{
  e->accept(FCActionDrag::canDecode(e));
}

void FCToolBar::dragLeaveEvent ( QDragLeaveEvent * )
{
}

void FCToolBar::dragMoveEvent ( QDragMoveEvent * )
{
}

void FCToolBar::restorePreferences()
{
  FCCustomWidget::restorePreferences();

  if (bSaveColor)
  {
    long def = (200 << 16) | (208 << 8) | 212;
    long col = hPrefGrp->GetInt("Color", def);

    int b = col >> 16;  col -= b << 16;
    int g = col >> 8;   col -= g << 8;
    int r = col;

    QColor color(r, g, b);
    if (color.isValid())
    {
//      setPalette(QPalette(color, color));
//      setBackgroundMode(PaletteBackground);
    }
  }

  if (hPrefGrp->GetBool("visible", true) == false)
    hide();
}

void FCToolBar::savePreferences()
{
  FCCustomWidget::savePreferences();

  if (bSaveColor)
  {
    QColor col = backgroundColor();
 	  long lcol = (col.blue() << 16) | (col.green() << 8) | col.red();
    hPrefGrp->SetInt("Color", lcol);
  }

  hPrefGrp->SetBool("visible",  !isHidden());
}

///////////////////////////////////////////////////////////////////////////////////

FCPopupMenu::FCPopupMenu(QWidget * parent, const char * name, const char* menu )
: QPopupMenu(parent, name), FCCustomWidget("Menus", name), bAllowDrag(false)
{
  hCommonGrp = GetApplication().GetParameterGroupByPath("System parameter:BaseApp/WindowSettings");
	hCommonGrp->Attach(this);
  // allow drag and drop
//  setAcceptDrops(true);
  if (menu)
    this->parent = menu;
}

FCPopupMenu::~FCPopupMenu()
{
  hCommonGrp->Detach(this);
}

void FCPopupMenu::OnChange(FCSubject<const char*> &rCaller, const char * sReason)
{
  if (strcmp(sReason, "AllowDrag") == 0)
  {
    FCParameterGrp& rclGrp = ((FCParameterGrp&)rCaller);
    bAllowDrag = rclGrp.GetBool("AllowDrag", false);
  }
}

void FCPopupMenu::update(FCCommandManager& rclMgr)
{
  if (!canModify())
    return; // no need to read again

  clear();
  for (std::vector<std::string>::iterator it = _clItems.begin(); it != _clItems.end(); ++it)
  {
    if (*it == "Separator")
      insertSeparator();
    else
      rclMgr.AddTo(it->c_str(), this);
  }

  // get also the commands from other workbenches
  std::vector<std::string> items;
  for (WorkbenchItems::iterator it2 = _clWbItems.begin(); it2 != _clWbItems.end(); ++it2)
  {
    items = it2->second;
    for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it)
    {
      if (*it == "Separator")
        insertSeparator();
      else
        rclMgr.AddTo(it->c_str(), this);
    }
  }
}

void FCPopupMenu::dropEvent ( QDropEvent * e)
{
  // create a new button
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  FCCommand* pCom = NULL;

  std::vector<QString> actions = FCActionDrag::actions;
  for (std::vector<QString>::iterator it = actions.begin(); it != actions.end(); ++it)
  {
    pCom = cCmdMgr.GetCommandByName(it->latin1());
    if (pCom != NULL)
    {
      if (pCom->addTo(this))
        _clItems.push_back(it->latin1());
    }
  }

  FCActionDrag::actions.clear();
}

void FCPopupMenu::dragEnterEvent ( QDragEnterEvent * e)
{
  e->accept(/*FCActionDrag::canDecode(e)*/false);
}

void FCPopupMenu::dragLeaveEvent ( QDragLeaveEvent * )
{
}

void FCPopupMenu::dragMoveEvent ( QDragMoveEvent * )
{
}

void FCPopupMenu::mouseMoveEvent ( QMouseEvent * e)
{
  if ( e->state() == LeftButton && bAllowDrag)
  {
    // try to find out the correct menu item
    // in most cases this should work correctly
    //
    //
    int id = idAt(e->pos());
    if (id == -1) 
      return; // out of range

    FCActionDrag::actions.clear();
    
    // get pixmap and text of this item
    QPixmap* pix = pixmap(id);
    QString txt = text(id);

    // find the corresponding command to this item
    const std::map<std::string, FCCommand*>& rclCmds = ApplicationWindow::Instance->GetCommandManager().GetCommands();

    // search item with same text first
    for (std::map<std::string, FCCommand*>::const_iterator it = rclCmds.begin(); it != rclCmds.end(); ++it)
    {
      QAction* a = it->second->GetAction();
      if (a != NULL)
      {
        if ( a->menuText() == txt )
        {
          FCActionDrag *ad = new FCActionDrag( it->second->GetName(), this );

          if (pix)
            ad->setPixmap(QPixmap(*pix),QPoint(8,8));
          ad->dragCopy();
          return;
        }
      }
    }

    // if nothing found search item with similar text
#ifdef FC_OS_LINUX
    for (std::map<std::string, FCCommand*>::const_iterator it = rclCmds.begin(); it != rclCmds.end(); ++it)
#else
    for (it = rclCmds.begin(); it != rclCmds.end(); ++it)
#endif
    {
      QAction* a = it->second->GetAction();
      if (a != NULL)
      {
        // check if menu item starts with txt
        // both strings need not to be equal (because of accelarators)
        if ( txt.startsWith(a->menuText()) )
        {
          FCActionDrag *ad = new FCActionDrag( it->second->GetName(), this );

          if (pix)
            ad->setPixmap(QPixmap(*pix),QPoint(8,8));
          ad->dragCopy();
          return;
        }
      }
    }

    // error
    GetConsole().Warning("No corresponding Action object found\n");
  }
  else
    QPopupMenu::mouseMoveEvent(e);
}

void FCPopupMenu::mouseReleaseEvent( QMouseEvent * e )
{
  if (FCWhatsThis::inWhatsThisMode())
  {
    int id = idAt(mapFromGlobal(QCursor::pos()));
    close();
    FCWhatsThis::leaveWhatsThisMode(whatsThis(id));
  }
  else
    QPopupMenu::mouseReleaseEvent(e);
}

void FCPopupMenu::restorePreferences()
{
  FCCustomWidget::restorePreferences();
  bAllowDrag = hCommonGrp->GetBool("AllowDrag", false);
}

void FCPopupMenu::savePreferences()
{
  FCCustomWidget::savePreferences();
  if (! parent.isEmpty())
    hPrefGrp->SetASCII("Parent Menu", parent.latin1());
  hPrefGrp->SetBool("AllowDrag", bAllowDrag);
}

///////////////////////////////////////////////////////////////////////////////////

struct FCCustomWidgetManagerP
{
  FCCustomWidgetManagerP(FCCommandManager& rclMgr, FCStackBar* pStackBar)
    : _clCmdMgr(rclMgr), _pclStackBar(pStackBar)
  {
  }

  int                                  _iSeparator;
  std::map <std::string,FCPopupMenu*>  _clPopupMenus;
  std::map <FCPopupMenu*,int>          _clPopupID;
  std::map <std::string,FCToolBar*>    _clToolbars;
  std::map <std::string,FCToolBar*>    _clCmdbars;
  std::map <std::string,FCDockWindow*> _clDocWindows;
  FCCommandManager&                    _clCmdMgr;
  FCStackBar*                          _pclStackBar;
};

FCCustomWidgetManager::FCCustomWidgetManager(FCCommandManager& rclMgr, FCStackBar* pStackBar)
{
  d = new FCCustomWidgetManagerP(rclMgr, pStackBar);
}

FCCustomWidgetManager::~FCCustomWidgetManager()
{
  for (std::map <std::string,FCPopupMenu*>::iterator it1 = d->_clPopupMenus.begin(); it1 != d->_clPopupMenus.end(); ++it1)
    it1->second->saveXML();
  for (std::map <std::string,FCToolBar*>::iterator it2 = d->_clToolbars.begin(); it2 != d->_clToolbars.end(); ++it2)
    it2->second->saveXML();
  for (std::map <std::string,FCToolBar*>::iterator it3 = d->_clCmdbars.begin(); it3 != d->_clCmdbars.end(); ++it3)
    it3->second->saveXML();

  delete d;d=NULL;
}

bool FCCustomWidgetManager::loadCustomWidegts(const char* workbench)
{
  std::vector<std::string> dummy;
  bool bFound = false;
  FCParameterGrp::handle hGrp = FCWidgetPrefs::getRootParamGrp();
  hGrp = hGrp->GetGroup("Workbenches")->GetGroup(workbench);

  std::vector<FCParameterGrp::handle> hSubGrps;
  std::vector<FCParameterGrp::handle>::iterator it;
  FCParameterGrp::handle hCmdGrp  = hGrp->GetGroup("Cmdbar");
  FCParameterGrp::handle hToolGrp = hGrp->GetGroup("Toolbars");
  FCParameterGrp::handle hMenuGrp = hGrp->GetGroup("Menus");

  hSubGrps = hCmdGrp->GetGroups();
  bFound |= (hSubGrps.size() > 0);
  for (it = hSubGrps.begin(); it != hSubGrps.end(); ++it)
  {
    addCmdBar((*it)->GetGroupName(), dummy);
  }
  if (d->_clCmdbars.size() > 0)
  {
    if (d->_pclStackBar->showedView() == NULL)
      d->_pclStackBar->showView(d->_clCmdbars.begin()->second);
  }

  hSubGrps = hToolGrp->GetGroups();
  bFound |= (hSubGrps.size() > 0);
  for (it = hSubGrps.begin(); it != hSubGrps.end(); ++it)
  {
    addToolBar((*it)->GetGroupName(), dummy);
  }

  hSubGrps = hMenuGrp->GetGroups();
  bFound |= (hSubGrps.size() > 0);
  for (it = hSubGrps.begin(); it != hSubGrps.end(); ++it)
  {
    if ((*it)->GetASCII("Parent Menu", "nix") != "nix")
      addPopupMenu((*it)->GetGroupName(), dummy, (*it)->GetASCII("Parent Menu").c_str());
    else
      addPopupMenu((*it)->GetGroupName(), dummy);
  }

  return bFound;
}

bool FCCustomWidgetManager::update(const char* workbench)
{
  QString wb = ApplicationWindow::Instance->GetActiveWorkbench();
  std::map <std::string,FCPopupMenu*> clPopupMenus = d->_clPopupMenus;
  for (std::map <std::string,FCPopupMenu*>::iterator it1 = clPopupMenus.begin(); it1 != clPopupMenus.end(); ++it1)
  {
    if (wb != it1->second->getWorkbench())
      delPopupMenu(it1->first.c_str());
  }
  std::map <std::string,FCToolBar*> clToolbars = d->_clToolbars;
  for (std::map <std::string,FCToolBar*>::iterator it2 = clToolbars.begin(); it2 != clToolbars.end(); ++it2)
  {
    if (wb != it2->second->getWorkbench())
      delToolBar(it2->first.c_str());
  }
  std::map <std::string,FCToolBar*> clCmdbars = d->_clCmdbars;
  for (std::map <std::string,FCToolBar*>::iterator it3 = clCmdbars.begin(); it3 != clCmdbars.end(); ++it3)
  {
    if (wb != it3->second->getWorkbench())
      delCmdBar(it3->first.c_str());
  }

  return loadCustomWidegts(workbench);
}

bool FCCustomWidgetManager::update()
{
  // clear the content of all custom widgets and read it from preferences again
  //
  std::map <std::string,FCPopupMenu*> clPopupMenus = d->_clPopupMenus;
  for (std::map <std::string,FCPopupMenu*>::iterator it1 = clPopupMenus.begin(); it1 != clPopupMenus.end(); ++it1)
  {
    it1->second->loadXML();
    it1->second->update(d->_clCmdMgr);
  }
  std::map <std::string,FCToolBar*> clToolbars = d->_clToolbars;
  for (std::map <std::string,FCToolBar*>::iterator it2 = clToolbars.begin(); it2 != clToolbars.end(); ++it2)
  {
    it2->second->loadXML();
    it2->second->update(d->_clCmdMgr);
  }
  std::map <std::string,FCToolBar*> clCmdbars = d->_clCmdbars;
  for (std::map <std::string,FCToolBar*>::iterator it3 = clCmdbars.begin(); it3 != clCmdbars.end(); ++it3)
  {
    it3->second->loadXML();
    it3->second->update(d->_clCmdMgr);
  }

  return true;
}

void FCCustomWidgetManager::addPopupMenu(const std::string& type, const std::vector<std::string>& defIt, const char* parent)
{
  FCPopupMenu* popup = getPopupMenu(type.c_str(), parent);

  // append if these items are from another workbench
  QString oldWb = popup->getWorkbench();
  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  if (oldWb != newWb)
  {
    FCParameterGrp::handle hPrefGrp;
    hPrefGrp = FCWidgetPrefs::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
    hPrefGrp = hPrefGrp->GetGroup("Menus");
    hPrefGrp = hPrefGrp->GetGroup(type.c_str());
    
    popup->appendItems(hPrefGrp, defIt);
    popup->update(d->_clCmdMgr);
    return;
  }

  popup->loadXML();
  if (!popup->hasCustomItems())
  {
    popup->setItems(defIt);
  }

  popup->update(d->_clCmdMgr);
}

FCPopupMenu* FCCustomWidgetManager::getPopupMenu(const char* name, const char* parent)
{
	std::map <std::string,FCPopupMenu*>::iterator It = d->_clPopupMenus.find(name);
	if ( It!=d->_clPopupMenus.end() )
  {
		return It->second;
  }
	else if (parent == 0 || strcmp(parent,"") == 0)
	{
    FCPopupMenu *pcPopup = new FCPopupMenu( ApplicationWindow::Instance, name );
		d->_clPopupMenus[name] = pcPopup;

    // if the help menu should be inserted
    if (strcmp(name, "Help") == 0)
    {
      d->_iSeparator = ApplicationWindow::Instance->menuBar()->insertSeparator();
    }

    int id = ApplicationWindow::Instance->menuBar()->insertItem( QObject::tr(name), pcPopup );
    d->_clPopupID[pcPopup] = id;

    // search for the "Help" menu if inside
    if (strcmp(name, "Help") != 0)
    {
      It = d->_clPopupMenus.find("Help");
      // not found
      if ( It!=d->_clPopupMenus.end())
      {
        // get old id, remove it from the menubar, insert it immediately and
        // set the new id
        ApplicationWindow::Instance->menuBar()->removeItem(d->_iSeparator);
        d->_iSeparator = ApplicationWindow::Instance->menuBar()->insertSeparator();
        id = d->_clPopupID[It->second];
        ApplicationWindow::Instance->menuBar()->removeItem(id);
        id = ApplicationWindow::Instance->menuBar()->insertItem( QObject::tr("Help"), It->second );
        d->_clPopupID[It->second] = id;
      }
    }

		return pcPopup;
	}
  else
  {
    FCPopupMenu *pParent = getPopupMenu(parent);
    FCPopupMenu *pcPopup = new FCPopupMenu( pParent, name, parent );
		d->_clPopupMenus[name] = pcPopup;
    int id = pParent->insertItem(QObject::tr(name), pcPopup);
    d->_clPopupID[pcPopup] = id;
		return pcPopup;
  }
}

std::vector<FCPopupMenu*> FCCustomWidgetManager::getPopupMenus()
{
  std::vector<FCPopupMenu*> aclMenus;
	for (std::map <std::string,FCPopupMenu*>::iterator It = d->_clPopupMenus.begin(); It != d->_clPopupMenus.end(); ++It)
  {
    aclMenus.push_back(It->second);
  }

  return aclMenus;
}

void FCCustomWidgetManager::delPopupMenu(const char* name)
{
	std::map <std::string,FCPopupMenu*>::iterator It = d->_clPopupMenus.find(name);
	if( It!=d->_clPopupMenus.end() )
	{
    if (!It->second->isRemovable())
      return; // cannot be removed

    It->second->saveXML();
    if (strcmp(It->second->parentWidget()->className(), ApplicationWindow::Instance->className()) == 0)
    {
      // remove properly from menubar
      int id = d->_clPopupID[It->second];
      ApplicationWindow::Instance->menuBar()->removeItem(id);

#ifdef FC_DEBUG
    printf("%s removed from menubar\n", name);
#endif
    }
    else if (strcmp(It->second->parentWidget()->className(), It->second->className()) == 0)
    {
      // remove properly from parent menu
      FCPopupMenu* pParent = (FCPopupMenu*)It->second->parentWidget();
      int id = d->_clPopupID[It->second];
      pParent->removeItem(id);

#ifdef FC_DEBUG
    printf("%s removed from parent menu\n", name);
#endif
    }

		delete It->second;
		d->_clPopupMenus.erase(It);
	}
}

void FCCustomWidgetManager::removeMenuItems(const std::string& type, const std::vector<std::string>& items)
{
  FCPopupMenu* popup = getPopupMenu(type.c_str());

  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  FCParameterGrp::handle hPrefGrp;
  hPrefGrp = FCWidgetPrefs::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
  hPrefGrp = hPrefGrp->GetGroup("Menus");
  hPrefGrp = hPrefGrp->GetGroup(type.c_str());

  popup->FCCustomWidget::removeItems(hPrefGrp,items);
  popup->update(d->_clCmdMgr);
}

int FCCustomWidgetManager::countPopupMenus()
{
  return int(d->_clPopupMenus.size());
}

void FCCustomWidgetManager::addToolBar(const std::string& type, const std::vector<std::string>& defIt)
{
  FCToolBar* toolbar = getToolBar(type.c_str());

  // append if these items are from another workbench
  QString oldWb = toolbar->getWorkbench();
  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  if (oldWb != newWb)
  {
    FCParameterGrp::handle hPrefGrp;
    hPrefGrp = FCWidgetPrefs::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
    hPrefGrp = hPrefGrp->GetGroup("Toolbars");
    hPrefGrp = hPrefGrp->GetGroup(type.c_str());
    
    toolbar->appendItems(hPrefGrp, defIt);
    toolbar->update(d->_clCmdMgr);
    return;
  }

  toolbar->loadXML();
  if (!toolbar->hasCustomItems())
  {
    toolbar->setItems(defIt);
  }

  toolbar->update(d->_clCmdMgr);
}

FCToolBar* FCCustomWidgetManager::getToolBar(const char* name)
{
	std::map <std::string,FCToolBar*>::iterator It = d->_clToolbars.find(name);
	if( It!=d->_clToolbars.end() )
		return It->second;
	else
	{
    FCToolBar *pcToolBar = new FCToolBar( ApplicationWindow::Instance, name );
    ApplicationWindow::Instance->addToolBar(pcToolBar);
		d->_clToolbars[name] = pcToolBar;
		pcToolBar->show();
		return pcToolBar;
	}
}

std::vector<FCToolBar*> FCCustomWidgetManager::getToolBars()
{
  std::vector<FCToolBar*> aclToolbars;
	for (std::map <std::string,FCToolBar*>::iterator It = d->_clToolbars.begin(); It != d->_clToolbars.end(); ++It)
  {
    aclToolbars.push_back(It->second);
  }

  return aclToolbars;
}

void FCCustomWidgetManager::delToolBar(const char* name)
{
	std::map <std::string,FCToolBar*>::iterator It = d->_clToolbars.find(name);
	if( It!=d->_clToolbars.end() )
	{
    if (!It->second->isRemovable())
      return; // cannot be removed
    It->second->saveXML();
    ApplicationWindow::Instance->removeToolBar(It->second);
		delete It->second;
		d->_clToolbars.erase(It);
	}
}

void FCCustomWidgetManager::removeToolBarItems(const std::string& type, const std::vector<std::string>& items)
{
  FCToolBar* tb = getToolBar(type.c_str());

  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  FCParameterGrp::handle hPrefGrp;
  hPrefGrp = FCWidgetPrefs::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
  hPrefGrp = hPrefGrp->GetGroup("Toolbars");
  hPrefGrp = hPrefGrp->GetGroup(type.c_str());

  tb->FCCustomWidget::removeItems(hPrefGrp,items);
  tb->update(d->_clCmdMgr);
}

int FCCustomWidgetManager::countToolBars()
{
  return int(d->_clToolbars.size());
}

void FCCustomWidgetManager::addCmdBar(const std::string& type, const std::vector<std::string>& defIt)
{
  FCToolBar* toolbar = getCmdBar(type.c_str());

  // append if these items are from another workbench
  QString oldWb = toolbar->getWorkbench();
  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  if (oldWb != newWb)
  {
    FCParameterGrp::handle hPrefGrp;
    hPrefGrp = FCWidgetPrefs::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
    hPrefGrp = hPrefGrp->GetGroup("Cmdbar");
    hPrefGrp = hPrefGrp->GetGroup(type.c_str());
    
    toolbar->appendItems(hPrefGrp, defIt);
    toolbar->update(d->_clCmdMgr);
    return;
  }

  toolbar->loadXML();
  if (!toolbar->hasCustomItems())
  {
    toolbar->setItems(defIt);
  }

  toolbar->update(d->_clCmdMgr);
}

FCToolBar* FCCustomWidgetManager::getCmdBar(const char* name)
{
	std::map <std::string,FCToolBar*>::iterator It = d->_clCmdbars.find(name);
	if( It!=d->_clCmdbars.end() )
		return It->second;
	else
	{
    FCToolBar *pcToolBar = new FCToolboxBar( name, d->_pclStackBar, name );
		d->_clCmdbars[name] = pcToolBar;
    d->_pclStackBar->addView(pcToolBar, name);
		return pcToolBar;
	}
}

std::vector<FCToolBar*> FCCustomWidgetManager::getCmdBars()
{
  std::vector<FCToolBar*> aclCmdbars;
	for (std::map <std::string,FCToolBar*>::iterator It = d->_clCmdbars.begin(); It != d->_clCmdbars.end(); ++It)
  {
    aclCmdbars.push_back(It->second);
  }

  return aclCmdbars;
}

void FCCustomWidgetManager::delCmdBar(const char* name)
{
	std::map <std::string,FCToolBar*>::iterator It = d->_clCmdbars.find(name);
	if( It!=d->_clCmdbars.end() )
	{
    if (!It->second->isRemovable())
      return; // cannot be removed
    It->second->saveXML();
    d->_pclStackBar->remView(It->second);
		d->_clCmdbars.erase(It);
	}
}

void FCCustomWidgetManager::removeCmdBarItems(const std::string& type, const std::vector<std::string>& items)
{
  FCToolBar* tb = getCmdBar(type.c_str());

  QString newWb = ApplicationWindow::Instance->GetActiveWorkbench();
  FCParameterGrp::handle hPrefGrp;
  hPrefGrp = FCWidgetPrefs::getRootParamGrp()->GetGroup("Workbenches")->GetGroup(newWb.latin1());
  hPrefGrp = hPrefGrp->GetGroup("Cmdbar");
  hPrefGrp = hPrefGrp->GetGroup(type.c_str());

  tb->FCCustomWidget::removeItems(hPrefGrp,items);
  tb->update(d->_clCmdMgr);
}

int FCCustomWidgetManager::countCmdBars()
{
  return int(d->_clCmdbars.size());
}

void FCCustomWidgetManager::addDockWindow(const char* name,FCDockWindow *pcDocWindow, const char* sCompanion, KDockWidget::DockPosition pos, int percent)
{
  ApplicationWindow* pApp = ApplicationWindow::Instance;
	d->_clDocWindows[name] = pcDocWindow;
	QString str = name;
	str += " dockable window";

  if (sCompanion)
	{
		FCDockWindow* pcWnd = getDockWindow(sCompanion);
		assert(pcWnd);
		pApp->addToolWindow( pcDocWindow, pos, pcWnd, percent, str, name);
	}
  else
  {
		pApp->addToolWindow( pcDocWindow, pos, pApp->m_pMdi, percent, str, name);
  }
}

FCDockWindow* FCCustomWidgetManager::getDockWindow(const char* name)
{
	std::map <std::string,FCDockWindow*>::iterator It = d->_clDocWindows.find(name);

  if (It!=d->_clDocWindows.end())
		return It->second;
	else
		return NULL;
}

std::vector<FCDockWindow*> FCCustomWidgetManager::getDockWindows()
{
  std::vector<FCDockWindow*> dockWindows;
  for (std::map <std::string,FCDockWindow*>::iterator It = d->_clDocWindows.begin(); It!=d->_clDocWindows.end(); ++It)
    dockWindows.push_back(It->second);

  return dockWindows;
}

void FCCustomWidgetManager::delDockWindow(const char* name)
{
	std::map <std::string,FCDockWindow*>::iterator It = d->_clDocWindows.find(name);
	if( It!=d->_clDocWindows.end() )
	{
		delete It->second;
		d->_clDocWindows.erase(It);
	}
}

#include "moc_PrefWidgets.cpp"
