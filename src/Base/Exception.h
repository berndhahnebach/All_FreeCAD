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
	std::string _sErrMsg;
};
   


inline void FCException::SetMessage(const char * sMessage)
{
  _sErrMsg = sMessage;  
}



#endif

