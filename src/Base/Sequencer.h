/** \file Sequencer.h
 *  \brief Sequencer base class
 *  \author Werner Mayer
 *  \version 1.3
 *  \date    2004/10/03 21:57:09
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

#ifndef _PreComp_
# include <list>
# include <vector>
#endif

namespace Base {
class BaseExport SequencerBase
{
public:
  static SequencerBase& Instance();

  virtual bool start(const char* pszStr, unsigned long steps);
  virtual bool next();
  virtual bool stop();

  bool isRunning() const;
  bool wasCanceled() const;
  void tryToCancel();

  int pendingOperations() const;

protected:
  SequencerBase();
  virtual ~SequencerBase();

  virtual void setText (const char* pszTxt);
  virtual void resetData();

protected:
  unsigned long nProgress; /**< Stores the current amount of progress.*/
  unsigned long nTotalSteps; /**< Stores the total number of steps */

private:
  void _setGlobalInstance ();

  bool _bCanceled; /**< Is set to true if the last pending operation was canceled */
  int _nInstStarted; /**< Stores the number of pending operations */ 
  int _nMaxInstStarted; /**< Stores the number of maximum pending operations until all pending operations 
                            are finished. */
  std::list<unsigned long> _aSteps; /**< Stores the number of steps for each operation */
  static std::vector<SequencerBase*> _aclInstances; /**< A vector of all created instances */ 
};

class BaseExport ConsoleSequencer : public SequencerBase
{
public:
  static ConsoleSequencer* Instance();

  bool start(const char* pszStr, unsigned long steps);
  bool next();

private:
  ConsoleSequencer ();
  ~ConsoleSequencer ();

  void setText (const char* pszTxt);
  void resetData();

  static ConsoleSequencer* _pclSingleton; 
  int _iLastPercentage;
};

/** Access to the only SequencerBase instance */
inline SequencerBase& Sequencer ()
{
  return SequencerBase::Instance();
}

} // namespace Base

#endif // __SEQUENCER_H__
