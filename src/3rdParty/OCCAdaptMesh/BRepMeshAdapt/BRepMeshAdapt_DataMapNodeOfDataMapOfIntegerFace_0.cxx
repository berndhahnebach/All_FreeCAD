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

#include <BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace.hxx>

#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _TopoDS_Face_HeaderFile
#include <TopoDS_Face.hxx>
#endif
#ifndef _TColStd_MapIntegerHasher_HeaderFile
#include <TColStd_MapIntegerHasher.hxx>
#endif
#ifndef _BRepMeshAdapt_DataMapOfIntegerFace_HeaderFile
#include <BRepMeshAdapt_DataMapOfIntegerFace.hxx>
#endif
#ifndef _BRepMeshAdapt_DataMapIteratorOfDataMapOfIntegerFace_HeaderFile
#include <BRepMeshAdapt_DataMapIteratorOfDataMapOfIntegerFace.hxx>
#endif
//BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace::~BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace() {}
 


Standard_EXPORT Handle_Standard_Type& BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace_Type_()
{

    static Handle_Standard_Type aType1 = STANDARD_TYPE(TCollection_MapNode);
  static Handle_Standard_Type aType2 = STANDARD_TYPE(MMgt_TShared);
  static Handle_Standard_Type aType3 = STANDARD_TYPE(Standard_Transient);
 

  static Handle_Standard_Transient _Ancestors[]= {aType1,aType2,aType3,NULL};
  static Handle_Standard_Type _aType = new Standard_Type("BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace",
			                                 sizeof(BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace),
			                                 1,
			                                 (Standard_Address)_Ancestors,
			                                 (Standard_Address)NULL);

  return _aType;
}


// DownCast method
//   allow safe downcasting
//
const Handle(BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace) Handle(BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace)::DownCast(const Handle(Standard_Transient)& AnObject) 
{
  Handle(BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace) _anOtherObject;

  if (!AnObject.IsNull()) {
     if (AnObject->IsKind(STANDARD_TYPE(BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace))) {
       _anOtherObject = Handle(BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace)((Handle(BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace)&)AnObject);
     }
  }

  return _anOtherObject ;
}
const Handle(Standard_Type)& BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace::DynamicType() const 
{ 
  return STANDARD_TYPE(BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace) ; 
}
//Standard_Boolean BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace::IsKind(const Handle(Standard_Type)& AType) const 
//{ 
//  return (STANDARD_TYPE(BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace) == AType || TCollection_MapNode::IsKind(AType)); 
//}
//Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace::~Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace() {}
#define TheKey Standard_Integer
#define TheKey_hxx <Standard_Integer.hxx>
#define TheItem TopoDS_Face
#define TheItem_hxx <TopoDS_Face.hxx>
#define Hasher TColStd_MapIntegerHasher
#define Hasher_hxx <TColStd_MapIntegerHasher.hxx>
#define TCollection_DataMapNode BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace
#define TCollection_DataMapNode_hxx <BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace.hxx>
#define TCollection_DataMapIterator BRepMeshAdapt_DataMapIteratorOfDataMapOfIntegerFace
#define TCollection_DataMapIterator_hxx <BRepMeshAdapt_DataMapIteratorOfDataMapOfIntegerFace.hxx>
#define Handle_TCollection_DataMapNode Handle_BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace
#define TCollection_DataMapNode_Type_() BRepMeshAdapt_DataMapNodeOfDataMapOfIntegerFace_Type_()
#define TCollection_DataMap BRepMeshAdapt_DataMapOfIntegerFace
#define TCollection_DataMap_hxx <BRepMeshAdapt_DataMapOfIntegerFace.hxx>
#include <TCollection_DataMapNode.gxx>

