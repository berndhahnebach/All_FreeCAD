/***************************************************************************
 *   Copyright (c) 2009 Juergen Riegel  (FreeCAD@juergen-riegel.net>       *
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


#ifndef GUI_SelectionFilter_H
#define GUI_SelectionFilter_H

#include <string>
#include <CXX/Extensions.hxx>

namespace App {
    class DocumentObject;
}

namespace Gui {
    struct Node_Block;
    

/**
 * The Selection object class
 */
class GuiExport SelectionFilter 
{

public:
    /** Constructs a SelectionFilter object. */
    SelectionFilter(const char* filter);
    SelectionFilter(const std::string& filter);
    virtual ~SelectionFilter();

    bool match(void);

    void addError(const char* e);
 
    friend class SelectionSingleton;

    std::vector<std::vector<SelectionObject> > Result;

protected:

    std::string Filter;
    std::string Errors;
    bool parse(void);

    Node_Block *Ast;

};

/**
 * Python binding for SelectionFilter class.
 * @see SelectionFilter
 * @author Werner Mayer
 */
class SelectionFilterPy : public Py::PythonExtension<SelectionFilterPy> 
{
private:
    SelectionFilter filter;

public:
    static void init_type(void);    // announce properties and methods

    SelectionFilterPy(const std::string&);
    ~SelectionFilterPy();

    Py::Object repr();
    Py::Object match(const Py::Tuple&);
    Py::Object result(const Py::Tuple&);

private:
    static PyObject *PyMake(struct _typeobject *, PyObject *, PyObject *);
};

// === Abstract syntax tree (AST) ===========================================

struct Node_Slice 
{
    Node_Slice(int min=1,int max=INT_MAX):Min(min),Max(max){}
    int Min,Max;

};


struct Node_Object 
{
    Node_Object(std::string *namespc,std::string *type,Node_Slice* slc=0)
        :Slice(slc)
    {
        std::string type_name = *namespc + "::" + *type;
        ObjectType = Base::Type::fromName(type_name.c_str());
        delete (namespc);
        delete (type);
    }
    Base::Type ObjectType;
    Node_Slice  *Slice;

};

struct Node_Block 
{
    Node_Block(Node_Object* obj){Objects.push_back(obj);}
    std::vector< Node_Object *> Objects;
};


} // namespace Gui


#endif // GUI_SelectionFilter_H 
