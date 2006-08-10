/***************************************************************************
 *   Copyright (c) 2005 Imetric 3D GmbH                                    *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#include "Definitions.h"
#include "Iterator.h"

#include "MeshKernel.h"
#include "MeshIO.h"
#include "Builder.h"

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include <Base/Reader.h>
#include <Base/Writer.h>
#include <Base/Persistance.h>

#include <math.h>


using namespace MeshCore;

char *upper(char * string) {
    int i;
    int l;
    
    if (string != NULL) {
       l = strlen(string);
       for (i=0; i<l; i++)
         string[i] = toupper(string[i]);
    }

  return string;
}

char *ltrim (char *psz)
{
  int i, sl;

  if (psz) {
     for (i = 0; (psz[i] == 0x20) || (psz[i] == 0x09); i++);
     sl = strlen (psz + i);
     memmove (psz, psz + i, sl);
     psz[sl] = 0;
  }
  return psz;
}

/* Define a vector.  */
typedef struct _Vector
	{
		double  x;
		double  y;
		double  z;
	} Vector, *VectorPtr;

/* Usage by CMeshNastran, CMeshCadmouldFE. Added by Sergey Sukhov (26.04.2002)*/
struct NODE {float x, y, z;};
struct TRIA {int iV[3];};
struct QUAD {int iVer[4];};

/* Takes the modulus of v */
#define VMod(v)		(sqrt((v).x*(v).x + (v).y*(v).y + (v).z*(v).z))

/* Returns the dot product of v1 & v2 */
#define VDot(v1, v2)	((v1).x*(v2).x + (v1).y*(v2).y + (v1).z*(v2).z)

/* Fills the fields of a vector.	*/
#define VNew(a, b, c, r)	((r).x = (a), (r).y = (b), (r).z = (c))

#define VAdd(v1, v2, r)		((r).x = (v1).x + (v2).x , \
							 (r).y = (v1).y + (v2).y , \
							 (r).z = (v1).z + (v2).z )

#define VSub(v1, v2, r)		((r).x = (v1).x - (v2).x , \
							 (r).y = (v1).y - (v2).y , \
							 (r).z = (v1).z - (v2).z )

#define VCross(v1, v2, r)	((r).x = (v1).y * (v2).z - (v1).z * (v2).y , \
				 			 (r).y = (v1).z * (v2).x - (v1).x * (v2).z , \
				 			 (r).z = (v1).x * (v2).y - (v1).y * (v2).x )

#define VScalarMul(v, d, r)	((r).x = (v).x * (d) , \
				 			 (r).y = (v).y * (d) , \
				 			 (r).z = (v).z * (d) )

#define VUnit(v, t, r)		((t) = 1 / VMod(v) , \
				 			 VScalarMul(v, t, r) )

typedef struct _Quaternion {
	Vector	vect_part;
	double	real_part;
	} Quaternion;

Quaternion
Build_Rotate_Quaternion(Vector axis, double cos_angle)
{
	Quaternion	quat;
	double	sin_half_angle;
	double	cos_half_angle;
	double	angle;

	/* The quaternion requires half angles. */
	if ( cos_angle > 1.0 ) cos_angle = 1.0;
	if ( cos_angle < -1.0 ) cos_angle = -1.0;
	angle = acos(cos_angle);
	sin_half_angle = sin(angle / 2);
	cos_half_angle = cos(angle / 2);

	VScalarMul(axis, sin_half_angle, quat.vect_part);
	quat.real_part = cos_half_angle;

	return quat;
}

static Quaternion
QQMul(Quaternion *q1, Quaternion *q2)
{
	Quaternion	res;
	Vector		temp_v;

	res.real_part = q1->real_part * q2->real_part -
					VDot(q1->vect_part, q2->vect_part);
	VCross(q1->vect_part, q2->vect_part, res.vect_part);
	VScalarMul(q1->vect_part, q2->real_part, temp_v);
	VAdd(temp_v, res.vect_part, res.vect_part);
	VScalarMul(q2->vect_part, q1->real_part, temp_v);
	VAdd(temp_v, res.vect_part, res.vect_part);

	return res;
}

static void
Quaternion_To_Axis_Angle(Quaternion *q, Vector *axis, double *angle)
{
	double	half_angle;
	double	sin_half_angle;

	half_angle = acos(q->real_part);
	sin_half_angle = sin(half_angle);
	*angle = half_angle * 2;
	if ( sin_half_angle < 1e-8 && sin_half_angle > -1e-8 )
		VNew(1, 0, 0, *axis);
	else
	{
		sin_half_angle = 1 / sin_half_angle;
		VScalarMul(q->vect_part, sin_half_angle, *axis);
	}
}

static void
Convert_Camera_Model(Vector *pos, Vector *at, Vector *up, Vector *res_axis,
                     double *res_angle)
{
	Vector		n, v;

	Quaternion	rot_quat;
	Vector		norm_axis;
	Quaternion	norm_quat;
	Quaternion	inv_norm_quat;
	Quaternion	y_quat, new_y_quat, rot_y_quat;
	Vector		new_y;

	double		temp_d;
	Vector		temp_v;

	/* n = (norm)(pos - at) */
	VSub(*at, *pos, n);
	VUnit(n, temp_d, n);

	/* v = (norm)(view_up - (view_up.n)n) */
	VUnit(*up, temp_d, *up);
	temp_d = VDot(*up, n);
	VScalarMul(n, temp_d, temp_v);
	VSub(*up, temp_v, v);
	VUnit(v, temp_d, v);

	VNew(n.y, -n.x, 0, norm_axis);
	if ( VDot(norm_axis, norm_axis) < 1e-8 )
	{
		/* Already aligned, or maybe inverted. */
		if ( n.z > 0.0 )
		{
			norm_quat.real_part = 0.0;
			VNew(0, 1, 0, norm_quat.vect_part);
		}
		else
		{
			norm_quat.real_part = 1.0;
			VNew(0, 0, 0, norm_quat.vect_part);
		}
	}
	else
	{
		VUnit(norm_axis, temp_d, norm_axis);
		norm_quat = Build_Rotate_Quaternion(norm_axis, -n.z);
	}

	/* norm_quat now holds the rotation needed to line up the view directions.
	** We need to find the rotation to align the up vectors also.
	*/

	/* Need to rotate the world y vector to see where it ends up. */
	/* Find the inverse rotation. */
	inv_norm_quat.real_part = norm_quat.real_part;
	VScalarMul(norm_quat.vect_part, -1, inv_norm_quat.vect_part);

	/* Rotate the y. */
	y_quat.real_part = 0.0;
	VNew(0, 1, 0, y_quat.vect_part);
	new_y_quat = QQMul(&norm_quat, &y_quat);
	new_y_quat = QQMul(&new_y_quat, &inv_norm_quat);
	new_y = new_y_quat.vect_part;

	/* Now need to find out how much to rotate about n to line up y. */
	VCross(new_y, v, temp_v);
	if ( VDot(temp_v, temp_v) < 1.e-8 )
	{
		/* The old and new may be pointing in the same or opposite. Need
		** to generate a vector perpendicular to the old or new y.
		*/
		VNew(0, -v.z, v.y, temp_v);
		if ( VDot(temp_v, temp_v) < 1.e-8 )
			VNew(v.z, 0, -v.x, temp_v);
	}
	VUnit(temp_v, temp_d, temp_v);
	rot_y_quat = Build_Rotate_Quaternion(temp_v, VDot(new_y, v));

	/* rot_y_quat holds the rotation about the initial camera direction needed
	** to align the up vectors in the final position.
	*/

	/* Put the 2 rotations together. */
	rot_quat = QQMul(&rot_y_quat, &norm_quat);

	/* Extract the axis and angle from the quaternion. */
	Quaternion_To_Axis_Angle(&rot_quat, res_axis, res_angle);
}

// --------------------------------------------------------------

/** Loads an STL file either in binary or ASCII format. 
 * Therefore the file header gets checked to decide if the file is binary or not.
 */
bool MeshInput::LoadSTL (std::istream &rstrIn)
{
  char szBuf[200];

  if ( !rstrIn || rstrIn.bad() == true )
    return false;

  // Read in 50 characters from position 80 on and check for keywords like 'SOLID', 'FACET', 'NORMAL',
  // 'VERTEX', 'ENDFACET' or 'ENDLOOP'.
  // As the file can be binary with one triangle only we must not read in more than (max.) 54 bytes because
  // the file size has only 134 bytes in this case. On the other hand we must overread the first 80 bytes
  // because it can happen that the file is binary but contains one of these keywords.
  std::streambuf* buf = rstrIn.rdbuf();
  if (!buf) return false;
  buf->pubseekoff(80, std::ios::beg, std::ios::in);
  unsigned long ulCt;
  rstrIn.read((char*)&ulCt, sizeof(ulCt));
  // Either it's really an invalid STL file or it's just empty. In this case the number of facets must be 0.
  if ( rstrIn.read(szBuf, 50) == false )
    return (ulCt==0);
  szBuf[50] = 0;
  upper(szBuf);

  try{
    if ((strstr(szBuf, "SOLID") == NULL)  && (strstr(szBuf, "FACET") == NULL)    && (strstr(szBuf, "NORMAL") == NULL) &&
        (strstr(szBuf, "VERTEX") == NULL) && (strstr(szBuf, "ENDFACET") == NULL) && (strstr(szBuf, "ENDLOOP") == NULL))
    {  // wahrscheinlich stl binaer
      buf->pubseekoff(0, std::ios::beg, std::ios::in);
      return LoadBinarySTL(rstrIn);
    }
    else
    {  // stl ascii
      buf->pubseekoff(0, std::ios::beg, std::ios::in);
      return LoadAsciiSTL(rstrIn);
    }
  }
  catch( const Base::MemoryException& e ){
    _rclMesh.Clear();
    Base::Sequencer().halt();
    throw e;
  }
  catch( const Base::AbortException& e ){
    _rclMesh.Clear();
    throw e;
  }
  catch ( const Base::Exception& e ){
    _rclMesh.Clear();
    Base::Sequencer().halt();
    throw e;
  }
  catch (...)
  {
    _rclMesh.Clear();
    Base::Sequencer().halt();
    throw;
  }

  return true;
}

