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

#include <Base/Sequencer.h>

#include <math.h>


using namespace Mesh;

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

// --------------------------------------------------------------

MeshSTL::MeshSTL (MeshKernel &rclM)
: _rclMesh(rclM)
{
}

bool MeshSTL::Load (FileStream &rstrIn)
{
  char szBuf[200];

  if ((rstrIn.IsOpen() == false) || (rstrIn.IsBad() == true))
    return false;

  //  80 Zeichen ab Position 80 einlesen und auf Key-Woerter solid,facet,normal,vertex,endfacet,endloop testen
  rstrIn.SetPosition(80);
  if (rstrIn.Read(szBuf, 80) == false)
    return false;
  szBuf[80] = 0; 

  upper(szBuf);
  if ((strstr(szBuf, "SOLID") == NULL)  && (strstr(szBuf, "FACET") == NULL)    && (strstr(szBuf, "NORMAL") == NULL) &&
      (strstr(szBuf, "VERTEX") == NULL) && (strstr(szBuf, "ENDFACET") == NULL) && (strstr(szBuf, "ENDLOOP") == NULL))
  {  // wahrscheinlich stl binaer
    rstrIn.SetPosition(0);
    return LoadBinary(rstrIn);
  }
  else
  {  // stl ascii
    rstrIn.SetPosition(0);
    return LoadAscii(rstrIn);
  }

  return true;
}

bool MeshSTL::LoadAscii (FileStream &rstrIn)
{
  char szLine[200], szKey1[200], szKey2[200];
  unsigned long ulVertexCt, ulCt;
  float fX, fY, fZ;
  MeshGeomFacet clFacet;
  std::vector<MeshGeomFacet>  clFacetAry;

  if ((rstrIn.IsOpen() == false) || (rstrIn.IsBad() == true))
    return false;

  ulCt = rstrIn.FileSize();
  Base::Sequencer().start("loading...", ulCt+1);  

  ulVertexCt = 0;
  while ((rstrIn.IsEof() == false) && (rstrIn.IsBad() == false))
  {
    rstrIn.ReadLine(szLine, 200);
    upper(ltrim(szLine));
    if (strncmp(szLine, "FACET", 5) == 0)  // normale
    {
      if (sscanf(szLine, "%s %s %f %f %f", szKey1, szKey2, &fX, &fY, &fZ) == 5)
        clFacet.SetNormal(Vector3D(fX, fY, fZ));
    }
    else if (strncmp(szLine, "VERTEX", 6) == 0)  // vertex
    {
      if (sscanf(szLine, "%s %f %f %f", szKey1, &fX, &fY, &fZ) == 4)
      {
        clFacet._aclPoints[ulVertexCt++].Set(fX, fY, fZ);
        if (ulVertexCt == 3)
        {
          ulVertexCt = 0;
          clFacetAry.push_back(clFacet);
        }
      }
    }

    Base::Sequencer().next( true ); // allow to cancel
  }

  Base::Sequencer().stop();
  _rclMesh = clFacetAry;  

  return true;
}

bool MeshSTL::LoadBinary (FileStream &rstrIn)
{
  char szInfo[80];
  Vector3D clVects[4];
  unsigned short usAtt; 
  unsigned long ulCt;

  if ((rstrIn.IsOpen() == false) || (rstrIn.IsBad() == true))
    return false;

  // Header-Info ueberlesen
  rstrIn.Read(szInfo, sizeof(szInfo));
 
  // Anzahl Facets
  rstrIn.Read((char*)&ulCt, sizeof(ulCt));
  if (rstrIn.IsBad() == true)
    return false;

  // get file size and calculate the number of facets
  unsigned long ulSize = rstrIn.FileSize(); 
  unsigned long ulFac = (ulSize - (80 + sizeof(unsigned long)))/50;

  // compare the calculated with the read value
  if (ulCt > ulFac)
    return false;// not a valid STL file

#ifdef Use_EdgeList
  Base::Sequencer().start("create mesh structure...", ulCt * 4 + 1);
#else
  Base::Sequencer().start("create mesh structure...", ulCt * 3 / 2);
#endif

  MeshPointBuilder  clMap;
  clMap.resize(ulCt*3);

  unsigned long k = 0;

  // n-Facets einlesen
  for (unsigned long i = 0; i < ulCt; i++)
  {
    // Normale, Eckpunkte
    rstrIn.Read((char*)&clVects, sizeof(clVects));

    if (rstrIn.IsBad() == true)
      return false;

    // Umlaufrichtung an Normale anpassen
    if ((((clVects[2] - clVects[1]) % (clVects[3] - clVects[1])) * clVects[0]) >= 0.0f)
    {
      clMap[k++].Set(0, i, clVects[1]);
      clMap[k++].Set(1, i, clVects[2]);
      clMap[k++].Set(2, i, clVects[3]);
    }
    else
    {
      clMap[k++].Set(0, i, clVects[1]);
      clMap[k++].Set(1, i, clVects[3]);
      clMap[k++].Set(2, i, clVects[2]);
    }

    // ueberlesen Attribut
    rstrIn.Read((char*)&usAtt, sizeof(usAtt));

    Base::Sequencer().next( true ); // allow to cancel
  } 

  _rclMesh.Assign(clMap);

  Base::Sequencer().stop();
 
  return true;
}

