// Definitionsmodul
// erstellt von INNOVATOR 6.1

#include "../Config.h"
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#endif

#include "Exception.h"
#include "Console.h"


FCException::FCException(void)
{
  _sErrMsg = "FreeCAD Exception";
}

FCException::FCException(const FCException &inst)
{
	SetMessage(inst._sErrMsg.c_str());
}


FCException::FCException(const char * sMessage)
{
  SetMessage(sMessage);
  ReportException();
}


FCException &FCException::operator=(const FCException &inst)
{
  SetMessage(inst._sErrMsg.c_str());
  return *this;
}

const char* FCException::what(void)
{
	return _sErrMsg.c_str();
}

void FCException::ReportException (void)
{
	GetConsole().Error("Exception (%s): %s \n",GetConsole().Time(),what());
}