/** Loads an ASCII STL file. */
bool MeshInput::LoadAsciiSTL (std::istream &rstrIn)
{
  char szLine[200], szKey1[200], szKey2[200];
  unsigned long ulVertexCt, ulFacetCt=0;
  float fX, fY, fZ;
  MeshGeomFacet clFacet;

  if ( !rstrIn || rstrIn.bad() == true )
    return false;

  long ulSize=ULONG_MAX;
  std::streambuf* buf = rstrIn.rdbuf();
  if ( !buf )
    return false;

  ulSize = buf->pubseekoff(0, std::ios::end, std::ios::in);
  buf->pubseekoff(0, std::ios::beg, std::ios::in);
  ulSize -= 20;

  // count facets
  while ((rstrIn.eof() == false) && (rstrIn.bad() == false))
  {
    rstrIn.getline(szLine, 200);
    upper(ltrim(szLine));
    if (strncmp(szLine, "FACET", 5) == 0)
      ulFacetCt++;
    // prevent from reading EOF (as I don't know how to reread the file then)
    else if (rstrIn.tellg() > ulSize)
      break;
    else if (strncmp(szLine, "ENDSOLID", 8) == 0)
      break;
  }

  // restart from the beginning
  buf->pubseekoff(0, std::ios::beg, std::ios::in);

  MeshBuilder builder(this->_rclMesh);
  builder.Initialize(ulFacetCt);

  ulVertexCt = 0;
  while ((rstrIn.eof() == false) && (rstrIn.bad() == false))
  {
    rstrIn.getline(szLine, 200);
    upper(ltrim(szLine));
    if (strncmp(szLine, "FACET", 5) == 0)  // normale
    {
      if (sscanf(szLine, "%s %s %f %f %f", szKey1, szKey2, &fX, &fY, &fZ) == 5)
        clFacet.SetNormal(Base::Vector3f(fX, fY, fZ));
    }
    else if (strncmp(szLine, "VERTEX", 6) == 0)  // vertex
    {
      if (sscanf(szLine, "%s %f %f %f", szKey1, &fX, &fY, &fZ) == 4)
      {
        clFacet._aclPoints[ulVertexCt++].Set(fX, fY, fZ);
        if (ulVertexCt == 3)
        {
          ulVertexCt = 0;
          builder.AddFacet(clFacet);
        }
      }
    }
  }

  builder.Finish();

  return true;
}

/** Loads a binary STL file. */
bool MeshInput::LoadBinarySTL (std::istream &rstrIn)
{
  char szInfo[80];
  Base::Vector3f clVects[4];
  unsigned short usAtt; 
  unsigned long ulCt;

  if ( !rstrIn || rstrIn.bad() == true )
    return false;

  // Header-Info ueberlesen
  rstrIn.read(szInfo, sizeof(szInfo));
 
  // Anzahl Facets
  rstrIn.read((char*)&ulCt, sizeof(ulCt));
  if (rstrIn.bad() == true)
    return false;

  // get file size and calculate the number of facets
  unsigned long ulSize = 0; 
  std::streambuf* buf = rstrIn.rdbuf();
  if ( buf )
  {
    unsigned long ulCurr;
    ulCurr = buf->pubseekoff(0, std::ios::cur, std::ios::in);
    ulSize = buf->pubseekoff(0, std::ios::end, std::ios::in);
    buf->pubseekoff(ulCurr, std::ios::beg, std::ios::in);
  }

  unsigned long ulFac = (ulSize - (80 + sizeof(unsigned long))) / 50;

  // compare the calculated with the read value
  if (ulCt > ulFac)
    return false;// not a valid STL file
 
  MeshBuilder builder(this->_rclMesh);
  builder.Initialize(ulCt);

  for (unsigned long i = 0; i < ulCt; i++)
  {
    // read normal, points
    rstrIn.read((char*)&clVects, sizeof(clVects));

    std::swap(clVects[0], clVects[3]);
    builder.AddFacet(clVects);

    // overread 2 bytes attribute
    rstrIn.read((char*)&usAtt, sizeof(usAtt));
  }

  builder.Finish();

  return true;
}

/** Loads the mesh object from an XML file. */
void MeshInput::LoadXML (Base::XMLReader &reader)
{
  int Cnt,i;
  MeshPointArray cPoints;
  MeshFacetArray cFacets;
 
//  reader.readElement("Mesh");

  reader.readElement("Points");
  Cnt = reader.getAttributeAsInteger("Count");

  cPoints.resize(Cnt);
  for(i=0 ;i<Cnt ;i++)
  {
    reader.readElement("P");
    cPoints[i].x = reader.getAttributeAsFloat("x");
    cPoints[i].y = reader.getAttributeAsFloat("y");
    cPoints[i].z = reader.getAttributeAsFloat("z");
  }
  reader.readEndElement("Points");

  reader.readElement("Faces");
  Cnt = reader.getAttributeAsInteger("Count");

  cFacets.resize(Cnt);
  for(i=0 ;i<Cnt ;i++)
  {
    reader.readElement("F");
     
    cFacets[i]._aulPoints[0] = reader.getAttributeAsInteger("p0");
    cFacets[i]._aulPoints[1] = reader.getAttributeAsInteger("p1");
    cFacets[i]._aulPoints[2] = reader.getAttributeAsInteger("p2");
    cFacets[i]._aulNeighbours[0] = reader.getAttributeAsInteger("n0");
    cFacets[i]._aulNeighbours[1] = reader.getAttributeAsInteger("n1");
    cFacets[i]._aulNeighbours[2] = reader.getAttributeAsInteger("n2");
  }
  reader.readEndElement("Faces");

  reader.readEndElement("Mesh");

  _rclMesh.Adopt(cPoints, cFacets);
}