bool MeshSTL::SaveAscii (FileStream &rstrOut) const
{
  MeshFacetIterator clIter(_rclMesh), clEnd(_rclMesh);  
  const MeshGeomFacet *pclFacet;
  unsigned long i, ulCtFacet;
  char szBuf[200]; 

  if ((rstrOut.IsOpen() == false) || (rstrOut.IsBad() == true) ||
      (_rclMesh.CountFacets() == 0))
  {
    return false;
  }

  Base::Sequencer().start("saving...", _rclMesh.CountFacets() + 1);  

  strcpy(szBuf, "solid MESH\n");
  rstrOut.Write(szBuf, strlen(szBuf));

  clIter.Begin();
  clEnd.End();
  ulCtFacet = 0;
  while (clIter < clEnd)
  {
    pclFacet = &(*clIter);
      
    // normale
    sprintf(szBuf, "  facet normal %.4f %.4f %.4f\n", pclFacet->GetNormal().x,
             pclFacet->GetNormal().y,  pclFacet->GetNormal().z);
    rstrOut.Write(szBuf, strlen(szBuf));

    strcpy(szBuf, "    outer loop\n");
    rstrOut.Write(szBuf, strlen(szBuf));

    for (i = 0; i < 3; i++)
    {
      sprintf(szBuf, "      vertex %.4f %.4f %.4f\n", pclFacet->_aclPoints[i].x,
              pclFacet->_aclPoints[i].y, pclFacet->_aclPoints[i].z);
      rstrOut.Write(szBuf, strlen(szBuf));
    }

    strcpy(szBuf, "    endloop\n");
    rstrOut.Write(szBuf, strlen(szBuf));

    strcpy(szBuf, "  endfacet\n");
    rstrOut.Write(szBuf, strlen(szBuf));

    ++clIter; 
    Base::Sequencer().next( true );// allow to cancel
  } 

  strcpy(szBuf, "endsolid MESH\n");
  rstrOut.Write(szBuf, strlen(szBuf));

  Base::Sequencer().stop();
  
  return true;
}

bool MeshSTL::SaveBinary (FileStream &rstrOut) const
{
  MeshFacetIterator clIter(_rclMesh), clEnd(_rclMesh);  
  const MeshGeomFacet *pclFacet;
  unsigned long i, ulCtFacet;
  unsigned short usAtt;
  char szInfo[80];

  if ((rstrOut.IsOpen() == false) || (rstrOut.IsBad() == true) || (_rclMesh.CountFacets() == 0))
  {
    return false;
  }

  Base::Sequencer().start("saving...", _rclMesh.CountFacets() + 1);  
 
  strcpy(szInfo, "MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH-MESH\n");
  rstrOut.Write(szInfo, strlen(szInfo));
  rstrOut << (unsigned long)(_rclMesh.CountFacets());

  usAtt = 0;
  clIter.Begin();
  clEnd.End();
  ulCtFacet = 0;
  while (clIter < clEnd)
  {
    pclFacet = &(*clIter);
    // Normale
    rstrOut << float(pclFacet->GetNormal().x) <<
               float(pclFacet->GetNormal().y) <<
               float(pclFacet->GetNormal().z);

    // Eckpunkte
    for (i = 0; i < 3; i++)
    {
      rstrOut << float(pclFacet->_aclPoints[i].x) <<
                 float(pclFacet->_aclPoints[i].y) <<
                 float(pclFacet->_aclPoints[i].z);
    }

    // Attribut 
    rstrOut << usAtt;

    ++clIter;
    Base::Sequencer().next( true ); // allow to cancel
  }

  Base::Sequencer().stop();

  return true;
}

