/***************************************************************************
                          Command.cpp  -  description
                             -------------------
    begin                : Tue Jan 2 2002
    copyright            : (C) 2001 by Juergen Riegel
    email                : juergen.riegel@web.de
 ***************************************************************************/



#include "PreCompiled.h"

#ifndef _PreComp_
#	include <string>
#	include <vector>
#	include <qaction.h>
#	include <qbuttongroup.h>
#	include <qcursor.h>
#	include <qobjectlist.h>
#	include <qprocess.h>
#	include <qstatusbar.h>
#	include <qtextbrowser.h>
#	include <qthread.h>
#	include <qtooltip.h>
#	include <qvalidator.h>
#	include <qwhatsthis.h>
#endif

#include "../Base/Console.h"

#include "Command.h"
#include "Application.h"
#include "Document.h"
#include "ButtonGroup.h"
#include "HtmlView.h"
#include "Macro.h"
#include "DlgUndoRedo.h"
#include "BitmapFactory.h"

#include "../Base/Exception.h"
#include "../Base/Interpreter.h"

//===========================================================================
// FCAction 
//===========================================================================
FCAction::FCAction ( FCCommand* pcCmd,QObject * parent, const char * name, bool toggle)
:QAction(parent, name, toggle),_pcCmd(pcCmd)
{
	connect( this, SIGNAL( activated() ) , this, SLOT( Activated() ) );
	connect( this, SIGNAL( toggled(bool) )   , this, SLOT( Toggled(bool) )   );
}

FCAction::~FCAction()
{ 
}

bool FCAction::addTo(QWidget *w)
{
  if (QAction::addTo(w) == true)
  {
    // connect html help with widget
    const QObjectList *l = w->children();
	if(l!=0)
	{
		QObjectListIt it(*l);
		QObject* o = it.toLast();
		if (o && o->isWidgetType())
		{
		  FCWhatsThis::add((QWidget*)o, whatsThis());
		}
	}

    if (w->inherits("FCCommandBar"))
    {
  		((FCCommandBar*)w)->addedButton(menuText());
    }
  }
  else
    return false;

  return true;
}

bool FCAction::removeFrom ( QWidget * w )
{
  QWidget* o;
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!=widgets.end(); ++it)
  {
    if ((*it)->parentWidget() == w)
    {
      o = *it;
      widgets.erase(it);
  		disconnect( o, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
      delete o;
    }
  }

  return QAction::removeFrom(w);
}

void FCAction::slotToolButtonToggled( bool on )
{
  if ( !isToggleAction() )
	  return;
  setOn( on );
}

void FCAction::slotShowStatusText( const QString& text )
{
  QObject* par;
  if ( ( par = parent() ) && par->inherits( "QActionGroup" ) )
	  par = par->parent();

  if ( !par || !par->isWidgetType() )
	  return;

  QObjectList* l = ( (QWidget*)par )->topLevelWidget()->queryList("QStatusBar");
  for ( QStatusBar* bar = (QStatusBar*) l->first(); bar; bar = (QStatusBar*)l->next() ) 
  {
	  if ( text.isEmpty() )
	    bar->clear();
	  else
	    bar->message( text );
  }

  delete l;
}

void FCAction::slotClearStatusText()
{
  slotShowStatusText( QString::null );

}

void FCAction::slotDestroyed()
{
  const QWidget* w = (QWidget*)sender();
  std::vector<QWidget*>::iterator it;
  if ((it = std::find(widgets.begin(), widgets.end(), w)) != widgets.end())
    widgets.erase(it);
}

void FCAction::Activated () 
{
	_pcCmd->activated();
}

void FCAction::Toggled ( bool b)
{
	_pcCmd->toggled(b);
} 

void FCAction::setEnabled ( bool b) 
{
  QAction::setEnabled(b);
  // update all widgets containing this action
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!=widgets.end(); ++it)
  {
    (*it)->setEnabled(b);
  }
}

//===========================================================================
// FCMultiAction 
//===========================================================================
FCMultiAction::FCMultiAction ( FCCommand* pcCmd,QObject * parent, const char * name, bool toggle)
:FCAction(pcCmd, parent, name, toggle)
{
}

FCMultiAction::~FCMultiAction()
{ 
}

