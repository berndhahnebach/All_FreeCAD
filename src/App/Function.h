/** \file Function.h
 *  \brief FileTemplate example header
 *  \author $Author$
 *  \version $Revision$
 *  \date    $Date$
 *  Here a example of a file layout for FreeCAD.
 *  @see Parameter.cpp
 */

/***************************************************************************
 *   (c) Jürgen Riegel (juergen.riegel@web.de) 2002                        *   
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License (LGPL)   *
 *   as published by the Free Software Foundation; either version 2 of     *
 *   the License, or (at your option) any later version.                   *
 *   for detail see the LICENCE text file.                                 *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with FreeCAD; if not, write to the Free Software        * 
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Juergen Riegel 2002                                                   *
 ***************************************************************************/

#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <Standard.hxx>
#include <TFunction_Driver.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Macro.hxx>
#include <Standard.hxx>
#include <Handle_TFunction_Driver.hxx>

class TFunction_Logbook;
class Standard_Transient;
class Handle_Standard_Type;
class Handle(TFunction_Driver);

namespace App
{
class Function;
class Feature;


AppExport Handle_Standard_Type& STANDARD_TYPE(Function);


class Handle(Function) : public Handle(TFunction_Driver) {
  public:
    inline void* operator new(size_t,void* anAddress)    {return anAddress;}
    inline void* operator new(size_t size)               {return Standard::Allocate(size);}
    inline void  operator delete(void *anAddress)        {if (anAddress) Standard::Free((Standard_Address&)anAddress); }
//    inline void  operator delete(void *anAddress, size_t size) {if (anAddress) Standard::Free((Standard_Address&)anAddress,size); }
    Handle(Function)():Handle(TFunction_Driver)()      {} 
    Handle(Function)(const Handle(Function)& aHandle) : Handle(TFunction_Driver)(aHandle) {}
    Handle(Function)(const Function* anItem) : Handle(TFunction_Driver)((TFunction_Driver *)anItem){}
    Handle(Function)& operator=(const Handle(Function)& aHandle)
	{
      Assign(aHandle.Access());
      return *this;
     }

    Handle(Function)& operator=(const Function* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    Function* operator->(){return (Function *)ControlAccess();}
    Function* operator->() const {return (Function *)ControlAccess();}

	AppExport ~Handle(Function)();
	AppExport static const Handle(Function) DownCast(const Handle(Standard_Transient)& AnObject);
};



class Function : public TFunction_Driver 
{

public:

//	Feature *GetFeature(void){return _pcFeature;}

	/** @name methodes needet for OCAF */
	//@{
	/** new operator for the CasCade handle system 
	 */	
    inline void* operator new(size_t,void* anAddress){return anAddress;}
	/** new operator for the CasCade handle system 
	 */	
    inline void* operator new(size_t size){return Standard::Allocate(size);}
	/** delete operator for the CasCade handle system 
	 */	
    inline void  operator delete(void *anAddress){if (anAddress) Standard::Free((Standard_Address&)anAddress);}
//    inline void  operator delete(void *anAddress, size_t size) 
//      { 
//        if (anAddress) Standard::Free((Standard_Address&)anAddress,size); 
//      }
	//@}

	
	/** @name methodes overiden from TFunction_Driver */
	//@{
	// new operator for the CasCade handle system 
	AppExport static const Standard_GUID& GetID() ;
	AppExport Function(void);
	AppExport virtual  void Validate(TFunction_Logbook& log) const;
	AppExport virtual  Standard_Boolean MustExecute(const TFunction_Logbook& log) const;
	AppExport virtual  Standard_Integer Execute(TFunction_Logbook& log) const;
	AppExport ~Function();
	//@}




	/** @name Type management */
	//@{
	AppExport friend Handle_Standard_Type& Function_Type_();
	AppExport const Handle(Standard_Type)& DynamicType() const;
	AppExport Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;
	//@}

};



} //namespace App



#endif // _FUNCTION_H_
