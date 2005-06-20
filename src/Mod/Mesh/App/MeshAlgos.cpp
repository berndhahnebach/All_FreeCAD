/***************************************************************************
 *   Copyright (c) Juergen Riegel         <juergen.riegel@web.de>          *
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
#ifndef _PreComp_
# ifdef FC_OS_LINUX
#	  include <unistd.h>
# endif
#endif


#include "MeshAlgos.h"
#include "Mesh.h"
#include "Core/MeshIO.h"
#include "Core/Stream.h"

#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Console.h>

using namespace Mesh;


Mesh::MeshWithProperty* MeshAlgos::Load(const char *FileName)
{
  MeshWithProperty *Mesh = new MeshWithProperty();
  // ask for read permisson
	if ( access(FileName, 4) != 0 )
    throw Base::Exception("MeshAlgos::Load() not able to open File!\n");
 
  MeshSTL aReader(* (Mesh->getKernel()) );

  // read STL file
  FileStream str( FileName, std::ios::in);
  if ( !aReader.Load( str ) )
    throw Base::Exception("STL read failed (load file)");

  return Mesh; 
}

void MeshAlgos::writeBin(MeshWithProperty* Mesh,const char *FileName)
{
   Base::FileInfo File(FileName);
  
  // checking on the file
  if(File.exists() && !File.isWritable())
        throw Base::Exception("File not writable");

  MeshSTL aReader(*(Mesh->getKernel()) );

  // read STL file
  FileStream str( File.filePath().c_str(), std::ios::out);

  if ( !aReader.SaveBinary( str ) )
    throw Base::Exception("STL write failed to write");
}

void MeshAlgos::writeAscii(MeshWithProperty* Mesh,const char *FileName)
{
   Base::FileInfo File(FileName);
  
  // checking on the file
  if(File.exists() && !File.isWritable())
        throw Base::Exception("File not writable");

  MeshSTL aReader(*(Mesh->getKernel()) );

  // read STL file
  FileStream str( File.filePath().c_str(), std::ios::out);

  if ( !aReader.SaveAscii( str ) )
    throw Base::Exception("STL write failed to write");
}


void MeshAlgos::calcVertexNormales(MeshWithProperty* Mesh)
{
  MeshPropertyNormal *prop = dynamic_cast<MeshPropertyNormal*> (Mesh->Get("VertexNormales") );

  if(prop && prop->isValid())
    return;

  // remove invalid Normales
  if(prop) Mesh->Remove("VertexNormales");

  const MeshKernel &MeshK = *(Mesh->getKernel());

  // create a property with the right size
  prop = new MeshPropertyNormal(MeshK.CountPoints());

  // data structure to hold all faces belongs to one vertex
  std::vector<std::set<unsigned int> > faceMap(MeshK.CountPoints());
  unsigned long p1,p2,p3;

  // colecting all Facetes indexes blonging to a vertex index
  for (unsigned int pFIter = 0;pFIter < MeshK.CountFacets(); pFIter++)
  {
    Mesh->getKernel()->GetFacetPoints(pFIter,p1,p2,p3);
    
    Vector3D Norm = (MeshK.GetPoint(p2)-MeshK.GetPoint(p1) ) % (MeshK.GetPoint(p3)-MeshK.GetPoint(p1));

    prop->Normales[p1] += Norm;
    prop->Normales[p2] += Norm;
    prop->Normales[p3] += Norm;
    
/*    faceMap[p1].insert(pFIter);
    faceMap[p2].insert(pFIter);
    faceMap[p3].insert(pFIter);*/
  }

/*  // calculating the normale weighted by size and write it to the property
  for( unsigned int ItVertex = 0;ItVertex < Mesh->getKernel()->CountFacets(); ItVertex++)
  {
    prop->Normales[ItVertex] = Vector3D(0.0,0.0,0.0);
  }*/

  Mesh->Add(prop,"VertexNormales");  
}


void MeshAlgos::offset(MeshWithProperty* Mesh, float fSize)
{
  MeshPropertyNormal *prop = dynamic_cast<MeshPropertyNormal*> (Mesh->Get("VertexNormales") );

  // calculate the propertie on demand...
  if(!prop || !prop->isValid())
  {
    calcVertexNormales(Mesh);
    prop = dynamic_cast<MeshPropertyNormal*> (Mesh->Get("VertexNormales") );
  }

  unsigned int i = 0;
  // go throug all the Vertex normales
  for(std::vector<Vector3D>::iterator It= prop->Normales.begin();It != prop->Normales.end();It++,i++)
    // and move each mesh point in the normal direction
    Mesh->getKernel()->MovePoint(i,It->Normalize() * fSize);

  // invalid because of points movement
  Mesh->Remove("VertexNormales");
}


