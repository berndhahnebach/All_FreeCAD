// File:	meshtriangle.cxx
// Created:	Tue Jun 11 17:40:33 1996
// Author:	Open CASCADE Support
//		<support@opencascade.com>
// Copyright:    Open CASCADE 2003

#include <meshtriangleAdapt.hxx>

#include <TriangleAdapt.hxx>

//=======================================================================
//function : BRepMeshAdapt_AddPoint
//purpose  : 
//=======================================================================

Standard_Boolean BRepMeshAdapt_AddPoint(const gp_Pnt2d& aPoint,
                                   const Standard_Integer anIndex)
{
  return triangleAdapt_AddPoint(aPoint.X(),aPoint.Y(),anIndex);
}

//=======================================================================
//function : BRepMeshAdapt_AddEdge
//purpose  : 
//=======================================================================

void BRepMeshAdapt_AddEdge(const TColStd_Array1OfInteger& aNodes,
                      const TopAbs_Orientation& anOrient,
                      const Standard_Boolean isFirst)
{
  triangleAdapt_AddEdge(&(aNodes.Value(1)),aNodes.Length(),
                   (anOrient == TopAbs_REVERSED ? 1 : 0),(int)isFirst);
}

//=======================================================================
//function : BRepMeshAdapt_Triangulate
//purpose  : 
//=======================================================================

Standard_Boolean BRepMeshAdapt_Triangulate(const TColgp_Array1OfPnt2d&    aNodes,
                                      const TColStd_Array1OfInteger& anIndices,
									  const TriangleAdapt_Parameters& parameters)
{
  Standard_Integer i, nbpoints = aNodes.Length();

  triangleAdapt_InitBehavior(parameters._minAngle,parameters._maxArea);

  triangleAdapt_InitMesh(nbpoints);

  for (i = 1; i <= nbpoints; i++)
  {
    const gp_Pnt2d& pnt = aNodes(i);
    triangleAdapt_AddPointIni(pnt.X(),pnt.Y(),anIndices(i));
  }

  return triangleAdapt_Triangulate();
}

//=======================================================================
//function : BRepMeshAdapt_InsertHoles
//purpose  : 
//=======================================================================

void BRepMeshAdapt_InsertHoles(const TColgp_SequenceOfPnt2d& aHoles)
{
  const Standard_Integer nbholes = aHoles.Length();
  if (nbholes)
  {
    const Standard_Integer nbreals = 2 * nbholes;

    Standard_Real *holelist = (Standard_Real *) malloc(nbreals * sizeof(double));
    if (holelist == (Standard_Real *) NULL)
    {
      Standard_Failure::Raise("BRepMeshAdapt_InsertHoles : Out of memory");
    }

    memset(holelist, 0, nbreals * sizeof(double));

    Standard_Integer i, j;
    for (i = 0, j = 1; i < nbreals; i++, j++)
    {
      const gp_Pnt2d& pnt = aHoles(j);
      holelist[i++] = pnt.X();
      holelist[i] = pnt.Y();
    }

    triangleAdapt_InsertHoles(holelist,nbholes);
  }
}

//=======================================================================
//function : BRepMeshAdapt_FinishTriangulation
//purpose  : 
//=======================================================================

Handle(Poly_Triangulation) BRepMeshAdapt_FinishTriangulation(Handle(TColStd_HArray1OfInteger)& anIndices,
                                                        const TopAbs_Orientation          anOrient)
{
  triangleAdapt_FinishTriangulation();
  Handle(Poly_Triangulation) aMesh = BRepMeshAdapt_GetTriangulation(anIndices, anOrient);
  BRepMeshAdapt_FreeTriangulation();
  return aMesh;
}

//=======================================================================
//function : BRepMeshAdapt_GetTriangulation
//purpose  : 
//=======================================================================

Handle(Poly_Triangulation) BRepMeshAdapt_GetTriangulation(Handle(TColStd_HArray1OfInteger)& anIndices,
                                                     const TopAbs_Orientation          anOrient)
{
  Handle(Poly_Triangulation) aMesh;

  // Read mesh size
  Standard_Integer nbnodes, nbtriangles;
  triangleAdapt_GetMeshSize(&nbnodes,&nbtriangles);
  if (nbnodes <= 0 || nbtriangles <= 0) return aMesh;

  aMesh = new Poly_Triangulation(nbnodes, nbtriangles, Standard_True);

  // Read nodes information
  anIndices = new TColStd_HArray1OfInteger(1, nbnodes);
  TColgp_Array1OfPnt2d& theNodes2d = aMesh->ChangeUVNodes();

  triangleAdapt_InitPoints();

  Standard_Real x, y;
  Standard_Integer pindex, dummy_int,pnumber = 1;
  while (triangleAdapt_NextPoint(pnumber,&x,&y,&pindex,&dummy_int ))
  {
    theNodes2d(pnumber).SetCoord(x,y);
    anIndices->SetValue(pnumber,pindex);
    pnumber++;
  }

  // Read triangles information
  Poly_Array1OfTriangle& theTriangles = aMesh->ChangeTriangles();

  triangleAdapt_InitTriangles();

  Standard_Integer p1, p2, p3, m1, m2, m3, pn, tnumber = 1;
  while (triangleAdapt_NextTriangle(&p1,&p2,&p3,&m1,&m2,&m3))
  {
    if (anOrient == TopAbs_REVERSED)
    {
      pn = p2; p2 = p3; p3 = pn;
    }
    theTriangles(tnumber++) = Poly_Triangle(p1,p2,p3);
  }

  return aMesh;
}

//=======================================================================
//function : BRepMeshAdapt_FreeTriangulation
//purpose  : 
//=======================================================================

void BRepMeshAdapt_FreeTriangulation()
{
  triangleAdapt_FreeTriangulation();
}
