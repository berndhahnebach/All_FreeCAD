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

namespace Base {

/**
 * This class gives the user an indication of the progress of an operation and
 * it is used to reassure them that the application is still running.
 *
 *  \par
 *  \code
 *  #include <Base/Sequencer.h>
 *  //first example
 *  if (Sequencer().start ("your text", 10))
 *  for (int i=0; i<10; i++)
 *  {
 *    // do something
 *		Sequencer().next ();
 *  }
 *  Sequencer().stop ();
 *
 *  //second example
 *  if (Sequencer().start ("your text", 10))
 *  do
 *  {
 *    // do something
 *  }
 *  while (Sequencer().next ());
 *  Sequencer().stop ();
 *  \endcode
 *  \par
 *
 * The implementation of this class also supports the interlocking of several running
 * operations at a time. 
 *
 *  \par
 *  \code
 *  //start the first operation
 *  Sequencer().start ("your text", 10)
 *  for (int i=0; i<10, i++)
 *  {
 *    // do something
 *
 *    // start the second operation while the first one is still running
 *    Sequencer().start ("another text", 10);
 *		for (int j=0; j<10; j++)
 *		{
 *			// do something other
 *			Sequencer().next ();
 *		}
 *		Sequencer().stop ();
 *
 *		Sequencer().next ();
 *  }
 *  Sequencer().stop ();
 *  \endcode
 *  \par
 *
 * This class is inherited by @see Gui::ProgressBar.
 */

class BaseExport CSequencer
{
public:
	/**
	 * Returns the last registered sequencer object
	 * @see setGlobalInstance
	 * @see _pclSequencer
	 */
	static CSequencer& Instance();

	/** @methodes to reimplement in subclasses */
	//@{
	/**
	 * Starts the sequencer, returns false if there is already a pending operation,
	 * true otherwise.
	 * Reimplemented in @see Gui::ProgressBar
	 */
	virtual bool start(const char* pszStr, unsigned long steps);
	/**
	 * Next step, returns false if the end is already reached, true otherwise.
	 * NOTE: For each call of start() you must call the
	 *	     corresponding @see stop() method.
	 * Reimplemented in @see Gui::ProgressBar.
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
	/** Construction */
	CSequencer();
	/** Destruction */
	virtual ~CSequencer();
	/** Reimplemented in @see Gui::ProgressBar */
	virtual void setText (const char* pszTxt);
	/** Resets the sequencer
	 * Reimplemented in @see Gui::ProgressBar
	 */
	virtual void resetBar();
	/** Aborts all pending operations
	 * Reimplemented in @see Gui::ProgressBar
	 */
	virtual void abort();

protected:
	bool _bCanceled; /**< Is set to true if the last pending operation was canceled */
	int _nInstStarted; /**< Stores the number of pending operations */ 
	int _nMaxInstStarted; /**< Stores the number of maximum pending operations until no pending operation 
												  is running. */
	unsigned long _nProgress, _nTotalSteps;

private:
	/**
	 * Sets a global Sequencer object.
	 * Access to the last registered
	 * object is performed by
	 * @see Sequencer().
	 */
	void setGlobalInstance ();
	/**
	 * The _pclSequencer member just stores the pointer of the
	 * last instaciated CSequencer object.
	 */
	static CSequencer* _pclSingleton; 
	std::list<unsigned long> _aSteps;
};

/** get the last registered instance */
inline CSequencer& Sequencer ()
{
	return CSequencer::Instance();
}

} // namespace Base

#endif // __SEQUENCER_H__
