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



#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
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



FCWidgetPrefs::FCWidgetPrefs(const char * name, bool bAttach) : pHandler(NULL)
{
  m_sPrefName = "_____default_____";

  if (name)
    m_sPrefName = name;

  if (bAttach)
  {
    setUserParameter();
    if (hPrefGrp.IsValid())
    {
      hPrefGrp->Attach(this);
    }

    // install a handler for automation stuff
    pHandler = new FCWidgetPrefsHandler(this);
  }
}

FCWidgetPrefs::~FCWidgetPrefs()
{
  hPrefGrp->Detach(this);
#ifdef FC_DEBUG
  if (m_sPrefName == "_____default_____")
  {
    GetConsole().Warning("Wrong widget name!. It must not be \"_____default_____\"\n");
    throw;
  }
#endif
}

void FCWidgetPrefs::setPrefName(QString pref)
{
  m_sPrefName = pref; 
}

QString FCWidgetPrefs::getPrefName()
{
  return m_sPrefName; 
}

void FCWidgetPrefs::setUserParameter()
{
  if (m_sPrefName == "_____default_____")
    printf("%s is not a valid name\n", m_sPrefName.latin1());
  hPrefGrp = getRootParamGrp();
  hPrefGrp = hPrefGrp->GetGroup(m_sPrefName.latin1());
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

void FCWidgetPrefs::OnChange(FCSubject &rCaller)
{
  restorePreferences();
}

FCParameterGrp::handle FCWidgetPrefs::getRootParamGrp()
{
  return GetApplication().GetUserParameter().GetGroup("BaseApp")->GetGroup("Windows")->GetGroup("Widget Preferences");
}

///////////////////////////////////////////////////////////////////////////////////

FCWidgetPrefsHandler::FCWidgetPrefsHandler(FCWidgetPrefs* p) : pPref(p)
{
}

void FCWidgetPrefsHandler::save()
{
  pPref->savePreferences();
  pPref->getParamGrp()->Notify();
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
  double fVal;
  if (m_iAccuracy == 0)
    fVal = (double)hPrefGrp->GetInt(getPrefName().latin1(), 0);
  else
    fVal = (double)hPrefGrp->GetFloat(getPrefName().latin1(), 0.0f);

  setValueFloat(fVal);
}

void FCEditSpinBox::savePreferences()
{
  if (m_iAccuracy == 0)
    hPrefGrp->SetInt(getPrefName().latin1(), (int)getValueFloat());
  else
    hPrefGrp->SetFloat(getPrefName().latin1(), getValueFloat());
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
  std::string text = hPrefGrp->GetASCII(getPrefName().latin1(), "");
  setText(text.c_str());
}

void FCLineEdit::savePreferences()
{
  hPrefGrp->SetASCII(getPrefName().latin1(), text().latin1());
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
  clear();
  std::vector<std::string> items = hPrefGrp->GetASCIIs(getPrefName().latin1());
  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    insertItem(it->c_str());

  int item = hPrefGrp->GetInt(getPrefName().latin1(), 0);
  setCurrentItem(item);
}

void FCComboBox::savePreferences()
{
  for (int i = 0; i < count(); i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getPrefName().latin1(), i);
    hPrefGrp->SetASCII(szBuf, text(i).latin1());
  }

  hPrefGrp->SetInt(getPrefName().latin1(), currentItem());
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
  clear();
  std::vector<std::string> items = hPrefGrp->GetASCIIs(getPrefName().latin1());
  for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it)
    insertItem(it->c_str());

  int item = hPrefGrp->GetInt(getPrefName().latin1(), 0);
  setCurrentItem(item);
}

void FCListBox::savePreferences()
{
  int size = int(count());
  for (int i = 0; i < size; i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getPrefName().latin1(), i);
    hPrefGrp->SetASCII(szBuf, item(i)->text().latin1());
  }

  hPrefGrp->SetInt(getPrefName().latin1(), currentItem());
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
  bool enable = hPrefGrp->GetBool(getPrefName().latin1(), false);
  setChecked(enable);
}