bool FCMultiAction::addTo(QWidget *w)
{
  if (w->inherits("QToolBar"))
  {
    QComboBox* combo = new QComboBox(w, "Combo");
    widgets.push_back(combo);
  	connect( combo, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
  	connect( combo, SIGNAL(  activated(int) )   , this, SLOT( activated(int) )   );
    combo->setMinimumWidth(130);
    QPixmap FCIcon = GetBitmapFactory().GetPixmap("FCIcon");
    for (std::vector<std::string>::iterator it = mItems.begin(); it!=mItems.end(); ++it)
    {
      combo->insertItem(FCIcon, it->c_str());
    }

    if (w->inherits("FCCommandBar"))
    {
  		((FCCommandBar*)w)->addedButton(menuText());
    }
  }
  else if (w->inherits("QPopupMenu"))
  {
    QPopupMenu* popup = new QPopupMenu(w, "Menu");
    widgets.push_back(popup);
  	connect( popup, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
    connect( popup, SIGNAL( aboutToShow() ), this, SLOT( onAboutToShow() ));
  	connect( popup, SIGNAL(  activated(int) )   , this, SLOT( activated(int) )   );

//    if (iconSet().isNull())
      ((QPopupMenu*)w)->insertItem(text(), popup);
//    else
//      ((QPopupMenu*)w)->insertItem(iconSet().pixmap(), mName.c_str(), popup);
  }
  else
    return false;

  return true;
}

void FCMultiAction::onAboutToShow()
{
  // fill up just before showing the menu 
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QPopupMenu"))
    {
      QPopupMenu* popup = (QPopupMenu*)(*it);
      popup->clear();

      int i=0;
      for (std::vector<std::string>::iterator it = mItems.begin(); it!=mItems.end(); ++it, i++)
      {
        popup->insertItem(/*QPixmap(FCIcon), */it->c_str(), i);
      }
    }
  }
}

void FCMultiAction::setItems(const std::vector<std::string>& items)
{
  mItems = items;
}

void FCMultiAction::insertItem(const char* item)
{
  mItems.push_back(item);
  QPixmap FCIcon = GetBitmapFactory().GetPixmap("FCIcon");
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QComboBox"))
    {
      QComboBox* combo = (QComboBox*)(*it);
      combo->insertItem(FCIcon, item);
    }
  }
}

void FCMultiAction::removeItem(const char* item)
{
  std::vector<std::string>::iterator ii = std::find(mItems.begin(), mItems.end(), std::string(item));
  if (ii != mItems.end()) mItems.erase(ii);

  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QComboBox"))
    {
      QComboBox* combo = (QComboBox*)(*it);
      for (int i = 0; i<combo->count(); i++)
      {
        if (combo->text(i) == QString(item))
        {
          combo->removeItem(i);
          break;
        }
      }
    }
  }
}

void FCMultiAction::clear()
{
  mItems.clear();
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QComboBox"))
    {
      QComboBox* combo = (QComboBox*)(*it);
      combo->clear();
    }
  }
}

void FCMultiAction::activate(int i)
{
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QComboBox"))
    {
      QComboBox* combo = (QComboBox*)(*it);
      if (combo->currentItem() != i)
        combo->setCurrentItem(i);
    }
  }
}

void FCMultiAction::activate(QString name)
{
  for (std::vector<QWidget*>::iterator it = widgets.begin(); it!= widgets.end(); ++it)
  {
    if ((*it)->inherits("QComboBox"))
    {
      QComboBox* combo = (QComboBox*)(*it);
	    if (combo->currentText() != name)
      {
		    for(int i=0;i<combo->count();i++)
		    {
			    if (combo->text(i) == name)
			    {
				    combo->setCurrentItem(i);
				    break;
			    }
		    }
	    }
    }
  }
}

void FCMultiAction::activated (int i)
{
  GetCommand()->Activated(i);
}

//===========================================================================
// FCUndoAction 
//===========================================================================