/** Loads an OpenInventor file. */
bool MeshInput::LoadInventor (std::istream &rstrIn)
{
  if ( !rstrIn || rstrIn.bad() == true)
    return false;

  // get file size and estimate the number of lines
  unsigned long ulSize = 0; 
  std::streambuf* buf = rstrIn.rdbuf();
  if ( buf )
  {
    unsigned long ulCurr;
    ulCurr = buf->pubseekoff(0, std::ios::cur, std::ios::in);
    ulSize = buf->pubseekoff(0, std::ios::end, std::ios::in);
    buf->pubseekoff(ulCurr, std::ios::beg, std::ios::in);
  }

  char szLine[512];
  MeshGeomFacet clFacet;
  std::vector<MeshGeomFacet> clFacetAry;
  std::vector<Base::Vector3f> aclPoints;

  // We have approx. 30 characters per line
  Base::SequencerLauncher seq("Loading...", ulSize/30);

  bool bFlag = false;
  bool normals = false;
  bool points = false;
  bool facets = false;
  while (rstrIn && (rstrIn.eof() == false) && (rstrIn.bad() == false) && (facets == false))
  {
    // If a line has more than 512 characters then the stream becomes invalid
    // somehow and the read process will be terminated. 
    // Anyway, files with so long line cannot be handled properly.
    rstrIn.getline(szLine ,512);
    upper(ltrim(szLine));
    
    // read the normals if they are defined
    if (!normals && strncmp(szLine, "NORMAL {", 8) == 0) {
      float fX, fY, fZ;
      normals = true; // okay, the normals are set by a Normal node
      bFlag = true;
      // Get the next line and check for the normal field which might begin
      // with the first vector already i.e. it's of the form 'vector [ 0.0 0.0 1.0,'
      // This is a special case to support also file formats directly written by
      // Inventor 2.1 classes.
      if ( (rstrIn.eof() == false) && (rstrIn.bad() == false) ) {
        rstrIn.getline(szLine, 200);
        upper(ltrim(szLine));
        if (sscanf(szLine, "VECTOR [ %f  %f  %f", &fX, &fY, &fZ) == 3) {
          clFacet.SetNormal(Base::Vector3f(fX, fY, fZ));
          clFacetAry.push_back(clFacet);
        }
      }
      while (rstrIn && (rstrIn.eof() == false) && (rstrIn.bad() == false) && bFlag)
      {
        rstrIn.getline(szLine, 200);

        if (sscanf(szLine, "%f %f %f", &fX, &fY, &fZ) == 3) {
          clFacet.SetNormal(Base::Vector3f(fX, fY, fZ));
          clFacetAry.push_back(clFacet);

          Base::Sequencer().next( true ); // allow to cancel
        } else {
          bFlag = false;
        }
      }
    } else if (!points && strncmp(szLine, "COORDINATE3 {", 13) == 0) {
      Base::Vector3f clPoint;
      points = true; // the Coordinate3 node
      bFlag = true;
      // Get the next line and check for the points field which might begin
      // with the first point already i.e. it's of the form 'point [ 0.0 0.0 0.0,'
      // This is a special case to support also file formats directly written by
      // Inventor 2.1 classes.
      if ( (rstrIn.eof() == false) && (rstrIn.bad() == false) ) {
        rstrIn.getline(szLine, 200);
        upper(ltrim(szLine));
        if (sscanf(szLine, "POINT [ %f  %f  %f", &(clPoint.x), &(clPoint.y), &(clPoint.z)) == 3) {
          aclPoints.push_back(clPoint);
        }
      }
      while (rstrIn && (rstrIn.eof() == false) && (rstrIn.bad() == false) && bFlag)
      {
        rstrIn.getline(szLine, 200);
        ltrim(szLine);
        if (sscanf(szLine, "%f  %f  %f", &(clPoint.x), &(clPoint.y), &(clPoint.z)) == 3) {
          aclPoints.push_back(clPoint);
          Base::Sequencer().next( true ); // allow to cancel
        } else {
          bFlag = false;
        }
      }
    } else if (points && strncmp(szLine, "INDEXEDFACESET {", 16) == 0) {
      // read the point indices of the facets
      unsigned long ulPoints[3];
      long lSeparator;
      facets = true;
      bFlag = true;
      unsigned long ulCt = 0;
      // Get the next line and check for the index field which might begin
      // with the first index already.
      // This is a special case to support also file formats directly written by
      // Inventor 2.1 classes.
      // Furthermore we must check whether more than one triple is given per line, which
      // is handled in the while-loop.
      if ( (rstrIn.eof() == false) && (rstrIn.bad() == false) ) {
        rstrIn.getline(szLine, 200);
        upper(ltrim(szLine));
        int len = strlen(szLine);
        // overread 'COORDINDEX [ '
        int pos = 13;
        while (pos < len ) {
          if (sscanf(szLine+pos, "%lu, %lu, %lu, %ld", &ulPoints[0], &ulPoints[1], &ulPoints[2], &lSeparator) == 4) {
            if ( normals ) {
              // get a reference to the facet with defined normal
              MeshGeomFacet& rclFacet = clFacetAry[ulCt++];
              for (int i = 0; i < 3; i++)
                rclFacet._aclPoints[i] = aclPoints[ulPoints[i]];
            } else {
              for (int i = 0; i < 3; i++)
                clFacet._aclPoints[i] = aclPoints[ulPoints[i]];
              clFacetAry.push_back(clFacet);
            }
          }

          // we only have positive indices, i.e. '-' is the beginning of the separator
          // add 3 because '-1,' has three characters
          pos += strcspn(szLine+pos, "-")+3;
        }
      }
      while (rstrIn && (rstrIn.eof() == false) && (rstrIn.bad() == false) && bFlag)
      {
        rstrIn.getline(szLine, 200);
        ltrim(szLine);
        if (sscanf(szLine, "%lu, %lu, %lu, %ld", &ulPoints[0], &ulPoints[1], &ulPoints[2], &lSeparator) == 4) {
          if ( normals ) {
            // get a reference to the facet with defined normal
            MeshGeomFacet& rclFacet = clFacetAry[ulCt++];
            for (int i = 0; i < 3; i++)
              rclFacet._aclPoints[i] = aclPoints[ulPoints[i]];
          } else {
            for (int i = 0; i < 3; i++)
              clFacet._aclPoints[i] = aclPoints[ulPoints[i]];
            clFacetAry.push_back(clFacet);
          }

          // read in further triples of this line, if possible
          int len = strlen(szLine);
          int pos = strcspn(szLine, "-")+3;
          while (pos < len ) {
            if (sscanf(szLine+pos, "%lu, %lu, %lu, %ld", &ulPoints[0], &ulPoints[1], &ulPoints[2], &lSeparator) == 4) {
              if ( normals ) {
                // get a reference to the facet with defined normal
                MeshGeomFacet& rclFacet = clFacetAry[ulCt++];
                for (int i = 0; i < 3; i++)
                  rclFacet._aclPoints[i] = aclPoints[ulPoints[i]];
              } else {
                for (int i = 0; i < 3; i++)
                  clFacet._aclPoints[i] = aclPoints[ulPoints[i]];
                clFacetAry.push_back(clFacet);
              }
            }

            // we only have positive indices, i.e. '-' is the beginning of the separator
            // add 3 because '-1,' has three characters
            pos += strcspn(szLine+pos, "-")+3;
          }

          Base::Sequencer().next( true ); // allow to cancel
        } else {
          bFlag = false;
        }
      }
    }
  }

  _rclMesh = clFacetAry;
  return (rstrIn?true:false);
}

/** Loads a Nastran file. */
bool MeshInput::LoadNastran (std::istream &rstrIn)
{
  if ((!rstrIn) || (rstrIn.bad() == true))
    return false;

  char szLine[100], szKey[50], cBuffer[16];
  int iI, iN, i, iIndx = 1, iEnd, iNext=0;
  float fLength[2], fDx, fDy, fDz;

  MeshGeomFacet clMeshFacet;
  std::vector <MeshGeomFacet> vTriangle;
 
  std::map <int, NODE> mNode;
  std::map <int, TRIA> mTria;
  std::map <int, QUAD> mQuad;

  while ((rstrIn.eof() == false) && (rstrIn.bad() == false))
  {
    rstrIn.getline(szLine, 100);
    upper(ltrim(szLine));

    if (strncmp(szLine, "GRID*", 5) == 0)
    {
      sscanf(szLine, "%s %d", szKey, &iI);
      memcpy (cBuffer, szLine+40, 16);
      mNode[iI].x = (float)atof(cBuffer);
      memcpy (cBuffer, "                ", 16);
      memcpy (cBuffer, szLine+56, 16);
      mNode[iI].y = (float)atof(cBuffer);
      memcpy (cBuffer, "                ", 16);
      memcpy (cBuffer, szLine+73, 7);
      iNext = (int)atof(cBuffer);
      memcpy (cBuffer, "                ", 16);
    }
    else if (strncmp(szLine, "GRID ", 5) == 0)
    {
      sscanf(szLine, "%s %d", szKey, &iN);
      memcpy (cBuffer, szLine+24, 8);
      mNode[iN].x = (float)atof (cBuffer);
      memcpy (cBuffer, "                ", 16);
      memcpy (cBuffer, szLine+32, 8);
      mNode[iN].y = (float)atof (cBuffer);
      memcpy (cBuffer, "                ", 16);
      memcpy (cBuffer, szLine+40, 8);
      mNode[iN].z = (float)atof (cBuffer);
      memcpy (cBuffer, "                ", 16);
    }
    else if (strncmp(szLine, "*", 1) == 0)
    {
      memcpy (cBuffer, szLine+1, 7);
      if ((int)atof (cBuffer) == iNext)
      {
        memcpy (cBuffer, szLine+8, 16);
        mNode[iI].z = (float)atof(cBuffer);
        memcpy (cBuffer, "                ", 16);
      }
    }
    else if (strncmp(szLine, "GRID", 4) == 0)
    {
      sscanf(szLine, "%s %d", szKey, &iI);
      memcpy (cBuffer, szLine+24, 8);
      mNode[iI].x = (float)atof (cBuffer);
      memcpy (cBuffer, szLine+32, 8);
      mNode[iI].y = (float)atof (cBuffer);
      memcpy (cBuffer, szLine+40, 8);
      mNode[iI].z = (float)atof (cBuffer);
    }
    else if (strncmp(szLine, "CTRIA3", 6) == 0)
    {
      sscanf(szLine, "%s", szKey);
      sscanf(szLine, "%s %d %d %d %d %d", szKey, &i, &i, &(mTria[iIndx].iV[0]), &(mTria[iIndx].iV[1]), &(mTria[iIndx].iV[2]));
      iIndx++;
    }
    else if (strncmp(szLine, "CQUAD4", 6) == 0)
    {
      sscanf(szLine, "%s %d", szKey, &iI);
      sscanf(szLine, "%s %d %d %d %d %d %d", szKey, &i, &i, &(mQuad[iI].iVer[0]), &(mQuad[iI].iVer[1]), &(mQuad[iI].iVer[2]), &(mQuad[iI].iVer[3]));
    }
  }

  for (std::map <int, QUAD>::iterator QI=mQuad.begin(); QI!=mQuad.end(); QI++)
  {
    for (int i = 0; i < 2; i++)
    {
      fDx = mNode[(*QI).second.iVer[i+2]].x - mNode[(*QI).second.iVer[i]].x;
      fDy = mNode[(*QI).second.iVer[i+2]].y - mNode[(*QI).second.iVer[i]].y;
      fDz = mNode[(*QI).second.iVer[i+2]].z - mNode[(*QI).second.iVer[i]].z;
      fLength[i] = fDx*fDx + fDy*fDy + fDz*fDz;
    }
    iEnd = mTria.size()+1; 
    if (fLength[0] < fLength[1])
    {
      mTria[iEnd].iV[0] = (*QI).second.iVer[0];
      mTria[iEnd].iV[1] = (*QI).second.iVer[1];
      mTria[iEnd].iV[2] = (*QI).second.iVer[2];

      mTria[iEnd+1].iV[0] = (*QI).second.iVer[0];
      mTria[iEnd+1].iV[1] = (*QI).second.iVer[2];
      mTria[iEnd+1].iV[2] = (*QI).second.iVer[3];
    }
    else
    {
      mTria[iEnd].iV[0] = (*QI).second.iVer[0];
      mTria[iEnd].iV[1] = (*QI).second.iVer[1];
      mTria[iEnd].iV[2] = (*QI).second.iVer[3];

      mTria[iEnd+1].iV[0] = (*QI).second.iVer[1];
      mTria[iEnd+1].iV[1] = (*QI).second.iVer[2];
      mTria[iEnd+1].iV[2] = (*QI).second.iVer[3];
    }
  }

  /* Converting data to Mesh. Negatiw conversion for right orientation of normal-vectors. */
  for (std::map<int, TRIA>::iterator MI=mTria.begin(); MI!=mTria.end(); MI++)
  {
    clMeshFacet._aclPoints[0].x = mNode[(*MI).second.iV[1]].x;
    clMeshFacet._aclPoints[0].y = mNode[(*MI).second.iV[1]].y;
    clMeshFacet._aclPoints[0].z = mNode[(*MI).second.iV[1]].z;

    clMeshFacet._aclPoints[1].x = mNode[(*MI).second.iV[0]].x;
    clMeshFacet._aclPoints[1].y = mNode[(*MI).second.iV[0]].y;
    clMeshFacet._aclPoints[1].z = mNode[(*MI).second.iV[0]].z;

    clMeshFacet._aclPoints[2].x = mNode[(*MI).second.iV[2]].x;
    clMeshFacet._aclPoints[2].y = mNode[(*MI).second.iV[2]].y;
    clMeshFacet._aclPoints[2].z = mNode[(*MI).second.iV[2]].z;

    vTriangle.push_back (clMeshFacet);
  }

  _rclMesh = vTriangle;

  return true;
}

