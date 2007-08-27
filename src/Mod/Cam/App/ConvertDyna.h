#ifndef CONVERTDYNA_H
#define CONVERTDYNA_H
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/App/Core/TopoAlgorithm.h>
#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/MeshPy.h>
#include <Mod/Mesh/App/Mesh.h>
#include <Mod/Mesh/App/MeshAlgos.h>

typedef struct
{
	unsigned int PointIndex;
	std::vector<double> Coords;
	bool Constrained;							//Another pair of points constraining this point...?
	bool Constrain;								//This point constraining another point...?
	std::vector<unsigned int> ConstrainedBy;    
	std::vector<unsigned int> Constraining;
	std::vector<unsigned int> FacetRef;

}VERTICES;   /*!< VERTICES struct. Contains coordinates, index, face reference, and constraining informations*/

typedef struct
{
	unsigned int FaceIndex;
	std::vector<unsigned int> PointIndex;
	
}FACE; /*!< FACE struct. Contains the face index, and the reference to the point*/



typedef struct
{
	std::vector<unsigned int> PointIndex;
}STLINDEX;  /*!< STLINDEX struct, contains only the triangulated face vertex indexes */

/*! \brief DYNA Files Reader

	This Function will read in a DYNA file, designated by inputname. Only three Information will be parsed:-
	*NODE, *CONSTRAINTS_ADAPTIVITY, and *ELEMENT_SHELL_THICKNESS (for the third, only first line will be parsed)

	From *NODE the coordinates and vertex index will be parsed. From *ELEMENT_SHELL_THICKNESS only the face nodal
	will be parsed, and from *CONSTRAINTS_ADAPTIVITY the constraints point (flagged by Constrain and Constrained) will
	be parsed.

	As output, a mesh.
	
	TODO:- 
	1. A face might consists of 2 opposite constraint points, 3 constraint points or 4 constraint points in it's
	edges. These are not implemented yet.
	2. Some other keyword might contain the face information other than *ELEMENT_SHELL_THICKNESS
	3. Multiple-PIDS DYNA files is not yet implemented. This program still assumes that it's only a single PID DYNA
*/
class ReadDyna
{
public:
	ReadDyna(MeshCore::MeshKernel &m,const char* &inputname);
protected:
	void ReadNode(std::ifstream &inputfile);
	void ReadConstraints(std::ifstream &inputfile);
	void ReadShellThickness(std::ifstream &inputfile);
	void Convert();
	void PutInMesh(MeshCore::MeshKernel &mesh);
private:
	std::map<unsigned int, VERTICES> Pointlist;
	std::map<unsigned int, FACE> Facelist;
	std::vector<STLINDEX> Stllist;

};
#endif