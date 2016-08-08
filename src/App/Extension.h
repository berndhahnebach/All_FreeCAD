/***************************************************************************
 *   Copyright (c) Stefan Tröger          (stefantroeger@gmx.net) 2016     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef APP_EXTENSION_H
#define APP_EXTENSION_H

#include "PropertyContainer.h"
#include "PropertyPythonObject.h"
#include "Base/Interpreter.h"
#include <CXX/Objects.hxx>

namespace App {
    
/**
 * @brief Base class for all extension that can be added to a DocumentObject
 * 
 * For general documentation on why extension system exists and how to use it see the ExtensionContainer 
 * documentation. Following is a description howto create custom extensions. 
 * 
 * Extensions are like every other FreeCAD object and based on properties. All information storage 
 * and persistance should be achieved by use of those. Additional any number of methods can be 
 * added to provide funtionality around the proerties. The only difference to normal objects is that
 * extensions must derive from the Extension class and that the tye needs to be initialised. This 
 * works as simple as 
 * @code
 * class MyExtension : public Extension {
 *   virtual bool overridableMethod(DocumentObject* obj) {};
 * };
 * 
 * MyExtension::MyExtension() {
 *     initExtension(MyExtension::getClassTypeId());
 * }
 * typedef ExtensionPythonT<MyExtension> MyExtensionPython;
 * @endcode 
 *  
 * The special python extension type created above is important, as only those python extensions 
 * can be added to an object from python. It does not work to add the c++ version directly there. 
 * 
 * Note that every method of the extension becomes part of the extendded object when added from c++. 
 * This means one should carefully design the API and make only neccessary methods public or protected.
 * Every internal method should be private.
 * 
 * The automatic availibility of methods in the class does not hold for the python interface, only 
 * for c++ classes. This is like every where else in FreeCAD, there is no automatic creation of python 
 * API from c++ classes. Hence the extension creator must also create a custom python object of its 
 * extension, which works exactly like the normal FreeCAD python object workflow. There is nothing 
 * special at all for extension python objects, the normal xml + imp.cpp approach is used. It must 
 * only be taken care that the objects father is the correct extension base class. Of course also 
 * makse sure your extension returns the correct python ojbect in its "getPyObject" call.
 * Every method you create in the extensions python will be later added to an extended object. This 
 * happens automatically for both, c++ and python extension, if "getPyObject" returns the correct 
 * python object. No extra work needs to be done.
 * 
 * A special case that needs to be handled for extensions is the possibility of overriden methods. 
 * Often it is desired to customise extension behaviour by allowing the user to override methods 
 * provided by the extension. On c++ side this is trival, such methods are simply marked as "virtual" 
 * and can than be overriden in any derived class. This is more involved for the python interface and
 * here special care needs to be taken. 
 * 
 * As already seen above one needs to create a special ExtensionPythonT<> object for extension from 
 * python. This is done exactly for the purpose of allowing to have overridable methods. The 
 * ExtensionPythonT wrapper adds a proxy property which holds a PyObject which itself will contain 
 * the implementations for the overridden methods. This design is equal to the ObjectPythonT<> design 
 * of normal document objects. 
 * As this wrapper inherits the c++ extension class it can also override the virtual functions the 
 * user designed to be overridden. What it should do at a call of the virtual method is to check if 
 * this method is implemented in the proxy object and if so call it, and if not call the normal 
 * c++ version. It is the extensions creators responsibility to implement this check and call behaviour
 * for every overridable method.
 * This is done by creating a custom wrapper just like ExtensionPythonT<> and overriding all virtual 
 * methods.
 * @code
 * template<typename ExtensionT> class MyExtensionPythonT : public ExtensionT {
 * public:
 *   
 *   MyExtensionPythonT() {}
 *   virtual ~MyExtensionPythonT() {}
 *
 *   virtual bool overridableMethod(DocumentObject* obj)  override {
 *       Py::Object pyobj = Py::asObject(obj->getPyObject());
 *       EXTENSION_PROXY_ONEARG(allowObject, pyobj);
 *               
 *       if(result.isNone())
 *           ExtensionT::allowObject(obj);
 *       
 *       if(result.isBoolean())
 *           return result.isTrue();
 *       
 *       return false;
 *   };
 * };
 * @endcode
 * @Note As seen in the code there are multiple helper macros to ease the repetitive work of querying 
 * and calling methods of the proxy object. See the maco documentation for howto use them.
 * 
 * To ensure that your wrapper is used when a extension is created from python the extension type must 
 * be exposed as follows:
 * @code
 * typedef ExtensionPythonT<MyExtensionPythonT<MyExtension>> MyExtensionPython;
 * @endcode
 * 
 * This boilerplate is absolutely nesseccary to allow overridable methods in python and it is the 
 * exension creators responsibility to ensure full implementation.
 * 
 */
