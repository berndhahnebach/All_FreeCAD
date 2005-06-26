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


#ifndef SEQUENCER_H
#define SEQUENCER_H

#ifndef _PreComp_
# include <list>
# include <vector>
#endif

namespace Base {

/**
 * \brief This class gives the user an indication of the progress of an operation and
 * it is used to reassure them that the application is still running.
 * 
 * Here are some code snippets of how to use the sequencer:
 *  \code
 *
 *  #include <Base/Sequencer.h>
 *
 *  //first example
 *  if (Base::Sequencer().start ("your text", 10))
 *  for (int i=0; i<10; i++)
 *  {
 *    // do something
 *    Base::Sequencer().next ();
 *  }
 *  Base::Sequencer().stop ();
 *
 *  //second example
 *  if (Base::Sequencer().start ("your text", 10))
 *  do
 *  {
 *    // do something
 *  }
 *  while ( Base::Sequencer().next() );
 *  Base::Sequencer().stop ();
 *
 *  \endcode
 *
 * The implementation of this class also supports the interlocking of several running
 * operations at a time. 
 *
 *  \code
 *
 *  //start the first operation
 *  Base::Sequencer().start ("your text", 10)
 *  for (int i=0; i<10, i++)
 *  {
 *    // do something
 *
 *    // start the second operation while the first one is still running
 *    Base::Sequencer().start ("another text", 10);
 *    for (int j=0; j<10; j++)
 *    {
 *      // do something different
 *      Base::Sequencer().next ();
 *    }
 *    Base::Sequencer().stop ();
 *
 *    Base::Sequencer().next ();
 *  }
 *  Base::Sequencer().stop ();
 *
 *  \endcode
 *
 * \author Werner Mayer
 */
class BaseExport SequencerBase
{
public:
  /**
   * Returns the last created sequencer instance.
   * If you create an instance of a class inheriting SequencerBase
   * this object is retrieved instead. 
   *
   * This mechanism is very useful to have an own sequencer for each layer of FreeCAD. 
   * For example, if FreeCAD is running in server mode you have/need no GUI layer 
   * and therewith no (graphical) progress bar; in this case ConsoleSequencer is taken.
   * But in cases FreeCAD is running with GUI the @ref Gui::ProgressBar is taken instead.
   * @see Sequencer
   */
  static SequencerBase& Instance();
  /**
   * Starts a new operation, returns false if there is already a pending operation,
   * otherwise it returns true.
   * In this method startStep() gets invoked that can be reimplemented in sub-classes.
   */
  bool start(const char* pszStr, unsigned long steps);
  /**
   * Performs the next step and returns true if the operation is not yet finished.
   * In this method nextStep() gets invoked that can be reimplemented in sub-classes.
   */
  bool next();
  /**
   * Reduces the number of pending operations by one and stops the
   * sequencer if all operations are finished. It returns false if there are still
   * pending operations, otherwise returns true.
   */
  virtual bool stop();
  /** If \a bLock is true then the sequencer gets locked. startStep() and nextStep()
   * don't get invoked any more on until the sequencer gets unlocked again. 
   */
  void setLocked( bool bLock );
  /** Returns true if the sequencer was locked, false otherwise. */
  bool isLocked() const;
  /** Returns true if the sequencer is running, otherwise returns false. */
  bool isRunning() const;
  /** 
   * Returns true if the pending operation was canceled. 
   */
  bool wasCanceled() const;
  /** 
   * Try to cancel the pending operation(s). 
   * E.g. @ref Gui::ProgressBar calls this method after the ESC button was pressed.
   */
  void tryToCancel();
  /** 
   * Returns the number of pending operations. This number complies with the number
   * of calls of @ref start() and @ref stop().
   */
  int pendingOperations() const;
  /** Returns the current state of progress in percent. */
  int progressInPercent() const;

protected:
  /** construction */
  SequencerBase();
  /** Destruction */
  virtual ~SequencerBase();
  /** 
   * Sets a text what the pending operation is doing. The default implementation 
   * does nothing.
   */
  virtual void setText (const char* pszTxt);
  /**
   * This method can be reimplemented in sub-classes to give the user a feedback
   * when a new sequence starts. The default implementation does nothing.
   */
  virtual void startStep();
  /**
   * This method can be reimplemented in sub-classes to give the user a feedback
   * when the next is performed. The default implementation does nothing.
   */
  virtual void nextStep();
  /** 
   * Resets internal data. 
   * If you want to reimplement this method, it is very important to call it ín your code.
   */
  virtual void resetData();

protected:
  unsigned long nProgress; /**< Stores the current amount of progress.*/
  unsigned long nTotalSteps; /**< Stores the total number of steps */

private:
  /**
   * Sets a global sequencer object.
   * Access to the last registered
   * object is performed by
   * @see Sequencer().
   */
  void _setGlobalInstance ();

  bool _bLocked; /**< Lock/unlock sequencer. */
  bool _bCanceled; /**< Is set to true if the last pending operation was canceled */
  int _nInstStarted; /**< Stores the number of pending operations */ 
  int _nMaxInstStarted; /**< Stores the number of maximum pending operations until all pending operations 
                            are finished. */
  int _nLastPercentage; /**< Progress in percent. */
  std::list<unsigned long> _aSteps; /**< Stores the number of steps for each operation */
  static std::vector<SequencerBase*> _aclInstances; /**< A vector of all created instances */ 
};


/**
 * \brief This class writes the progress to the console window.
 */
class BaseExport ConsoleSequencer : public SequencerBase
{
public:
  /** construction */
  ConsoleSequencer ();
  /** Destruction */
  ~ConsoleSequencer ();

protected:
  /** Starts the sequencer */
  void startStep();
  /** Writes the current progress to the console window. */
  void nextStep();

private:
  /** Puts text to the console window */
  void setText (const char* pszTxt);
  /** Resets the sequencer */
  void resetData();
};

/** Access to the only SequencerBase instance */
inline SequencerBase& Sequencer ()
{
  return SequencerBase::Instance();
}

} // namespace Base

#endif // SEQUENCER_H
