#include "PreCompiled.h"
#include "Exception.h"
namespace Swig_1_3_33 {
#include "swigpyrun_1.3.33.h"
}

int createSWIGPointerObj_1_3_33(const char* TypeName, void* obj, PyObject** ptr, int own)
{
    Swig_1_3_33::swig_module_info *module = Swig_1_3_33::SWIG_GetModule(NULL);
    if (!module)
        return 1;

    Swig_1_3_33::swig_type_info * swig_type = 0;
    swig_type = Swig_1_3_33::SWIG_TypeQuery(TypeName);
    if (!swig_type)
        throw Base::Exception("Cannot find type information for requested type");
    
    *ptr = Swig_1_3_33::SWIG_Python_NewPointerObj(obj,swig_type,own);
    if (*ptr == 0)
        throw Base::Exception("Cannot convert into requested type");

    // success
    return 0;
}

int convertSWIGPointerObj_1_3_33(const char* TypeName, PyObject* obj, void** ptr, int flags)
{
    Swig_1_3_33::swig_module_info *module = Swig_1_3_33::SWIG_GetModule(NULL);
    if (!module)
        return 1;

    Swig_1_3_33::swig_type_info * swig_type = 0;
    swig_type = Swig_1_3_33::SWIG_TypeQuery(TypeName);
    if (!swig_type)
        throw Base::Exception("Cannot find type information for requested type");

    // return value of 0 is on success
    if (Swig_1_3_33::SWIG_ConvertPtr(obj, ptr, swig_type, flags))
        throw Base::Exception("Cannot convert into requested type");

    // success
    return 0;
}