/** Loads a Cadmould FE file. */
bool MeshInput::LoadCadmouldFE (std::ifstream &rstrIn)
{
  if ((rstrIn.is_open() == false) || (rstrIn.bad() == true))
    return false;

  char szLine[100];
  unsigned long ulNumNodes, ulNumElems;

  MeshGeomFacet clMeshFacet;
  std::vector <MeshGeomFacet> vTriangle;
 
  std::map <int, NODE> node;
  std::map <int, TRIA> elem;

  if ((rstrIn.eof() == false) && (rstrIn.bad() == false))
  {
    rstrIn.getline(szLine, 100);
    upper(ltrim(szLine));
    if (sscanf(szLine, "%lu %lu", &ulNumNodes, &ulNumElems)!=2)
      return false;
    Base::Console().Log("Expected number of nodes: %d, number of elements: %d\n", ulNumNodes, ulNumElems);
  }
  else 
    return false;

  unsigned long ulNumElemsCounter = 1;
  while ((ulNumElemsCounter<=ulNumElems) && (rstrIn.eof() == false) && (rstrIn.bad() == false))
  {
    rstrIn.getline(szLine, 100);
    upper(ltrim(szLine));
    if (sscanf(szLine, "%d %d %d", &(elem[ulNumElemsCounter].iV[0]),
                                   &(elem[ulNumElemsCounter].iV[1]),
                                   &(elem[ulNumElemsCounter].iV[2])) != 3)
    ulNumElemsCounter++;
  }
  unsigned long ulNumNodesCounter = 1;
  while ((ulNumNodesCounter<=ulNumNodes) && (rstrIn.eof() == false) && (rstrIn.bad() == false))
  {
    rstrIn.getline(szLine, 100);
    upper(ltrim(szLine));
    if(sscanf(szLine, "%f %f %f", &(node[ulNumNodesCounter].x),
                                  &(node[ulNumNodesCounter].y),
                                  &(node[ulNumNodesCounter].z)) != 3)
    ulNumNodesCounter++;
  }

  unsigned long ulNumCyl = 0;
  /* Converting data to Mesh. Negativ convertation for right orientation of normal-vectors. */
  for (std::map<int, TRIA>::iterator MI=elem.begin(); MI!=elem.end(); MI++)
  {
    if ((*MI).second.iV[2] == -1) 
    {
      ulNumCyl++;
      continue;
    }
    clMeshFacet._aclPoints[0].x = node[(*MI).second.iV[0]].x;
    clMeshFacet._aclPoints[0].y = node[(*MI).second.iV[0]].y;
    clMeshFacet._aclPoints[0].z = node[(*MI).second.iV[0]].z;

    clMeshFacet._aclPoints[1].x = node[(*MI).second.iV[1]].x;
    clMeshFacet._aclPoints[1].y = node[(*MI).second.iV[1]].y;
    clMeshFacet._aclPoints[1].z = node[(*MI).second.iV[1]].z;

    clMeshFacet._aclPoints[2].x = node[(*MI).second.iV[2]].x;
    clMeshFacet._aclPoints[2].y = node[(*MI).second.iV[2]].y;
    clMeshFacet._aclPoints[2].z = node[(*MI).second.iV[2]].z;

    vTriangle.push_back (clMeshFacet);
  }

  _rclMesh = vTriangle;

  return true;
}

// --------------------------------------------------------------

/** Saves the mesh object into an ASCII file. */
bool MeshOutput::SaveAsciiSTL (std::ostream &rstrOut) const
{
  MeshFacetIterator clIter(_rclMesh), clEnd(_rclMesh);  
  const MeshGeomFacet *pclFacet;
  unsigned long i, ulCtFacet;
  char szBuf[200];

  if ( !rstrOut || rstrOut.bad() == true || _rclMesh.CountFacets() == 0 )
    return false;

  Base::SequencerLauncher seq("saving...", _rclMesh.CountFacets() + 1);  

  strcpy(szBuf, "solid MESH\n");
  rstrOut.write(szBuf, strlen(szBuf));

  clIter.Begin();
  clEnd.End();
  ulCtFacet = 0;
  while (clIter < clEnd)
  {
    pclFacet = &(*clIter);
      
    // normale
    sprintf(szBuf, "  facet normal %.4f %.4f %.4f\n", pclFacet->GetNormal().x,
             pclFacet->GetNormal().y,  pclFacet->GetNormal().z);
    rstrOut.write(szBuf, strlen(szBuf));

    strcpy(szBuf, "    outer loop\n");
    rstrOut.write(szBuf, strlen(szBuf));

    for (i = 0; i < 3; i++)
    {
      sprintf(szBuf, "      vertex %.4f %.4f %.4f\n", pclFacet->_aclPoints[i].x,
              pclFacet->_aclPoints[i].y, pclFacet->_aclPoints[i].z);
      rstrOut.write(szBuf, strlen(szBuf));
    }

    strcpy(szBuf, "    endloop\n");
    rstrOut.write(szBuf, strlen(szBuf));

    strcpy(szBuf, "  endfacet\n");
    rstrOut.write(szBuf, strlen(szBuf));

    ++clIter; 
    Base::Sequencer().next( true );// allow to cancel
  } 

  strcpy(szBuf, "endsolid MESH\n");
  rstrOut.write(szBuf, strlen(szBuf));

 
  return true;
}

/** Saves the mesh object into a binary file. */
bool MeshOutput::SaveBinarySTL (std::ostream &rstrOut) const
{
  MeshFacetIterator clIter(_rclMesh), clEnd(_rclMesh);  
  const MeshGeomFacet *pclFacet;
  unsigned long i, ulCtFacet;
  unsigned short usAtt;
  char szInfo[81];

  if ( !rstrOut || rstrOut.bad() == true /*|| _rclMesh.CountFacets() == 0*/ )
    return false;

  Base::SequencerLauncher seq("saving...", _rclMesh.CountFacets() + 1);  
 
  strcpy(szInfo, "MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH\n");
  rstrOut.write(szInfo, strlen(szInfo));

  unsigned long uCtFts = _rclMesh.CountFacets();
  rstrOut.write((const char*)&uCtFts, sizeof(unsigned long));

  usAtt = 0;
  clIter.Begin();
  clEnd.End();
  ulCtFacet = 0;
  while (clIter < clEnd)
  {
    pclFacet = &(*clIter);
    // Normale
    rstrOut.write((const char*)&(pclFacet->GetNormal().x), sizeof(float));
    rstrOut.write((const char*)&(pclFacet->GetNormal().y), sizeof(float));
    rstrOut.write((const char*)&(pclFacet->GetNormal().z), sizeof(float));

    // Eckpunkte
    for (i = 0; i < 3; i++)
    {
      rstrOut.write((const char*)&(pclFacet->_aclPoints[i].x), sizeof(float));
      rstrOut.write((const char*)&(pclFacet->_aclPoints[i].y), sizeof(float));
      rstrOut.write((const char*)&(pclFacet->_aclPoints[i].z), sizeof(float));
    }

    // Attribut 
    rstrOut.write((const char*)&usAtt, sizeof(unsigned short));

    ++clIter;
    Base::Sequencer().next( true ); // allow to cancel
  }

  return true;
}

