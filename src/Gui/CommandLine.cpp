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




#include "CommandLine.h"
#include "Application.h"


////////////////////////////////////////////////////////////////////
/** The FCCommandLine class
 */
FCCommandLine::FCCommandLine(void)
: QLineEdit(NULL)
{
  setFixedWidth(400);
  connect(this, SIGNAL(returnPressed ()), this, SLOT(clear()));
}

FCCommandLine::~FCCommandLine(void)
{
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

void FCCommandLine::ScrollUp()
{
  if (_alCmdList.size() > 0)
  {
    if (_TIterator != _alCmdList.begin())
    {
      _TIterator--;
      setText(_TIterator->c_str());
    }
  }
}

void FCCommandLine::ScrollDown()
{
  if (_alCmdList.size() > 0)
  {
    if (_TIterator != _alCmdList.end())
    {
      _TIterator++;
      if (_TIterator != _alCmdList.end())
      {
        setText(_TIterator->c_str());
      }
      else
      {
        clear();
      }
    }
    else
    {
      clear();
    }
  }
}

void FCCommandLine::wheelEvent ( QWheelEvent * e )
{
  if (e->delta() > 0)
  {
    ScrollUp();
  }
  else if (e->delta() < 0)
  {
    ScrollDown();
  }
}

void FCCommandLine::keyPressEvent ( QKeyEvent * e )
{
  switch (e->key())
  {
    case Key_Up:
    {
      ScrollUp();
      break;
    }
    case Key_Down:
    {
      ScrollDown();
      break;
    }
    case Key_Return:
    case Key_Enter:
    {
      stlport::string txt = text().latin1();
      if (txt != "")
      {
        if (txt == "clear")
          _alCmdList.clear();
        else
          _alCmdList.push_back(txt);

        _TIterator = _alCmdList.end();
      }

      QLineEdit::keyPressEvent(e);
      break;
    }
    case 0: // this is the "^"-key
    {
      if (_alCmdList.size() > 0)
      {
        PopupCmdList();
      }
      break;
    }
    default:
      QLineEdit::keyPressEvent(e);
  }
}

void FCCommandLine::mouseDoubleClickEvent ( QMouseEvent * e )
{
  if (e->button() == MidButton)
    PopupCmdList();
  else
    QLineEdit::mouseDoubleClickEvent(e);
}

void FCCommandLine::PopupCmdList()
{
  QListBox* box = new FCCommandListBox(NULL, "", WStyle_Tool|WStyle_StaysOnTop|WType_Popup);
  box->setCurrentItem(box->topItem());

  connect(box, SIGNAL(highlighted(const QString&)), this, SLOT(setText(const QString&)));
  connect(box, SIGNAL(returnPressed ( QListBoxItem * )), this, SLOT(SetCmdText(QListBoxItem *)));

  // insert the commands
  for (stlport::list<stlport::string>::iterator it = _alCmdList.begin(); it != _alCmdList.end(); ++it)
    box->insertItem(it->c_str());

  // set the sizes adapted to the number of elements
  box->setMinimumWidth(width());
  int iHeight = 0;
  int iItemHeight = box->itemHeight();

  if (iItemHeight > 0)
    iHeight = iItemHeight * (box->count() +1);
  else
    iHeight = width();

  iHeight = iHeight < width() ? iHeight : width();

//  box->setMinimumHeight(width());
  box->setMinimumHeight(iHeight);
  box->setMaximumHeight(iHeight);

  box->move(mapToGlobal(QPoint(0,-box->height())));

  box->setMinimumWidth(width());
//  box->setMinimumHeight(width());
  box->setMinimumHeight(iHeight);
  box->setMaximumHeight(iHeight);

  // set the focus to box (important for copy)
  box->setFocus();

  // show the widgets
  box->show();
}

void FCCommandLine::SetCmdText( QListBoxItem * item)
{
  if (item != NULL)
    setText(item->text());
  else
    QMessageBox::critical(this, "Error", "bla bla bla");

  setFocus();
}

////////////////////////////////////////////////////////////////////
/** The FCCommandListBox class
 */
FCCommandListBox::FCCommandListBox( QWidget * parent, const char * name, WFlags f)
: QListBox(parent, name, f)
{
  connect(this, SIGNAL(returnPressed ( QListBoxItem * )), this, SLOT(Close()));
  connect(this, SIGNAL(doubleClicked ( QListBoxItem * )), this, SLOT(Close()));
}

void FCCommandListBox::Close()
{
  close();
}

void FCCommandListBox::keyPressEvent ( QKeyEvent * e )
{
  switch (e->key())
  {
    case 0: // this is the "^"-key
      Close();
      break;
    default:
      QListBox::keyPressEvent(e);
      break;
  }
}

#include "CommandLine_moc.cpp"
