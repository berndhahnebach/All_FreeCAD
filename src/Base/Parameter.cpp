
#ifdef _PreComp_
#	include "PreCompiled.h"
#else
#	include <assert.h>
#endif


#include "Parameter.h"


// ////////////////////////////////////////////////////////////////////
// Construction/Destruction
// ////////////////////////////////////////////////////////////////////



FCParameter::~FCParameter(void)
{

}


bool FCParameter::AddKeyLong(const char *Key, long lWert)
{
    // Key schon da?
    assert(mData.find(Key)==mData.end());
    char buff[34];
    sprintf(buff,"%d",lWert);
    mData.operator[](Key) = buff;

    return true;
}

bool FCParameter::AddKeyString(const char *Key, const char *Wert)
{
    // Key schon da?
    assert(mData.find(Key)==mData.end());
    mData.operator[](Key) = Wert;

    return true;
}

bool FCParameter::AddKeyFloat(const char *Key, float dWert)
{
    // Key schon da?
    assert(mData.find(Key)==mData.end());
    char buff[34];
    sprintf(buff,"%f",dWert);
    mData.operator[](Key) = buff;

    return true;
}

bool FCParameter::DeleteKey (const char *rclKey)
{
  return mData.erase(rclKey) != 0;
}

bool FCParameter::DeleteKeyFromString (const char *rclGroupKey, const char *rclValue)
{
  std::string clKey = FindKeyFromString(rclGroupKey, rclValue);
  if (clKey.empty() == false)
    return DeleteKey(clKey.c_str());
  return false;
}

const char *FCParameter::FindKeyFromString (const char *rclGroupKey, const char *rclValue) const
{
  for (std::map<std::string,std::string>::const_iterator pI = mData.begin(); pI != mData.end(); pI++)
  {
	  std::string::size_type nPos = pI->first.find_first_of(".");
    if ((pI->first.substr(0, nPos) == rclGroupKey) && (pI->second == rclValue))
      return pI->first.c_str();
  }
  return "";
}

bool FCParameter::SetKeyLong(const char *Key, long lWert)
{
    char buff[34];
    sprintf(buff,"%d",lWert);
    mData.operator[](Key) = buff;

    return true;
}

bool FCParameter::SetKeyString(const char *Key, const char *sWert)
{
    mData.operator[](Key) = sWert;

    return true;
}

bool FCParameter::SetKeyFloat(const char *Key, float dWert)
{
   // Key schon da?
    char buff[34];
    sprintf(buff,"%f",dWert);
    mData.operator[](Key) = buff;

    return true;
}

long      FCParameter::GetKeyLong(const char *Key, long lDef)
{
    if(mData.find(Key)!=mData.end()){
        return atoi(mData.operator[](Key).c_str());
    }else{
        SetKeyLong(Key, lDef);
        return lDef;
    }
}

const char *FCParameter::GetKeyString(const char *Key,const char *sDef)
{
    if(mData.find(Key)!=mData.end()){
        return mData.operator[](Key).c_str();
    }else{
        SetKeyString(Key, sDef);
        return sDef;
    }
}

float    FCParameter::GetKeyFloat(const char *Key,float fDef)
{
    if(mData.find(Key)!=mData.end()){
        return float(atof(mData.operator[](Key).c_str()));
    }else{
        SetKeyFloat(Key, fDef);
        return fDef;
    }
}
unsigned long FCParameter::GetGroup(const char *cKey,FCParameter &cPara) 
{
    int s=strlen(cKey);
    unsigned long ulSize=0;

	for(std::map<std::string,std::string>::iterator It=mData.begin();It!=mData.end();It++)
        if(!(It->first).compare(0,s,cKey))
		{
            cPara.mData[std::string(It->first,s,It->first.size())] = It->second;
			ulSize++;
		}
    return ulSize;
}

void FCParameter::SetGroup(const char *sGrp, const FCParameter &cGrp)
{
}

void FCParameter::DelGroup(const char *sGrp)
{
}

unsigned long FCParameter::Print(std::string &cBuf)
{
    /*
    RSmap<std::string,std::string> cMap;
    RScopy(begin(),end(),cMap.begin());
    RSsort(cMap.begin(),cMap.end());
*/

    return 0;
}



void FCParameter::PyRegister()
{

}

void FCParameter::PyInstance()
{

}

PYFUNCIMP( FCParameter, SetKeyString )
{
	return Py_None;                              /* None: no errors */

}

PYFUNCIMP( FCParameter, GetKeyString )
{
	return Py_None;                              /* None: no errors */

}

