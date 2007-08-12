
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

PyObject*  PropertyContainerPy::getTypeOfProperty(PyObject *args)
{
  Py::List ret;
	char *pstr;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  Property *prop = getPropertyContainerObject()->getPropertyByName(pstr);
  short Type =  getPropertyContainerObject()->getPropertyType(prop);

  if(Type & Prop_Hidden) 
    ret.append(Py::String("Hidden"));

  return Py::new_reference_to(ret);
}

PyObject*  PropertyContainerPy::getGroupOfProperty(PyObject *args)
{
	char *pstr;
  if (!PyArg_ParseTuple(args, "s", &pstr))     // convert args: Python->C 
    return NULL;                             // NULL triggers exception 

  Property *prop = getPropertyContainerObject()->getPropertyByName(pstr);
  const char* Group = getPropertyContainerObject()->getPropertyGroup(prop);
  if(Group)
    return Py::new_reference_to(Py::String(Group));
  else
    return Py::new_reference_to(Py::String(""));
}

Py::List PropertyContainerPy::getPropertiesList(void) const
{
  Py::List ret;
  std::map<std::string,Property*> Map;

  getPropertyContainerObject()->getPropertyMap(Map);

  for(std::map<std::string,Property*>::const_iterator It=Map.begin();It!=Map.end();++It)
    ret.append(Py::String(It->first));

  return ret;
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


