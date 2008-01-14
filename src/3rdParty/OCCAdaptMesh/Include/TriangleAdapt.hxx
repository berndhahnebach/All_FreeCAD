/* File:	TriangleAdapt.hxx
 * Created:	23.06.2004
 * Author:	Michael SAZONOV
 * Copyright:    Open CASCADE 2004
*/

/* NOTE: This file is included to triangleAdapt.c, so its sintax should
 *       comply with ANSI C (no // comments; no other OCC includes)
*/

#ifndef _TriangleAdapt_HeaderFile
#define _TriangleAdapt_HeaderFile

#ifdef __cplusplus
extern "C" {
#endif

#define REAL double
#define ANSI_DECLARATORS
#include <triangleAdapt.h>
#undef REAL

  TRIANGLE_API void triangleAdapt_InitBehavior(double minangle, double maxarea);
  TRIANGLE_API void triangleAdapt_InitMesh(int nbnodes);
  TRIANGLE_API int  triangleAdapt_AddPoint(double x, double y, int index);
  TRIANGLE_API void triangleAdapt_AddPointIni(double x, double y, int index);
  TRIANGLE_API void triangleAdapt_AddEdge(const int *nodes, int length, int orient, int first);
  TRIANGLE_API void triangleAdapt_InsertHoles(double *holes, int length);
  TRIANGLE_API void triangleAdapt_GetMeshSize(int* nbnodes, int* nbtriangles);
  TRIANGLE_API void triangleAdapt_InitPoints();
  TRIANGLE_API int  triangleAdapt_NextPoint(int iterator, double *x, double *y, int *index);
  TRIANGLE_API void triangleAdapt_InitTriangles();
  TRIANGLE_API int  triangleAdapt_NextTriangle(int *n1, int *n2, int *n3, int *m1, int *m2, int *m3);
  TRIANGLE_API int  triangleAdapt_Triangulate();
  TRIANGLE_API void triangleAdapt_FinishTriangulation();
  TRIANGLE_API void triangleAdapt_FreeTriangulation();
  /* Modified by mpv Dec 25 2006 OCC14376: increases precision of calculations (but slower) */
  TRIANGLE_API void triangleAdapt_IncreasePrecision(); 

#ifdef __cplusplus
}
#endif

#endif
