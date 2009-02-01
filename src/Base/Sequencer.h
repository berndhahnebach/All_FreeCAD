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


#ifndef BASE_SEQUENCER_H
#define BASE_SEQUENCER_H

#include <vector>

#include "Exception.h"

namespace Base
{

class AbortException;
class SequencerLauncher;

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
 *  if (Base::Sequencer().start ("my text", 10))
 *  for (int i=0; i<10; i++)
 *  {
 *    // do something
 *    Base::Sequencer().next ();
 *  }
 *  Base::Sequencer().stop ();
 *
 *  //second example
 *  if (Base::Sequencer().start ("my text", 10))
 *  do
 *  {
 *    // do something
 *  }
 *  while ( Base::Sequencer().next() );
 *  Base::Sequencer().stop ();
 *
 *  \endcode
 *
 * The implementation of this class also supports the nesting of several running
 * operations at a time.
 *
 *  \code
 *  try{
 *    //start the first operation
 *    Base::Sequencer().start ("my text", 10)
 *    for (int i=0; i<10, i++)
 *    {
 *      // do something
 *
 *      // start the second operation while the first one is still running
 *      Base::Sequencer().start ("another text", 10);
 *      for (int j=0; j<10; j++)
 *      {
 *        // do something different
 *        Base::Sequencer().next ();
 *      }
 *      Base::Sequencer().stop ();
 *
 *      Base::Sequencer().next ( true ); // allow to cancel
 *    }
 *    Base::Sequencer().stop ();
 *  }catch(const Base::AbortException&){
 *    // cleanup your data if needed
 *  }catch(...){
 *    // cleanup your data
 *  }
 *
 *  \endcode
 *
 * \note If using the sequencer with Sequencer().next(\a true ) then you must take into account
 * that the exception AbortException could be thrown, e.g. in case the ESC was pressed. So in this
 * case it's always a good idea to use the sequencer within a try-catch block.
 *
 * \note In case the operation was aborted Sequencer().stop() needs not to be called in the catch
 * block since SequencerBase cleans up internal data on its own.
 * @see SequencerLauncher
 *
 * \note In case several nested sequencers are used then no percentage progress is possible any more
 * because the number of total steps cannot be determined for sure. Then a busy indicator is shown
 * instead of.
 *
 * \author Werner Mayer
 */
class BaseExport SequencerBase
{
    friend class SequencerLauncher;

protected:
    /**
     * Starts a new operation, returns false if there is already a pending operation,
     * otherwise it returns true.
     * In this method startStep() gets invoked that can be reimplemented in sub-classes.
     */
    bool start(const char* pszStr, size_t steps);
    /**
     * Performs the next step and returns true if the operation is not yet finished.
     * In this method nextStep() gets invoked that can be reimplemented in sub-classes.
     * If \a canAbort is true then the operations can be aborted, otherwise (the default)
     * the operation cannot be aborted and the sequencer just acts as an indicator of how
     * long the operation will take.
     */
    bool next(bool canAbort = false);
    /**
     * Reduces the number of pending operations by one and stops the
     * sequencer if all operations are finished. It returns false if there are still
     * pending operations, otherwise it returns true.
     */
    bool stop();

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
     * Breaks the sequencer if needed. The default implementation does nothing.
     * Every pause() must eventually be followed by a corresponding @ref resume().
     * @see Gui::ProgressBar.
     */
    virtual void pause();
    /**
     * Continue with progress. The default implementation does nothing.
     * @see pause(), @see Gui::ProgressBar.
     */
    virtual void resume();
    /** If \a bLock is true then the sequencer gets locked. startStep() and nextStep()
     * don't get invoked any more until the sequencer gets unlocked again.
     * This method returns the previous lock state.
     */
    bool setLocked(bool bLock);
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
     * If you tried to cancel but then decided to continue the operation.
     * E.g. in @ref Gui::ProgressBar a dialog appears asking if you really want to cancel. If you decide
     * to continue this method must be called.
     */
    void rejectCancel();
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
     * when the next is performed. The default implementation does nothing. If \a canAbort
     * is true then the pending operation can aborted, otherwise not.
     */
    virtual void nextStep(bool canAbort);
    /**
     * Resets internal data.
     * If you want to reimplement this method, it is very important to call it ín your code.
     */
    virtual void resetData();

protected:
    size_t nProgress; /**< Stores the current amount of progress.*/
    size_t nTotalSteps; /**< Stores the total number of steps */

private:
    /** Sets a global sequencer object.
     * Access to the last registered object is performed by @see Sequencer().
     */
    void _setGlobalInstance ();

