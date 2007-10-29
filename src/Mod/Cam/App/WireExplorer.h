#ifndef WIREEXPLORER_H
#define WIREEXPLORER_H

#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Pnt.hxx>
#include <TopTools_MapOfShape.hxx>
#include <map>
#include <list>

struct WireExplorer_gp_PntLess
{
       bool operator()(const gp_Pnt& _Left, const gp_Pnt& _Right) const
       {
               Standard_Real x1,y1,z1,x2,y2,z2;
               _Left.Coord(x1,y1,z1);
               _Right.Coord(x2,y2,z2);
               if ( x1 != x2 )
                       return x1 < x2;
               else if ( y1 != y2 )
                       return y1 < y2;
               return z1 < z2;
       }
};

typedef std::list<TopoDS_Edge> tEdgeList;
typedef std::map<gp_Pnt,tEdgeList,WireExplorer_gp_PntLess>  tMapPntShapes;
typedef std::pair<gp_Pnt,tEdgeList> tMapPntShapesPair;

class WireExplorer
{
public:
       Standard_EXPORT WireExplorer(const TopoDS_Wire& wire);
       Standard_EXPORT virtual ~WireExplorer(void);

       Standard_EXPORT void Init();
       Standard_EXPORT bool More();
       Standard_EXPORT void Next();
       Standard_EXPORT const TopoDS_Edge& Current();
private:
       void Perform();
       void Perform(const TopoDS_Edge& edge);
       bool PerformEdges(gp_Pnt& point);
       bool IsValidEdge(const TopoDS_Edge& edge);
       TopoDS_Wire m_wire;

       tMapPntShapes m_vertices;

       bool m_done;
       tEdgeList m_edges;
       tEdgeList::const_iterator m_edgeIter;
};

#endif