/** Saves the mesh object into an XML file. */
void MeshOutput::SaveXML (Base::Writer &writer) const
{
  const MeshPointArray& rPoints = _rclMesh.GetPoints();
  const MeshFacetArray& rFacets = _rclMesh.GetFacets();

  //  writer << writer.ind() << "<Mesh>" << std::endl;

  writer.incInd();
  writer << writer.ind() << "<Points Count=\"" << _rclMesh.CountPoints() << "\">" << std::endl;

  writer.incInd();
  for (MeshPointArray::_TConstIterator itp = rPoints.begin(); itp != rPoints.end(); itp++)
  {
    writer <<  writer.ind() << "<P "
                               << "x=\"" <<  itp->x << "\" "
                               << "y=\"" <<  itp->y << "\" "
                               << "z=\"" <<  itp->z << "\"/>"
                         << std::endl;
  }
  writer.decInd();
  writer << writer.ind() << "</Points>" << std::endl;

  // write the faces....
  writer <<writer.ind() << "<Faces Count=\"" << _rclMesh.CountFacets() << "\">" << std::endl;

  writer.incInd();
  for (MeshFacetArray::_TConstIterator it = rFacets.begin(); it != rFacets.end(); it++)
  {

    writer << writer.ind() << "<F "
                               << "p0=\"" <<  it->_aulPoints[0] << "\" "
                               << "p1=\"" <<  it->_aulPoints[1] << "\" "
                               << "p2=\"" <<  it->_aulPoints[2] << "\" " 
                               << "n0=\"" <<  it->_aulNeighbours[0] << "\" "
                               << "n1=\"" <<  it->_aulNeighbours[1] << "\" "
                               << "n2=\"" <<  it->_aulNeighbours[2] << "\"/>" 
                               << std::endl;
  } 
  writer.decInd();
  writer << writer.ind() << "</Faces>" << std::endl;

  writer << writer.ind() << "</Mesh>" << std::endl;
  writer.decInd();
}

/** Writes an OpenInventor file. */
bool MeshOutput::SaveInventor (std::ostream &rstrOut) const
{
  if ((!rstrOut) || (rstrOut.bad() == true) || (_rclMesh.CountFacets() == 0))
    return false;

  MeshFacetIterator clIter(_rclMesh), clEnd(_rclMesh);
  MeshPointIterator clPtIter(_rclMesh), clPtEnd(_rclMesh);
  const MeshGeomFacet* pclFacet;
  unsigned long ulCtFacet, ulAllFacets = _rclMesh.CountFacets();
  char szBuf[200];

  Base::SequencerLauncher seq("Saving...", _rclMesh.CountFacets() + 1);

  // Header info
  strcpy(szBuf, "#Inventor V2.1 ascii\n\n");
  rstrOut.write(szBuf, strlen(szBuf));
  strcpy(szBuf, "# Created by FreeCAD <http://free-cad.sourceforge.net>\n");
  rstrOut.write(szBuf, strlen(szBuf));
  sprintf(szBuf, "# Triangle mesh contains %lu vertices and %lu faces\n",
      _rclMesh.CountPoints(), _rclMesh.CountFacets());
  rstrOut.write(szBuf, strlen(szBuf));
  strcpy(szBuf, "Separator {\n\n");
  rstrOut.write(szBuf, strlen(szBuf));
  strcpy(szBuf, "  Label {\n");
  rstrOut.write(szBuf, strlen(szBuf));
  strcpy(szBuf, "    label \"Triangle mesh\"\n  }\n");
  rstrOut.write(szBuf, strlen(szBuf));

  // write out the normals of the facets
  strcpy(szBuf, "  Normal { \n");
  rstrOut.write(szBuf, strlen(szBuf));
  strcpy(szBuf, "    vector [ ");
  rstrOut.write(szBuf, strlen(szBuf));

  clIter.Begin();
  clEnd.End();
  ulCtFacet = 0;

  pclFacet = &(*clIter);
  sprintf(szBuf, "%.6f  %.6f  %.6f", pclFacet->GetNormal().x,
          pclFacet->GetNormal().y, pclFacet->GetNormal().z);
  rstrOut.write(szBuf, strlen(szBuf));
  ++clIter;

  while (clIter < clEnd)
  {
    pclFacet = &(*clIter);
    sprintf(szBuf, ",\n        %.6f  %.6f  %.6f", pclFacet->GetNormal().x,
            pclFacet->GetNormal().y, pclFacet->GetNormal().z);
    rstrOut.write(szBuf, strlen(szBuf));
    ++clIter;

    Base::Sequencer().next( true ); // allow to cancel
  }
  strcpy(szBuf, " ]\n\n  }\n");
  rstrOut.write(szBuf, strlen(szBuf));

  // coordinates of the vertices
  strcpy(szBuf, "  NormalBinding {\n    value PER_FACE\n  }\n");
  rstrOut.write(szBuf, strlen(szBuf));
  strcpy(szBuf, "  Coordinate3 {\n    point [ ");
  rstrOut.write(szBuf, strlen(szBuf));

  clPtIter.Begin();
  clPtEnd.End();
  ulCtFacet = 0;

  sprintf(szBuf, "%.6f  %.6f  %.6f", clPtIter->x, clPtIter->y, clPtIter->z);
  rstrOut.write(szBuf, strlen(szBuf));
  ++clPtIter;

  while (clPtIter < clPtEnd)
  {
    sprintf(szBuf, ",\n        %.6f  %.6f  %.6f", clPtIter->x, clPtIter->y, clPtIter->z);
    rstrOut.write(szBuf, strlen(szBuf));
    ++clPtIter;

    Base::Sequencer().next( true ); // allow to cancel
  }
  strcpy(szBuf, " ]\n\n  }\n");
  rstrOut.write(szBuf, strlen(szBuf));

  // and finally the facets with their point indices
  strcpy(szBuf, "  IndexedFaceSet {\n    coordIndex [ ");
  rstrOut.write(szBuf, strlen(szBuf));

  const MeshFacet clFacet = _rclMesh.GetFacets()[0];
  sprintf(szBuf, "%lu, %lu, %lu, %d", clFacet._aulPoints[0],
            clFacet._aulPoints[1], clFacet._aulPoints[2], - 1);
  rstrOut.write(szBuf, strlen(szBuf));

  unsigned long i = 1;
  while (i < ulAllFacets)
  {
    // write two triples per line
    const MeshFacet clFacet = _rclMesh.GetFacets()[i];
    if ( i%2==0 ) {
      sprintf(szBuf, ",\n        %lu, %lu, %lu, %d", clFacet._aulPoints[0],
                clFacet._aulPoints[1], clFacet._aulPoints[2], -1);
    } else {
      sprintf(szBuf, ", %lu, %lu, %lu, %d", clFacet._aulPoints[0],
                clFacet._aulPoints[1], clFacet._aulPoints[2], -1);
    }
    rstrOut.write(szBuf, strlen(szBuf));
    ++i;
  }


  strcpy(szBuf, " ]\n\n  }\n");
  rstrOut.write(szBuf, strlen(szBuf));

  strcpy(szBuf, "#End of triangle mesh \n}\n\n");
  rstrOut.write(szBuf, strlen(szBuf));

  return true;
}

/** Writes a VRML file. */
bool MeshOutput::SaveVRML (std::ostream &rstrOut) const
{
  App::Material rclMat;
  return SaveMeshVRML(_rclMesh).Save(rstrOut, rclMat);
}

/** Writes a Nastran file. */
bool MeshOutput::SaveNastran (std::ostream &rstrOut) const
{
  if ((!rstrOut) || (rstrOut.bad() == true) || (_rclMesh.CountFacets() == 0))
    return false;

  MeshPointIterator clPIter(_rclMesh);
  MeshFacetIterator clTIter(_rclMesh);
  char szBuf[200];
  int iIndx = 1, iDec, iSign, iCount = 0;

  Base::Sequencer().start("Saving...", _rclMesh.CountFacets() + 1);
  
  for (clPIter.Init(); clPIter.More(); clPIter.Next())
  {
    strcpy(szBuf, "GRID");
    rstrOut.write(szBuf, strlen(szBuf));

#ifdef __GNUC__
    fcvt ((float)iIndx, iCount, &iDec, &iSign);
#else
    _fcvt ((float)iIndx, iCount, &iDec, &iSign);
#endif
    int i;
    for (i = iDec; i <= 11; i++)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }
    sprintf(szBuf, "%d", iIndx);
    rstrOut.write(szBuf, strlen(szBuf));

#ifdef __GNUC__
    fcvt ((*clPIter).x, iCount, &iDec, &iSign);
#else
    _fcvt ((*clPIter).x, iCount, &iDec, &iSign);
#endif
    if (iSign == 0)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }
    if (fabs((*clPIter).x) < pow (10.0, (iDec-1))) iDec--;
    if (iDec <= 0) iDec = 1;
    for (i = iDec; i <= 10; i++)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }
    sprintf(szBuf, "%.3f", (*clPIter).x);
    rstrOut.write(szBuf, strlen(szBuf));

#ifdef __GNUC__
    fcvt ((*clPIter).y, iCount, &iDec, &iSign);
#else
    _fcvt ((*clPIter).y, iCount, &iDec, &iSign);
