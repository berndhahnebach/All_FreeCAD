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
#	include <qstring.h>
#	include <qurl.h>
#	include <ctype.h>
#endif

#include "PrefWidgets.h"
#include "Application.h"
#include "WidgetFactory.h"
#include "ButtonGroup.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"

#include <cmath>
#include <qvalidator.h>
#include <qobjcoll.h>



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

void FCWidgetPrefsHandler::save()
{
  pPref->savePreferences();
  if (pPref->getParamGrp().IsValid())
    pPref->getParamGrp()->Notify(pPref->getEntryName().latin1());
}

void FCWidgetPrefsHandler::restore()
{
  pPref->restorePreferences();
}

///////////////////////////////////////////////////////////////////////////////////

FCEditSpinBox::FCEditSpinBox ( QWidget * parent, const char * name )
: QSpinBox(parent, name), FCWidgetPrefs(name), m_pValidator(NULL), m_iAccuracy(2), m_fDivisor(0), m_fEpsilon(0)
{
  double fMinValue = -10.0f;
  double fMaxValue =  10.0f;
  double fStep     =  0.1f;

  m_pValidator = new QDoubleValidator(fMinValue, fMaxValue, m_iAccuracy+1, this, QString(name) + "FCEditSpinBoxValidator" );
  setValidator(m_pValidator);
  
  m_fDivisor = int(pow(10.0, double(m_iAccuracy)));
  m_fEpsilon = 1.0 / pow(10.0, double(m_iAccuracy+1));
  QSpinBox::setMaxValue(int(fMaxValue * m_fDivisor));
  QSpinBox::setMinValue(int(fMinValue * m_fDivisor));
  QSpinBox::setLineStep(int(fStep     * m_fDivisor));
}

FCEditSpinBox::~FCEditSpinBox() 
{
  delete m_pValidator; 
}

void FCEditSpinBox::restorePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot restore!\n");
    return;
  }

  double fVal;
  if (m_iAccuracy == 0)
    fVal = (double)hPrefGrp->GetInt(getEntryName().latin1(), value());
  else
    fVal = (double)hPrefGrp->GetFloat(getEntryName().latin1(), getValueFloat());

  setValueFloat(fVal);
}

void FCEditSpinBox::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  if (m_iAccuracy == 0)
    hPrefGrp->SetInt(getEntryName().latin1(), (int)getValueFloat());
  else
    hPrefGrp->SetFloat(getEntryName().latin1(), getValueFloat());
}

int FCEditSpinBox::getAccuracy() const
{
  return m_iAccuracy;
}

void FCEditSpinBox::setAccuracy(int i)
{
  m_iAccuracy = i;
  m_fDivisor = int(pow(10.0, double(m_iAccuracy)));
  m_fEpsilon = 1.0 / pow(10.0, double(m_iAccuracy+1));
}

double FCEditSpinBox::getValueFloat() const
{ 
  return QSpinBox::value() / double(m_fDivisor); 
}

void FCEditSpinBox::setMinValueFloat(double value)
{ 
  QSpinBox::setMinValue(int(m_fDivisor * value)); 
}

void FCEditSpinBox::setMaxValueFloat(double value)
{ 
  QSpinBox::setMaxValue(int(m_fDivisor * value)); 
}

double FCEditSpinBox::getMinValueFloat () const
{
  return QSpinBox::minValue() / double(m_fDivisor); 
}

double FCEditSpinBox::getMaxValueFloat () const
{
  return QSpinBox::maxValue() / double(m_fDivisor); 
}

QString FCEditSpinBox::mapValueToText(int value)
{ 
  return QString::number(double(value) / m_fDivisor, 'f', m_iAccuracy); 
}

int FCEditSpinBox::mapTextToValue(bool*)
{ 
  double fEps = value() > 0.0 ? m_fEpsilon : -m_fEpsilon;
  return int(text().toDouble() * m_fDivisor + fEps); 
}

void FCEditSpinBox::valueChange()
{
  QSpinBox::valueChange();
  emit valueChanged( QSpinBox::value() / double(m_fDivisor) );
}

void FCEditSpinBox::setValueFloat(double value)
{ 
  double fEps = value > 0.0 ? m_fEpsilon : -m_fEpsilon;
  QSpinBox::setValue(int(m_fDivisor * value + fEps)); 
}

