/** \file Observer.h
 *  \brief Observer facility of FreeCAD
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here the definition of the Observer pattern implementation of FreeCAD. 
 *  Its used in several classes of FreeCAD for message distribution and 
 *  class observation.
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
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
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/


#ifndef __OBSERVER_H__
#define __OBSERVER_H__

// Std. configurations

#include <set>

class FCSubject;


/** Observer class
 *  Implementation of the well known Observer Design Pattern.
 *  The observed object, which inherit FCSubject, will call all
 *  its observers in case of changes. A observer class has to 
 *  Attach itself to the observed object.
 *  @see FCSubject
 */
class BaseExport FCObserver
{
public:

       
	/**
	 * A constructor.
	 * No special function so far.
	 */
	FCObserver();

	/**
	 * A destructor.
	 * No special function so far.
	 */
	virtual ~FCObserver();

	/**
	 * This method need to be reimplemented from the concrete Observer
	 * and get called by the observed class
	 * @param pCaller a referenc to the calling object
	 */
	virtual void OnChange(FCSubject &rCaller)=0;

	/**
	 * This method need to be reimplemented from the concrete Observer
	 * and get called by the observed class
	 * @param pCaller a referenc to the calling object
	 */
	virtual void OnDestroy(FCSubject &rCaller){}
};

/** Subject class
 *  Implementation of the well known Observer Design Pattern.
 *  The observed object, which inherit FCSubject, will call all
 *  its observers in case of changes. A observer class has to 
 *  Attach itself to the observed object.
 *  @see FCObserver
 */
class BaseExport FCSubject
{
public:

       
	/**
	 * A constructor.
	 * No special function so far.
	 */
	FCSubject();

	/**
	 * A destructor.
	 * No special function so far.
	 */
	~FCSubject();

	/** Attach an Observer
	 * Attach an Observer to the list of Observers which get   
	 * called when Notify is called.
	 * @param ToObserv A pointer to a concrete Observer
	 * @see Notify
	 */
	void Attach(FCObserver *ToObserv);

	/** Detach an Observer
	 * Detach an Observer from the list of Observers which get   
	 * called when Notify is called.
	 * @param ToObserv A pointer to a concrete Observer
	 * @see Notify
	 */
	void Detach(FCObserver *ToObserv);

	/** Notify all Observers
	 * Send a message to all Observers attached to this subject.
	 * The Message depends on the implementation of a concrete 
	 * Oberserver and Subject.
	 * @see Notify
	 */
	void Notify(void);

protected:
	/// Vector of attached observers
#	pragma warning( disable : 4251 )
	std::set<FCObserver *> _ObserverSet;
#	pragma warning( default : 4251 )

};





#endif // __OBSERVER_H__
