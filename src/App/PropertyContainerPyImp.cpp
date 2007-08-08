
#include "PreCompiled.h"

#include "PropertyContainer.h"
#include "Property.h"

// inclution of the generated files (generated out of PropertyContainerPy.xml)
#include "PropertyContainerPy.h"
#include "PropertyContainerPy.cpp"

using namespace App;

// returns a string which represent the object e.g. when printed in python
const char *PropertyContainerPy::representation(void)
{
	return "PropertyContainerPy";
}


Py::List PropertyContainerPy::getPropertiesList(void) const
{
	return Py::List();
}


PyObject *PropertyContainerPy::getCustomAttributes(const char* attr) const
{
        // search in PropertyList
  Property *prop = getPropertyContainerObject()->getPropertyByName(attr);
  if(prop)
  {
    return prop->getPyObject();
  } else if (Base::streq(attr, "__dict__")) {
    // get the properties to the C++ PropertyContainer class
    std::map<std::string,App::Property*> Map;
    getPropertyContainerObject()->getPropertyMap(Map);
    PyObject *dict = PyDict_New();
    if (dict) { 
      for ( std::map<std::string,App::Property*>::iterator it = Map.begin(); it != Map.end(); ++it )
        PyDict_SetItem(dict, PyString_FromString(it->first.c_str()), PyString_FromString(""));
      if (PyErr_Occurred()) { Py_DECREF(dict);dict = NULL;}
    }
    return dict;
  }

	return 0;
}

int PropertyContainerPy::setCustomAttributes(const char* attr, PyObject *obj)
{
	   // search in PropertyList
  Property *prop = getPropertyContainerObject()->getPropertyByName(attr);
  if (prop) {
    prop->setPyObject(obj);
    return 1;
  } 

  return 0;
}