bool MeshInventor::Load (FileStream &rstrIn)
{
  char szLine[200];
  unsigned long i, ulCt, ulPoints[3];
  long lSeparator;
  bool bFlag;
  Vector3D clPoint;
  float fX, fY, fZ;
  MeshGeomFacet clFacet;
  std::vector<MeshGeomFacet> clFacetAry;
  std::vector<Vector3D> aclPoints;


  if ((rstrIn.IsOpen() == false) || (rstrIn.IsBad() == true))
    return false;

  ulCt = rstrIn.FileSize();
  Base::Sequencer().start("loading...", ulCt+1);  
///*
  bFlag = true;
  while ((rstrIn.IsEof() == false) && (rstrIn.IsBad() == false) && bFlag)
  {
      rstrIn.ReadLine(szLine ,200);
      upper(ltrim(szLine));
      // read the normals
      if (strncmp(szLine, "VECTOR [", 8) == 0){
          while ((rstrIn.IsEof() == false) && (rstrIn.IsBad() == false) && bFlag)
          {  
              rstrIn.ReadLine(szLine, 200);

              if (sscanf(szLine, "%f %f %f", &fX, &fY, &fZ) == 3){
                  clFacet.SetNormal(Vector3D(fX, fY, fZ));
                  clFacetAry.push_back(clFacet);

                  Base::Sequencer().next( true ); // allow to cancel

              }
              else{
                  bFlag = false;
              }
          }
      }
  }

//*/
  bFlag = true;
  while ((rstrIn.IsEof() == false) && (rstrIn.IsBad() == false) && bFlag)
  {
      rstrIn.ReadLine(szLine ,200);
      upper(ltrim(szLine));
      // read the points
      if (strncmp(szLine, "POINT [", 7) == 0){
          while ((rstrIn.IsEof() == false) && (rstrIn.IsBad() == false) && bFlag)
          {
              rstrIn.ReadLine(szLine, 200);
              if (sscanf(szLine, "%f  %f  %f", &(clPoint.x), &(clPoint.y), &(clPoint.z)) == 3)
              {
                 aclPoints.push_back(clPoint);

                 Base::Sequencer().next( true ); // allow to cancel
              }
              else{
                  bFlag = false;
              }
          }
      }
  }

  ulCt = 0;
  bFlag = true;
  while ((rstrIn.IsEof() == false) && (rstrIn.IsBad() == false) && bFlag)
  {
      rstrIn.ReadLine(szLine ,200);
      upper(ltrim(szLine));
      // read the points of the facets
      if (strncmp(szLine, "COORDINDEX [ ", 13) == 0){
          while ((rstrIn.IsEof() == false) && (rstrIn.IsBad() == false) && bFlag)
          {
              rstrIn.ReadLine(szLine, 200);
              if (sscanf(szLine, "%lu, %lu, %lu, %ld",
                  &ulPoints[0], &ulPoints[1], &ulPoints[2], &lSeparator) == 4)
              {
                  clFacet = clFacetAry[ulCt];
                  for (i = 0; i < 3; i++)
                  {
                    clFacet._aclPoints[i] = aclPoints[ulPoints[i]];
                  }
                  clFacetAry[ulCt++] = clFacet;

                  Base::Sequencer().next( true ); // allow to cancel
              }
              else
              {
                  bFlag = false;
              }
          }
      }
  }


  Base::Sequencer().stop();
  _rclMesh = clFacetAry;
  return true;
}

