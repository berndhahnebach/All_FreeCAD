
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

FCParameter::sValue &FCParameter::GetValue(const char *Key)
{
	int pos=-1;
	char sBuf[512];
	// kritical buffer overun
	assert(strlen(Key)<511);

	// search for the last slash
	for(int i=0;Key[i]!=0;i++)
		if(Key[i]=='/')pos=i;

	if(pos==-1) // with no slash in Key
		return mData["_MAIN"][Key];

	if(pos==0) // with a starting slash
	{
		// copy without slash
		strcpy(sBuf,Key+1);
		return mData["_MAIN"][sBuf];
	}

	if(pos>0) // with a slash inside
	{
		// extracting name
		char name[200];
		strcpy(name,Key+pos);

		// extrakting group
		char group[512];
		for (int l=0;l<pos;l++)
			group[l]=Key[l];
		group[l]=0;

		return mData[group][name];
	}

}

bool FCParameter::AddKeyLong(const char *Key, long lWert)
{
	sValue sVal= GetValue(Key);

	sVal.Value.Long = lWert;
	sVal.Type = sValue::Long;

    return true;
}

bool FCParameter::AddKeyString(const char *Key, const char *Wert)
{

    return true;
}

bool FCParameter::AddKeyFloat(const char *Key, float dWert)
{

    return true;
}

bool FCParameter::DeleteKey (const char *rclKey)
{
  return true;
}

bool FCParameter::DeleteKeyFromString (const char *rclGroupKey, const char *rclValue)
{
  return false;
}

const char *FCParameter::FindKeyFromString (const char *rclGroupKey, const char *rclValue) const
{
  return "";
}

bool FCParameter::SetKeyLong(const char *Key, long lWert)
{
	sValue sVal= GetValue(Key);
	assert(sVal.Type == sValue::Long);
	sVal.Value.Long = lWert;
	
    return true;
}

bool FCParameter::SetKeyString(const char *Key, const char *sWert)
{
    return true;
}

bool FCParameter::SetKeyFloat(const char *Key, float dWert)
{
    return true;
}

long      FCParameter::GetKeyLong(const char *Key, long lDef)
{
	sValue sVal= GetValue(Key);
	assert(sVal.Type == sValue::Long);
	return sVal.Value.Long;
	
}

const char *FCParameter::GetKeyString(const char *Key,const char *sDef)
{
	return sDef;
}

float    FCParameter::GetKeyFloat(const char *Key,float fDef)
{
	return fDef;
}
unsigned long FCParameter::GetGroup(const char *cKey,FCParameter &cPara) 
{
    unsigned long ulSize=0;
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

