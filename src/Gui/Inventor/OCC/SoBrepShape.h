/*************************************************************
*
* SoBrepShape class
*
* http://www.opencascade.org   http://www.coin3d.org
*
* c-attanasio@opencascade.com
*
*************************************************************/

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <Inventor/nodes/SoSeparator.h>


class SoBrepShape
{

public:
	SoBrepShape();
	~SoBrepShape();


	// Compute the mesh for Coin3d
	Standard_Boolean Compute(SoSeparator* aRoot);

	// Simplified version ;-)
	Standard_Boolean Compute2(SoSeparator* aRoot);

	// Set the filename of the Brep file
	void SetFile(const char* aBrepFilePath);

	void SetShape(TopoDS_Shape cShape);

	// Set option for rendering
	void SetRenderOptions(int Strips,int Strip_Color,int Nurbs,int selection);

	// Set mesher deflection
	void SetDeflection(float defle);

	int GetNbStrips();

private:

	// Read the TopoDS_Shape
	Standard_Boolean ReadShape();

	char* filename;
	int nurbs_enable;
	int strip_enable;
	int strip_color;
	int nbstrips;
	int selection;
	double deflection;
    TopoDS_Shape myShape;

};




/*
	Based on code by Brad Grantham of SGI.
*/

#include "TColStd_SequenceOfInteger.hxx"

#ifndef TRUE

#define TRUE	1
#define FALSE	0

#endif /* ifndef TRUE */

// error signal
#define MESHIFIER_ERROR -1


/*
 * Change this if you need more or less precision
 */
#define REAL	double


typedef REAL meshVec3[3];
typedef REAL meshVec4[4];


#define Xi	0	/* indices into vector coordinate or normal */
#define Yi	1
#define Zi	2


#define Ri	0	/* indices into texture coordinate */
#define Si	1
#define Ti	2


	/* "R" already defined */
#define Gi	1	/* green */
#define Bi	2	/* blue */
#define Ai	3	/* alpha */


#define MAX_SHARE	32	/* # of edges that can share a vertex */


#define MAX_TRIS	10000	/* # of maximum triangles */


/*
 * On some architectures, shorter strips may perform better than longer
 * strips because parallel transform engines may have to share or duplicate
 * vertices if the engines have small FIFOs.  (e.g. a GE with a 12 vertex
 * FIFO would swallow 12 vertices immediately and then the 13th and on
 * would block until the FIFO was empty, or if another GE is available it
 * would have to copy the previous two vertices to process the next
 * part of the strip.  Here's the maximum strip size to create, in vertices.
 *
 * Currently it tries to do the best job, but you might want to set it to
 * suit your hardware.
 */

// test
// #define MAX_STRIP 6
// #define MAX_STRIP 3
// #define MAX_STRIP 4

#define MAX_STRIP	(MAX_TRIS + 2) 	/* # verts = # tris + 2 */


struct meshifierVertex {
	int v_index; /* index in some other system..., unused by actual meshifier */
    meshVec3	v;			/* coordinate */
    meshVec3	n;			/* normal */
	int c_index; // colour_index
    meshVec4	c;			/* color */
	int t_index; // texture index
    meshVec3	t;			/* texture, but normally only x,y */
    int		conncnt;		/* # of edges to which this belongs */
    int		conn[MAX_SHARE][2];	/* [0] == index of connected vertex */
					/* [1] == index of connecting edge */
};


/*
 * Notice that this implementation won't properly handle more than 2 tris
 * sharing an edge but wouldn't be too hard to extend.
 */

struct meshifierEdge {
    int t[2];			/* triangles sharing this edge */
};


struct meshifierTriangle {
    int	v[3];			/* indices of vertices */
    int	tcnt;			/* # of adjacent triangles */
    int	t[3];			/* adjacent triangles */
    int	done;			/* been inserted into strip? */
    int	next;			/* next triangle in strip */
};


#define	MESH_DRAWSTRIPOUTLINES	0
#define	MESH_DRAWCONNECTIVITY	1
#define	MESH_PRINTSTRIPCONTENTS	2
#define	MESH_PRINTADDTRIANGLES	3


typedef void (*meshDrawTextProc)(REAL v[3], char *text);
typedef void (*meshDrawLineProc)(REAL v1[3], REAL v2[3]);
typedef void (*meshDrawEdgeProc)(meshifierVertex *v1, meshifierVertex *v2);
typedef void (*meshBeginStripProc)(meshifierVertex *v1, meshifierVertex *v2);
typedef void (*meshContStripProc)(meshifierVertex *v);
typedef void (*meshEndStripProc)(void);


void meshSetProcs(meshDrawTextProc dt, meshDrawLineProc dl, meshDrawEdgeProc de,
    meshBeginStripProc bs, meshContStripProc cs, meshEndStripProc es);

void meshSetFlag(int flag, int value);

int meshCreateStrips(bool MakeStrips,TColStd_SequenceOfInteger& Strips);

void meshReset(void);

void meshAddVertex(int v_index, meshVec3 v, meshVec3 n, int c_index, 
						meshVec4 c, int t_index, meshVec3 t);

void meshAddTriangle(int v1, int v2, int v3);
