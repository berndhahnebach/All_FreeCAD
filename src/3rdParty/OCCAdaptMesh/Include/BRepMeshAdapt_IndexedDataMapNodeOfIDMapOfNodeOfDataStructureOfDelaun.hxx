// File generated by CPPExt (Transient)
//
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

#ifndef _BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun_HeaderFile
#define _BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun_HeaderFile

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Handle_BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun_HeaderFile
#include <Handle_BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun.hxx>
#endif

#ifndef _BRepMeshAdapt_Vertex_HeaderFile
#include <BRepMeshAdapt_Vertex.hxx>
#endif
#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _TColStd_ListOfInteger_HeaderFile
#include <TColStd_ListOfInteger.hxx>
#endif
#ifndef _TCollection_MapNodePtr_HeaderFile
#include <TCollection_MapNodePtr.hxx>
#endif
#ifndef _TCollection_MapNode_HeaderFile
#include <TCollection_MapNode.hxx>
#endif
class BRepMeshAdapt_Vertex;
class TColStd_ListOfInteger;
class BRepMeshAdapt_NodeHasherOfDataStructureOfDelaun;
class BRepMeshAdapt_IDMapOfNodeOfDataStructureOfDelaun;



class BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun : public TCollection_MapNode {

public:
 // Methods PUBLIC
 // 

BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun(const BRepMeshAdapt_Vertex& K1,const Standard_Integer K2,const TColStd_ListOfInteger& I,const TCollection_MapNodePtr& n1,const TCollection_MapNodePtr& n2);

  BRepMeshAdapt_Vertex& Key1() const;

  Standard_Integer& Key2() const;

  TCollection_MapNodePtr& Next2() const;

  TColStd_ListOfInteger& Value() const;
//Standard_EXPORT ~BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun();




 // Type management
 //
 Standard_EXPORT const Handle(Standard_Type)& DynamicType() const;
 //Standard_EXPORT Standard_Boolean	       IsKind(const Handle(Standard_Type)&) const;

protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //
BRepMeshAdapt_Vertex myKey1;
Standard_Integer myKey2;
TColStd_ListOfInteger myValue;
TCollection_MapNodePtr myNext2;


};

#define TheKey BRepMeshAdapt_Vertex
#define TheKey_hxx <BRepMeshAdapt_Vertex.hxx>
#define TheItem TColStd_ListOfInteger
#define TheItem_hxx <TColStd_ListOfInteger.hxx>
#define Hasher BRepMeshAdapt_NodeHasherOfDataStructureOfDelaun
#define Hasher_hxx <BRepMeshAdapt_NodeHasherOfDataStructureOfDelaun.hxx>
#define TCollection_IndexedDataMapNode BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun
#define TCollection_IndexedDataMapNode_hxx <BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun.hxx>
#define Handle_TCollection_IndexedDataMapNode Handle_BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun
#define TCollection_IndexedDataMapNode_Type_() BRepMeshAdapt_IndexedDataMapNodeOfIDMapOfNodeOfDataStructureOfDelaun_Type_()
#define TCollection_IndexedDataMap BRepMeshAdapt_IDMapOfNodeOfDataStructureOfDelaun
#define TCollection_IndexedDataMap_hxx <BRepMeshAdapt_IDMapOfNodeOfDataStructureOfDelaun.hxx>

#include <TCollection_IndexedDataMapNode.lxx>

#undef TheKey
#undef TheKey_hxx
#undef TheItem
#undef TheItem_hxx
#undef Hasher
#undef Hasher_hxx
#undef TCollection_IndexedDataMapNode
#undef TCollection_IndexedDataMapNode_hxx
#undef Handle_TCollection_IndexedDataMapNode
#undef TCollection_IndexedDataMapNode_Type_
#undef TCollection_IndexedDataMap
#undef TCollection_IndexedDataMap_hxx


// other Inline functions and methods (like "C++: function call" methods)
//


#endif