bool FCUndoAction::addTo(QWidget* w)
{
  if (w->inherits("QToolBar"))
  {
    QWidget* dlg = ((FCCmdUndo*)GetCommand())->GetWidget();

	  QToolButton* button = new FCToolButtonDropDown((QToolBar*)w, iconSet().pixmap(), dlg);
    button->setToggleButton( isToggleAction() );
    button->setIconSet( iconSet() );

    // do this before the tool tip stuff
    if (w->inherits("FCCommandBar"))
    {
  		((FCCommandBar*)w)->addedButton(menuText());
      button->setTextLabel(menuText());
      button->setUsesTextLabel(true);
    }

    QToolTip::add( button, toolTip(), tipGroup, statusTip() );
    QWhatsThis::add(button, whatsThis());
    FCWhatsThis::add(button, whatsThis());
  	connect( button, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
    connect( button, SIGNAL( clicked() ), this, SIGNAL( activated() ) );
    connect( button, SIGNAL( toggled(bool) ), this, SLOT( slotToolButtonToggled(bool) ) );
	  connect( tipGroup, SIGNAL( showTip(const QString&) ), this, SLOT(slotShowStatusText(const QString&)) );
	  connect( tipGroup, SIGNAL( removeTip() ), this, SLOT(slotClearStatusText()) );
    widgets.push_back(button);

    return true;
  }

  return FCAction::addTo(w);
}

//===========================================================================
// FCRedoAction 
//===========================================================================

bool FCRedoAction::addTo(QWidget* w)
{
  if (w->inherits("QToolBar"))
  {
    QWidget* dlg = ((FCCmdRedo*)GetCommand())->GetWidget();

	  QToolButton* button = new FCToolButtonDropDown((QToolBar*)w, iconSet().pixmap(), dlg);
    button->setToggleButton( isToggleAction() );
    button->setIconSet( iconSet() );

    // do this before the tool tip stuff
    if (w->inherits("FCCommandBar"))
    {
  		((FCCommandBar*)w)->addedButton(menuText());
      button->setTextLabel(menuText());
      button->setUsesTextLabel(true);
    }

    QToolTip::add( button, toolTip(), tipGroup, statusTip() );
    QWhatsThis::add(button, whatsThis());
    FCWhatsThis::add(button, whatsThis());
  	connect( button, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );
    connect( button, SIGNAL( clicked() ), this, SIGNAL( activated() ) );
    connect( button, SIGNAL( toggled(bool) ), this, SLOT( slotToolButtonToggled(bool) ) );
	  connect( tipGroup, SIGNAL( showTip(const QString&) ), this, SLOT(slotShowStatusText(const QString&)) );
	  connect( tipGroup, SIGNAL( removeTip() ), this, SLOT(slotClearStatusText()) );
    widgets.push_back(button);

    return true;
  }

  return FCAction::addTo(w);
}

//===========================================================================
// FCCommand 
//===========================================================================


FCCommand::FCCommand(const char* name,CMD_Type eType)
	:_eType(eType),sName(name),_pcAction(0)
{
	sAppModule		= "";
	sGroup			= "";

}


bool FCCommand::addTo(QWidget *pcWidget)
{
	if (!_pcAction)
		_pcAction = CreateAction();

	return _pcAction->addTo(pcWidget);
}

bool FCCommand::removeFrom(QWidget *pcWidget)
{
	if (!_pcAction)
    return false;
	return _pcAction->removeFrom(pcWidget);
}

 
ApplicationWindow *FCCommand::GetAppWnd(void)
{
	return ApplicationWindow::Instance;
}

FCGuiDocument* FCCommand::GetActiveDocument(void)
{
	return GetAppWnd()->GetActiveDocument();
}

FCDocument*	   FCCommand::GetActiveOCCDocument(void)
{
	FCGuiDocument * pcDoc = GetAppWnd()->GetActiveDocument();
	if(pcDoc)
		return pcDoc->GetDocument();
	else
		return 0l;
}

FCAction* FCCommand::GetAction() 
{ 
	if (!_pcAction)
		_pcAction = CreateAction();

	return _pcAction; 
}

bool FCCommand::IsToggle(void)
{
	return (_eType&Cmd_Toggle) != 0; 
}


void FCCommand::activated ()
{
	if(_eType == Cmd_Normal)
	{
		GetConsole().Log("Activate %s\n",_pcAction->text().latin1());
		Activated(0);
	}
}
void FCCommand::toggled (bool b)
{
	if(_eType == Cmd_Toggle)
	{
		GetConsole().Log("Toggled %s\n",_pcAction->text().latin1());
		if(b)
			Activated(1);
		else
			Activated(0);
	}
}

void FCCommand::TestActive(void)
{
	if(!_pcAction) return;
	
	bool bActive = IsActive();
	_pcAction->setEnabled ( bActive );
}


//--------------------------------------------------------------------------
// UNDO REDO transaction handling  
//--------------------------------------------------------------------------
/** Open a new Undo transaction on the active document
 *  This methode open a new UNDO transaction on the active document. This transaction
 *  will later apear in the UNDO REDO dialog with the name of the command. If the user 
 *  recall the transaction everything changed on the document between OpenCommand() and 
 *  CommitCommand will be undone (or redone). You can use an alternetive name for the 
 *  operation default is the Command name.
 *  @see CommitCommand(),AbortCommand()
 */
void FCCommand::OpenCommand(const char* sCmdName)
{
	// Using OpenCommand with no active document !
	assert(GetAppWnd()->GetActiveDocument());

	if(sCmdName)
		GetAppWnd()->GetActiveDocument()->OpenCommand(sCmdName);
	else
		GetAppWnd()->GetActiveDocument()->OpenCommand(sName.c_str());

}

void FCCommand::CommitCommand(void)
{
	GetAppWnd()->GetActiveDocument()->CommitCommand();
}

void FCCommand::AbortCommand(void)
{
	GetAppWnd()->GetActiveDocument()->AbortCommand();
}

/// Run a App level Action 
void FCCommand::DoCommand(DoCmd_Type eType,const char* sCmd,...)
{
	// temp buffer
	char* format = (char*) malloc(strlen(sCmd)+1024);
    va_list namelessVars;
    va_start(namelessVars, sCmd);  // Get the "..." vars
    vsprintf(format, sCmd, namelessVars);
    va_end(namelessVars);

	if(eType == Gui)
		GetAppWnd()->GetMacroMngr()->AddLine(FCMacroManager::Gui,format);
	else
		GetAppWnd()->GetMacroMngr()->AddLine(FCMacroManager::Base,format);
	GetInterpreter().RunFCCommand(format);

	free (format);

}

/// Activate an other Commands
void FCCommand::ActivateCommand(const char* sCmdName)
{
	FCCommand* pcCmd = GetAppWnd()->GetCommandManager().GetCommandByName(sCmdName);
	if(pcCmd)
	{
		assert(!(pcCmd->IsToggle()));
		pcCmd->Activated(0);
	}

}

/// Toggles other Commands
void FCCommand::ToggleCommand(const char* sCmdName,bool bToggle)
{
	FCCommand* pcCmd = GetAppWnd()->GetCommandManager().GetCommandByName(sCmdName);
	if(pcCmd)
	{
		assert(pcCmd->IsToggle());
		pcCmd->_pcAction->setOn(bToggle?1:0);
	}

}

/// Updates the (active) document (propagate changes)
void FCCommand::UpdateActive(void)
{

	GetAppWnd()->UpdateActive();
}

/// Updates the (all or listed) documents (propagate changes)
void FCCommand::UpdateAll(std::list<FCGuiDocument*> cList)
{
	if(cList.size()>0)
	{
		for(std::list<FCGuiDocument*>::iterator It= cList.begin();It!=cList.end();It++)
			(*It)->Update();
	}else{
		GetAppWnd()->Update();
	}

}

//--------------------------------------------------------------------------
// Online help handling  
//--------------------------------------------------------------------------

/// returns the begin of a online help page
const char * FCCommand::BeginCmdHelp(void)
{
	return  "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n"
			"<html>\n"
			"<head>\n"
			"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">\n"
			"<title>FreeCAD Main Index</title>\n"
			"</head>\n"
			"<body bgcolor=\"#ffffff\">\n\n";
}
/// returns the end of a online help page
const char * FCCommand::EndCmdHelp(void)
{
	return "</body></html>\n\n";
}

//===========================================================================
// FCCppCommand 
//===========================================================================

FCCppCommand::FCCppCommand(const char* name,CMD_Type eType)
	:FCCommand(name,eType)
{
	sMenuText		="Not set!!";
	sToolTipText	="Not set!!";
	sWhatsThis		="Not set!!";
	sStatusTip		="Not set!!";
	sPixmap			=0;
	iAccel			=0;

}

std::string FCCppCommand::GetResource(const char* sName)
{

	return "";

}


FCAction * FCCppCommand::CreateAction(void)
{
	FCAction *pcAction;

	pcAction = new FCAction(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
	pcAction->setText(QObject::tr(sMenuText));
	pcAction->setMenuText(QObject::tr(sMenuText));
	pcAction->setToolTip(QObject::tr(sToolTipText));
	pcAction->setStatusTip(QObject::tr(sStatusTip));
	pcAction->setWhatsThis(QObject::tr(sWhatsThis));
	if(sPixmap)
		pcAction->setIconSet(GetBitmapFactory().GetPixmap(sPixmap));
	pcAction->setAccel(iAccel);

	return pcAction;
}


//===========================================================================
// FCScriptCommand 
//===========================================================================

FCScriptCommand::FCScriptCommand(const char* name)
	:FCCommand(name,Cmd_Normal)
{
	_sMenuText		="Not set!!";
	_sToolTipText	="Not set!!";
	_sWhatsThis		="Not set!!";
	_sStatusTip		="Not set!!";
	_iAccel			=0;
  sAppModule    = "";
  sGroup        = "Macros";
}

std::string FCScriptCommand::GetResource(const char* sName)
{

	return "";

}


FCAction * FCScriptCommand::CreateAction(void)
{
	FCAction *pcAction;

	pcAction = new FCAction(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
	pcAction->setText(QObject::tr(_sMenuText.c_str()));
	pcAction->setMenuText(QObject::tr(_sMenuText.c_str()));
	pcAction->setToolTip(QObject::tr(_sToolTipText.c_str()));
	pcAction->setStatusTip(QObject::tr(_sStatusTip.c_str()));
	pcAction->setWhatsThis(QObject::tr(_sWhatsThis.c_str()));
	if(_sPixmap!="")
		pcAction->setIconSet(GetBitmapFactory().GetPixmap(_sPixmap.c_str()));
	pcAction->setAccel(_iAccel);

	return pcAction;
}


void FCScriptCommand::Activated(int iMsg)
{
//	OpenCommand("Excecute Macro");
//
//	DoCommand(Doc,"execfile(%s)",_sScriptName.c_str());
//
//	void CommitCommand(void);
  std::string cMacroPath = GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Macro/")->GetASCII("MacroPath",GetApplication().GetHomePath());
  ApplicationWindow::Instance->GetMacroMngr()->Run(FCMacroManager::File,(cMacroPath + _sScriptName.c_str()).c_str());
}

std::string FCScriptCommand::CmdHelpURL(void)
{
	return _sHelpURL;
}

void FCScriptCommand::CmdHelpPage(std::string &rcHelpPage)
{
	rcHelpPage += _sHelpPage;
}


//===========================================================================
// FCPythonCommand 
//===========================================================================


FCPythonCommand::FCPythonCommand(const char* name,PyObject * pcPyCommand)
	:FCCommand(name),_pcPyCommand(pcPyCommand)
{
	Py_INCREF(_pcPyCommand);

	// call the methode "GetResources()" of the command object
	_pcPyResourceDict = GetInterpreter().RunMethodObject(_pcPyCommand, "GetResources");
	// check if the "GetResources()" methode returns a Dict object
	if(! PyDict_Check(_pcPyResourceDict) ) 
		throw FCException("FCPythonCommand::FCPythonCommand(): Methode GetResources() of the python command object returns the wrong type (has to be Py Dictonary)");	

}

std::string FCPythonCommand::GetResource(const char* sName)
{
	PyObject* pcTemp;
	PyBuf ResName(sName);


	// get the "MenuText" resource string 
	pcTemp = PyDict_GetItemString(_pcPyResourceDict,ResName.str);
	if(! pcTemp ) 
		return std::string();
	if(! PyString_Check(pcTemp) ) 
		throw FCException("FCPythonCommand::FCPythonCommand(): Methode GetResources() of the python command object returns a dictionary which holds not only strings");

	return std::string(PyString_AsString(pcTemp) );

}


void FCPythonCommand::Activated(int iMsg)
{
	try{
		GetInterpreter().RunMethodVoid(_pcPyCommand, "Activated");
	}catch (FCException e){
		GetConsole().Error("Running the python command %s faild,try to resume",sName);
	}
}

bool FCPythonCommand::IsActive(void)
{
	return true;
}

std::string FCPythonCommand::CmdHelpURL(void)
{
	PyObject* pcTemp;

	pcTemp = GetInterpreter().RunMethodObject(_pcPyCommand, "CmdHelpURL"); 

	if(! pcTemp ) 
		return std::string();
	if(! PyString_Check(pcTemp) ) 
		throw FCException("FCPythonCommand::CmdHelpURL(): Methode CmdHelpURL() of the python command object returns no string");
	
	return std::string( PyString_AsString(pcTemp) );
}

void FCPythonCommand::CmdHelpPage(std::string &rcHelpPage)
{
	PyObject* pcTemp;

	pcTemp = GetInterpreter().RunMethodObject(_pcPyCommand, "CmdHelpPage"); 

	if(! pcTemp ) 
		return ;
	if(! PyString_Check(pcTemp) ) 
		throw FCException("FCPythonCommand::CmdHelpURL(): Methode CmdHelpURL() of the python command object returns no string");
	
	rcHelpPage = PyString_AsString(pcTemp) ;


}

FCAction * FCPythonCommand::CreateAction(void)
{
	FCAction *pcAction;

	pcAction = new FCAction(this,ApplicationWindow::Instance,sName.c_str(),(_eType&Cmd_Toggle) != 0);
	pcAction->setText(sName.c_str());
	pcAction->setMenuText(GetResource("MenuText").c_str());
	pcAction->setToolTip(GetResource("ToolTip").c_str());
	pcAction->setStatusTip(GetResource("StatusTip").c_str());
	pcAction->setWhatsThis(GetResource("WhatsThis").c_str());
	if(GetResource("Pixmap") != "")
		pcAction->setIconSet(GetBitmapFactory().GetPixmap(GetResource("Pixmap").c_str()));

	return pcAction;
}



//===========================================================================
// FCCommandManager 
//===========================================================================


void FCCommandManager::AddCommand(FCCommand* pCom)
{
	_sCommands[pCom->GetName()] = pCom;//	pCom->Init();
}

void FCCommandManager::RemoveCommand(FCCommand* pCom)
{
	std::map <std::string,FCCommand*>::iterator It = _sCommands.find(pCom->GetName());
  if (It != _sCommands.end())
  {
		delete It->second;
		_sCommands.erase(It);
  }
}

void FCCommandManager::AddTo(const char* Name,QWidget *pcWidget)
{
  if (_sCommands.find(Name) == _sCommands.end())
  {
		GetConsole().Error("FCCommandManager::AddTo() try to add an unknown command (%s) to a widget!\n",Name);
  }
  else
  {
	  FCCommand* pCom = _sCommands[Name];
	  pCom->addTo(pcWidget);
  }
}

void FCCommandManager::RemoveFrom(const char* Name,QWidget *pcWidget)
{
  if (_sCommands.find(Name) != _sCommands.end())
  {
	  FCCommand* pCom = _sCommands[Name];
	  pCom->removeFrom(pcWidget);
  }
}

std::vector <FCCommand*> FCCommandManager::GetModuleCommands(const char *sModName)
{
	std::vector <FCCommand*> vCmds;

	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
		if( strcmp(It->second->GetAppModuleName(),sModName) == 0)
			vCmds.push_back(It->second);
	}

	return vCmds;
}

std::string FCCommandManager::GetAppModuleName(QAction* pAction)
{
	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();++It)
	{
    if ( It->second->GetAction() == pAction )
      return It->second->GetAppModuleName();
	}

  return "Not found";
}

std::string FCCommandManager::GetAppModuleNameByName(const char* sName)
{
	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();++It)
	{
    if ( strcmp(It->second->GetName(), sName) == 0 )
      return It->second->GetAppModuleName();
	}

  return "Not found";
}

std::vector <FCCommand*> FCCommandManager::GetAllCommands(void)
{
	std::vector <FCCommand*> vCmds;

	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
		vCmds.push_back(It->second);
	}

	return vCmds;
}

FCCommand* FCCommandManager::GetCommandByName(const char* sName)
{
  FCCommand* pCom = NULL;
  if (_sCommands.find(sName) != _sCommands.end())
  {
    pCom = _sCommands[sName];
  }

  return pCom;
}

FCCommand* FCCommandManager::GetCommandByActionText(const char* sName)
{
	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
    if (It->second->GetAction())
    {
      if (It->second->GetAction()->text() == sName)
      {
        return It->second;
      }
    }
	}

  return NULL;
}

void FCCommandManager::RunCommandByName (const char* sName)
{
  FCCommand* pCmd = GetCommandByName(sName);

  if (pCmd)
    pCmd->activated();
}

void FCCommandManager::TestActive(void)
{
	for( std::map<std::string, FCCommand*>::iterator It= _sCommands.begin();It!=_sCommands.end();It++)
	{
		It->second->TestActive();
	}
}



#include "moc_Command.cpp"

