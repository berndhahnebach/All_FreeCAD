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

// --------------------------------------------------------------


bool LoadMeshSTL::Load (std::istream &rstrIn)
{
  char szBuf[200];

  if ( !rstrIn || rstrIn.bad() == true )
    return false;

  //  80 Zeichen ab Position 80 einlesen und auf Key-Woerter solid,facet,normal,vertex,endfacet,endloop testen
  std::streambuf* buf = rstrIn.rdbuf();
  if (!buf) return false;
  buf->pubseekoff(80, std::ios::beg, std::ios::in);
  if ( rstrIn.read(szBuf, 80) == false )
    return false;
  szBuf[80] = 0; 
  upper(szBuf);

  try{
    if ((strstr(szBuf, "SOLID") == NULL)  && (strstr(szBuf, "FACET") == NULL)    && (strstr(szBuf, "NORMAL") == NULL) &&
        (strstr(szBuf, "VERTEX") == NULL) && (strstr(szBuf, "ENDFACET") == NULL) && (strstr(szBuf, "ENDLOOP") == NULL))
    {  // wahrscheinlich stl binaer
      buf->pubseekoff(0, std::ios::beg, std::ios::in);
      return LoadBinary(rstrIn);
    }
    else
    {  // stl ascii
      buf->pubseekoff(0, std::ios::beg, std::ios::in);
      return LoadAscii(rstrIn);
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

bool LoadMeshSTL::LoadAscii (std::istream &rstrIn)
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

bool LoadMeshSTL::LoadBinary (std::istream &rstrIn)
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

bool SaveMeshSTL::SaveAscii (std::ostream &rstrOut) const
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

bool SaveMeshSTL::SaveBinary (std::ostream &rstrOut) const
{
  MeshFacetIterator clIter(_rclMesh), clEnd(_rclMesh);  
  const MeshGeomFacet *pclFacet;
  unsigned long i, ulCtFacet;
  unsigned short usAtt;
  char szInfo[81];

  if ( !rstrOut || rstrOut.bad() == true || _rclMesh.CountFacets() == 0 )
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
#if 0
bool MeshInventor::Load (FileStream &rstrIn)
{
  char szLine[200];
  unsigned long i, ulCt, ulPoints[3];
  long lSeparator;
  bool bFlag;
  Vector3f clPoint;
  float fX, fY, fZ;
  MeshGeomFacet clFacet;
  std::vector<MeshGeomFacet> clFacetAry;
  std::vector<Vector3f> aclPoints;


  if ((rstrIn.IsOpen() == false) || (rstrIn.IsBad() == true))
    return false;

  ulCt = rstrIn.FileSize();
  Base::SequencerLauncher seq("loading...", ulCt+1);  
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
                  clFacet.SetNormal(Vector3f(fX, fY, fZ));
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

  Base::SequencerLauncher seq("saving...", _rclMesh.CountFacets() + 1);  

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


  return true;
}
#endif


void MeshDocXML::Save (Base::Writer &writer) const
{
//  writer << writer.ind() << "<Mesh>" << std::endl;

  writer.incInd();
  writer << writer.ind() << "<Points Count=\"" << _rclMesh.CountPoints() << "\">" << std::endl;

  writer.incInd();
  for (MeshPointArray::_TConstIterator itp = _rclMesh._aclPointArray.begin(); itp != _rclMesh._aclPointArray.end(); itp++)
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
  for (MeshFacetArray::_TConstIterator it = _rclMesh._aclFacetArray.begin(); it != _rclMesh._aclFacetArray.end(); it++)
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

void MeshDocXML::Restore(Base::XMLReader &reader)
{ 
  int Cnt,i;
 
//  reader.readElement("Mesh");

  reader.readElement("Points");
  Cnt = reader.getAttributeAsInteger("Count");

  _rclMesh._aclPointArray.resize(Cnt);
  for(i=0 ;i<Cnt ;i++)
  {
    reader.readElement("P");
    _rclMesh._aclPointArray[i].x = reader.getAttributeAsFloat("x");
    _rclMesh._aclPointArray[i].y = reader.getAttributeAsFloat("y");
    _rclMesh._aclPointArray[i].z = reader.getAttributeAsFloat("z");
     

  }
  reader.readEndElement("Points");

  reader.readElement("Faces");
  Cnt = reader.getAttributeAsInteger("Count");

  _rclMesh._aclFacetArray.resize(Cnt);
  for(i=0 ;i<Cnt ;i++)
  {
    reader.readElement("F");
     
    _rclMesh._aclFacetArray[i]._aulPoints[0] = reader.getAttributeAsInteger("p0");
    _rclMesh._aclFacetArray[i]._aulPoints[1] = reader.getAttributeAsInteger("p1");
    _rclMesh._aclFacetArray[i]._aulPoints[2] = reader.getAttributeAsInteger("p2");
    _rclMesh._aclFacetArray[i]._aulNeighbours[0] = reader.getAttributeAsInteger("n0");
    _rclMesh._aclFacetArray[i]._aulNeighbours[1] = reader.getAttributeAsInteger("n1");
    _rclMesh._aclFacetArray[i]._aulNeighbours[2] = reader.getAttributeAsInteger("n2");

  }
  reader.readEndElement("Faces");

  reader.readEndElement("Mesh");

  _rclMesh.RecalcBoundBox();
}

