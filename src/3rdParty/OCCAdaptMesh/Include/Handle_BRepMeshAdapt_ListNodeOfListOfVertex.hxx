// File generated by CPPExt (Transient)
//
//                     Copyright (C) 1991 - 2000 by  
//                      Matra Datavision SA.  All rights reserved.
//  
//                     Copyright (C) 2001 - 2004 by
//                     Open CASCADE SA.  All rights reserved.
// 
// This file is part of the Open CASCADE Technology software.
//
// This software may be distributed and/or modified under the terms and
// conditions of the Open CASCADE Public License as defined by Open CASCADE SA
// and appearing in the file LICENSE included in the packaging of this file.
//  
// This software is distributed on an "AS IS" basis, without warranty of any
// kind, and Open CASCADE SA hereby disclaims all such warranties,
// including without limitation, any warranties of merchantability, fitness
// for a particular purpose or non-infringement. Please see the License for
// the specific terms and conditions governing rights and limitations under the
// License.

#ifndef _Handle_BRepMeshAdapt_ListNodeOfListOfVertex_HeaderFile
#define _Handle_BRepMeshAdapt_ListNodeOfListOfVertex_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Handle_TCollection_MapNode_HeaderFile
#include <Handle_TCollection_MapNode.hxx>
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(TCollection_MapNode);
class BRepMeshAdapt_ListNodeOfListOfVertex;
Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(BRepMeshAdapt_ListNodeOfListOfVertex);

class Handle(BRepMeshAdapt_ListNodeOfListOfVertex) : public Handle(TCollection_MapNode) {
  public:
    Handle(BRepMeshAdapt_ListNodeOfListOfVertex)():Handle(TCollection_MapNode)() {} 
    Handle(BRepMeshAdapt_ListNodeOfListOfVertex)(const Handle(BRepMeshAdapt_ListNodeOfListOfVertex)& aHandle) : Handle(TCollection_MapNode)(aHandle) 
     {
     }

    Handle(BRepMeshAdapt_ListNodeOfListOfVertex)(const BRepMeshAdapt_ListNodeOfListOfVertex* anItem) : Handle(TCollection_MapNode)((TCollection_MapNode *)anItem) 
     {
     }

    Handle(BRepMeshAdapt_ListNodeOfListOfVertex)& operator=(const Handle(BRepMeshAdapt_ListNodeOfListOfVertex)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(BRepMeshAdapt_ListNodeOfListOfVertex)& operator=(const BRepMeshAdapt_ListNodeOfListOfVertex* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    BRepMeshAdapt_ListNodeOfListOfVertex* operator->() const
     {
      return (BRepMeshAdapt_ListNodeOfListOfVertex *)ControlAccess();
     }

//   Standard_EXPORT ~Handle(BRepMeshAdapt_ListNodeOfListOfVertex)();
 
   Standard_EXPORT static const Handle(BRepMeshAdapt_ListNodeOfListOfVertex) DownCast(const Handle(Standard_Transient)& AnObject);
};
#endif
