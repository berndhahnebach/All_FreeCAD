/***************************************************************************
                          WaitCursor.h  -  description
                             -------------------
    begin                : 2002/08/19 21:11:52
    copyright            : (C) 2002 by Werner Mayer
    email                : werner.wm.mayer@gmx.de
 ***************************************************************************/

/** \file $RCSfile$
 *  \brief Automatic and semi-automatic wait cursor 
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




#ifndef DLG_WAIT_CURSOR_H
#define DLG_WAIT_CURSOR_H

#ifndef _PreComp_
#	include <string>
#	include <vector>
#	include <qthread.h>
#endif


/** 
 * This class sets a waiting cursor automatically while a  slow operation is running. 
 * Therefore you have just to  create an instance of FCWaitingCursor before the time 
 * consuming operation.
 * If the operation takes less time than the @ref minimumDuration (1500 ms
 * by default), the waiting cursor will not appear at all. For example
 *
 * \code:
 * FCWaitingCursor ac;
 * ...
 * ...								 // slow operation
 * ...
 * \endcode
 *  
 * The @ref setWaitCursor() method is called in the constructor, @ref restoreCursor()
 * at destruction time of FCWaitingCursor.
 *
 * Sometimes you have two slow operations with some user interactions between them.
 * Avoiding to show the waiting cursor then you have to call the methods @ref restoreCursor()
 * and setWaitCursor manually, like:
 *
 * \code:
 * FCWaitingCursor ac;
 * ...
 * ...								 // 1st slow operation
 * ac.restoreCursor();
 * ...
 * ...                 // some dialog stuff
 * ac.setWaitCursor();
 * ...
 * ...								 // 2nd slow operation
 * \endcode
 *  
 * @author Werner Mayer
 */
class GuiExport FCWaitingCursor : public QThread
{
	public:
		 FCWaitingCursor();
		~FCWaitingCursor();

		void setWaitCursor();
		void restoreCursor();
		int  minimumDuration();
		void setMinimumDuration ( int ms );

	private:
		void run();
		struct FCWaitingCursorP* d;
};

/**
 * The FCAutoWaitCursor sets automatically the
 * waiting cursor if the application is busy
 *
 * NOTE:
 * When clicking on the window bar (under Windows OS) the waiting cursor appears
 * after a while although the application actuallay is not really busy. 
 * Under Linux (Debian) I got some mysterious errors and the application hanged up.
 * So do not use this class but use the  @ref FCWaitingCursor class instead.
 * @author Werner Mayer
 */
class GuiExport FCAutoWaitCursor : public QThread
{
	public:
		static void Destruct(void);
		static FCAutoWaitCursor &Instance(void);

		void SetWaitCursor();

		// Singleton
	private:
		 FCAutoWaitCursor(uint id, int i);
		~FCAutoWaitCursor();

		static FCAutoWaitCursor* _pclSingleton;

	protected:
		/// run the thread, decrement an internal counter
		void run();

		QMutex mutex;
		uint main_threadid;
		int nInterval;
		bool bActive;
		bool bRun;

	private:
		class FCAutoWaitCursorP* d;
		friend class FCAutoWaitCursorP;
};

#endif