void MeshAlgos::coarsen(MeshWithProperty* Mesh, float f)
{
  GtsSurface * surface;

  // create a GTS surface
  surface = MeshAlgos::createGTSSurface(Mesh);

  Mesh->clear();

  guint stop_number=100000;
  gdouble fold = 3.1415 / 180.;

  gts_surface_coarsen (surface, 
                       NULL, NULL, 
                       NULL, NULL, 
                       (GtsStopFunc)gts_coarsen_stop_number,
                       &stop_number, fold);

  // get the standard mesh
  fillMeshFromGTSSurface(Mesh,surface);
}


MeshWithProperty* MeshAlgos::boolean(MeshWithProperty* pMesh1, MeshWithProperty* pMesh2, MeshWithProperty* pResult,int Type)
{
  GtsSurface * s1, * s2, * s3;
  GtsSurfaceInter * si;
  GNode * tree1, * tree2;
  int c = 0;
  gboolean verbose = TRUE;
  gboolean inter = FALSE;
  gboolean check_self_intersection = FALSE;
  gchar * operation, * file1, * file2;
  gboolean closed = TRUE, is_open1, is_open2;


  // create a GTS surface
  s1 = MeshAlgos::createGTSSurface(pMesh1);
  s2 = MeshAlgos::createGTSSurface(pMesh2);

  // clear the mesh (mermory)
  //Mesh1.clear();
  //Mesh2.clear();

  /* check that the surfaces are orientable manifolds */
  if (!gts_surface_is_orientable (s1)) {
    gts_object_destroy (GTS_OBJECT (s1));
    gts_object_destroy (GTS_OBJECT (s2));
    throw "surface 1 is not an orientable manifold\n" ;
  }
  if (!gts_surface_is_orientable (s2)) {
    gts_object_destroy (GTS_OBJECT (s1));
    gts_object_destroy (GTS_OBJECT (s2));
    throw "surface 2 is not an orientable manifold\n";
  }

  /* check that the surfaces are not self-intersecting */
  if (check_self_intersection) {
    GtsSurface * self_intersects;

    self_intersects = gts_surface_is_self_intersecting (s1);
    if (self_intersects != NULL) {
//      if (verbose)
//	      gts_surface_print_stats (self_intersects, stderr);
//      gts_surface_write (self_intersects, stdout);
      gts_object_destroy (GTS_OBJECT (self_intersects));
      gts_object_destroy (GTS_OBJECT (s1));
      gts_object_destroy (GTS_OBJECT (s2));
      throw "surface is self-intersecting\n";
    }
    self_intersects = gts_surface_is_self_intersecting (s2);
    if (self_intersects != NULL) {
//      if (verbose)
//	      gts_surface_print_stats (self_intersects, stderr);
//      gts_surface_write (self_intersects, stdout);
      gts_object_destroy (GTS_OBJECT (self_intersects));
      gts_object_destroy (GTS_OBJECT (s1));
      gts_object_destroy (GTS_OBJECT (s2));
      throw"surface is self-intersecting\n";
    }
  }

  /* build bounding box tree for first surface */
  tree1 = gts_bb_tree_surface (s1);
  is_open1 = gts_surface_volume (s1) < 0. ? TRUE : FALSE;

  /* build bounding box tree for second surface */
  tree2 = gts_bb_tree_surface (s2);
  is_open2 = gts_surface_volume (s2) < 0. ? TRUE : FALSE;

  si = gts_surface_inter_new (gts_surface_inter_class (),
			      s1, s2, tree1, tree2, is_open1, is_open2);
  g_assert (gts_surface_inter_check (si, &closed));
  if (!closed) {
    gts_object_destroy (GTS_OBJECT (s1));
    gts_object_destroy (GTS_OBJECT (s2));
    gts_bb_tree_destroy (tree1, TRUE);
    gts_bb_tree_destroy (tree2, TRUE);  
    throw"the intersection of 1 and  2 is not a closed curve\n";
  }

  s3 = gts_surface_new (gts_surface_class (),
			gts_face_class (),
			gts_edge_class (),
			gts_vertex_class ());  
  if (Type==0) {
    gts_surface_inter_boolean (si, s3, GTS_1_OUT_2);
    gts_surface_inter_boolean (si, s3, GTS_2_OUT_1);
  }
  else if (Type==1) {
    gts_surface_inter_boolean (si, s3, GTS_1_IN_2);
    gts_surface_inter_boolean (si, s3, GTS_2_IN_1);
  }
  else if (Type==2) {
    gts_surface_inter_boolean (si, s3, GTS_1_OUT_2);
    gts_surface_inter_boolean (si, s3, GTS_2_IN_1);
    gts_surface_foreach_face (si->s2, (GtsFunc) gts_triangle_revert, NULL);
    gts_surface_foreach_face (s2, (GtsFunc) gts_triangle_revert, NULL);
  }
  
  // check that the resulting surface is not self-intersecting 
  if (check_self_intersection) {
    GtsSurface * self_intersects;

    self_intersects = gts_surface_is_self_intersecting (s3);
    if (self_intersects != NULL) {
//      if (verbose)
//	      gts_surface_print_stats (self_intersects, stderr);
 //     gts_surface_write (self_intersects, stdout);
      gts_object_destroy (GTS_OBJECT (self_intersects));
      gts_object_destroy (GTS_OBJECT (s1));
      gts_object_destroy (GTS_OBJECT (s2));
      gts_object_destroy (GTS_OBJECT (s3));
      gts_object_destroy (GTS_OBJECT (si));
      gts_bb_tree_destroy (tree1, TRUE);
      gts_bb_tree_destroy (tree2, TRUE);  
      throw "the resulting surface is self-intersecting\n";
    }
  }
  // display summary information about the resulting surface 
//  if (verbose)
//    gts_surface_print_stats (s3, stderr);
  // write resulting surface to standard output 

  // get the standard mesh
  fillMeshFromGTSSurface(pResult,s3);


  // destroy surfaces 
  gts_object_destroy (GTS_OBJECT (s1));
  gts_object_destroy (GTS_OBJECT (s2));
//  gts_object_destroy (GTS_OBJECT (s3));
//  gts_object_destroy (GTS_OBJECT (si));

  // destroy bounding box trees (including bounding boxes) 
//  gts_bb_tree_destroy (tree1, TRUE);
//  gts_bb_tree_destroy (tree2, TRUE);  
  
  
  return pMesh1;
}





