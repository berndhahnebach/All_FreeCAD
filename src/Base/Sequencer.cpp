/** \file Sequencer.cpp
 *  \brief Sequencer base class
 *  \author Werner Mayer
 *  \version $Revision$
 *  \date    $Date$
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

#include "PreCompiled.h"

#ifndef _PreComp_
#endif

#include "Sequencer.h"
#include "Exception.h"

using namespace Base;

// -------------------------------------------------------

SequencerBase* SequencerBase::_pclSingleton = 0L;

SequencerBase& SequencerBase::Instance ()
{
	// not initialized?
	if ( !_pclSingleton )
	{
		_pclSingleton = new SequencerBase();
	}

	return *_pclSingleton;
}

SequencerBase::SequencerBase()
	: _bCanceled(false), _nInstStarted(0), _nMaxInstStarted(1),
		_nProgress(0), _nTotalSteps(0)
{
	setGlobalInstance();
}

SequencerBase::~SequencerBase()
{
}

void SequencerBase::setGlobalInstance ()
{
	_pclSingleton = this;
}

bool SequencerBase::wasCanceled() const
{
	return _bCanceled;
}

bool SequencerBase::isRunning() const
{
	return _nInstStarted > 0;
}

bool SequencerBase::start(const char* pszStr, unsigned long steps)
{
	// increment the number of running instances
	_nInstStarted++;

	// several sequencer started
	if (_nInstStarted > _nMaxInstStarted)
	{
		// calculate the number of iterations using Horner scheme
		_aSteps.push_front(steps);
		_nTotalSteps = 1;
		for (std::list<unsigned long>::iterator it=_aSteps.begin(); it!=_aSteps.end();++it)
			_nTotalSteps = _nTotalSteps * (*it) + 1;
		_nTotalSteps -= 1;

		_nMaxInstStarted = _nInstStarted;
	  }
	else if (_nInstStarted == 1)
	{
		_aSteps.push_front(steps);
		_nTotalSteps = steps;
		_nProgress = 0;
		_bCanceled = false;

		setText(pszStr);

		return true;
	}

	return false;
}

bool SequencerBase::next()
{
  if (!wasCanceled())
	{
		_nProgress++;
	}
	else
	{
		// force to abort the operation
		abort();
	}

	return _nProgress < _nTotalSteps;
}

bool SequencerBase::stop()
{
	_nInstStarted--;
	if (_nInstStarted == 0)
	{
		resetBar();
	}

	return (_nInstStarted == 0);
}

void SequencerBase::resetBar()
{
	_nInstStarted = 0;
	_nMaxInstStarted = 1;
	_aSteps.clear();
}

void SequencerBase::abort()
{
	//resets
	resetBar();
	FCException exc("Aborting...");
	throw exc;
}

void SequencerBase::setText(const char*)
{
}