#endif
    if (iSign == 0)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }
    if (fabs((*clPIter).y) < pow (10.0, (iDec-1))) iDec--;
    if (iDec <= 0) iDec = 1;
    for (i = iDec; i <= 2; i++)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }
    sprintf(szBuf, "%.3f", (*clPIter).y);
    rstrOut.write(szBuf, strlen(szBuf));

#ifdef __GNUC__
    fcvt ((*clPIter).z, iCount, &iDec, &iSign);
#else
    _fcvt ((*clPIter).z, iCount, &iDec, &iSign);
#endif
    if (iSign == 0)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }
    if (fabs((*clPIter).z) < pow (10.0, (iDec-1))) iDec--;
    if (iDec <= 0) iDec = 1;
    for (i = iDec; i <= 2; i++)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }

    sprintf(szBuf, "%.3f\n", (*clPIter).z);
    rstrOut.write(szBuf, strlen(szBuf));

    iIndx++;
    Base::Sequencer().next();
  }

  iIndx = 1;
  for (clTIter.Init(); clTIter.More(); clTIter.Next())
  {
    strcpy(szBuf, "CTRIA3");
    rstrOut.write(szBuf, strlen(szBuf));

#ifdef __GNUC__
    fcvt ((float)iIndx, iCount, &iDec, &iSign);
#else
    _fcvt ((float)iIndx, iCount, &iDec, &iSign);
#endif
    int i;
    for (i = iDec; i <= 9; i++)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }

    sprintf(szBuf, "%d       0", iIndx);
    rstrOut.write(szBuf, strlen(szBuf));

#ifdef __GNUC__
    fcvt ((float)clTIter.GetIndicies()._aulPoints[1]+1, iCount, &iDec, &iSign);
#else
    _fcvt ((float)clTIter.GetIndicies()._aulPoints[1]+1, iCount, &iDec, &iSign);
#endif
    for (i = iDec; i <= 7; i++)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }

    sprintf(szBuf, "%lu", clTIter.GetIndicies()._aulPoints[1]+1);
    rstrOut.write(szBuf, strlen(szBuf));

#ifdef __GNUC__
    fcvt ((float)clTIter.GetIndicies()._aulPoints[0]+1, iCount, &iDec, &iSign);
#else
    _fcvt ((float)clTIter.GetIndicies()._aulPoints[0]+1, iCount, &iDec, &iSign);
#endif
    for (i = iDec; i <= 7; i++)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }

    sprintf(szBuf, "%lu", clTIter.GetIndicies()._aulPoints[0]+1);
    rstrOut.write(szBuf, strlen(szBuf));

#ifdef __GNUC__
    fcvt ((float)clTIter.GetIndicies()._aulPoints[2]+1, iCount, &iDec, &iSign);
#else
    _fcvt ((float)clTIter.GetIndicies()._aulPoints[2]+1, iCount, &iDec, &iSign);
#endif
    for (i = iDec; i <= 7; i++)
    {
      strcpy(szBuf, " ");
      rstrOut.write(szBuf, strlen(szBuf));
    }

    sprintf(szBuf, "%lu\n", clTIter.GetIndicies()._aulPoints[2]+1);
    rstrOut.write(szBuf, strlen(szBuf));

//    sprintf(szBuf, "CTRIA3 %d 0 %d %d %d\n", iIndx, clTIter.GetIndicies()._aulPoints[0]+1, clTIter.GetIndicies()._aulPoints[1]+1, clTIter.GetIndicies()._aulPoints[2]+1);
//    rstrOut.write(szBuf, strlen(szBuf));
    iIndx++;
    Base::Sequencer().next();
  }
  strcpy(szBuf, "ENDDATA");
  rstrOut.write(szBuf, strlen(szBuf));

  Base::Sequencer().stop();

  return true;
}

/** Writes a Cadmould FE file. */
bool MeshOutput::SaveCadmouldFE (std::ostream &rstrOut) const
{
  return false;
}

// --------------------------------------------------------------

SaveMeshVRML::SaveMeshVRML (const MeshKernel &rclM)
: _rclMesh(rclM), _pclVRMLInfo(0)
{
}

SaveMeshVRML::SaveMeshVRML (const MeshKernel &rclM, VRMLInfo* pclVRMLInfo)
: _rclMesh(rclM), _pclVRMLInfo(pclVRMLInfo)
{
}

