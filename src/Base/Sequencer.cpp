/** \file Sequencer.cpp
 *  \brief Sequencer base class
 *  \author Werner Mayer
 *  \version 1.3 
 *  \date    2004/10/03 21:57:09
 */

/***************************************************************************
 *   (c) Werner Mayer (werner.wm.mayer@gmx.de) 2000 - 2003                 *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Werner Mayer 2002                                                     *
 ***************************************************************************/

/** \class Base::SequencerBase
 * \brief This class gives the user an indication of the progress of an operation and
 * it is used to reassure them that the application is still running.
 * 
 * Here are some code snippets of how to use the sequencer:
 *  \code
 *
 *  #include <Base/Sequencer.h>
 *
 *  //first example
 *  if (Base::Sequencer().start ("your text", 10))
 *  for (int i=0; i<10; i++)
 *  {
 *    // do something
 *    Base::Sequencer().next ();
 *  }
 *  Base::Sequencer().stop ();
 *
 *  //second example
 *  if (Base::Sequencer().start ("your text", 10))
 *  do
 *  {
 *    // do something
 *  }
 *  while ( Base::Sequencer().next() );
 *  Base::Sequencer().stop ();
 *
 *  \endcode
 *
 * The implementation of this class also supports the interlocking of several running
 * operations at a time. 
 *
 *  \code
 *
 *  //start the first operation
 *  Base::Sequencer().start ("your text", 10)
 *  for (int i=0; i<10, i++)
 *  {
 *    // do something
 *
 *    // start the second operation while the first one is still running
 *    Base::Sequencer().start ("another text", 10);
 *    for (int j=0; j<10; j++)
 *    {
 *      // do something different
 *      Base::Sequencer().next ();
 *    }
 *    Base::Sequencer().stop ();
 *
 *    Base::Sequencer().next ();
 *  }
 *  Base::Sequencer().stop ();
 *
 *  \endcode
 *
 */

#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include "Sequencer.h"
#include "Exception.h"

using Base::SequencerBase;

/**
 * The _pclSequencer member just stores the pointer of the
 * all instanciated SequencerBase objects.
 */
std::vector<SequencerBase*> SequencerBase::_aclInstances;

/**
 * Returns the last created sequencer instance.
 * If you create an instance of a class inheriting SequencerBase
 * this object is retrieved instead. 
 *
 * This mechanism is very useful to have an own sequencer for each layer of FreeCAD. 
 * For example, if FreeCAD is running in server mode you have/need no GUI layer 
 * and therewith no (graphical) progress bar; in this case SequencerBase is taken.
 * But in cases FreeCAD is running with GUI the @ref Gui::ProgressBar is taken instead.
 * @see Sequencer
 */
SequencerBase& SequencerBase::Instance ()
{
  // not initialized?
  if ( _aclInstances.size() == 0 )
  {
    new SequencerBase();
  }

  return *_aclInstances.back();
}

/** Construction */
SequencerBase::SequencerBase()
  : _bCanceled(false), _nInstStarted(0), _nMaxInstStarted(1),
    nProgress(0), nTotalSteps(0)
{
  _setGlobalInstance();
}

/** Destruction */
SequencerBase::~SequencerBase()
{
  std::vector<SequencerBase*>::iterator it;
  it = std::find(_aclInstances.begin(), _aclInstances.end(), this);
  _aclInstances.erase( it );
}

/**
 * Sets a global sequencer object.
 * Access to the last registered
 * object is performed by
 * @see Sequencer().
 */
void SequencerBase::_setGlobalInstance ()
{
  _aclInstances.push_back( this );
}

/**
 * Starts a new operation, returns false if there is already a pending operation,
 * otherwise returns true.
 * If you want to reimplement this method, it is very important to call it at first 
 * ín your own implementatation.
 */
