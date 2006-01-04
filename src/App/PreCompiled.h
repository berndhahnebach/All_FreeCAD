#ifndef __PRECOMPILED__
#define __PRECOMPILED__

#include "../FCConfig.h"

#ifdef _PreComp_

/// here get the warnings of to long specifieres disabled (needet for VC6)
#ifdef _MSC_VER
#	pragma warning( disable : 4251 )
#	pragma warning( disable : 4503 )
#	pragma warning( disable : 4786 )  // specifier longer then 255 chars
#endif

// standard
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <time.h>

// STL 
#include <string>
#include <list>
#include <map>
#include <vector>
#include <set>
#include <stack>
#include <sstream>
#include <queue>

// OpenCasCade =====================================================================================
// Base
#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <Standard_AbortiveTransaction.hxx>
#include <Standard_Address.hxx>
#include <Standard_AncestorIterator.hxx>
#include <Standard_BasicMap.hxx>
#include <Standard_BasicMapIterator.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Byte.hxx>
#include <Standard_Character.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Container.hxx>
#include <Standard_CString.hxx>
#include <Standard_ctype.hxx>
#include <Standard_DBHandle.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_ExtCharacter.hxx>
#include <Standard_ExtString.hxx>
#include <Standard_Failure.hxx>
#include <Standard_ForMapOfTypes.hxx>
#include <Standard_GUID.hxx>
#include <Standard_ImmutableObject.hxx>
#include <Standard_Integer.hxx>
#include <Standard_InternalType.hxx>
#include <Standard_IStream.hxx>
#include <Standard_KindOfType.hxx>
#include <Standard_LicenseError.hxx>
#include <Standard_LicenseNotFound.hxx>
#include <Standard_Macro.hxx>
#include <Standard_MapOfTypes.hxx>
#include <Standard_math.hxx>
#include <Standard_MultiplyDefined.hxx>
#include <Standard_MyMapOfStringsHasher.hxx>
#include <Standard_MyMapOfTypes.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_NullValue.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_OId.hxx>
#include <Standard_OStream.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Overflow.hxx>
#include <Standard_Persistent.hxx>
#include <Standard_Persistent_proto.hxx>
#include <Standard_PForMapOfTypes.hxx>
#include <Standard_PrimitiveTypes.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Real.hxx>
#include <Standard_ShortReal.hxx>
#include <Standard_SStream.hxx>
#include <Standard_Static.hxx>
#include <Standard_Storable.hxx>
#include <Standard_Stream.hxx>
#include <Standard_String.hxx>
#include <Standard_theForMapOfTypes.hxx>
#include <Standard_TooManyUsers.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Transient_proto.hxx>
#include <Standard_Type.hxx>
#include <Standard_TypeDef.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_Underflow.hxx>
#include <Standard_UUID.hxx>
#include <Standard_WayOfLife.hxx>

 
#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfExtendedString.hxx>

#include <PCollection_HExtendedString.hxx>

// OCAF
#include <TDF_Label.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_Reference.hxx>
#include <TDF_Tool.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TDocStd_Application.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Comment.hxx>
#include <TDataStd_Point.hxx>
#include <TDataStd_Plane.hxx>
#include <TDataStd_Constraint.hxx>
#include <TDataStd_Axis.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_Directory.hxx>
#include <TDataStd_Geometry.hxx>
#include <TDataStd_ConstraintEnum.hxx>
#include <TDataStd_GeometryEnum.hxx>
#include <TDataStd_RealEnum.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Builder.hxx>
#include <TFunction_Logbook.hxx>
#include <TFunction_DriverTable.hxx>
#include <TFunction_Function.hxx>

#include <CDM_MessageDriver.hxx>
#include <MDF_ASDriver.hxx>
#include <PDF_Attribute.hxx>

#include <OSD_Process.hxx>

#include <Quantity_Date.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>


//#include <TDataStd_Reference.hxx>

#include <xercesc/util/XercesDefs.hpp>


#endif //_PreComp_

#endif