bool MeshInventor::Save (FileStream &rstrOut) const
{
  MeshFacetIterator clIter(_rclMesh), clEnd(_rclMesh);
  MeshPointIterator clPtIter(_rclMesh), clPtEnd(_rclMesh);
  const MeshGeomFacet* pclFacet;
  MeshFacet clFacet;
  unsigned long i, ulCtFacet, ulAllFacets = _rclMesh.CountFacets();
  char szBuf[200]; 

  if ((rstrOut.IsOpen() == false) || (rstrOut.IsBad() == true) ||
      (_rclMesh.CountFacets() == 0))
  {
    return false;
  }

  Base::Sequencer().start("saving...", _rclMesh.CountFacets() + 1);  

  // Einleitung
  strcpy(szBuf, "#Inventor V2.1 ascii\n\n");
  rstrOut.Write(szBuf, strlen(szBuf));
  strcpy(szBuf, "FreeCAD\n");
  rstrOut.Write(szBuf, strlen(szBuf));
  sprintf(szBuf, "# Triangulation Data contains %lu Points and %lu Facets\n",
      _rclMesh.CountPoints(), _rclMesh.CountFacets());
  rstrOut.Write(szBuf, strlen(szBuf));
  strcpy(szBuf, "Separator { \n\n\n");
  rstrOut.Write(szBuf, strlen(szBuf));
  strcpy(szBuf, " Label {\n");
  rstrOut.Write(szBuf, strlen(szBuf));
  strcpy(szBuf, "  label triangle mesh\n  }\n");
  rstrOut.Write(szBuf, strlen(szBuf));
///*
  // hier stehen die Normalen der Facets
  strcpy(szBuf, "Normal { \n");
  rstrOut.Write(szBuf, strlen(szBuf));
  strcpy(szBuf, "vector [\n");
  rstrOut.Write(szBuf, strlen(szBuf));

  clIter.Begin();
  clEnd.End();
  ulCtFacet = 0;

  pclFacet = &(*clIter);
  sprintf(szBuf, "%.6f  %.6f  %.6f", pclFacet->GetNormal().x,
          pclFacet->GetNormal().y, pclFacet->GetNormal().z);
  rstrOut.Write(szBuf, strlen(szBuf));
  ++clIter;

  while (clIter < clEnd)
  {
      pclFacet = &(*clIter);

      sprintf(szBuf, ",\n%.6f  %.6f  %.6f", pclFacet->GetNormal().x, 
              pclFacet->GetNormal().y, pclFacet->GetNormal().z);
      rstrOut.Write(szBuf, strlen(szBuf));
      ++clIter;

      Base::Sequencer().next( true ); // allow to cancel
  }
  strcpy(szBuf, "\n]\n}\n");
  rstrOut.Write(szBuf, strlen(szBuf));
//*/
  // hier stehen die Koordinaten der Punkte
  strcpy(szBuf, " NormalBinding {\n  value PER_FACE\n }\n");
  rstrOut.Write(szBuf, strlen(szBuf));
  strcpy(szBuf, "Coordinate3 { \npoint [\n");
  rstrOut.Write(szBuf, strlen(szBuf));

  clPtIter.Begin();
  clPtEnd.End();
  ulCtFacet = 0;

  sprintf(szBuf, "%.6f  %.6f  %.6f", clPtIter->x, clPtIter->y, clPtIter->z);
  rstrOut.Write(szBuf, strlen(szBuf));
  ++clPtIter;

  while (clPtIter < clPtEnd)
  {
      sprintf(szBuf, ",\n%.6f  %.6f  %.6f", clPtIter->x, clPtIter->y, clPtIter->z);
      rstrOut.Write(szBuf, strlen(szBuf));
      ++clPtIter;

      Base::Sequencer().next( true ); // allow to cancel
  }
  strcpy(szBuf, "\n]\n}\n");
  rstrOut.Write(szBuf, strlen(szBuf));

  // hier stehen die Facets mit den Punktindizes
  strcpy(szBuf, "IndexedFaceSet { \n");
  rstrOut.Write(szBuf, strlen(szBuf));
  strcpy(szBuf, "coordIndex [ \n");
  rstrOut.Write(szBuf, strlen(szBuf));


  i = 0;
  while (i < ulAllFacets - 1)
  {
    clFacet = _rclMesh._aclFacetArray[i];

    sprintf(szBuf, "%lu, %lu, %lu, %d,\n", clFacet._aulPoints[0],
              clFacet._aulPoints[1], clFacet._aulPoints[2], -1);
    rstrOut.Write(szBuf, strlen(szBuf));
    
    ++i; 
  } 

  clFacet = _rclMesh._aclFacetArray[ulAllFacets - 1];
  sprintf(szBuf, "%lu, %lu, %lu, %d\n", clFacet._aulPoints[0],
            clFacet._aulPoints[1], clFacet._aulPoints[2], - 1);
  rstrOut.Write(szBuf, strlen(szBuf));

  strcpy(szBuf, "]\n}\n");
  rstrOut.Write(szBuf, strlen(szBuf));

  strcpy(szBuf, "#End of Triangulation Data \n  }\n\n");
  rstrOut.Write(szBuf, strlen(szBuf));

  Base::Sequencer().stop();

  return true;
}
