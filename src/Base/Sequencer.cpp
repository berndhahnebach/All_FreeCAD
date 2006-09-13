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
#include "Console.h"

using namespace Base;

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
    _nInstStarted(0), _nMaxInstStarted(1), _nLastPercentage(-1), _nNewSteps(0)
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
  if ( _nInstStarted > _nMaxInstStarted )
  {
    // if more instances of the sequencer are running then we switch to busy indicator
    // because it's not possible to determine the total number of steps
    nTotalSteps = 0;
    _nMaxInstStarted = _nInstStarted;
    _nNewSteps = steps;

    // reimplemented in sub-classes
    if ( !_bLocked )
      startStep();
  }
  else if (_nInstStarted == 1)
  {
    _nNewSteps = 1000; // every 10 steps call nextStep() later on
    nTotalSteps = steps;
    nProgress = 0;
    _bCanceled = false;

    setText(pszStr);

    // reimplemented in sub-classes
    if ( !_bLocked )
      startStep();

    ret = true;
  }

  return ret;
}

void SequencerBase::startStep()
{
}

bool SequencerBase::next(bool canAbort)
{
  nProgress++;
  unsigned long uDiv = nTotalSteps > 0 ? nTotalSteps : _nNewSteps;
  int perc = nProgress*100 / uDiv;

  // do only an update if we have increased by one percent
  if ( perc > _nLastPercentage )
  {
    _nLastPercentage = perc;
  
    // if not locked
    if ( !_bLocked )
      nextStep(canAbort);
  }

  return nProgress < nTotalSteps;
}

void SequencerBase::nextStep( bool )
{
}

bool SequencerBase::stop()
{
  _nInstStarted--;
  if (_nInstStarted == 0)
  {
    resetData();
  }
  else if ( _nInstStarted < 0)
  {
    // stop() has been called too often 
    _nInstStarted = 0;
#ifdef FC_DEBUG
    Base::Console().Error("Must not call 'SequencerBase::stop()'\n");
#endif
  }

  return (_nInstStarted == 0);
}

void SequencerBase::halt()
{
  if (_nInstStarted != 0)
  {
    _nInstStarted = 0;
    resetData();
  }
}

void SequencerBase::pause()
{
}

void SequencerBase::resume()
{
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

void SequencerBase::rejectCancel()
{
  _bCanceled = false;
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
  _bCanceled = false;
  _nInstStarted = 0;
  _nMaxInstStarted = 1;
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

void ConsoleSequencer::nextStep( bool )
{
  if ( nTotalSteps != 0 )
    printf("\t\t\t\t\t\t(%2.1f %%)\t\r", (float)progressInPercent());
}

void ConsoleSequencer::resetData()
{
  SequencerBase::resetData();
  printf("\t\t\t\t\t\t\t\t\r");
}

// ---------------------------------------------------------

SequencerLauncher::SequencerLauncher(const char* pszStr, unsigned long steps)
{
  SequencerBase::Instance().start( pszStr, steps );
}

SequencerLauncher::~SequencerLauncher()
{
  SequencerBase::Instance().stop();
}

bool SequencerLauncher::next( bool canAbort )
{
  return SequencerBase::Instance().next( canAbort );
}