/// helper function - construct a Edge out of two Vertexes if not allready there
static GtsEdge * new_edge (GtsVertex * v1, GtsVertex * v2)
{
  GtsSegment * s = gts_vertices_are_connected (v1, v2);
  if( s == NULL ) 
    return gts_edge_new (gts_edge_class (), v1, v2);
  else
    return GTS_EDGE (s);
}


GtsSurface* MeshAlgos::createGTSSurface(MeshWithProperty* Mesh)
{
  GtsSurface* Surf = gts_surface_new (gts_surface_class (),
                                      gts_face_class (),
                                      gts_edge_class (),
                                      gts_vertex_class () );

  const MeshKernel &MeshK = *(Mesh->getKernel());
  unsigned long p1,p2,p3;
  Vector3D Vertex;


  // Geting all the points
  GtsVertex ** aVertex = (GtsVertex **) malloc(MeshK.CountPoints() * sizeof (GtsVertex *));
  for (unsigned int PIter = 0;PIter < MeshK.CountPoints(); PIter++)
  {
    Vertex = MeshK.GetPoint(PIter);
    aVertex[PIter] = gts_vertex_new (gts_vertex_class (), Vertex.x, Vertex.y, Vertex.z);
  }

    // cycling through the facets
  for (unsigned int pFIter = 0;pFIter < MeshK.CountFacets(); pFIter++)
  {
    // geting the three points of the facet
    Mesh->getKernel()->GetFacetPoints(pFIter,p1,p2,p3);
    
    // creating the edges and add the face to the surface
    gts_surface_add_face (Surf, 
  	    gts_face_new (Surf->face_class,
          new_edge (aVertex[p1],aVertex[p2]),
          new_edge (aVertex[p2],aVertex[p3]),
          new_edge (aVertex[p3],aVertex[p1])));
  }

  Base::Console().Log("GTS [%d faces, %d Points, %d Edges,%s ,%s]\n",gts_surface_face_number(Surf),
                                                                     gts_surface_vertex_number(Surf),
                                                                     gts_surface_edge_number(Surf),
                                                                     gts_surface_is_orientable (Surf)?"orientable":"not orientable",
                                                                     gts_surface_is_self_intersecting(Surf)?"self-intersections":"no self-intersection" ); 

  return Surf;
}

/// helper function for the face (triangle iteration
static void onFaces (GtsTriangle * t,  std::vector<MeshGeomFacet> *VAry )
{
  GtsVertex *mv0,*mv1,*mv2;

  gts_triangle_vertices (t,&mv0,&mv1,&mv2);

  VAry->push_back(MeshGeomFacet(Vector3D(mv0->p.x,mv0->p.y,mv0->p.z),
                                Vector3D(mv1->p.x,mv1->p.y,mv1->p.z),
                                Vector3D(mv2->p.x,mv2->p.y,mv2->p.z)));

}

/*
static void onVertices(GtsVertex *v, MeshKernel *pKernel )
{
  Vector3D Point(GTS_POINT(v)->x,GTS_POINT(v)->y,GTS_POINT(v)->z);
}*/

void MeshAlgos::fillMeshFromGTSSurface(MeshWithProperty* pMesh, GtsSurface* pSurface)
{
  MeshKernel &MeshK = *(pMesh->getKernel());
  std::vector<MeshGeomFacet> VAry;

  // remove old mesh
  MeshK.Clear();

//  gts_surface_foreach_vertex(pSurface,(GtsFunc) onVertices,&MeshK);
  gts_surface_foreach_face (pSurface, (GtsFunc) onFaces,&VAry);

  // destroy surfaces 
  gts_object_destroy (GTS_OBJECT (pSurface));

  // put the facets the simple way in the mesh, totp is recalculated!
  MeshK = VAry;

}

