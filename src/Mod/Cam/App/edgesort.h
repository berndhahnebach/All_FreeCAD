#ifndef EDGESORT_H
#define EDGESORT_H

#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <TopTools_MapOfShape.hxx>
#include <map>
#include <list>

struct Edgesort_gp_Pnt_Less
{
       bool operator()(const gp_Pnt & _Left, const gp_Pnt & _Right) const
       {
               Standard_Real x1,y1,z1,x2,y2,z2;
			   _Left.Coord(x1,y1,z1);
			   _Right.Coord(x2,y2,z2);
               if ( fabs(x1- x2) > 0.01 )
                       return x1 < x2;
               else if ( fabs(y1 -y2) > 0.01 )
                       return y1 < y2;
			   else if ( fabs(z1 -z2) > 0.01 )
                       return z1 < z2;
               return false;
       }
};

typedef std::list<TopoDS_Edge> tEdgeList;
typedef std::map<gp_Pnt,tEdgeList,Edgesort_gp_Pnt_Less>  tMapPntEdge;
typedef std::pair<gp_Pnt,tEdgeList> tMapPntEdgePair;

class Edgesort
{
public:
       Standard_EXPORT Edgesort(const TopoDS_Shape& aShape);
       Standard_EXPORT virtual ~Edgesort(void);

       Standard_EXPORT void Init();
       Standard_EXPORT bool More();
       Standard_EXPORT void Next();
       Standard_EXPORT const TopoDS_Edge& Current();
private:
       void Perform();
       void Perform(const TopoDS_Edge& edge);
       bool PerformEdges(gp_Pnt& point);
       bool IsValidEdge(const TopoDS_Edge& edge);
       TopoDS_Shape m_shape;

       tMapPntEdge m_vertices;

       bool m_done;
       tEdgeList m_edges;
       tEdgeList::const_iterator m_edgeIter;
};

#endif

