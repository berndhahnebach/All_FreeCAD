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


#include "../Base/PyExport.h"
#include <qaction.h>
#include <string>
#include <map>
//#	pragma warning( disable : 4786 )

class FCCommandManager;
class ApplicationWindow;
class FCGuiDocument;
class FCDocument;

void CreateStdCommands(void);
void CreateViewStdCommands(void);


class GuiExport FCAction : public QAction
{
  public:
    FCAction ( QObject * parent = 0, const char * name = 0, bool toggle = FALSE );
    FCAction ( const QString & text, const QIconSet & icon, const QString & menuText, int accel, QObject * parent, const char * name = 0, bool toggle = FALSE );
    FCAction ( const QString & text, const QString & menuText, int accel, QObject * parent, const char * name = 0, bool toggle = FALSE );
    virtual ~FCAction();

    virtual bool addTo(QWidget *);
};
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

	FCstring sName;

};

*/


enum CMD_Type { 
	Cmd_Normal=0,
	Cmd_Toggle=1
};


/** The CommandManager class
 *  This class manage all available commands in FreeCAD. All 
 *  Commands will registered here, also commands from Application
 *  modules. Also activation / deactivation, Icons Tooltips and so
 *  on are handles here. Further the Building of Toolbars and (Context) 
 *  menus is implemented here.
 *  @see FCCommand
 */
class GuiExport FCCommand :/*public FCPyObject,*/ public QObject
{
	//Py_Header;			// always start with Py_Header


	Q_OBJECT			// also a QT Object

public:

	FCCommand(const char* name,CMD_Type eType=Cmd_Normal);

	// Initialize the Action
	void Init(void);
	bool IsToggle(void);
	const char* Name(void);

	/// Helpers
	ApplicationWindow *AppWnd(void);

	// Profile methode, gives the command all the text and pixmap
	virtual void CmdProfile(char** sMenuText, char** sToolTipText, char** sWhatsThis, char** sStatusTip, QPixmap &cPixmap, int &iAccel)=0;
	/// Method which get called when activated, needs to be reimplemented!
	virtual void Activated(void){assert(_eType&Cmd_Toggle == 0);}
	/// Method which get called when toggled, needs to be reimplemented!
	virtual void Toogled  (bool){assert(_eType&Cmd_Toggle != 0);}
	
	friend FCCommandManager;

	// Helper Methodes
	FCGuiDocument* GetActiveDocument(void);
	FCDocument*	   GetActiveOCCDocument(void);

  FCAction* GetAction() { return _pcAction; }
  const char* GetName() { return _pcName; }

private slots:
	virtual void activated (); 
	virtual void toggled ( bool ); 


private:
	const char* _pcName;
	FCAction *_pcAction;
	CMD_Type _eType;
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
	void AddCommand(FCCommand* pCom);

	void AddTo(const char* Name,QWidget *pcWidget);

  const FCmap<FCstring, FCCommand*>& GetCommands() { return _sCommands; }

private:
#	pragma warning( disable : 4251 )
	FCmap<FCstring,FCCommand*> _sCommands;

};

/** The Command Macro Standard
 *  This macro makes it easyer to define a new command.
 *  The parameters are the class name and the command name
 */
//#ifndef aa__linux /* virtual does not compile */

#define DEF_STD_CMD(X,Y) class X : public FCCommand \
{\
public:\
	X():FCCommand(Y){}\
	virtual void CmdProfile(char** sMenuText, char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel);\
	virtual void Activated(void);\
};

/*#else

#define DEF_STD_CMD(X,Y) class X : public FCCommand \
{\
public:\
	X():FCCommand(Y){}\
	void CmdProfile(char** sMenuText, char** sToolTipText,char** sWhatsThis,char** sStatusTip,QPixmap &cPixmap,int &iAccel);\
	void Activated(void);\
};

#endif //__linux*/



#endif // __Command_h__
