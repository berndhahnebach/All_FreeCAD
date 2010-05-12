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
# include <QWriteLocker>
# include <QReadLocker>
# include <QReadWriteLock>
#endif

#include "Sequencer.h"
#include "Console.h"

using namespace Base;

namespace Base {
    struct SequencerP {
        // members
        static std::vector<SequencerBase*> _instances; /**< A vector of all created instances */
        static SequencerLauncher* _topLauncher; /**< The outermost launcher */
        static QReadWriteLock rwlock; /**< A mutex-like read/write locker for the launcher */
        /** Sets a global sequencer object.
         * Access to the last registered object is performed by @see Sequencer().
         */
        static void appendInstance (SequencerBase* s)
        {
            _instances.push_back(s);
        }
        static void removeInstance (SequencerBase* s)
        {
            std::vector<SequencerBase*>::iterator it;
            it = std::find(_instances.begin(), _instances.end(), s);
            _instances.erase(it);
        }
        static SequencerBase& getInstance ()
        {
            return *_instances.back();
        }
    };

    /**
     * The _instances member just stores the pointer of the
     * all instanciated SequencerBase objects.
     */
    std::vector<SequencerBase*> SequencerP::_instances;
    SequencerLauncher* SequencerP::_topLauncher = 0;
    QReadWriteLock SequencerP::rwlock/*(QReadWriteLock::Recursive)*/;
};

SequencerBase& SequencerBase::Instance ()
{
    // not initialized?
    if (SequencerP::_instances.size() == 0) {
        new ConsoleSequencer();
    }

    return SequencerP::getInstance();
}

SequencerBase::SequencerBase()
  : nProgress(0), nTotalSteps(0), _bLocked(false), _bCanceled(false), _nLastPercentage(-1)
{
    SequencerP::appendInstance(this);
}

SequencerBase::~SequencerBase()
{
    SequencerP::removeInstance(this);
}

bool SequencerBase::start(const char* pszStr, size_t steps)
{
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
    QWriteLocker locker(&SequencerP::rwlock);
    bool old = this->_bLocked;
    this->_bLocked = bLocked;
    return old;
}

bool SequencerBase::isLocked() const
{
    QReadLocker locker(&SequencerP::rwlock);
    return this->_bLocked;
}

bool SequencerBase::isRunning() const
{
    QReadLocker locker(&SequencerP::rwlock);
    return (SequencerP::_topLauncher != 0);
}

bool SequencerBase::wasCanceled() const
{
    QReadLocker locker(&SequencerP::rwlock);
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
    QWriteLocker locker(&SequencerP::rwlock);
    // Have we already an instance of SequencerLauncher created?
    if (!SequencerP::_topLauncher) {
        SequencerBase::Instance().start(pszStr, steps);
        SequencerP::_topLauncher = this;
    }
}

SequencerLauncher::~SequencerLauncher()
{
    QReadLocker locker(&SequencerP::rwlock);
    if (SequencerP::_topLauncher == this)
        SequencerBase::Instance().stop();
    locker.unlock();
    QWriteLocker write_locker(&SequencerP::rwlock);
    if (SequencerP::_topLauncher == this) {
        SequencerP::_topLauncher = 0;
    }
}

void SequencerLauncher::setText (const char* pszTxt)
{
    QReadLocker locker(&SequencerP::rwlock);
    SequencerBase::Instance().setText(pszTxt);
}

bool SequencerLauncher::next(bool canAbort)
{
    QReadLocker locker(&SequencerP::rwlock);
    if (SequencerP::_topLauncher != this)
        return true; // ignore
    return SequencerBase::Instance().next(canAbort);
}

void SequencerLauncher::setProgress(size_t pos)
{
    QReadLocker locker(&SequencerP::rwlock);
    SequencerBase::Instance().setProgress(pos);
}

size_t SequencerLauncher::numberOfSteps() const
{
    QReadLocker locker(&SequencerP::rwlock);
    return SequencerBase::Instance().numberOfSteps();
}

bool SequencerLauncher::wasCanceled() const
{
    return SequencerBase::Instance().wasCanceled();
}
