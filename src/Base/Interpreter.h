#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

// Std. configurations
#include "Export.h"

class BaseExport FCInterpreter
{
public:
	FCInterpreter();
	~FCInterpreter();

	void Launch(const char *psCmd);
	void LaunchFile(const char*pxFileName);
	bool LoadModule(const char* psModName);

	void RunFCCommand(const char * psCom,...);

//	void Register(FCPythonExport *pcPyExporter);
	void SetComLineArgs(int argc,char *argv[]);
	int  RunCommandLine(char *prompt);

	// singelton
	static FCInterpreter &Instance(void);
	static void Destruct(void);
//	static void Init(void);

protected:

	// singelton
	static FCInterpreter *_pcSingelton;
	friend FCInterpreter &GetInterpreter(void); 


};


inline FCInterpreter &GetInterpreter(void)
{
	return FCInterpreter::Instance();
}


#endif