void FCEditSpinBox::stepChange () 
{
  QSpinBox::stepChange();
}

QString FCEditSpinBox::getEntryName () const
{
  return FCWidgetPrefs::getEntryName();
}

QString FCEditSpinBox::getParamGrpPath () const
{
  return FCWidgetPrefs::getParamGrpPath();
}

void FCEditSpinBox::setEntryName (QString name)
{
  FCWidgetPrefs::setEntryName(name);
}

void FCEditSpinBox::setParamGrpPath (QString name)
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

  clear();
  std::vector<std::string> items = hPrefGrp->GetASCIIs(getEntryName().latin1());
  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    insertItem(it->c_str());

  int item = hPrefGrp->GetInt(getEntryName().latin1(), 0);
  setCurrentItem(item);
}

void FCComboBox::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  for (int i = 0; i < count(); i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getEntryName().latin1(), i);
    hPrefGrp->SetASCII(szBuf, text(i).latin1());
  }

  hPrefGrp->SetInt(getEntryName().latin1(), currentItem());
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

  clear();
  std::vector<std::string> items = hPrefGrp->GetASCIIs(getEntryName().latin1());
  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    insertItem(it->c_str());

  int item = hPrefGrp->GetInt(getEntryName().latin1(), 0);
  setCurrentItem(item);
}

void FCListBox::savePreferences()
{
  if (hPrefGrp.IsNull())
  {
    GetConsole().Warning("Cannot save!\n");
    return;
  }

  int size = int(count());
  for (int i = 0; i < size; i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getEntryName().latin1(), i);
    hPrefGrp->SetASCII(szBuf, item(i)->text().latin1());
  }

  hPrefGrp->SetInt(getEntryName().latin1(), currentItem());
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

  FCParameterGrp::handle hPrefs = hPrefGrp->GetGroup("Settings");
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

  FCParameterGrp::handle hPrefs = hPrefGrp->GetGroup("Settings");
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
: FCWidgetPrefs(name, false)
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
  clearAll();
  for (std::vector<std::string>::iterator it = _clItems.begin(); it != _clItems.end(); ++it)
  {
    if (*it == "Separator")
      addSeparator();
    else
      rclMgr.AddTo(it->c_str(), this);
  }
}

