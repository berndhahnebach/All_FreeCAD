/** \file $RCSfile$
 *  \brief The windows base class, father of all windows
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
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/
#ifndef __Windows_h__
#define __Windows_h__

#include "../Base/Export.h"

#include <qwidget.h>
#include <qextmdichildview.h>
class FCGuiDocument;



/** Adapter class to the parameter of FreeCAD for all QT Windows
 * Retrive the parameter group of the specific window
 * by the windowname.
 */
class GuiExport FCWindow 
{
public:
	FCWindow(const char *name);
	~FCWindow();

private:

};




/** A test class. A more elaborate class description.
 * Detaild description with some formating:
 *  \par
 *  bla blablablablablablabl:
 *  \code
 *  #include <Base/Console.h>
 *  GetConsole().Log("Stage: %d",i);
 *  \endcode
 */
class GuiExport FCDockWindow :public QWidget, public FCWindow
{
	Q_OBJECT;

public:
	FCDockWindow(QWidget *parent=0, const char *name=0, WFlags f=0);
	~FCDockWindow();


private:

};


/** A test class. A more elaborate class description.
 * Detaild description with some formating:
 *  \par
 *  bla blablablablablablabl:
 *  \code
 *  #include <Base/Console.h>
 *  GetConsole().Log("Stage: %d",i);
 *  \endcode
 */
class FCViewWindow: public QextMdiChildView, public FCWindow
{
	Q_OBJECT;

public:
	FCViewWindow( FCGuiDocument* pcDocument, QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCViewWindow();

private:
    FCGuiDocument*	_pcDocument;

};



#endif
