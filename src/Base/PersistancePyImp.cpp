
// 
#include "PreCompiled.h"
#include "Writer.h"
#include "PersistancePy.h"
#include "Persistance.h"
using namespace Base;


Py::String PersistancePy::getContent(void)
{
  Base::StringWriter writer;
  getPersistanceObject()->Save(writer);

  return  Py::String (writer.getString());
}

void  PersistancePy::setContent(Py::String arg)
{

}

Py::Int PersistancePy::getMemSize(void)
{
  return Py::Int((long)getPersistanceObject()->getMemSize());
}

void  PersistancePy::setMemSize(Py::Int arg)
{
  
}

PyObject *PersistancePy::getCustomAttributes(const char* attr)
{
  return 0;
}

int PersistancePy::setCustomAttributes(const char*,PyObject *obj)
{
  return 0; 
}