    bool _bLocked; /**< Lock/unlock sequencer. */
    bool _bCanceled; /**< Is set to true if the last pending operation was canceled */
    int _nInstStarted; /**< Stores the number of pending operations */
    int _nMaxInstStarted; /**< Stores the number of maximum pending operations until all pending operations
                            are finished. */
    int _nLastPercentage; /**< Progress in percent. */
    size_t _nNewSteps; /**< Is used for nested calls of the sequencer. */
    static std::vector<SequencerBase*> _aclInstances; /**< A vector of all created instances */
    static SequencerLauncher* _topLauncher; /**< The outermost launcher */
};

/** This special sequencer might be useful if you want to suppress any indication
 * of the progress to the user.
 */
class BaseExport EmptySequencer : public Base::SequencerBase
{
public:
    /** construction */
    EmptySequencer();
    /** Destruction */
    ~EmptySequencer();
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
    void nextStep(bool canAbort);

private:
    /** Puts text to the console window */
    void setText (const char* pszTxt);
    /** Resets the sequencer */
    void resetData();
};

/** The SequencerLauncher class is provided for convenience. It allows you to run an instance of the
 * sequencer by instanciating an object of this class -- most suitable on the stack. So this mechanism
 * can be used for try-catch-blocks to destroy the object automatically if the C++ exception mechanism
 * cleans up the stack.
 *
 * This class has been introduced to simplify the use with the sequencer. In the FreeCAD Gui layer there
 * is a subclass of SequencerBase called ProgressBar that grabs the keyboard and filters most of the incoming
 * events. If the programmer uses the API of SequencerBase directly to start an instance without due diligence
 * with exceptions then a not handled exception could block the whole application -- the user has to kill the
 * application then.
 *
 * Below is an example of a not correctly used sequencer.
 *
 * \code
 *
 *  #include <Base/Sequencer.h>
 *
 *  void runOperation();
 *
 *  void myTest()
 *  {
 *    try{
 *       runOperation();
 *    } catch(...) {
 *       // the programmer forgot to stop the sequencer here
 *       // Under circumstances the sequencer never gets stopped so the keyboard never gets ungrabbed and
 *       // all Gui events still gets filtered.
 *    }
 *  }
 *
 *  void runOperation()
 *  {
 *    Base::Sequencer().start ("my text", 10);
 *
 *    for (int i=0; i<10; i++)
 *    {
 *      // do something where an exception be thrown
 *      ...
 *      Base::Sequencer().next ();
 *    }
 *
 *    Base::Sequencer().stop ();
 *  }
 *
 * \endcode
 *
 * To avoid such problems the SequencerLauncher class can be used as follows:
 *
 * \code
 *
 *  #include <Base/Sequencer.h>
 *
 *  void runOperation();
 *
 *  void myTest()
 *  {
 *    try{
 *       runOperation();
 *    } catch(...) {
 *       // the programmer forgot to halt the sequencer here
 *       // If SequencerLauncher leaves its scope the object gets destructed automatically and
 *       // stops the running sequencer.
 *    }
 *  }
 *
 *  void runOperation()
 *  {
 *    // create an instance on the stack (not on any terms on the heap)
 *    SequencerLauncher seq("my text", 10);
 *
 *    for (int i=0; i<10; i++)
 *    {
 *      // do something (e.g. here can be thrown an exception)
 *      ...
 *      Base::Sequencer().next ();
 *    }
 *    // Base::Sequencer().stop() needs not to be called, the destructor of SequencerLauncher does it for us.
 *  }
 *
 * \endcode
 *
 * @author Werner Mayer
 */
class BaseExport SequencerLauncher
{
public:
    SequencerLauncher(const char* pszStr, size_t steps);
    ~SequencerLauncher();
    bool next(bool canAbort = false);
};

/** Access to the only SequencerBase instance */
inline SequencerBase& Sequencer ()
{
    return SequencerBase::Instance();
}

} // namespace Base

#endif // BASE_SEQUENCER_H
