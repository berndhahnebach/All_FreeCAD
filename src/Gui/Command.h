/** \file Command.h
 *  \brief The visual command module
 *  \author Juergen Riegel
 *  \version 0.1
 *  \date    4.2002
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
#ifndef __Command_h__
#define __Command_h__

#include "../Base/Export.h"

#include "../base/PyExport.h"
#include <qaction.h>
#include <string>
#include <map>


/*
class GuiExport FCAction :public FCPyObject, public QAction
{
	Py_Header;			// always start with Py_Header


//	Q_OBJECT


public:
	FCAction(const char* Name);

	~FCAction();

	static PyObject *PyMake(PyObject *, PyObject *);

	// signals from QT ++++++++++++++++++++++++++++++++++++++++++++++++++++
	virtual void activated (); 
	virtual void toggled ( bool );

	// exported functions goes here +++++++++++++++++++++++++++++++++++++++
	void Do(void);								// Geter for the handled OCC class
	// python exports goes here +++++++++++++++++++++++++++++++++++++++++++	
	PyObject *_getattr(char *attr);				// __getattr__ function
	// geter setter
	int _setattr(char *attr, PyObject *value);	// __setattr__ function
	// methods
	PyObject *PyDo(PyObject *args);	// Python wrapper
	static PyObject *sPyDo(PyObject *self, PyObject *args, PyObject *kwd){return ((FCAction*)self)->PyDo(args);};

protected:

	stlport::string sName;

};

*/

/** The CommandManager class
 *  This class manage all available commands in FreeCAD. All 
 *  Commands will registered here, also commands from Application
 *  modules. Also activation / deactivation, Icons Tooltips and so
 *  on are handles here. Further the Building of Toolbars and (Context) 
 *  menus is implemented here.
 *  @see FCCommand
 */
class FCCommand 
{
public:

	FCCommand(void)
		:_pcAction(0)
	{}

	FCCommand(QAction *pcAction)
		:_pcAction(pcAction)
	{}
	FCCommand(const FCCommand& copy)
		:_pcAction(copy._pcAction)
	{}

	QAction *Action(void){return _pcAction;}


private:	
	QAction *_pcAction;
};

/** The CommandManager class
 *  This class manage all available commands in FreeCAD. All 
 *  Commands will registered here, also commands from Application
 *  modules. Also activation / deactivation, Icons Tooltips and so
 *  on are handles here. Further the Building of Toolbars and (Context) 
 *  menus is implemented here.
 *  @see FCCommand
 */
class GuiExport FCCommandManager
{
public:
	void AddCommand(const char* Name,const FCCommand& Com)
	{
		_sCommands[Name] = Com;	
	}

	void AddTo(const char* Name,QWidget *pcWidget)
	{
		_sCommands[Name].Action()->addTo(pcWidget);
	}

private:
#	pragma warning( disable : 4251 )
		stlport::map<stlport::string,FCCommand> _sCommands;
#	pragma warning( default : 4251 )
};


#endif // __Command_h__