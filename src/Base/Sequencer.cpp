/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
# include <stdio.h>
#endif

#include "Sequencer.h"
#include "Exception.h"

using Base::SequencerBase;

/**
 * The _pclSequencer member just stores the pointer of the
 * all instanciated SequencerBase objects.
 */
std::vector<SequencerBase*> SequencerBase::_aclInstances;

SequencerBase& SequencerBase::Instance ()
{
  // not initialized?
  if ( _aclInstances.size() == 0 )
  {
    new ConsoleSequencer();
  }

  return *_aclInstances.back();
}

SequencerBase::SequencerBase()
  : nProgress(0), nTotalSteps(0), _bLocked(false), _bCanceled(false), 
    _nInstStarted(0), _nMaxInstStarted(1), _nLastPercentage(-1)
{
  _setGlobalInstance();
}

SequencerBase::~SequencerBase()
{
  std::vector<SequencerBase*>::iterator it;
  it = std::find(_aclInstances.begin(), _aclInstances.end(), this);
  _aclInstances.erase( it );
}

void SequencerBase::_setGlobalInstance ()
{
  _aclInstances.push_back( this );
}

bool SequencerBase::start(const char* pszStr, unsigned long steps)
{
  bool ret = false;

  // reset current state of progress (in percent)
  _nLastPercentage = -1;

  // increment the number of running instances
  _nInstStarted++;

  // several sequencer started
  if (_nInstStarted > _nMaxInstStarted)
  {
    // calculate the number of iterations using Horner scheme
    _aSteps.push_front(steps);
    nTotalSteps = 1;
    for (std::list<unsigned long>::iterator it=_aSteps.begin(); it!=_aSteps.end();++it)
    {
      if ( *it == 0 )
      {
        // one process has undetermined number of steps, so
        // the whole process must be undetermined
        nTotalSteps = 1;
        break;
      }

      nTotalSteps = nTotalSteps * (*it) + 1;
    }

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

    ret = true;
  }

  // reimplemented in sub-classes
  if ( !_bLocked )
    startStep();

  return ret;
}

void SequencerBase::startStep()
{
}

bool SequencerBase::next()
{
  nProgress++;
  int perc = nProgress*100 / nTotalSteps;

  // do only an update if we have increased by one percent
  if ( perc > _nLastPercentage )
  {
    _nLastPercentage = perc;
  
    // if not locked
    if ( !_bLocked )
      nextStep();
  }

  return nProgress < nTotalSteps;
}

void SequencerBase::nextStep()
{
}

bool SequencerBase::stop()
{
  _nInstStarted--;
  if (_nInstStarted == 0)
  {
    resetData();
  }

  return (_nInstStarted == 0);
}

void SequencerBase::setLocked( bool bLocked )
{
  _bLocked = bLocked;
}

bool SequencerBase::isLocked() const
{
  return _bLocked;
}

bool SequencerBase::isRunning() const
{
  return _nInstStarted > 0;
}

bool SequencerBase::wasCanceled() const
{
  return _bCanceled;
}

void SequencerBase::tryToCancel()
{
  _bCanceled = true;
}

int SequencerBase::pendingOperations() const
{
  return _nInstStarted;
}

int SequencerBase::progressInPercent() const
{
  return _nLastPercentage;
}

void SequencerBase::resetData()
{
  _nInstStarted = 0;
  _nMaxInstStarted = 1;
  _aSteps.clear();
}

void SequencerBase::setText(const char*)
{
}

// ---------------------------------------------------------

using Base::ConsoleSequencer;

ConsoleSequencer::ConsoleSequencer ()
{
}

ConsoleSequencer::~ConsoleSequencer ()
{
}

void ConsoleSequencer::setText (const char* pszTxt)
{
  printf("%s...\n", pszTxt);
}

void ConsoleSequencer::startStep()
{
}

void ConsoleSequencer::nextStep()
{
  printf("\t\t\t\t\t\t(%2.1f %%)\t\r", (float)progressInPercent());
}

void ConsoleSequencer::resetData()
{
  SequencerBase::resetData();
  printf("\t\t\t\t\t\t\t\t\r");
}

