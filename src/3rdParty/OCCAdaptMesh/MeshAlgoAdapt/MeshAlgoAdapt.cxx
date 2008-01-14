// File:	MeshAlgoAdapt.cxx
// Created:	Wed Dec 22 18:38:09 1993
// Author:	Didier PIFFAULT
//		<dpf@zerox>

#include <MeshAlgoAdapt.ixx>

Standard_EXPORT Standard_Boolean Triangulation_Chrono=Standard_False;
Standard_EXPORT Standard_Integer Triangulation_Trace=0;


void MeshAlgoAdapt::SetMesure(const Standard_Boolean val)
{
  Triangulation_Chrono=val;
}

void MeshAlgoAdapt::SetTrace(const Standard_Integer val)
{
  Triangulation_Trace=val;
}

