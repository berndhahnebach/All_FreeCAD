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


#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#ifndef _PreComp_
# include <qprogressbar.h>
#endif

#include "../Base/Sequencer.h"

namespace Gui {

struct ProgressBarPrivate;

/**
 * FreeCAD's progress bar for long operations
 * To see how to use the progress bar have a look at the following examples:
 *
 * \code
 * unsigned long steps = ...
 * Base::Sequencer().start("Starting progress bar", 0);
 *
 * for (unsigned long i=0; i<steps;i++)
 * {
 *   Base::Sequencer().next();
 *   // do one step of your algorithm
 * }
 *
 * Base::Sequencer().stop();
 * 
 * \endcode
 *
 * The example below shows how to use two nested progresses.
 *
 * \code
 * void function1()
 * {
 *   unsigned long steps = ...
 *   Base::Sequencer().start("Starting progress bar", 0);
 *
 *   for (unsigned long i=0; i<steps;i++)
 *   {
 *     Base::Sequencer().next();
 *     // do one step of your algorithm
 *   }
 *
 *   Base::Sequencer().stop();
 * }
 * 
 * void function2()
 * {
 *   unsigned long steps = ...
 *   Base::Sequencer().start("Starting progress bar", 0);
 *
 *   for (unsigned long i=0; i<steps;i++)
 *   {
 *     Base::Sequencer().next();
 *     // do one step of your algorithm calling function1
 *     function1();
 *   }
 *
 *   Base::Sequencer().stop();
 * }
 *
 * \endcode
 *
 * If the total number of steps cannot be determined before, use 0 instead of to show
 * just a busy indicator instead of percentage steps.
 * \author Werner Mayer
 */
class ProgressBar : public QProgressBar, public Base::SequencerBase
{
  Q_OBJECT

public:
  /** Returns the sequencer object. */
  static ProgressBar* instance();
  /** Sets the progress to \a progress. */
  void setProgress( int progress );
  /** Handles all incoming events while the progress bar is running. All key and mouse
   * events are ignored to block user input.
   */
  bool eventFilter(QObject* o, QEvent* e);
  /** Returns the time in milliseconds that must pass before the progress bar appears.
   */
  int minimumDuration() const;
  /** This restores the last overridden cursor and release the keyboard while the progress bar 
   * is running. This is useful e.g. if a modal dialog appears while a long operation is performed
   * to indicate that the user can click on the dialog. Every pause() must eventually be followed 
   * by a corresponding @ref resume().
   */
  void pause();
  /** This sets the wait cursor again and grabs the keyboard. @see pause() */
  void resume();

public slots:
  /** Sets the time that must pass before the progress bar appears to \a ms.
   */
  void setMinimumDuration ( int ms );

protected:
  /** Starts the progress bar */
  void startStep();
  /** Increase the progress bar. */
  void nextStep(bool canAbort);
  void showEvent( QShowEvent* );

protected slots:
  /* Shows the progress bar if it is still hidden after the operation has been started
   * and minimumDuration milliseconds have passed.
   */
  void forceShow();

protected:
  /** Construction */
  ProgressBar ( QWidget * parent=0, const char * name=0, WFlags f=0 );
  /** Destruction */
  ~ProgressBar ();

private:
  /** @name for internal use only */
  //@{
  /** Puts text to the status bar */
  void setText (const char* pszTxt);
  /** Gets the events under control */
  void enterControlEvents();
  /** Loses the control over incoming events*/
  void leaveControlEvents();
  /** Throws an exception to stop the pending operation. */
  void abort();
  /** Resets the sequencer */
  void resetData();
  /** Draws the content of the progress bar */
  //    void drawContents( QPainter *p );
  /** Reimplemented */
  bool setIndicator ( QString & indicator, int progress, int totalSteps );
  //@}
  ProgressBarPrivate* d;
  static ProgressBar* _pclSingleton;
};

} // namespace Gui

#endif // PROGRESS_BAR_H
