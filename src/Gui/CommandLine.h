/** \file $RCSfile$
 *  \brief The command line module
 *  \author $Author$
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
 



#ifndef __COMMAND_LINE_H__
#define __COMMAND_LINE_H__
 
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <assert.h>
#endif

#include "../Base/Export.h"
#include <qlineedit.h>

/** The command line class
 */
class GuiExport FCCommandLine : public QLineEdit
{
private:
	// Singleton
	FCCommandLine(void);
	~FCCommandLine(void);

	static FCCommandLine *_pcSingleton;

public:
  void SetParent(QWidget* parent);
	static void Destruct(void);
	static FCCommandLine &Instance(void);
	friend FCCommandLine &GetCmdLine(void); 
};

inline FCCommandLine &GetCmdLine(void)
{
  return FCCommandLine::Instance();
}


#endif // __COMMAND_LINE_H__