bool SequencerBase::start(const char* pszStr, unsigned long steps)
{
  // increment the number of running instances
  _nInstStarted++;

  // several sequencer started
  if (_nInstStarted > _nMaxInstStarted)
  {
    // calculate the number of iterations using Horner scheme
    _aSteps.push_front(steps);
    nTotalSteps = 1;
    for (std::list<unsigned long>::iterator it=_aSteps.begin(); it!=_aSteps.end();++it)
      nTotalSteps = nTotalSteps * (*it) + 1;
    nTotalSteps -= 1;

    _nMaxInstStarted = _nInstStarted;
  }
  else if (_nInstStarted == 1)
  {
    _aSteps.push_front(steps);
    nTotalSteps = steps;
    nProgress = 0;
    _bCanceled = false;

    setText(pszStr);

    return true;
  }

  return false;
}

/**
 * Performs the next step and returns true if the operation is not yet finished.
 * The default implementation does nothing and returns false.
 */
bool SequencerBase::next()
{
  return false;
}

/**
 * Reduces the number of pending operations by one and stops the
 * sequencer if all operations are finished. It returns false if there are still
 * pending operations, otherwise returns true.
 */
bool SequencerBase::stop()
{
  _nInstStarted--;
  if (_nInstStarted == 0)
  {
    resetData();
  }

  return (_nInstStarted == 0);
}

/** Returns true if the sequencer is running, otherwise returns false. */
bool SequencerBase::isRunning() const
{
  return _nInstStarted > 0;
}

/** 
 * Returns true if the pending operation was canceled. 
 */
bool SequencerBase::wasCanceled() const
{
  return _bCanceled;
}

/** 
 * Try to cancel the pending operation(s). 
 * E.g. @ref Gui::ProgressBar calls this method after the ESC button was pressed.
 */
void SequencerBase::tryToCancel()
{
  _bCanceled = true;
}

/** 
 * Returns the number of pending operations. This number complies with the number
 * of calls of @ref start() and @ref stop().
 */
int SequencerBase::pendingOperations() const
{
  return _nInstStarted;
}

/** 
 * Resets internal data. 
 * If you want to reimplement this method, it is very important to call it ín your code.
 */
void SequencerBase::resetData()
{
  _nInstStarted = 0;
  _nMaxInstStarted = 1;
  _aSteps.clear();
}

/** 
 * Sets a text what the pending operation is doing. The default implementation 
 * does nothing.
 */
void SequencerBase::setText(const char*)
{
}

// ---------------------------------------------------------

using Base::ConsoleSequencer;

/** \class Base::ConsoleSequencer
 * \brief This class writes the progress to the console window.
 */

ConsoleSequencer* ConsoleSequencer::_pclSingleton = 0; 

/**
 * Returns the console sequencer object
 */
ConsoleSequencer* ConsoleSequencer::Instance()
{
  // not initialized?
  if ( !_pclSingleton )
  {
    _pclSingleton = new ConsoleSequencer;
  }

  return _pclSingleton;
}

/** Starts the sequencer */
bool ConsoleSequencer::start(const char* pszStr, unsigned long steps)
{
	// base stuff
	bool ret = SequencerBase::start(pszStr, steps);
  _iLastPercentage = -1;
  return ret;
}

/**
 * Writes the current progress to the console window.
 */
bool ConsoleSequencer::next()
{
  nProgress++;
  int perc = nProgress*100 / nTotalSteps;
  if ( perc > _iLastPercentage )
  {
    _iLastPercentage = perc;
    printf("\t\t\t\t\t\t(%2.1f %%)\t\r", (float)perc);
  }

  return nProgress < nTotalSteps;
}

/** construction */
ConsoleSequencer::ConsoleSequencer ()
{
}

/** Destruction */
ConsoleSequencer::~ConsoleSequencer ()
{
}

/** Puts text to the console window */
void ConsoleSequencer::setText (const char* pszTxt)
{
  printf("%s...\n", pszTxt);
}

/** Resets the sequencer */
void ConsoleSequencer::resetData()
{
  printf("\n");
}

