/***************************************************************************
                          CommandLine.cpp  -  description
                             -------------------
    begin                : 2002/08/06 19:52:20
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file CommandLine.cpp
 *  \brief The command line module
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
#	include <qclipboard.h>
#	include <qscrollbar.h>
#	include <qcombobox.h>
#	include <qmessagebox.h>
#endif

#include "CommandLine.h"
#include "Application.h"
#include "../Base/Interpreter.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"

////////////////////////////////////////////////////////////////////

FCConsoleValidator::FCConsoleValidator ( QWidget * parent, const char * name )
: QValidator(parent, name)
{
}

FCConsoleValidator::~FCConsoleValidator ()
{
}

QValidator::State FCConsoleValidator::validate ( QString & txt, int & i) const
{
  if (txt.isEmpty())
    return Intermediate;
  else
    return Acceptable;
}

void FCConsoleValidator::fixup ( QString & txt) const
{
}

////////////////////////////////////////////////////////////////////

FCCommandLine::FCCommandLine(void)
: QComboBox(true, NULL), FCWindow("command line")
{
  // hide the botton from combo box
  setStyle(new FCWindowsStyle);
  ReadCmdList();
  //setFixedWidth(400);
  setMinimumWidth(200);
  setAutoCompletion ( true );
  setValidator(new FCConsoleValidator(this));
  connect(lineEdit(), SIGNAL(returnPressed ()), this, SLOT(slotLaunchCommand()));
}

FCCommandLine::~FCCommandLine(void)
{
  SaveCmdList();
}

void FCCommandLine::SaveCmdList()
{
  // write the recent commands into file
  FCParameterGrp::handle hCmdGrp = GetWindowParameter()->GetGroup("CommandList");
  int iMaxCnt = hCmdGrp->GetInt("MaxCommands", 20);

  // copy from list box first
  std::list<std::string> alCmdList;
  for (int ii=0; ii < count(); ii++)
    alCmdList.push_back(text(ii).latin1());
  while ( int(alCmdList.size()) > iMaxCnt )
	  alCmdList.erase( alCmdList.begin() );

  long i=0;
  for (std::list<std::string>::iterator it = alCmdList.begin(); it != alCmdList.end(); ++it, i++)
  {
    char szBuf[200];
    sprintf(szBuf, "Command %d", i);
    hCmdGrp->SetASCII(szBuf, it->c_str());
  }
}

void FCCommandLine::ReadCmdList()
{
  // get the recent commands
  FCParameterGrp::handle hCmdGrp = GetWindowParameter()->GetGroup("CommandList");
  std::vector<std::string> cmd = hCmdGrp->GetASCIIs("Command");

  int i=0;
  for (std::vector<std::string>::iterator it = cmd.begin(); it != cmd.end(); ++it, i++)
  {
    try
    {
      insertItem(it->c_str());
    }
    catch(...)
    {
      GetConsole().Error("Invalid entry found\n");
    }
  }

  lineEdit()->clear();
}

void FCCommandLine::slotLaunchCommand()
{
  // launch the python command
  try
  {
    GetInterpreter().Launch(text(currentItem()).latin1());
  }
  catch (const FCException& rclE)
  {
    std::string txt = (const_cast<FCException&>(rclE)).what();
    std::string err = "'" + std::string(text(currentItem())) + "' is not defined!";
    QMessageBox::warning(this, txt.c_str(), err.c_str());
  }
  catch (...)
  {
    QMessageBox::critical(this, "Error", "A really nesty error occurred");
  }

  // hold the focus
  setFocus();
  // and clear the command line for the next command
  lineEdit()->clear();
}

void FCCommandLine::slotClearConsole()
{
  clear();
}

void FCCommandLine::slotOpenConsole()
{
  popup();
}

void FCCommandLine::SetParent(QWidget* parent)
{
  _pcSingleton->reparent(parent, QPoint());
}

// Singleton stuff
FCCommandLine * FCCommandLine::_pcSingleton = 0;


void FCCommandLine::Destruct(void)
{
	// not initialized or double destruct!
  assert(_pcSingleton);
	delete _pcSingleton;
}

FCCommandLine & FCCommandLine::Instance(void)
{
	// not initialized?
	if(!_pcSingleton)
	{
		_pcSingleton = new FCCommandLine;
	}

  return *_pcSingleton;
}

void FCCommandLine::wheelEvent ( QWheelEvent * e )
{
  if (count() < 1) return;
  int delta = e->delta() > 0 ? 1 : -1;
  int iCur = (currentItem() + delta) % count();
  if (iCur < 0)  iCur = count() - 1;
  setCurrentItem(iCur);
}

void FCCommandLine::keyPressEvent ( QKeyEvent * e )
{
  switch (e->key())
  {
    case Key_Menu:
    {
      QPopupMenu *pclPopup = CreatePopupMenu();
      pclPopup->exec(QCursor::pos());
      break;
    }
    case Key_Up:
    {
      // show last command
      if (currentText().isEmpty() && count() > 0)
      {
        int item = currentItem();
        setCurrentItem(item);
        break;
      }
    }
    default:
      QComboBox::keyPressEvent(e);
  }
}

QPopupMenu* FCCommandLine::CreatePopupMenu()
{
  // context menu
  QPopupMenu* pclPopup = new QPopupMenu(0L);
  int iOpen  = pclPopup->insertItem (tr( "Open Console" ), this, SLOT(slotOpenConsole())); 
  int iClear = pclPopup->insertItem (tr( "Clear Console" ), this, SLOT(slotClearConsole())); 
  pclPopup->setItemEnabled(iOpen,  count() > 0);
  pclPopup->setItemEnabled(iClear, count() > 0);

  return pclPopup;
}

void FCCommandLine::mousePressEvent ( QMouseEvent * e )
{
  if ( e->button() == RightButton )
  {
    QPopupMenu *pclPopup = CreatePopupMenu();
    pclPopup->exec(QCursor::pos());
  }
  else
    QComboBox::mousePressEvent(e);
}

void FCCommandLine::enterEvent ( QEvent * e)
{
  setStyle(new QWindowsStyle);
}

void FCCommandLine::leaveEvent ( QEvent * e)
{
  setStyle(new FCWindowsStyle);
}

#include "moc_CommandLine.cpp"
