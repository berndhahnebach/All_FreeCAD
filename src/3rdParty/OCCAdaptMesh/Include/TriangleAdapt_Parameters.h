#ifndef _TriangleAdapt_Parameters_HeaderFile
#define _TriangleAdapt_Parameters_HeaderFile

struct TriangleAdapt_Parameters
{
   TriangleAdapt_Parameters()
       :_minAngle(-1),
       _maxArea(-1),
       _maxTriangleSideSize(-1),
       _minEdgeSplit(-1),
       _minNbPntsPerEdgeLine(-1),
       _minNbPntsPerEdgeOther(-1)

   {}
   double _minAngle;
   double _maxArea;
   double _maxTriangleSideSize;
   int _minEdgeSplit;
   int _minNbPntsPerEdgeLine;
   int _minNbPntsPerEdgeOther;
};
#endif // _TriangleAdapt_Parameters_HeaderFile

