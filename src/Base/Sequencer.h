/** \file Sequencer.h
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


#ifndef __SEQUENCER_H__
#define __SEQUENCER_H__

#include <list>

/** Declaration of class Sequencer.
 * This class gives the user an indication of the progress of an operation and
 * is used to reassure them that the application is still running.
 * This class is inherited by @see FCProgressBar.
 *
 *  \par
 *  \code
 *  #include <Base/Sequencer.h>
 *  //first example
 *  if (GetSequencer().start ("your text", 10))
 *  for (int i=0; i<10; i++)
 *  {
 *    // do something
 *		GetSequencer().next ();
 *  }
 *  GetSequencer().stop ();
 *
 *  //second example
 *  if (GetSequencer().start ("your text", 10))
 *  do
 *  {
 *    // do something
 *  }
 *  while (GetSequencer().next ());
 *  GetSequencer().stop ();
 *  \endcode
 *  \par
 */

class BaseExport Sequencer
{
public:
	Sequencer();
	virtual ~Sequencer();

	/** @methodes to reimplement in subclasses*/
	//@{
	/**
	 * Starts the sequencer, returns false if there is already a pending operation,
	 * true otherwise.
	 * Reimplemented in @see FCProgressBar
	 */
	virtual bool start(const char* pszStr, unsigned long steps);
	/**
	 * Next step, returns false if the end is already reached, true otherwise.
	 * NOTE: For each call of start() you must call the
	 *	     corresponding @see stop() method.
	 * Reimplemented in @see FCProgressBar.
	 */
	virtual bool next();
	/**
	 * Stops the (running) sequencer, returns false if there are still
	 * pending operations, true otherwise.
	 */
	virtual bool stop();
	//@}
	/** Checks if the sequencer is running */
	bool isRunning() const;
	/** Returns true if the operation was canceled (e.g. by pressing ESC)
	 */
	bool wasCanceled() const;

protected:
	/**
	 * Sets a global Sequencer object.
	 * Access to the last registered
	 * object is performed by
	 * @see GetSequencer().
	 */
	void setGlobalInstance ();
	/// Reimplemented in @see FCProgressBar
	virtual void setText (const char* pszTxt);
	/** Resets the sequencer
	 * Reimplemented in @see FCProgressBar
	 */
	virtual void resetBar();
	/** Aborts all pending operations
	 * Reimplemented in @see FCProgressBar
	 */
	virtual void abort();

protected:
	bool _bCanceled;
	int _nInstStarted, _nMaxInstStarted;
	unsigned long _nProgress, _nTotalSteps;

private:
	std::list<unsigned long> _aSteps;
};

// get global instance
BaseExport Sequencer& GetSequencer ();

#endif // __SEQUENCER_H__
