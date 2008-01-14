// File:	meshchrono.cxx
// Created:	Tue Aug 26 14:21:35 2003
// Author:	Open CASCADE Support
//		<support@opencascade.com>
// Copyright:	 Open CASCADE 2003

#include <meshchronoAdapt.hxx>

#ifdef DEB_MESH_CHRONO

Standard_Boolean AffichTriangles = Standard_False;

Standard_Integer D0Control, D0Internal, D0Unif, D0Edges, NbControls;
OSD_Chronometer chTotal, chInternal, chControl, chUnif, chAddPoint;
OSD_Chronometer chEdges, chMaillEdges, chEtuInter, chLastControl, chStock;
OSD_Chronometer chAdd11, chAdd12, chAdd2, chUpdate, chPointValid, chIsos, chPointsOnIsos;
OSD_Chronometer chPlan, chCyl, chCon, chSph, chTor, chBez, chBSpl, chRev, chExt, chOffset, chOther;

#endif // DEB_MESH_CHRONO
