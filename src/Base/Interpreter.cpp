

#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <python.h>
#endif

#include "Console.h"
#include "Interpreter.h"
#include "PyTools.h"
#include "Exception.h"


char format2[1024];  //Warning! Can't go over 512 characters!!! 


FCInterpreter::FCInterpreter()
{
	Py_Initialize(); 
}

FCInterpreter::~FCInterpreter()
{

}


void FCInterpreter::Launch(const char *psCmd)
{
	int i = strlen(psCmd);
	char *sBuf = (char*) malloc(i+2);
	strcpy(sBuf,psCmd);

	int ret = PyRun_SimpleString(sBuf);

	free (sBuf);

	if(ret == -1) throw FCException("script fails");
}

void FCInterpreter::LaunchFile(const char*pxFileName)
{
	static char sBuf[_MAX_PATH];
	strcpy(sBuf,pxFileName);
	
	FILE *fp = fopen(sBuf,"r");
	if(fp == NULL) 
		throw FCException("File not found");
	PyRun_SimpleFile(fp,sBuf);
}

bool FCInterpreter::LoadModule(const char* psModName)
{
	// buffer acrobatics
	static char sBuf[_MAX_PATH];
	strcpy(sBuf,psModName);

	if(!PP_Load_Module(sBuf))
		return false;

	return true;
}

void FCInterpreter::RunFCCommand(const char * psCom,...)
{
    va_list namelessVars;
    va_start(namelessVars, psCom);  // Get the "..." vars
    vsprintf(format2, psCom, namelessVars);
    va_end(namelessVars);
	assert(strlen(psCom) < 800);
	GetConsole().Log("Run Com: %s\n",format2);
	Launch(format2);
}


// Singelton:

FCInterpreter * FCInterpreter::_pcSingelton = 0;

FCInterpreter & FCInterpreter::Instance(void)
{
	// not initialized!
	if(!_pcSingelton)
		_pcSingelton = new FCInterpreter();
	return *_pcSingelton;
}

void FCInterpreter::Destruct(void)
{
	// not initialized or doubel destruct!
	assert(_pcSingelton);
	delete _pcSingelton;
}

void FCInterpreter::SetComLineArgs(int argc,char *argv[])
{
	PySys_SetArgv(argc, argv);
}

int FCInterpreter::RunCommandLine(char *prompt)
{
	return PP_Run_Command_Line(prompt);
}

/*
void FCInterpreter::Register(FCPythonExport *pcPyExporter)
{
	pcPyExporter->PyRegister();
}
*/
