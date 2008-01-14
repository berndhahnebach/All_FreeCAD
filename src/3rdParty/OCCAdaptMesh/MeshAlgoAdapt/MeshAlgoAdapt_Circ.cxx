// File:	MeshAlgoAdapt_Circ.cxx
// Created:	Mon Aug  9 17:48:36 1993
// Author:	Didier PIFFAULT
//		<dpf@zerox>

#include <MeshAlgoAdapt_Circ.ixx>

MeshAlgoAdapt_Circ::MeshAlgoAdapt_Circ()
{}

MeshAlgoAdapt_Circ::MeshAlgoAdapt_Circ(const gp_XY& loc, const Standard_Real rad)
     : location(loc), radius(rad)
{}

void MeshAlgoAdapt_Circ::SetLocation(const gp_XY& loc)
{
  location=loc;
}

void MeshAlgoAdapt_Circ::SetRadius(const Standard_Real rad)
{
  radius=rad;
}