bool SaveMeshVRML::Save (std::ostream &rstrOut, const std::vector<App::Color> &raclColor, const App::Material &rclMat, bool bColorPerVertex) const
{
  if ((!rstrOut) || (rstrOut.bad() == true) ||
      (_rclMesh.CountFacets() == 0))
  {
    return false;
  }


  Base::BoundBox3f clBB = _rclMesh.GetBoundBox();
  Base::Vector3f   clCenter = clBB.CalcCenter();

  Base::SequencerLauncher seq("Saving VRML file...", _rclMesh.CountPoints() + raclColor.size() + _rclMesh.CountFacets());

  char szBuf[1000];
  strcpy(szBuf, "#VRML V2.0 utf8\n");
  rstrOut.write(szBuf, strlen(szBuf));

  if (_pclVRMLInfo)
    WriteVRMLHeaderInfo(rstrOut);
  else{
    strcpy(szBuf, "WorldInfo {\n"
                  "  title \"Exported tringle mesh to VRML97\"\n"
                  "  info [\"Created by FreeCAD\"\n"
                  "        \"<http://free-cad.sourceforge.net>\"]\n"
                  "}\n");
    rstrOut.write(szBuf, strlen(szBuf));
  }

  // Write background
  // Note: We must write the background info straight after the header and before any geometry.
  // To see the background color in the Inventor viewer we must either skip the skyAngle property
  // or set it to 1.57, otherwise we'll see a strange beam in the scene.
  // FIXME: Test this also with external VRML viewer plugins.
  if (_pclVRMLInfo)
  {
    float r = float(_pclVRMLInfo->_clColor.r) / 255.0f;
    float g = float(_pclVRMLInfo->_clColor.g) / 255.0f;
    float b = float(_pclVRMLInfo->_clColor.b) / 255.0f;
    sprintf(szBuf, "Background\n{\n  skyAngle    1.57\n  skyColor    %.1f %.1f %.1f\n}\n", r, g, b);
  }
  else
    strcpy(szBuf, "Background\n{\n  skyAngle    1.57\n  skyColor    0.1 0.2 0.2\n}\n");
  rstrOut.write(szBuf, strlen(szBuf));
  
  // Transform
  strcpy(szBuf, "Transform\n{\n");
  rstrOut.write(szBuf, strlen(szBuf));
  strcpy(szBuf,  "scale             1 1 1\n");
  rstrOut.write(szBuf, strlen(szBuf));
  strcpy(szBuf,  "rotation          0 0 1 0\n");
  rstrOut.write(szBuf, strlen(szBuf));
  strcpy(szBuf,  "scaleOrientation  0 0 1 0\n");
  rstrOut.write(szBuf, strlen(szBuf));
  strcpy(szBuf,  "bboxCenter        0 0 0\n");
  rstrOut.write(szBuf, strlen(szBuf));
  sprintf(szBuf, "bboxSize      %.3f %.3f %.3f\n", clBB.LengthX(), clBB.LengthY(), clBB.LengthZ());
  rstrOut.write(szBuf, strlen(szBuf));
  sprintf(szBuf, "center        %.3f  %.3f  %.3f\n", 0.0f, 0.0f, 0.0f);
  rstrOut.write(szBuf, strlen(szBuf));
  sprintf(szBuf,  "translation  %.3f  %.3f  %.3f\n", -clCenter.x, -clCenter.y, -clCenter.z);
  rstrOut.write(szBuf, strlen(szBuf));

  strcpy(szBuf, "children\n[\n");
  rstrOut.write(szBuf, strlen(szBuf));

  strcpy(szBuf, "Shape\n{\n");
  rstrOut.write(szBuf, strlen(szBuf));  // Begin Shape

  // write appearance
  strcpy(szBuf, "  appearance Appearance\n  {\n    material Material\n    {\n");
  strcat(szBuf, "      ambientIntensity 0.2\n");
  if (raclColor.size() > 0) // black
  {
    strcat(szBuf, "      diffuseColor     0.2 0.2 0.2\n");
    strcat(szBuf, "      emissiveColor    0.2 0.2 0.2\n");
    strcat(szBuf, "      specularColor    0.2 0.2 0.2\n");
  }
  else
  {
    App::Color clCol;
    clCol = rclMat.diffuseColor;
    sprintf(szBuf, "%s      diffuseColor      %.2f %.2f %.2f\n", szBuf, clCol.r , clCol.g  , clCol.b ); 
    clCol = rclMat.emissiveColor;
    sprintf(szBuf, "%s      emissiveColor     %.2f %.2f %.2f\n", szBuf, clCol.r , clCol.g  , clCol.b ); 
    clCol = rclMat.specularColor;
    sprintf(szBuf, "%s      specularColor     %.2f %.2f %.2f\n", szBuf, clCol.r , clCol.g  , clCol.b ); 
//    strcat(szBuf, "      diffuseColor     0.2 0.2 0.8\n");
//    strcat(szBuf, "      emissiveColor    0.2 0.2 0.8\n");
//    strcat(szBuf, "      specularColor    0.2 0.2 0.8\n");
  }
  sprintf(szBuf, "%s      shininess        %.2f\n", szBuf, rclMat.shininess);
  sprintf(szBuf, "%s      transparency     %.2f\n", szBuf, rclMat.transparency);
//  strcat(szBuf, "      shininess        0.3\n");
//  strcat(szBuf, "      transparency     0.0\n");
  strcat(szBuf, "    }\n  }\n");
  rstrOut.write(szBuf, strlen(szBuf));  // end write appearance


  strcpy(szBuf, "  geometry IndexedFaceSet\n  {\n");
  rstrOut.write(szBuf, strlen(szBuf));  // Begin IndexedFaceSet

  strcpy(szBuf, "    solid   FALSE\n");
  rstrOut.write(szBuf, strlen(szBuf));

  // write coords
  strcpy(szBuf, "    coord Coordinate\n    {\n      point\n      [\n");
  rstrOut.write(szBuf, strlen(szBuf));
  MeshPointIterator pPIter(_rclMesh);
  unsigned long i = 0, k = _rclMesh.CountPoints();
  for (pPIter.Init(); pPIter.More(); pPIter.Next())
  {
    sprintf(szBuf, "        %.3f %.3f %.3f", pPIter->x, pPIter->y, pPIter->z);
    if (i++ < (k-1))
      strcat(szBuf, ",\n");
    else
      strcat(szBuf, "\n");
    rstrOut.write(szBuf, strlen(szBuf));

    Base::Sequencer().next();
  }
  strcpy(szBuf, "      ]\n    }\n");
  rstrOut.write(szBuf, strlen(szBuf));  // end write coord  

  sprintf(szBuf, "    colorPerVertex  %s\n", bColorPerVertex ? "TRUE" : "FALSE");
  rstrOut.write(szBuf, strlen(szBuf));

  if (raclColor.size() > 0)
  {  // write colors for each vertex
    strcpy(szBuf, "    color Color\n    {\n       color\n       [\n");
    rstrOut.write(szBuf, strlen(szBuf));
    
    for (std::vector<App::Color>::const_iterator pCIter = raclColor.begin(); pCIter != raclColor.end(); pCIter++)
    { 
      sprintf(szBuf, "         %.3f %.3f %.3f", float(pCIter->r) / 255.0f, float(pCIter->g) / 255.0f, float(pCIter->b) / 255.0f); 
      if (pCIter < (raclColor.end() - 1))
        strcat(szBuf, ",\n");
      else 
        strcat(szBuf, "\n");
      rstrOut.write(szBuf, strlen(szBuf));
      
      Base::Sequencer().next();
    }
    strcpy(szBuf, "      ]\n    }\n");
    rstrOut.write(szBuf, strlen(szBuf));

/*
    strcpy(szBuf, "    colorIndex\n    [\n");
    rstrOut.write(szBuf, strlen(szBuf));
    for (int i = 0; i < (_rclMesh.CountPoints() - 1); i++)
    {
      sprintf(szBuf, "      %d,\n", i+1);
      rstrOut.write(szBuf, strlen(szBuf));     
    }

    sprintf(szBuf, "      %d\n    ]\n", _rclMesh.CountPoints());
    rstrOut.write(szBuf, strlen(szBuf));     
*/
  }

  // write face index
  strcpy(szBuf, "    coordIndex\n    [\n");
  rstrOut.write(szBuf, strlen(szBuf));
  MeshFacetIterator pFIter(_rclMesh);
  i = 0, k = _rclMesh.CountFacets();
  for (pFIter.Init(); pFIter.More(); pFIter.Next())
  {
    MeshFacet clFacet = pFIter.GetIndicies();
    sprintf(szBuf, "      %lu, %lu, %lu, -1", clFacet._aulPoints[0], clFacet._aulPoints[1], clFacet._aulPoints[2]);
    if (i++ < (k-1))
      strcat(szBuf, ",\n");
    else
      strcat(szBuf, "\n");
    rstrOut.write(szBuf, strlen(szBuf));

    Base::Sequencer().next();
  }
  strcpy(szBuf, "    ]\n");
  rstrOut.write(szBuf, strlen(szBuf));  // 

  strcpy(szBuf, "  }\n");
  rstrOut.write(szBuf, strlen(szBuf));  // End IndexedFaceSet


  strcpy(szBuf, "}\n");
  rstrOut.write(szBuf, strlen(szBuf));  // End Shape

  // close children and Transform
  strcpy(szBuf, "]\n}\n");
  rstrOut.write(szBuf, strlen(szBuf));

  // write viewpoints
  for (i = 0; i < 6; i++)
  {
    strcpy(szBuf, "Viewpoint\n{\n");
    strcat(szBuf, "  jump         TRUE\n");

    Base::Vector3f clPos = clCenter;
    float     fLen  = clBB.CalcDiagonalLength();
    switch (i)
    {
      case 0:
      {
        Vector at, pos, up, rot_axis;
        double rot_angle;
        at.x  = clPos.x;        at.y  = clPos.y;        at.z  = clPos.z;
        pos.x = clPos.x;        pos.y = clPos.y;        pos.z = clPos.z + fLen;
        up.x  = 0.0;            up.y  = 1.0;            up.z  = 0.0;
        Convert_Camera_Model(&pos, &at, &up, &rot_axis, &rot_angle);
        sprintf(szBuf, "%s  orientation   %.3f %.3f %.3f %.3f\n", szBuf, rot_axis.x, rot_axis.y, rot_axis.z, rot_angle);
        strcat(szBuf, "  description  \"top\"\n");
        clPos.z += fLen;
        break;
      }
      case 1:
      {
        Vector at, pos, up, rot_axis;
        double rot_angle;
        at.x  = clPos.x;        at.y  = clPos.y;        at.z  = clPos.z;
        pos.x = clPos.x;        pos.y = clPos.y;        pos.z = clPos.z - fLen;
        up.x  = 0.0;            up.y  = -1.0;           up.z  = 0.0;
        Convert_Camera_Model(&pos, &at, &up, &rot_axis, &rot_angle);
        sprintf(szBuf, "%s  orientation   %.3f %.3f %.3f %.3f\n", szBuf, rot_axis.x, rot_axis.y, rot_axis.z, rot_angle);
        strcat(szBuf, "  description  \"bottom\"\n");
        clPos.z -= fLen;
        break;
      }
      case 2:
      {
        Vector at, pos, up, rot_axis;
        double rot_angle;
        at.x  = clPos.x;        at.y  = clPos.y;        at.z  = clPos.z;
        pos.x = clPos.x;        pos.y = clPos.y - fLen; pos.z = clPos.z;
        up.x  = 0.0;            up.y  = 0.0;            up.z  = 1.0;
        Convert_Camera_Model(&pos, &at, &up, &rot_axis, &rot_angle);
        sprintf(szBuf, "%s  orientation   %.3f %.3f %.3f %.3f\n", szBuf, rot_axis.x, rot_axis.y, rot_axis.z, rot_angle);
        strcat(szBuf, "  description  \"front\"\n");
        clPos.y -= fLen;
        break;
      }
      case 3:
      {
        Vector at, pos, up, rot_axis;
        double rot_angle;
        at.x  = clPos.x;        at.y  = clPos.y;        at.z  = clPos.z;
        pos.x = clPos.x;        pos.y = clPos.y + fLen; pos.z = clPos.z;
        up.x  = 0.0;            up.y  = 0.0;            up.z  = 1.0;
        Convert_Camera_Model(&pos, &at, &up, &rot_axis, &rot_angle);
        sprintf(szBuf, "%s  orientation   %.3f %.3f %.3f %.3f\n", szBuf, rot_axis.x, rot_axis.y, rot_axis.z, rot_angle);
        strcat(szBuf, "  description  \"back\"\n");
        clPos.y += fLen;
        break;
      }
      case 4:
      {
        Vector at, pos, up, rot_axis;
        double rot_angle;
        at.x  = clPos.x;        at.y  = clPos.y;        at.z  = clPos.z;
        pos.x = clPos.x - fLen; pos.y = clPos.y;        pos.z = clPos.z;
        up.x  = 0.0;            up.y  = 0.0;            up.z  = 1.0;
        Convert_Camera_Model(&pos, &at, &up, &rot_axis, &rot_angle);
        sprintf(szBuf, "%s  orientation   %.3f %.3f %.3f %.3f\n", szBuf, rot_axis.x, rot_axis.y, rot_axis.z, rot_angle);
        strcat(szBuf, "  description  \"right\"\n");
        clPos.x -= fLen;
        break;
      }
      case 5:
      {
        Vector at, pos, up, rot_axis;
        double rot_angle;
        at.x  = clPos.x;        at.y  = clPos.y;        at.z  = clPos.z;
        pos.x = clPos.x + fLen; pos.y = clPos.y;        pos.z = clPos.z;
        up.x  = 0.0;            up.y  = 0.0;            up.z  = 1.0;
        Convert_Camera_Model(&pos, &at, &up, &rot_axis, &rot_angle);
        sprintf(szBuf, "%s  orientation   %.3f %.3f %.3f %.3f\n", szBuf, rot_axis.x, rot_axis.y, rot_axis.z, rot_angle);
        strcat(szBuf, "  description  \"left\"\n");
        clPos.x += fLen;
        break;
      }
    }
    rstrOut.write(szBuf, strlen(szBuf));
    sprintf(szBuf, "  position     %.3f %.3f %.3f\n", clPos.x, clPos.y, clPos.z);
    rstrOut.write(szBuf, strlen(szBuf));
    strcpy(szBuf, "}\n");
    rstrOut.write(szBuf, strlen(szBuf)); 
  }

  // write navigation info
  strcpy(szBuf, "NavigationInfo\n{\n");
  strcat(szBuf, "  avatarSize       [0.25, 1.6, 0.75]\n");
  strcat(szBuf, "  headlight        TRUE\n");
  strcat(szBuf, "  speed            1.0\n");
  strcat(szBuf, "  type             \"EXAMINE\"\n");
  strcat(szBuf, "  visibilityLimit  0.0\n");
  strcat(szBuf, "}\n");
  rstrOut.write(szBuf, strlen(szBuf));

  // write custom viewpoints
  if (_pclVRMLInfo)
  {
    WriteVRMLViewpoints(rstrOut);
  }

  if (_pclVRMLInfo)
    WriteVRMLAnnotations(rstrOut);

  Base::Sequencer().stop();

  return true;
}