bool FCToolBar::isAllowed(QWidget* w)
{
  if (!w)
    return false; // no valid widget

  // this extensible of course ;-)
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
    // create a new button
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

  // clear all and rebuild it again 
  //
  _clItems.clear();

  clearAll();
  FCCommandManager & cCmdMgr = ApplicationWindow::Instance->GetCommandManager();
  FCCommand* pCom = NULL;
  for (it2 = items.begin(); it2!=items.end(); ++it2)
  {
    pCom = cCmdMgr.GetCommandByName(it2->c_str());
    if (pCom != NULL)
    {
      if (pCom->addTo(this))
        _clItems.push_back(*it2);
    }
  }
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
      setPalette(QPalette(color, color));
      setBackgroundMode(PaletteBackground);
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
: QPopupMenu(parent, name), FCCustomWidget("Menus", name)
{
  // allow drag and drop
//  setAcceptDrops(true);
  if (menu)
    this->parent = menu;
}

FCPopupMenu::~FCPopupMenu()
{
}

void FCPopupMenu::update(FCCommandManager& rclMgr)
{
  clear();
  for (std::vector<std::string>::iterator it = _clItems.begin(); it != _clItems.end(); ++it)
  {
    if (*it == "Separator")
      insertSeparator();
    else
      rclMgr.AddTo(it->c_str(), this);
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
  if ( e->state() == LeftButton )
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

# if QT_VERSION <= 230 // set python command to clipboard (not available yet)
          QApplication::clipboard()->setData(new QTextDrag("Not yet implemented"));
# endif

          if (pix)
            ad->setPixmap(QPixmap(*pix),QPoint(8,8));
          ad->dragCopy();
          return;
        }
      }
    }

    // if nothing found search item with similar text
    for (it = rclCmds.begin(); it != rclCmds.end(); ++it)
    {
      QAction* a = it->second->GetAction();
      if (a != NULL)
      {
        // check if menu item starts with txt
        // both strings need not to be equal (because of accelarators)
        if ( txt.startsWith(a->menuText()) )
        {
          FCActionDrag *ad = new FCActionDrag( it->second->GetName(), this );

# if QT_VERSION <= 230 // set python command to clipboard (not available yet)
          QApplication::clipboard()->setData(new QTextDrag("Not yet implemented"));
# endif

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

void FCPopupMenu::restorePreferences()
{
  FCCustomWidget::restorePreferences();
}

void FCPopupMenu::savePreferences()
{
  FCCustomWidget::savePreferences();
  if (! parent.isEmpty())
    hPrefGrp->SetASCII("Parent Menu", parent.latin1());
}

///////////////////////////////////////////////////////////////////////////////////

FCCustomWidgetManager::FCCustomWidgetManager(FCCommandManager& rclMgr, FCStackBar* pStackBar)
  :_clCmdMgr(rclMgr), _pclStackBar(pStackBar)
{
}

FCCustomWidgetManager::~FCCustomWidgetManager()
{
  for (std::map <std::string,FCPopupMenu*>::iterator it1 = _clPopupMenus.begin(); it1 != _clPopupMenus.end(); ++it1)
    it1->second->saveXML();
  for (std::map <std::string,FCToolBar*>::iterator it2 = _clToolbars.begin(); it2 != _clToolbars.end(); ++it2)
    it2->second->saveXML();
  for (std::map <std::string,FCToolBar*>::iterator it3 = _clCmdbars.begin(); it3 != _clCmdbars.end(); ++it3)
    it3->second->saveXML();
}

bool FCCustomWidgetManager::init(const char* workbench)
{
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
    addCmdBar((*it)->GetGroupName());
  }
  if (_clCmdbars.size() > 0)
  {
    if (_pclStackBar->showedView() == NULL)
      _pclStackBar->showView(_clCmdbars.begin()->second);
  }

  hSubGrps = hToolGrp->GetGroups();
  bFound |= (hSubGrps.size() > 0);
  for (it = hSubGrps.begin(); it != hSubGrps.end(); ++it)
  {
    addToolBar((*it)->GetGroupName());
  }

  hSubGrps = hMenuGrp->GetGroups();
  bFound |= (hSubGrps.size() > 0);
  for (it = hSubGrps.begin(); it != hSubGrps.end(); ++it)
  {
    if ((*it)->GetASCII("Parent Menu", "nix") != "nix")
      addPopupMenu((*it)->GetGroupName(), (*it)->GetASCII("Parent Menu").c_str());
    else
      addPopupMenu((*it)->GetGroupName());
  }

  return bFound;
}

bool FCCustomWidgetManager::update(const char* workbench)
{
  QString wb = ApplicationWindow::Instance->GetActiveWorkbench();
  std::map <std::string,FCPopupMenu*> clPopupMenus = _clPopupMenus;
  for (std::map <std::string,FCPopupMenu*>::iterator it1 = clPopupMenus.begin(); it1 != clPopupMenus.end(); ++it1)
  {
    if (wb != it1->second->getWorkbench() && it1->second->getWorkbench() != "Standard")
      delPopupMenu(it1->first.c_str());
  }
  std::map <std::string,FCToolBar*> clToolbars = _clToolbars;
  for (std::map <std::string,FCToolBar*>::iterator it2 = clToolbars.begin(); it2 != clToolbars.end(); ++it2)
  {
    if (wb != it2->second->getWorkbench() && it2->second->getWorkbench() != "Standard")
      delToolBar(it2->first.c_str());
  }
  std::map <std::string,FCToolBar*> clCmdbars = _clCmdbars;
  for (std::map <std::string,FCToolBar*>::iterator it3 = clCmdbars.begin(); it3 != clCmdbars.end(); ++it3)
  {
    if (wb != it3->second->getWorkbench() && it3->second->getWorkbench() != "Standard")
      delCmdBar(it3->first.c_str());
  }

  return init(workbench);
}

bool FCCustomWidgetManager::update()
{
  // clear the content of all custom widgets and read it from preferences again
  //
  std::map <std::string,FCPopupMenu*> clPopupMenus = _clPopupMenus;
  for (std::map <std::string,FCPopupMenu*>::iterator it1 = clPopupMenus.begin(); it1 != clPopupMenus.end(); ++it1)
  {
    it1->second->loadXML();
    it1->second->update(_clCmdMgr);
  }
  std::map <std::string,FCToolBar*> clToolbars = _clToolbars;
  for (std::map <std::string,FCToolBar*>::iterator it2 = clToolbars.begin(); it2 != clToolbars.end(); ++it2)
  {
    it2->second->loadXML();
    it2->second->update(_clCmdMgr);
  }
  std::map <std::string,FCToolBar*> clCmdbars = _clCmdbars;
  for (std::map <std::string,FCToolBar*>::iterator it3 = clCmdbars.begin(); it3 != clCmdbars.end(); ++it3)
  {
    it3->second->loadXML();
    it3->second->update(_clCmdMgr);
  }

  return true;
}

void FCCustomWidgetManager::addPopupMenu(const std::string& type, const std::vector<std::string>& defIt, const char* parent)
{
  FCPopupMenu* popup = getPopupMenu(type.c_str(), parent);

  popup->loadXML();
  if (!popup->hasCustomItems())
  {
    popup->setItems(defIt);
  }

  popup->update(_clCmdMgr);
}

void FCCustomWidgetManager::addToolBar(const std::string& type, const std::vector<std::string>& defIt)
{
  FCToolBar* toolbar = getToolBar(type.c_str());

  toolbar->loadXML();
  if (!toolbar->hasCustomItems())
  {
    toolbar->setItems(defIt);
  }

  toolbar->update(_clCmdMgr);
}

void FCCustomWidgetManager::addCmdBar(const std::string& type, const std::vector<std::string>& defIt)
{
  FCToolBar* toolbar = getCmdBar(type.c_str());

  toolbar->loadXML();
  if (!toolbar->hasCustomItems())
  {
    toolbar->setItems(defIt);
  }

  toolbar->update(_clCmdMgr);
}

void FCCustomWidgetManager::addItem(const std::string& item)
{
  _clDefaultItems.push_back(item);
}

void FCCustomWidgetManager::addToolBar(const std::string& type)
{
  addToolBar(type, _clDefaultItems);
  _clDefaultItems.clear();
}

void FCCustomWidgetManager::addCmdBar(const std::string& type)
{
  addCmdBar(type, _clDefaultItems);
  _clDefaultItems.clear();
}

void FCCustomWidgetManager::addPopupMenu(const std::string& type, const char* parent)
{
  addPopupMenu(type, _clDefaultItems, parent);
  _clDefaultItems.clear();
}

FCToolBar* FCCustomWidgetManager::getToolBar(const char* name)
{
	std::map <std::string,FCToolBar*>::iterator It = _clToolbars.find(name);
	if( It!=_clToolbars.end() )
		return It->second;
	else
	{
    FCToolBar *pcToolBar = new FCToolBar( ApplicationWindow::Instance, name );
		_clToolbars[name] = pcToolBar;
		pcToolBar->show();
		return pcToolBar;
	}
}

std::vector<FCToolBar*> FCCustomWidgetManager::getToolBars()
{
  std::vector<FCToolBar*> aclToolbars;
	for (std::map <std::string,FCToolBar*>::iterator It = _clToolbars.begin(); It != _clToolbars.end(); ++It)
  {
    aclToolbars.push_back(It->second);
  }

  return aclToolbars;
}

void FCCustomWidgetManager::delToolBar(const char* name)
{
	std::map <std::string,FCToolBar*>::iterator It = _clToolbars.find(name);
	if( It!=_clToolbars.end() )
	{
    It->second->saveXML();
    ApplicationWindow::Instance->removeToolBar(It->second);
		delete It->second;
		_clToolbars.erase(It);
	}
}

int FCCustomWidgetManager::countToolBars()
{
  return int(_clToolbars.size());
}

FCToolBar* FCCustomWidgetManager::getCmdBar(const char* name)
{
	std::map <std::string,FCToolBar*>::iterator It = _clCmdbars.find(name);
	if( It!=_clCmdbars.end() )
		return It->second;
	else
	{
    FCToolBar *pcToolBar = new FCToolboxBar( name, _pclStackBar, name );
		_clCmdbars[name] = pcToolBar;
    _pclStackBar->addView(pcToolBar, name);
		return pcToolBar;
	}
}

std::vector<FCToolBar*> FCCustomWidgetManager::getCmdBars()
{
  std::vector<FCToolBar*> aclCmdbars;
	for (std::map <std::string,FCToolBar*>::iterator It = _clCmdbars.begin(); It != _clCmdbars.end(); ++It)
  {
    aclCmdbars.push_back(It->second);
  }

  return aclCmdbars;
}

void FCCustomWidgetManager::delCmdBar(const char* name)
{
	std::map <std::string,FCToolBar*>::iterator It = _clCmdbars.find(name);
	if( It!=_clCmdbars.end() )
	{
    It->second->saveXML();
    _pclStackBar->remView(It->second);
		_clCmdbars.erase(It);
	}
}

int FCCustomWidgetManager::countCmdBars()
{
  return int(_clCmdbars.size());
}

FCPopupMenu* FCCustomWidgetManager::getPopupMenu(const char* name, const char* parent)
{
	std::map <std::string,FCPopupMenu*>::iterator It = _clPopupMenus.find(name);
	if( It!=_clPopupMenus.end() )
  {
    // insert it again into another popup menu
    if (parent != 0)
    {
      FCPopupMenu *pParent = getPopupMenu(parent);
      pParent->insertItem(name, It->second);
    }

		return It->second;
  }
	else if (parent == 0)
	{
    FCPopupMenu *pcPopup = new FCPopupMenu( ApplicationWindow::Instance, name );
		_clPopupMenus[name] = pcPopup;
    ApplicationWindow::Instance->menuBar()->insertItem( name, pcPopup );
		return pcPopup;
	}
  else
  {
    FCPopupMenu *pParent = getPopupMenu(parent);
    FCPopupMenu *pcPopup = new FCPopupMenu( pParent, name, parent );
		_clPopupMenus[name] = pcPopup;
    pParent->insertItem(name, pcPopup);
		return pcPopup;
  }
}

std::vector<FCPopupMenu*> FCCustomWidgetManager::getPopupMenus()
{
  std::vector<FCPopupMenu*> aclMenus;
	for (std::map <std::string,FCPopupMenu*>::iterator It = _clPopupMenus.begin(); It != _clPopupMenus.end(); ++It)
  {
    aclMenus.push_back(It->second);
  }

  return aclMenus;
}

void FCCustomWidgetManager::delPopupMenu(const char* name)
{
	std::map <std::string,FCPopupMenu*>::iterator It = _clPopupMenus.find(name);
	if( It!=_clPopupMenus.end() )
	{
    It->second->saveXML();
		delete It->second;
		_clPopupMenus.erase(It);
	}
}

int FCCustomWidgetManager::countPopupMenus()
{
  return int(_clPopupMenus.size());
}

FCDockWindow* FCCustomWidgetManager::getDockWindow(const char* name)
{
	std::map <std::string,FCDockWindow*>::iterator It = _clDocWindows.find(name);

  if (It!=_clDocWindows.end())
		return It->second;
	else
		return NULL;
}

std::vector<FCDockWindow*> FCCustomWidgetManager::getDockWindows()
{
  std::vector<FCDockWindow*> dockWindows;
  for (std::map <std::string,FCDockWindow*>::iterator It = _clDocWindows.begin(); It!=_clDocWindows.end(); ++It)
    dockWindows.push_back(It->second);

  return dockWindows;
}

void FCCustomWidgetManager::delDockWindow(const char* name)
{
	std::map <std::string,FCDockWindow*>::iterator It = _clDocWindows.find(name);
	if( It!=_clDocWindows.end() )
	{
		delete It->second;
		_clDocWindows.erase(It);
	}
}

void FCCustomWidgetManager::addDockWindow(const char* name,FCDockWindow *pcDocWindow, const char* sCompanion, KDockWidget::DockPosition pos, int percent)
{
  ApplicationWindow* pApp = ApplicationWindow::Instance;
	_clDocWindows[name] = pcDocWindow;
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

#include "moc_PrefWidgets.cpp"
