#ifndef _RSEXCEPTION_HXX
#define _RSEXCEPTION_HXX

#include <exception>
#include "Export.h"
#include "string"



class BaseExport FCException: public exception
{      
public:
	FCException(const char * sMessage);
    FCException(void);
    FCException(const FCException &inst);

	FCException &operator=(const FCException &inst);
  
    const char* what(void);
  
    void ReportException (void);

    inline void SetMessage(const char * sMessage);
  
protected:
#	pragma warning( disable : 4251 )
	stlport::string _sErrMsg;
#	pragma warning( default : 4251 )
};
   


inline void FCException::SetMessage(const char * sMessage)
{
  _sErrMsg = sMessage;  
}



#endif

