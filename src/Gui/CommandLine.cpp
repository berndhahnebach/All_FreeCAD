/***************************************************************************
                          CommandLine.cpp  -  description
                             -------------------
    begin                : 2002/08/06 19:52:20
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief The command line module
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
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
#endif

#include "CommandLine.h"
#include "Application.h"
#include "../Base/Interpreter.h"
#include "../Base/Console.h"
#include "../Base/Exception.h"

////////////////////////////////////////////////////////////////////
/** Validate input
 */
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
/** The FCCommandLine class
 */
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
  FClist<FCstring> alCmdList;
  for (int ii=0; ii < count(); ii++)
    alCmdList.push_back(text(ii).latin1());
  while ( int(alCmdList.size()) > iMaxCnt )
	  alCmdList.erase( alCmdList.begin() );

  long i=0;
  for (FClist<FCstring>::iterator it = alCmdList.begin(); it != alCmdList.end(); ++it, i++)
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
  FCvector<FCstring> cmd = hCmdGrp->GetASCIIs("Command");

  int i=0;
  for (FCvector<FCstring>::iterator it = cmd.begin(); it != cmd.end(); ++it, i++)
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
    FCstring txt = (const_cast<FCException&>(rclE)).what();
    FCstring err = "'" + text(currentItem()) + "' is not defined!";
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