void FCCheckBox::savePreferences()
{
  hPrefGrp->SetBool(getPrefName().latin1(), isChecked());
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
  bool enable = hPrefGrp->GetBool(getPrefName().latin1(), false);
  setChecked(enable);
}

void FCRadioButton::savePreferences()
{
  hPrefGrp->SetBool(getPrefName().latin1(), isChecked());
}

///////////////////////////////////////////////////////////////////////////////////

std::vector<QAction*> FCActionDrag::actions;

FCActionDrag::FCActionDrag ( QAction* action, QWidget * dragSource , const char * name  )
: QStoredDrag("FCActionDrag", dragSource, name)
{
  // store the QAction object
  actions.push_back(action);
}

FCActionDrag::~FCActionDrag ()
{
}

bool FCActionDrag::canDecode ( const QMimeSource * e )
{
  return e->provides( "FCActionDrag" );
}

bool FCActionDrag::decode ( const QMimeSource * e, QAction*&  a )
{
  if (actions.size() > 0)
  {
    a = *actions.begin();
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
  int i=0;
  FCParameterGrp::handle  hPGrp = hPrefGrp->GetGroup("Items");
  hPGrp->Clear();
  for (std::vector<std::string>::iterator it = _clItems.begin(); it != _clItems.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "%s%d", getPrefName().latin1(), i);
    hPGrp->SetASCII(szBuf, it->c_str());
  }
}

void FCCustomWidget::init(const char* grp, const char* name)
{
  _clWorkbench = ApplicationWindow::Instance->GetActiveWorkbench();
  setPrefName(ApplicationWindow::Instance->GetActiveWorkbench());
  setUserParameter();
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

# if QT_VERSION <= 230
  FCToolBar::FCToolBar ( const QString & label, QMainWindow *parent, QMainWindow::ToolBarDock pos, 
                         bool newLine, const char * name, const char* type )
  : QToolBar(label, parent, pos, newLine, name), FCCustomWidget(type, name)
  {
    // allow drag and drop
    setAcceptDrops(true);
    bSaveColor = false;
  }
# endif

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
  savePreferences();
}

void FCToolBar::clearAll()
{
  clear();
}

void FCToolBar::dropEvent ( QDropEvent * e)
{
  // store all widgets in STL vector 
  // because of the better handling ;-)
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
        childs.push_back((QWidget*)obj);
      }
    }
  }

  // find right position for the new button
  QPoint pt = e->pos();
  int pos=0;
  for (std::vector<QWidget*>::iterator it = childs.begin(); it != childs.end(); ++it)
  {
    if (orientation () == Qt::Horizontal)
    {
      pos += (*it)->width();
      if (pos >= pt.x())
        break;
    }
    else
    {
      pos += (*it)->height();
      if (pos >= pt.y())
        break;
    }
  }

  // create a new button
  std::vector<QAction*> actions = FCActionDrag::actions;
  for (std::vector<QAction*>::iterator it2 = actions.begin(); it2 != actions.end(); ++it2)
  {
    QAction* pAction = *it2;
    if ( pAction ) 
    {
      pAction->addTo(this);
      _clItems.push_back(pAction->name());
    }
  }
  FCActionDrag::actions.clear();

  // insert the rest of the "old" children after the new button
  for (; it != childs.end(); ++it)
  {
    (*it)->reparent(this, QPoint(0,0));
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
    FCParameterGrp::handle hColorGrp = hPrefGrp->GetGroup("Color");
    int r = hColorGrp->GetInt("red", 212);
    int g = hColorGrp->GetInt("green", 208);
    int b = hColorGrp->GetInt("blue", 200);
    QColor color(r, g, b);
    if (color.isValid())
    {
      setPalette(QPalette(color, color));
      setBackgroundMode(PaletteBackground);
    }
  }
}