class AppExport Extension : public virtual App::PropertyContainer
{

  //The cass does not have properties itself, but it is important to provide the property access
  //functions. see cpp file for details
  PROPERTY_HEADER(App::Extension);

public:

  Extension();
  virtual ~Extension();

  void initExtension(App::DocumentObject* obj);
    
  App::DocumentObject*       getExtendedObject() {return m_base;};
  const App::DocumentObject* getExtendedObject() const {return m_base;};
 
  //get extension name without namespace
  const char* name();

  bool isPythonExtension() {return m_isPythonExtension;};
  
  virtual PyObject* getExtensionPyObject(void);

protected:     
  void initExtension(Base::Type type);
  bool m_isPythonExtension = false;
  Py::Object ExtensionPythonObject;
  
private:
  Base::Type           m_extensionType;
  App::DocumentObject* m_base = nullptr;
};


/**
 * Generic Python extension class which allows to behave every extension
 * derived class as Python extension -- simply by subclassing.
 */
template <class ExtensionT>
class ExtensionPythonT : public ExtensionT
{
    PROPERTY_HEADER(App::ExtensionPythonT<ExtensionT>);

public:
    typedef ExtensionT Inherited;
    
    ExtensionPythonT() {
        ExtensionT::m_isPythonExtension = true;
        
        ADD_PROPERTY(Proxy,(Py::Object()));
    }
    virtual ~ExtensionPythonT() {
    }

    PropertyPythonObject Proxy;
};

typedef ExtensionPythonT<App::Extension> ExtensionPython;

//helper macros to define python extensions
#define EXTENSION_PROXY_FIRST(function) \
    Base::PyGILStateLocker lock;\
    Py::Object result;\
    try {\
        Property* proxy = this->getPropertyByName("Proxy");\
        if (proxy && proxy->getTypeId() == PropertyPythonObject::getClassTypeId()) {\
            Py::Object feature = static_cast<PropertyPythonObject*>(proxy)->getValue();\
            if (feature.hasAttr(std::string("function"))) {\
                if (feature.hasAttr("__object__")) {\
                    Py::Callable method(feature.getAttr(std::string("function")));
                    
                    
                    

#define EXTENSION_PROXY_SECOND(function)\
                    result = method.apply(args);\
                }\
                else {\
                    Py::Callable method(feature.getAttr(std::string("function")));
                    
#define EXTENSION_PROXY_THIRD()\
                    result = method.apply(args);\
                }\
            }\
        }\
    }\
    catch (Py::Exception&) {\
        Base::PyException e;\
        e.ReportException();\
    }
    
#define EXTENSION_PROXY_NOARG(function)\
    EXTENSION_PROXY_FIRST(function) \
    Py::Tuple args;\
    EXTENSION_PROXY_SECOND(function) \
    Py::Tuple args(1);\
    args.setItem(0, Py::Object(this->getExtensionPyObject(), true));\
    EXTENSION_PROXY_THIRD()

#define EXTENSION_PROXY_ONEARG(function, arg)\
    EXTENSION_PROXY_FIRST(function) \
    Py::Tuple args;\
    args.setItem(0, arg); \
    EXTENSION_PROXY_SECOND(function) \
    Py::Tuple args(2);\
    args.setItem(0, Py::Object(this->getExtensionPyObject(), true));\
    args.setItem(1, arg); \
    EXTENSION_PROXY_THIRD()

#define EXTENSION_PYTHON_OVERRIDE_VOID_NOARGS(function)\
    virtual void function() override {\
        EXTENSION_PROXY_NOARGS(function)\
    };
    
#define EXTENSION_PYTHON_OVERRIDE_OBJECT_NOARGS(function)\
    virtual PyObject* function() override {\
        EXTENSION_PROXY_NOARGS(function)\
        return res.ptr();\
    };
    
}; //App

#endif // APP_EXTENSION_H
