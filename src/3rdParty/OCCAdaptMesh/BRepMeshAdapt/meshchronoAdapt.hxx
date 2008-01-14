// File:	meshchronoAdapt.hxx
// Created:	Tue Aug 26 14:21:35 2003
// Author:	Open CASCADE Support
//		<support@opencascade.com>
// Copyright:	 Open CASCADE 2003

#ifndef _meshchronoAdapt_HeaderFile
#define _meshchronoAdapt_HeaderFile

// Enable / disable mesh chrono
//#define DEB_MESH_CHRONO

#ifdef DEB_MESH_CHRONOAdapt

#include <OSD_Chronometer.hxx>
#include <GeomAbs_SurfaceType.hxx>

extern Standard_Boolean AffichTriangles;

extern Standard_Integer D0Control, D0Internal, D0Unif, D0Edges, NbControls;
extern OSD_Chronometer chTotal, chInternal, chControl, chUnif, chAddPoint;
extern OSD_Chronometer chEdges, chMaillEdges, chEtuInter, chLastControl, chStock;
extern OSD_Chronometer chAdd11, chAdd12, chAdd2, chUpdate, chPointValid, chIsos, chPointsOnIsos;
extern OSD_Chronometer chPlan, chCyl, chCon, chSph, chTor, chBez, chBSpl, chRev, chExt, chOffset, chOther;

#ifdef DRAW
#include <Draw.hxx>
#include <DrawTrSurf_Triangulation2D.hxx>
#endif // DRAW

inline void MESH_CHRONO_INITAdapt ()
{
  D0Control = D0Internal = D0Unif = D0Edges = NbControls = 0;
  chTotal.Reset(); chInternal.Reset(); chControl.Reset(); chUnif.Reset();
  chAddPoint.Reset(); chEdges.Reset(); chMaillEdges.Reset(); chEtuInter.Reset();
  chLastControl.Reset(); chStock.Reset(); chAdd11.Reset(); chAdd12.Reset();
  chAdd2.Reset(); chUpdate.Reset(); chPointValid.Reset(); chIsos.Reset();
  chPointsOnIsos.Reset(); chPlan.Reset(); chCyl.Reset(); chCon.Reset();
  chSph.Reset(); chTor.Reset(); chBez.Reset(); chBSpl.Reset(); chRev.Reset();
  chExt.Reset(); chOffset.Reset(); chOther.Reset();
}

inline void MESH_CHRONO_TSTARTAdapt (const GeomAbs_SurfaceType thetype)
{
  switch (thetype)
  {
    case GeomAbs_Plane:               chPlan.Start();   break;
    case GeomAbs_Cylinder:            chCyl.Start();    break;
    case GeomAbs_Cone:                chCon.Start();    break;
    case GeomAbs_Sphere:              chSph.Start();    break;
    case GeomAbs_Torus:               chTor.Start();    break;
    case GeomAbs_BezierSurface:       chBez.Start();    break;
    case GeomAbs_BSplineSurface:      chBSpl.Start();   break;
    case GeomAbs_SurfaceOfRevolution: chRev.Start();    break;
    case GeomAbs_SurfaceOfExtrusion:  chExt.Start();    break;
    case GeomAbs_OffsetSurface:       chOffset.Start(); break;
    default:                          chOther.Start();
  }
}

inline void MESH_CHRONO_TSTOPAdapt (const GeomAbs_SurfaceType thetype)
{
  chTotal.Stop();
  switch (thetype)
  {
    case GeomAbs_Plane:               chPlan.Stop();   break;
    case GeomAbs_Cylinder:            chCyl.Stop();    break;
    case GeomAbs_Cone:                chCon.Stop();    break;
    case GeomAbs_Sphere:              chSph.Stop();    break;
    case GeomAbs_Torus:               chTor.Stop();    break;
    case GeomAbs_BezierSurface:       chBez.Stop();    break;
    case GeomAbs_BSplineSurface:      chBSpl.Stop();   break;
    case GeomAbs_SurfaceOfRevolution: chRev.Stop();    break;
    case GeomAbs_SurfaceOfExtrusion:  chExt.Stop();    break;
    case GeomAbs_OffsetSurface:       chOffset.Stop(); break;
    default:                          chOther.Stop();
  }
}

#endif // DEB_MESH_CHRONOAdapt

#endif // _meshchrono_HeaderFile