void FCToolBar::savePreferences()
{
  FCCustomWidget::savePreferences();

  if (bSaveColor)
  {
    FCParameterGrp::handle hColorGrp = hPrefGrp->GetGroup("Color");
    hColorGrp->SetInt("red", backgroundColor().red());
    hColorGrp->SetInt("green", backgroundColor().green());
    hColorGrp->SetInt("blue", backgroundColor().blue());
  }
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

void FCPopupMenu::dropEvent ( QDropEvent * e)
{
  // create a new button
  std::vector<QAction*> actions = FCActionDrag::actions;
  for (std::vector<QAction*>::iterator it = actions.begin(); it != actions.end(); ++it)
  {
    QAction* pAction = *it;
    if ( pAction ) 
    {
      pAction->addTo(this);
      _clItems.push_back(pAction->name());
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
          FCActionDrag *ad = new FCActionDrag( a, this );
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
          FCActionDrag *ad = new FCActionDrag( a, this );
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

FCCustomWidgetManager::FCCustomWidgetManager(FCCommandManager& rclMgr, FCCmdBar* pCmdBar)
  :_clCmdMgr(rclMgr), _pclCmdBar(pCmdBar)
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
  hGrp = hGrp->GetGroup(workbench);

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
    if (_pclCmdBar->showedView() == NULL)
      _pclCmdBar->showView(_clCmdbars.begin()->second);
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

void FCCustomWidgetManager::addPopupMenu(const std::string& type, const std::vector<std::string>& defIt, const char* parent)
{
  FCPopupMenu* popup = getPopupMenu(type.c_str(), parent);
  std::vector<std::string> items;

  popup->loadXML();
  if (popup->hasCustomItems())
  {
    items = popup->getItems();
    popup->clear();
  }
  else
  {
    items = defIt;
    popup->setItems(items);
  }

  for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it)
  {
    if (*it == "Separator")
      popup->insertSeparator();
    else
      _clCmdMgr.AddTo(it->c_str(), popup);
  }
}

void FCCustomWidgetManager::addToolBar(const std::string& type, const std::vector<std::string>& defIt)
{
  FCToolBar* toolbar = getToolBar(type.c_str());
  std::vector<std::string> items;

  toolbar->loadXML();
  if (toolbar->hasCustomItems())
  {
    items = toolbar->getItems();
    toolbar->clear();
  }
  else
  {
    items = defIt;
    toolbar->setItems(items);
  }

  for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it)
  {
    if (*it == "Separator")
      toolbar->addSeparator();
    else
      _clCmdMgr.AddTo(it->c_str(), toolbar);
  }
}

void FCCustomWidgetManager::addCmdBar(const std::string& type, const std::vector<std::string>& defIt)
{
  FCToolBar* toolbar = getCmdBar(type.c_str());
  std::vector<std::string> items;

  toolbar->loadXML();
  if (toolbar->hasCustomItems())
  {
    items = toolbar->getItems();
    toolbar->clearAll();
  }
  else
  {
    items = defIt;
    toolbar->setItems(items);
  }

  for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it)
  {
    if (*it == "Separator")
      toolbar->addSeparator();
    else
      _clCmdMgr.AddTo(it->c_str(), toolbar);
  }
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

FCToolBar* FCCustomWidgetManager::getCmdBar(const char* name)
{
	std::map <std::string,FCToolBar*>::iterator It = _clCmdbars.find(name);
	if( It!=_clCmdbars.end() )
		return It->second;
	else
	{
    FCToolBar *pcToolBar = new FCToolboxBar( name, _pclCmdBar, name );
		_clCmdbars[name] = pcToolBar;
    _pclCmdBar->addView(pcToolBar, name);
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
    _pclCmdBar->remView(It->second);
		_clCmdbars.erase(It);
	}
}

FCPopupMenu* FCCustomWidgetManager::getPopupMenu(const char* name, const char* parent)
{
	std::map <std::string,FCPopupMenu*>::iterator It = _clPopupMenus.find(name);
	if( It!=_clPopupMenus.end() )
		return It->second;
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

#include "moc_PrefWidgets.cpp"