//////////////////////////////////////////////////////////////////////
///** The FCCommandLine class
// */
//FCCommandLine::FCCommandLine(void)
//: QLineEdit(NULL), bOpen(true), FCWindow("command line")
//{
//  ReadCmdList();
//  setFixedWidth(400);
//  connect(this, SIGNAL(returnPressed ()), this, SLOT(slotLaunchCommand()));
//}
//
//FCCommandLine::~FCCommandLine(void)
//{
//  SaveCmdList();
//}
//
//void FCCommandLine::SaveCmdList()
//{
//  FCParameterGrp::handle hCmdGrp = GetParameter()->GetGroup("CommandList");
//
//  int iMaxCnt = hCmdGrp->GetInt("MaxCommands", 20);
//
//  while ( int(_alCmdList.size()) > iMaxCnt )
//	  _alCmdList.erase( _alCmdList.begin() );
//
//  hCmdGrp->SetInt("Commands", _alCmdList.size());
//
//  long i=0;
//  for (FClist<FCstring>::iterator it = _alCmdList.begin(); it != _alCmdList.end(); ++it, i++)
//  {
//    char szBuf[200];
//    sprintf(szBuf, "Command %d", i);
//    hCmdGrp->SetASCII(szBuf, it->c_str());
//  }
//}
//
//void FCCommandLine::ReadCmdList()
//{
//  FCParameterGrp::handle hCmdGrp = GetParameter()->GetGroup("CommandList");
//
//  int iCnt = hCmdGrp->GetInt("Commands");
//
//  for (int i=0; i<iCnt; i++)
//  {
//    char szBuf[200];
//    sprintf(szBuf, "Command %d", i);
//    FCstring cmd = hCmdGrp->GetASCII(szBuf);
//    _alCmdList.push_back(cmd);
//  }
//
//  _TIterator = _alCmdList.end();
//}
//
//void FCCommandLine::slotLaunchCommand()
//{
//  // launch the python command
//  try
//  {
//    GetInterpreter().Launch(text().latin1());
//  }
//  catch (const FCException& rclE)
//  {
//    FCstring txt = (const_cast<FCException&>(rclE)).what();
//    FCstring err = "'" + text() + "' is not defined!";
//    QMessageBox::warning(this, txt.c_str(), err.c_str());
//  }
//  catch (...)
//  {
//    QMessageBox::critical(this, "Error", "A really nesty error occurred");
//  }
//
//  // hold the focus
//  setFocus();
//  // and clear the command line for the next command
//  clear();
//}
//
//void FCCommandLine::slotCut()
//{
//  cut();
//}
//
//void FCCommandLine::slotCopy()
//{
//  copy();
//}
//
//void FCCommandLine::slotPaste()
//{
//  paste();
//}
//
//void FCCommandLine::slotSeleclAll()
//{
//  selectAll();
//}
//
//void FCCommandLine::slotClear()
//{
//  clear();
//}
//
//void FCCommandLine::slotClearConsole()
//{
//  clear();
//  _alCmdList.clear();
//}
//
//void FCCommandLine::slotOpenConsole()
//{
//  PopupCmdList();
//  bOpen = false;
//}
//
//void FCCommandLine::SetParent(QWidget* parent)
//{
//  _pcSingleton->reparent(parent, QPoint());
//}
//
//// Singleton stuff
//FCCommandLine * FCCommandLine::_pcSingleton = 0;
//
//
//void FCCommandLine::Destruct(void)
//{
//	// not initialized or double destruct!
//  assert(_pcSingleton);
//	delete _pcSingleton;
//}
//
//FCCommandLine & FCCommandLine::Instance(void)
//{
//	// not initialized?
//	if(!_pcSingleton)
//	{
//		_pcSingleton = new FCCommandLine;
//	}
//
//  return *_pcSingleton;
//}
//
//void FCCommandLine::ScrollUp()
//{
//  if (_alCmdList.size() > 0)
//  {
//    if (_TIterator != _alCmdList.begin())
//    {
//      _TIterator--;
//      setText(_TIterator->c_str());
//    }
//  }
//}
//
//void FCCommandLine::ScrollDown()
//{
//  if (_alCmdList.size() > 0)
//  {
//    if (_TIterator != _alCmdList.end())
//    {
//      _TIterator++;
//      if (_TIterator != _alCmdList.end())
//      {
//        setText(_TIterator->c_str());
//      }
//      else
//      {
//        clear();
//      }
//    }
//    else
//    {
//      clear();
//    }
//  }
//}
//
//void FCCommandLine::wheelEvent ( QWheelEvent * e )
//{
//  if (e->delta() > 0)
//  {
//    ScrollUp();
//  }
//  else if (e->delta() < 0)
//  {
//    ScrollDown();
//  }
//}
//
//void FCCommandLine::keyPressEvent ( QKeyEvent * e )
//{
//  switch (e->key())
//  {
//    case Key_Up:
//    {
//      ScrollUp();
//      break;
//    }
//    case Key_Down:
//    {
//      ScrollDown();
//      break;
//    }
//    case Key_Menu:
//    {
//      QPopupMenu *pclPopup = CreatePopupMenu();
//      pclPopup->exec(QCursor::pos());
//      break;
//    }
//    case Key_Return:
//    case Key_Enter:
//    {
//      FCstring txt = text().latin1();
//      if (txt != "")
//      {
//        _alCmdList.push_back(txt);
//        _TIterator = _alCmdList.end();
//      }
//
//      QLineEdit::keyPressEvent(e);
//      break;
//    }
//    case 0: // this is the "^"-key
//    {
//      if (bOpen)
//        PopupCmdList();
//      bOpen = true;
//      break;
//    }
//    default:
//      QLineEdit::keyPressEvent(e);
//  }
//}
//
//QPopupMenu* FCCommandLine::CreatePopupMenu()
//{
//  // context menu
//  QPopupMenu* pclPopup = new QPopupMenu(0L);
//  int iCut    = pclPopup->insertItem(tr( "Cu&t" ),       this, SLOT(slotCut()));
//  int iCopy   = pclPopup->insertItem(tr( "&Copy" ),      this, SLOT(slotCopy()));
//  int iPaste  = pclPopup->insertItem(tr( "&Paste" ),     this, SLOT(slotPaste()));
//  pclPopup->insertSeparator (); 
//  int iSelAll = pclPopup->insertItem(tr( "Select All" ), this, SLOT(slotSeleclAll()));
//  pclPopup->insertSeparator (); 
//  int iClear  = pclPopup->insertItem (tr( "Clear" ),     this, SLOT(slotClear())); 
//  int iCls    = pclPopup->insertItem (tr( "Clear Console" ), this, SLOT(slotClearConsole())); 
//  int iOCl    = pclPopup->insertItem (tr( "Open Console" ), this, SLOT(slotOpenConsole())); 
//
//  // enable/disable items
//  bool enableCut   = hasMarkedText() && !isReadOnly();
//  bool enableCopy  = hasMarkedText();
//  bool enablePaste = !isReadOnly() && !QApplication::clipboard()->text().isEmpty();
//  bool enableClear = !isReadOnly() && !text().isEmpty();
//  bool allSelected = text().length() > 0;
//  pclPopup->setItemEnabled(iCut,    enableCut);
//  pclPopup->setItemEnabled(iCopy,   enableCopy);
//  pclPopup->setItemEnabled(iPaste,  enablePaste);
//  pclPopup->setItemEnabled(iSelAll, allSelected);
//  pclPopup->setItemEnabled(iClear,  enableClear);
//  pclPopup->setItemEnabled(iCls,    _alCmdList.size() > 0);
//  pclPopup->setItemEnabled(iOCl,    true);
//
//  return pclPopup;
//}
//
//void FCCommandLine::mousePressEvent ( QMouseEvent * e )
//{
//  if ( e->button() == RightButton )
//  {
//    QPopupMenu *pclPopup = CreatePopupMenu();
//    pclPopup->exec(QCursor::pos());
//  }
//  else
//    QLineEdit::mousePressEvent(e);
//}
//
//void FCCommandLine::PopupCmdList()
//{
//  QListBox* box = new FCCommandListBox(NULL, "", WStyle_Tool|WStyle_StaysOnTop|WType_Popup);
//  box->setCurrentItem(box->topItem());
//
//  connect(box, SIGNAL(highlighted(const QString&)), this, SLOT(setText(const QString&)));
//  connect(box, SIGNAL(returnPressed ( QListBoxItem * )), this, SLOT(SetCmdText(QListBoxItem *)));
//
//  // insert the commands
//  for (FClist<FCstring>::iterator it = _alCmdList.begin(); it != _alCmdList.end(); ++it)
//    box->insertItem(it->c_str());
//
//  box->setMinimumWidth(width());
//  box->setMinimumHeight(width());
//
//  box->move(mapToGlobal(QPoint(0,-box->height())));
//
//  box->setMinimumWidth(width());
//  box->setMinimumHeight(width());
//
//  // set the focus to box (important for copy)
//  box->setFocus();
//
//  // show the widgets
//  box->show();
//}
//
//void FCCommandLine::SetCmdText( QListBoxItem * item)
//{
//  if (item != NULL)
//  {
//    setText(item->text());
//    bOpen = bOpen == true ? false : true;
//  }
//  else
//    QMessageBox::critical(this, "Error", "An unknown error occurred.");
//
//  setFocus();
//}
//
//////////////////////////////////////////////////////////////////////
///** The FCCommandListBox class
// */
//FCCommandListBox::FCCommandListBox( QWidget * parent, const char * name, WFlags f)
//: QListBox(parent, name, f)
//{
//  connect(this, SIGNAL(returnPressed ( QListBoxItem * )), this, SLOT(Close()));
//  connect(this, SIGNAL(doubleClicked ( QListBoxItem * )), this, SLOT(Close()));
//}
//
//void FCCommandListBox::Close()
//{
//  close();
//}
//
//void FCCommandListBox::keyPressEvent ( QKeyEvent * e )
//{
//  switch (e->key())
//  {
//  case 0: // this is the "^"-key
//      Close();
//      break;
//    default:
//      QListBox::keyPressEvent(e);
//      break;
//  }
//}

#include "CommandLine_moc.cpp"
