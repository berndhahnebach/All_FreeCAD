/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
# include <cstdio>
# include <algorithm>
#endif

#include "Sequencer.h"
#include "Console.h"

using namespace Base;

/**
 * The _pclSequencer member just stores the pointer of the
 * all instanciated SequencerBase objects.
 */
std::vector<SequencerBase*> SequencerBase::_aclInstances;

SequencerLauncher* SequencerBase::_topLauncher = 0;

SequencerBase& SequencerBase::Instance ()
{
    // not initialized?
    if (_aclInstances.size() == 0) {
        new ConsoleSequencer();
    }

    return *_aclInstances.back();
}

SequencerBase::SequencerBase()
  : nProgress(0), nTotalSteps(0), _bLocked(false), _bCanceled(false), _nLastPercentage(-1)
{
    _setGlobalInstance();
}

SequencerBase::~SequencerBase()
{
    std::vector<SequencerBase*>::iterator it;
    it = std::find(_aclInstances.begin(), _aclInstances.end(), this);
    _aclInstances.erase(it);
}

void SequencerBase::_setGlobalInstance ()
{
    _aclInstances.push_back(this);
}

bool SequencerBase::start(const char* pszStr, size_t steps)
{
    // we have already an instance of SequencerLauncher created
    if (SequencerBase::_topLauncher)
        return false;

    // reset current state of progress (in percent)
    this->_nLastPercentage = -1;

    this->nTotalSteps = steps;
    this->nProgress = 0;
    this->_bCanceled = false;

    setText(pszStr);

    // reimplemented in sub-classes
    if (!this->_bLocked)
        startStep();

    return true;
}

size_t SequencerBase::numberOfSteps() const
{
    return this->nTotalSteps;
}

void SequencerBase::startStep()
{
}

bool SequencerBase::next(bool canAbort)
{
    this->nProgress++;
    float fDiv = this->nTotalSteps > 0 ? (float)this->nTotalSteps : 1000.0f;
    int perc = (int)((float)this->nProgress * (100.0f / fDiv));

    // do only an update if we have increased by one percent
    if (perc > this->_nLastPercentage) {
        this->_nLastPercentage = perc;

        // if not locked
        if (!this->_bLocked)
            nextStep(canAbort);
    }

    return this->nProgress < this->nTotalSteps;
}

void SequencerBase::nextStep( bool )
{
}

void SequencerBase::setProgress(size_t)
{
}

bool SequencerBase::stop()
{
    if (SequencerBase::_topLauncher)
        return false;
    resetData();
    return true;
}

void SequencerBase::pause()
{
}

void SequencerBase::resume()
{
}

bool SequencerBase::isBlocking() const
{
    return true;
}

bool SequencerBase::setLocked(bool bLocked)
{
    bool old = this->_bLocked;
    this->_bLocked = bLocked;
    return old;
}

bool SequencerBase::isLocked() const
{
    return this->_bLocked;
}

bool SequencerBase::isRunning() const
{
    return (SequencerBase::_topLauncher != 0);
}

bool SequencerBase::wasCanceled() const
{
    return this->_bCanceled;
}

void SequencerBase::tryToCancel()
{
    this->_bCanceled = true;
}

void SequencerBase::rejectCancel()
{
    this->_bCanceled = false;
}

int SequencerBase::progressInPercent() const
{
    return this->_nLastPercentage;
}

void SequencerBase::resetData()
{
    this->_bCanceled = false;
}

void SequencerBase::setText(const char*)
{
}

// ---------------------------------------------------------

EmptySequencer::EmptySequencer()
{
}

EmptySequencer::~EmptySequencer()
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
    if (this->nTotalSteps != 0)
        printf("\t\t\t\t\t\t(%2.1f %%)\t\r", (float)progressInPercent());
}

void ConsoleSequencer::resetData()
{
    SequencerBase::resetData();
    printf("\t\t\t\t\t\t\t\t\r");
}

// ---------------------------------------------------------

SequencerLauncher::SequencerLauncher(const char* pszStr, size_t steps)
{
    SequencerBase::Instance().start(pszStr, steps);
    if (!SequencerBase::_topLauncher)
        SequencerBase::_topLauncher = this;
}

SequencerLauncher::~SequencerLauncher()
{
    if (SequencerBase::_topLauncher == this)
        SequencerBase::_topLauncher = 0;
    SequencerBase::Instance().stop();
}

void SequencerLauncher::setText (const char* pszTxt)
{
    SequencerBase::Instance().setText(pszTxt);
}

bool SequencerLauncher::next(bool canAbort)
{
    if (SequencerBase::_topLauncher != this)
        return true; // ignore
    return SequencerBase::Instance().next(canAbort);
}

void SequencerLauncher::setProgress(size_t pos)
{
    SequencerBase::Instance().setProgress(pos);
}

size_t SequencerLauncher::numberOfSteps() const
{
    return SequencerBase::Instance().numberOfSteps();
}

bool SequencerLauncher::wasCanceled() const
{
    return SequencerBase::Instance().wasCanceled();
}