void SaveMeshVRML::WriteVRMLHeaderInfo(std::ostream &clStream) const
{
  char szBuf[1000];
  // save information about file
  //
  sprintf(szBuf, "#=================================================#\n#\n");
  clStream.write(szBuf, strlen(szBuf));
  sprintf(szBuf, "# F I L E   I N F O R M A T I O N\n#\n");
  clStream.write(szBuf, strlen(szBuf));
  sprintf(szBuf, "# This file was created by %s\n", _pclVRMLInfo->_clAuthor.c_str());
  clStream.write(szBuf, strlen(szBuf));
  sprintf(szBuf, "# Creation Date:    %s\n", _pclVRMLInfo->_clDate.c_str());
  clStream.write(szBuf, strlen(szBuf));
  sprintf(szBuf, "# Company:          %s\n", _pclVRMLInfo->_clCompany.c_str());
  clStream.write(szBuf, strlen(szBuf));
  std::vector<std::string>::const_iterator sIt = _pclVRMLInfo->_clComments.begin();
  sprintf(szBuf, "# Further comments: %s\n", sIt->c_str());
  clStream.write(szBuf, strlen(szBuf));
  for (sIt++ ;sIt != _pclVRMLInfo->_clComments.end(); ++sIt)
  {
    sprintf(szBuf, "#                   %s\n", sIt->c_str());
    clStream.write(szBuf, strlen(szBuf));
  }
  sprintf(szBuf, "#=================================================#\n\n");
  clStream.write(szBuf, strlen(szBuf));
}

void SaveMeshVRML::WriteVRMLAnnotations(std::ostream &clStream) const
{
  float r = float(_pclVRMLInfo->_clColor.r) / 255.0f;
  float g = float(_pclVRMLInfo->_clColor.g) / 255.0f;
  float b = float(_pclVRMLInfo->_clColor.b) / 255.0f;
  float textr = 1.0f - r;
  float textg = 1.0f - g;
  float textb = 1.0f - b;

  if (fabs(textr-r) < 0.05)
    textr = 1.0f;
  if (fabs(textg-g) < 0.05)
    textg = 1.0f;
  if (fabs(textb-b) < 0.05)
    textb = 1.0f;

  // annotationen
  char szBuf[1000];
  sprintf(szBuf, 
    "DEF User ProximitySensor {\n"
    " size        1000000 1000000 1000000\n"
    "}\n"
    "\n"
    "    Group { \n"
    "      children [\n"
    " DEF UserPos Transform {\n"
    "   children [\n"
    "     # Text position\n"
    "     Transform {\n"
    "       translation  -1.0 -0.75 -2\n"
    "       children [\n"
    "		  Transform {\n"
    "		    translation 1.95 0.75 0\n"
    "		    children [\n"
    "		      Shape {\n");
  clStream.write(szBuf, strlen(szBuf));
  if (_pclVRMLInfo->_bSavePicture)
  {
  sprintf(szBuf, 
    "		        appearance Appearance {\n" 
    "			  texture ImageTexture { \n"
    "			    url \"%s\"\n"
    "			    repeatS FALSE\n"
    "			    repeatT FALSE\n"
    "			  }\n"
    "		        }\n"
    "		        geometry IndexedFaceSet {\n"
    "			  coord Coordinate { point [ -0.08 -0.8 0,\n"
    "						     0.08 -0.8 0,\n"
    "						     0.08  0.8 0,\n"
    "						     -0.08  0.8 0\n"
    "						   ]\n"
    "					   }\n"
    "			  coordIndex [0,1,2,3, -1]\n"
    "	          texCoord TextureCoordinate {\n"
    "	            point	[ 0 0,\n"
    "		            1 0,\n"
    "		            1 1,\n"
    "		            0 1 ]\n"
    "	          }\n"
    "	          texCoordIndex	[ 0, 1, 2, 3, -1 ]\n\n"
    "	          solid	FALSE\n"
    "		        }\n", _pclVRMLInfo->_clPicFileName.c_str());
  clStream.write(szBuf, strlen(szBuf)); 
  }
  sprintf(szBuf, 
    "		      }\n"
    "		    ]\n"
    "		  }\n"
    "  Shape {\n"
    "    appearance DEF COAP Appearance {\n"
    "      material Material {diffuseColor %.1f %.1f %.1f}} # text color\n"
    "    geometry   DEF MyText Text {\n"
    "      string \"%s\"\n"          
    "      fontStyle DEF COFS FontStyle {\n"
    "        family [ \"Verdana\", \"Arial\", \"Helvetica\" ]\n"
    "        size         0.08                     # text size\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "       ]\n"
    "     }\n"
    "   ]\n"
    " }\n"
    "      ]\n"
    "    }\n"
    ""
    "ROUTE User.position_changed TO UserPos.set_translation\n"
    "ROUTE User.orientation_changed TO UserPos.set_rotation\n", textr, textg,textb, _pclVRMLInfo->_clAnnotation.c_str());
    clStream.write(szBuf, strlen(szBuf)); 
}

void SaveMeshVRML::WriteVRMLViewpoints(std::ostream &rstrOut) const
{
  char szBuf[1000];
  // write viewpoints
  //
  Base::BoundBox3f clBB = _rclMesh.GetBoundBox();
  Base::Vector3f   clCenter = clBB.CalcCenter();
  for (std::vector<VRMLViewpointData>::iterator it = _pclVRMLInfo->_clViewpoints.begin(); it != _pclVRMLInfo->_clViewpoints.end(); ++it)
  {
		// build up the observer's coordinate system
		Base::Vector3f u,v,w;
		v = it->clVRefUp;
		w = it->clVRefPln;
		u = v % w;
		u.Normalize();
		v.Normalize();
		w.Normalize();

		// calc the view axis in world coordinates
		//
		Base::Vector3f p_uvw, p_xyz;
		p_uvw   = it->clPRefPt;
		p_xyz.x = u.x*p_uvw.x+v.x*p_uvw.y+w.x*p_uvw.z;
		p_xyz.y = u.y*p_uvw.x+v.y*p_uvw.y+w.y*p_uvw.z;
		p_xyz.z = u.z*p_uvw.x+v.z*p_uvw.y+w.z*p_uvw.z;
		p_xyz   = p_xyz + it->clVRefPt;

		float t = (clCenter - p_xyz)*w;
		Base::Vector3f a = p_xyz + t*w;

    Vector at, pos, up, rot_axis;
    double rot_angle;

		float fDistance = (float)(it->dVmax - it->dVmin);
		Base::Vector3f p = a + fDistance * w;
    pos.x = p.x;   pos.y = p.y;   pos.z = p.z;
    at.x  = a.x;   at.y  = a.y;   at.z  = a.z;
    up.x  = v.x;   up.y  = v.y;   up.z  = v.z;

    Convert_Camera_Model(&pos, &at, &up, &rot_axis, &rot_angle);

    strcpy(szBuf, "Viewpoint\n{\n");
    strcat(szBuf, "  jump         TRUE\n");
    sprintf(szBuf, "%s  orientation   %.3f %.3f %.3f %.3f\n", szBuf, rot_axis.x, rot_axis.y, rot_axis.z, rot_angle);
    rstrOut.write(szBuf, strlen(szBuf));
    sprintf(szBuf, "  description  \"%s\"\n", it->clName.c_str());
    rstrOut.write(szBuf, strlen(szBuf));
    sprintf(szBuf, "  position     %.3f %.3f %.3f\n", pos.x, pos.y, pos.z);
    rstrOut.write(szBuf, strlen(szBuf));
    strcpy(szBuf, "}\n");
    rstrOut.write(szBuf, strlen(szBuf)); 
  }
}

bool SaveMeshVRML::Save (std::ostream &rstrOut, const App::Material &rclMat) const
{
  std::vector<App::Color> aclDummy;
/*
for (int i = 0; i < _rclMesh.CountPoints(); i++)
{
unsigned short r =  (rand() * 255) / float(RAND_MAX);
unsigned short g =  (rand() * 255) / float(RAND_MAX);
unsigned short b =  (rand() * 255) / float(RAND_MAX);

  aclDummy.push_back(App::Color(r, g, b));
}
*/
  return Save(rstrOut, aclDummy, rclMat, false);
}

