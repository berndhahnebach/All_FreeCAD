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


#include <qwidget.h>
#include "qextmdi/qextmdichildview.h"
#include "../Base/Parameter.h"
#include "../Base/Console.h"

class FCGuiDocument;
class ApplicationWindow;
class FCView;
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QMultiLineEdit;
class QTabWidget;


/** Adapter class to the parameter of FreeCAD for all Windows
 * Retrive the parameter group of the specific window
 * by the windowname.
 */
class GuiExport FCWindowParameter 
{
public:
	FCWindowParameter(const char *name);
	~FCWindowParameter();

	virtual void OnParameterChanged(void);

	/// get the parameters
	FCParameterGrp::handle GetParameter(void);
	/// return the parameter group of this window
	FCParameterGrp::handle GetWindowParameter(void);

	ApplicationWindow* GetAppWnd(void);

private:
	FCParameterGrp::handle _handle;


};


/** father of all FreeCAD windows (execept the view containers)
 */
class GuiExport FCDockWindow:public QWidget, public FCWindowParameter 
{
	Q_OBJECT;

public:
	FCDockWindow(QWidget *parent=0, const char *name=0, WFlags f=0 );
	~FCDockWindow();

private:


};





/** Father of all view container classes
 */
/*
class FCViewContainer: public QextMdiChildView, public FCWindowParameter
{
	Q_OBJECT;

public:
	FCViewContainer( QWidget* parent, const char* name, int wflags=WDestructiveClose );
	~FCViewContainer();

	virtual FCView* GetActiveView(void){return 0L;}

private:

};

*/





#endif
