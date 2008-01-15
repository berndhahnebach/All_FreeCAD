// File:	occcallbackAdapt.cxx
// Created:	Tue Jun 11 17:40:33 1996
// Author:	Open CASCADE Support
//		<support@opencascade.com>
// Copyright:    Open CASCADE 2003

#include <Standard_Failure.hxx>


extern "C"
{
  void occinternalerrorAdapt ()
  {
    Standard_Failure::Raise("Error in package Triangle");
  }
}
