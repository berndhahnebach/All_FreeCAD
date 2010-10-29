/***************************************************************************
 *   Copyright (c) 2008 Jürgen Riegel (juergen.riegel@web.de)              *
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
# include <Python.h>
# include <memory>
#endif

#include <Base/Console.h>
#include <Base/VectorPy.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/DocumentObjectPy.h>
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/App/Core/Evaluation.h>

#include <SMESH_Gen.hxx>
#include <SMESH_Group.hxx>
#include <SMESHDS_Mesh.hxx>
#include <SMDS_MeshNode.hxx>
#include <StdMeshers_MaxLength.hxx>
#include <StdMeshers_LocalLength.hxx>
#include <StdMeshers_NumberOfSegments.hxx>
#include <StdMeshers_AutomaticLength.hxx>
#include <StdMeshers_TrianglePreference.hxx>
#include <StdMeshers_MEFISTO_2D.hxx>
#include <StdMeshers_Deflection1D.hxx>
#include <StdMeshers_MaxElementArea.hxx>
#include <StdMeshers_Regular_1D.hxx>
#include <StdMeshers_QuadranglePreference.hxx>
#include <StdMeshers_Quadrangle_2D.hxx>

#include <StdMeshers_LengthFromEdges.hxx>
#include <StdMeshers_NotConformAllowed.hxx>
#include <StdMeshers_Arithmetic1D.hxx>


#include "FemMesh.h"
#include "FemMeshObject.h"
#include "FemMeshPy.h"

#include <cstdlib>

#include <Standard_Real.hxx>

using namespace Fem;


/* module functions */
static PyObject * read(PyObject *self, PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;

    PY_TRY {
        std::auto_ptr<FemMesh> mesh(new FemMesh);
        try {
            mesh->read(Name);
            return new FemMeshPy(mesh.release());
        }
        catch(...) {
            PyErr_SetString(PyExc_Exception, "Loading of mesh was aborted");
            return NULL;
        }
    } PY_CATCH;

    Py_Return;
}

static PyObject * open(PyObject *self, PyObject *args)
{
    const char* Name;
    if (!PyArg_ParseTuple(args, "s",&Name))
        return NULL;

    PY_TRY {
        std::auto_ptr<FemMesh> mesh(new FemMesh);
        mesh->read(Name);
        Base::FileInfo file(Name);
        // create new document and add Import feature
        App::Document *pcDoc = App::GetApplication().newDocument("Unnamed");
        FemMeshObject *pcFeature = static_cast<FemMeshObject *>
            (pcDoc->addObject("Fem::FemMeshObject", file.fileNamePure().c_str()));
        pcFeature->Label.setValue(file.fileNamePure().c_str());
        pcFeature->FemMesh.setValuePtr(mesh.get());
        (void)mesh.release();
        pcFeature->purgeTouched();
    } PY_CATCH;

    Py_Return;
}

static PyObject * importer(PyObject *self, PyObject *args)
{
    const char* Name;
    const char* DocName=0;

    if (!PyArg_ParseTuple(args, "s|s",&Name,&DocName))
        return NULL;

    PY_TRY {
        App::Document *pcDoc = 0;
        if (DocName)
            pcDoc = App::GetApplication().getDocument(DocName);
        else
            pcDoc = App::GetApplication().getActiveDocument();

        if (!pcDoc) {
            pcDoc = App::GetApplication().newDocument(DocName);
        }

        std::auto_ptr<FemMesh> mesh(new FemMesh);
        mesh->read(Name);
        Base::FileInfo file(Name);
        FemMeshObject *pcFeature = static_cast<FemMeshObject *>
            (pcDoc->addObject("Fem::FemMeshObject", file.fileNamePure().c_str()));
        pcFeature->Label.setValue(file.fileNamePure().c_str());
        pcFeature->FemMesh.setValuePtr(mesh.get());
        (void)mesh.release();
        pcFeature->purgeTouched();
    } PY_CATCH;

    Py_Return;
}

static PyObject * import_NASTRAN(PyObject *self, PyObject *args)
{
	const char* filename;
	if (!PyArg_ParseTuple(args, "s",&filename))
		return NULL;

	PY_TRY {
        /* Extract Heading lines */
		//Open the file and perform standard check for file
		std::ifstream inputfile;
		inputfile.open(filename);
		if (!inputfile.is_open())  //Exists...?
		{
			std::cerr << "File not found. Exiting..." << std::endl;
			return NULL;
		}

		//Return the line pointer to the beginning of the file
		inputfile.seekg(std::ifstream::beg);
		std::string line1,line2,temp;
		std::stringstream astream;
		unsigned int nodeid;
		std::vector<unsigned int> tetra_element;
		std::vector<std::vector<unsigned int> > all_elements;
		std::vector<std::vector<unsigned int> >::iterator an_element_iterator;
		all_elements.clear();


		MeshCore::MeshKernel aMesh;
		MeshCore::MeshPointArray vertices;
		
		vertices.clear();
		MeshCore::MeshFacetArray faces;
		faces.clear();
		MeshCore::MeshPoint current_node;
		do
		{
			std::getline(inputfile,line1);
			if (line1.size() == 0) continue;
			if (line1.find("GRID*")!= std::string::npos) //We found a Grid line
			{
				//Now lets extract the GRID Points = Nodes
				//As each GRID Line consists of two subsequent lines we have to
				//take care of that as well
				std::getline(inputfile,line2);
				//Extract X Value
				astream.str(line1.substr(40,56));
				astream >> current_node.x;
				astream.str("");astream.clear();
				//Extract Y Value
				astream.str(line1.substr(56,72));
				astream >> current_node.y;
				astream.str("");astream.clear();
				//Extract Z Value
				astream.str(line2.substr(8,24));
				astream >> current_node.z;
				astream.str("");astream.clear();

				vertices.push_back(current_node);
			}
			else if (line1.find("CTETRA")!= std::string::npos)
			{
				tetra_element.clear();
				//Lets extract the elements
				//As each Element Line consists of two subsequent lines as well 
				//we have to take care of that
				//At a first step we only extract Quadratic Tetrahedral Elements
				std::getline(inputfile,line2);
				astream.str(line1.substr(24,32));
				astream >> nodeid;tetra_element.push_back(nodeid);
				astream.str("");astream.clear();
				astream.str(line1.substr(32,40));
				astream >> nodeid;tetra_element.push_back(nodeid);
				astream.str("");astream.clear();
				astream.str(line1.substr(40,48));
				astream >> nodeid;tetra_element.push_back(nodeid);
				astream.str("");astream.clear();
				astream.str(line1.substr(48,56));
				astream >> nodeid;tetra_element.push_back(nodeid);
				astream.str("");astream.clear();
				astream.str(line1.substr(56,64));
				astream >> nodeid;tetra_element.push_back(nodeid);
				astream.str("");astream.clear();
				astream.str(line1.substr(64,72));
				astream >> nodeid;tetra_element.push_back(nodeid);
				astream.str("");astream.clear();
				astream.str(line2.substr(8,16));
				astream >> nodeid;tetra_element.push_back(nodeid);
				astream.str("");astream.clear();
				astream.str(line2.substr(16,24));
				astream >> nodeid;tetra_element.push_back(nodeid);
				astream.str("");astream.clear();
				astream.str(line2.substr(24,32));
				astream >> nodeid;tetra_element.push_back(nodeid);
				astream.str("");astream.clear();
				astream.str(line2.substr(32,40));
				astream >> nodeid;tetra_element.push_back(nodeid);

				all_elements.push_back(tetra_element);
			}
			
		}
		while (inputfile.good());
		inputfile.close();
		//Now lets perform some minimization routines to min the bbox volume
		//To evaluate the COG and principal axes we have to generate a "Mesh" out of the points
		//therefore at least one face is required
		MeshCore::MeshFacet aFacet;
		aFacet._aulPoints[0] = 0;aFacet._aulPoints[1] = 1;aFacet._aulPoints[2] = 2;
		faces.push_back(aFacet);

		//Try to build up an own mesh kernel within SMESH
		SMESH_Gen* meshgen = new SMESH_Gen();
		SMESH_Mesh* mesh = meshgen->CreateMesh(1, true);
		

		SMESHDS_Mesh* meshds = mesh->GetMeshDS();
		
		meshds->AddNodeWithID(23.0,23.66,11.99,0);
		//(vertices[an_element_iterator->at(0)-1]).x,(vertices[an_element_iterator->at(0)-1]).y,(vertices[an_element_iterator->at(0)-1]).z);
		mesh->ExportDAT("C:/test.dat");	

		/*for (iteriere über Punktliste)
			meshds->AddNodeWithID(x,y,z,id++);

		id=0
			for (iteriere über Drei/Vierecke) {
				if (dreieck)
					meshds->AddFaceWithID(v1,v2,v3,id++)
				else if (viereck)
				meshds->AddFaceWithID(v1,v2,v3,4,id++)
				else
				Weiß nicht, ob das vorkommt
			}*/
	

	    aMesh.Adopt(vertices,faces);
		//First lets get the COG and the principal axes
		MeshCore::MeshEigensystem pca(aMesh);
		pca.Evaluate();
		Base::Matrix4D Trafo =  pca.Transform();
		aMesh.Transform(Trafo);
		MeshCore::MeshKernel MeshatCOG;
		MeshatCOG = aMesh;
		pca.Evaluate();
		Trafo =  pca.Transform();
		Trafo.inverse();
		Base::Vector3f cog;
		cog.x = float(Trafo[0][3]);cog.y = float(Trafo[1][3]);cog.z = float(Trafo[2][3]);
		//Now do Monte Carlo to minimize the BBox of the part
		//Rotate 45°+/- around each axes and choose the settings for the min bbox
		Base::Matrix4D trafo_xaxis,trafo_yaxis,trafo_zaxis;
		double alpha_x=0.0,alpha_y=0.0,alpha_z=0.0,conversion_factor;
		conversion_factor=2*PI/360.0;
		//Fill the rotation matrices
		trafo_xaxis[0][0]=1.0;trafo_xaxis[0][1]=0.0;trafo_xaxis[0][2]=0.0;trafo_xaxis[0][3]=0.0;
		trafo_xaxis[1][0]=0.0;trafo_xaxis[1][1]=cos(alpha_x*conversion_factor);trafo_xaxis[1][2]=(-sin(alpha_x*conversion_factor));trafo_xaxis[1][3]=0.0;
		trafo_xaxis[2][0]=0.0;trafo_xaxis[2][1]=sin(alpha_x*conversion_factor);trafo_xaxis[2][2]=cos(alpha_x*conversion_factor);trafo_xaxis[2][3]=0.0;
		trafo_xaxis[3][0]=0.0;trafo_xaxis[3][1]=0.0;trafo_xaxis[3][2]=0.0;trafo_xaxis[3][3]=1.0;

		trafo_yaxis[0][0]=cos(alpha_y*conversion_factor);trafo_yaxis[0][1]=0.0;trafo_yaxis[0][2]=sin(alpha_y*conversion_factor);trafo_yaxis[0][3]=0.0;
		trafo_yaxis[1][0]=0.0;trafo_yaxis[1][1]=1.0;trafo_yaxis[1][2]=0.0;trafo_yaxis[1][3]=0.0;
		trafo_yaxis[2][0]=(-sin(alpha_y*conversion_factor));trafo_yaxis[2][1]=0.0;trafo_yaxis[2][2]=cos(alpha_y*conversion_factor);trafo_yaxis[2][3]=0.0;
		trafo_yaxis[3][0]=0.0;trafo_yaxis[3][1]=0.0;trafo_yaxis[3][2]=0.0;trafo_yaxis[3][3]=1.0;
		
		trafo_zaxis[0][0]=cos(alpha_z*conversion_factor);trafo_zaxis[0][1]=(-sin(alpha_z*conversion_factor));trafo_zaxis[0][2]=0.0;trafo_zaxis[0][3]=0.0;
		trafo_zaxis[1][0]=sin(alpha_z*conversion_factor);trafo_zaxis[1][1]=cos(alpha_z*conversion_factor);trafo_zaxis[1][2]=0.0;trafo_zaxis[1][3]=0.0;
		trafo_zaxis[2][0]=0.0;trafo_zaxis[2][1]=0.0;trafo_zaxis[2][2]=1.0;trafo_zaxis[2][3]=0.0;
		trafo_zaxis[3][0]=0.0;trafo_zaxis[3][1]=0.0;trafo_zaxis[3][2]=0.0;trafo_zaxis[3][3]=1.0;


		Base::BoundBox3f aBBox,min_bbox;
		unsigned int ix,iy,iz,perfect_ix,perfect_iy,perfect_iz;
		float volumeBBOX,min_volumeBBOX;
		aBBox = aMesh.GetBoundBox();
		//Get the current min_volumeBBOX and look if we find a lower one
		min_volumeBBOX = aBBox.LengthX()*aBBox.LengthY()*aBBox.LengthZ();
		//as we would like to look at all possibilities for the bbox for a rotation around z-axis
		//we start at z=-45°
		alpha_x=-45.0;alpha_y=-45.0;alpha_z=-45.0;
		//Fill the rotation matrices
		trafo_xaxis[0][0]=1.0;trafo_xaxis[0][1]=0.0;trafo_xaxis[0][2]=0.0;trafo_xaxis[0][3]=0.0;
		trafo_xaxis[1][0]=0.0;trafo_xaxis[1][1]=cos(alpha_x*conversion_factor);trafo_xaxis[1][2]=(-sin(alpha_x*conversion_factor));trafo_xaxis[1][3]=0.0;
		trafo_xaxis[2][0]=0.0;trafo_xaxis[2][1]=sin(alpha_x*conversion_factor);trafo_xaxis[2][2]=cos(alpha_x*conversion_factor);trafo_xaxis[2][3]=0.0;
		trafo_xaxis[3][0]=0.0;trafo_xaxis[3][1]=0.0;trafo_xaxis[3][2]=0.0;trafo_xaxis[3][3]=1.0;
		trafo_yaxis[0][0]=cos(alpha_y*conversion_factor);trafo_yaxis[0][1]=0.0;trafo_yaxis[0][2]=sin(alpha_y*conversion_factor);trafo_yaxis[0][3]=0.0;
		trafo_yaxis[1][0]=0.0;trafo_yaxis[1][1]=1.0;trafo_yaxis[1][2]=0.0;trafo_yaxis[1][3]=0.0;
		trafo_yaxis[2][0]=(-sin(alpha_y*conversion_factor));trafo_yaxis[2][1]=0.0;trafo_yaxis[2][2]=cos(alpha_y*conversion_factor);trafo_yaxis[2][3]=0.0;
		trafo_yaxis[3][0]=0.0;trafo_yaxis[3][1]=0.0;trafo_yaxis[3][2]=0.0;trafo_yaxis[3][3]=1.0;
		trafo_zaxis[0][0]=cos(alpha_z*conversion_factor);trafo_zaxis[0][1]=(-sin(alpha_z*conversion_factor));trafo_zaxis[0][2]=0.0;trafo_zaxis[0][3]=0.0;
		trafo_zaxis[1][0]=sin(alpha_z*conversion_factor);trafo_zaxis[1][1]=cos(alpha_z*conversion_factor);trafo_zaxis[1][2]=0.0;trafo_zaxis[1][3]=0.0;
		trafo_zaxis[2][0]=0.0;trafo_zaxis[2][1]=0.0;trafo_zaxis[2][2]=1.0;trafo_zaxis[2][3]=0.0;
		trafo_zaxis[3][0]=0.0;trafo_zaxis[3][1]=0.0;trafo_zaxis[3][2]=0.0;trafo_zaxis[3][3]=1.0;
		//aMesh.Transform(trafo_xaxis);aMesh.Transform(trafo_yaxis);
		aMesh.Transform(trafo_zaxis);
		aBBox = aMesh.GetBoundBox();
		volumeBBOX = aBBox.LengthX()*aBBox.LengthY()*aBBox.LengthZ();
		//Now lets go in 1° Steps and do the rotations
		alpha_z=1.0;
		//Update the rotation matrices
		trafo_zaxis[0][0]=cos(alpha_z*conversion_factor);trafo_zaxis[0][1]=(-sin(alpha_z*conversion_factor));trafo_zaxis[0][2]=0.0;trafo_zaxis[0][3]=0.0;
		trafo_zaxis[1][0]=sin(alpha_z*conversion_factor);trafo_zaxis[1][1]=cos(alpha_z*conversion_factor);trafo_zaxis[1][2]=0.0;trafo_zaxis[1][3]=0.0;
		trafo_zaxis[2][0]=0.0;trafo_zaxis[2][1]=0.0;trafo_zaxis[2][2]=1.0;trafo_zaxis[2][3]=0.0;
		trafo_zaxis[3][0]=0.0;trafo_zaxis[3][1]=0.0;trafo_zaxis[3][2]=0.0;trafo_zaxis[3][3]=1.0;
		ix=1;iy=1;iz=1;
		do 
		{
			aMesh.Transform(trafo_zaxis);
			aBBox = aMesh.GetBoundBox();
			volumeBBOX = aBBox.LengthX()*aBBox.LengthY()*aBBox.LengthZ();
			if (volumeBBOX < min_volumeBBOX)
			{ 
				min_volumeBBOX=volumeBBOX;
				perfect_ix=ix;
				perfect_iy=iy;
				perfect_iz=iz;
			}
			iz++;
		} while (iz<=90);
		//Transform Original Mesh to min BBOX position
		alpha_z = -45.0 + perfect_iz;
		trafo_zaxis[0][0]=cos(alpha_z*conversion_factor);trafo_zaxis[0][1]=(-sin(alpha_z*conversion_factor));trafo_zaxis[0][2]=0.0;trafo_zaxis[0][3]=0.0;
		trafo_zaxis[1][0]=sin(alpha_z*conversion_factor);trafo_zaxis[1][1]=cos(alpha_z*conversion_factor);trafo_zaxis[1][2]=0.0;trafo_zaxis[1][3]=0.0;
		trafo_zaxis[2][0]=0.0;trafo_zaxis[2][1]=0.0;trafo_zaxis[2][2]=1.0;trafo_zaxis[2][3]=0.0;
		trafo_zaxis[3][0]=0.0;trafo_zaxis[3][1]=0.0;trafo_zaxis[3][2]=0.0;trafo_zaxis[3][3]=1.0;
		MeshatCOG.Transform(trafo_zaxis);

		//Move Mesh to stay fully in the positive octant
		aBBox = MeshatCOG.GetBoundBox();
		//Get Distance vector from current lower left corner of BBox to origin
		Base::Vector3f dist_vector;
		dist_vector.x = -aBBox.MinX;dist_vector.y=-aBBox.MinY;dist_vector.z=-aBBox.MinZ;
		Base::Matrix4D trans_matrix;
		trans_matrix[0][0]=1.0;trans_matrix[0][1]=0.0;trans_matrix[0][2]=0.0;trans_matrix[0][3]=dist_vector.x;
		trans_matrix[1][0]=0.0;trans_matrix[1][1]=1.0;trans_matrix[1][2]=0.0;trans_matrix[1][3]=dist_vector.y;
		trans_matrix[2][0]=0.0;trans_matrix[2][1]=0.0;trans_matrix[2][2]=1.0;trans_matrix[2][3]=dist_vector.z;
		trans_matrix[3][0]=0.0;trans_matrix[3][1]=0.0;trans_matrix[3][2]=0.0;trans_matrix[3][3]=1.0;
		MeshatCOG.Transform(trans_matrix);

		

		//Calculate Mesh Volume
		//For an accurate Volume Calculation of a quadratic Tetrahedron
		//we have to calculate the Volume of 8 Sub-Tetrahedrons
		Base::Vector3f a,b,c,a_b_product;
		double volume = 0.0;
		//Calc Volume with 1/6 * |(a x b) * c|
		//for(an_element_iterator = all_elements.begin();an_element_iterator != all_elements.end();an_element_iterator++)
		//{
		//	//1,5,8,7
		//	a = vertices[an_element_iterator->at(4)-1]-vertices[an_element_iterator->at(0)-1];
		//	b = vertices[an_element_iterator->at(7)-1]-vertices[an_element_iterator->at(0)-1];
		//	c = vertices[an_element_iterator->at(6)-1]-vertices[an_element_iterator->at(0)-1];
		//	a_b_product.x = a.y*b.z-b.y*a.z;a_b_product.y = a.z*b.x-b.z*a.x;a_b_product.z = a.x*b.y-b.x*a.y;
		//	volume += 1.0/6.0 * fabs((a_b_product.x * c.x)+ (a_b_product.y * c.y)+(a_b_product.z * c.z));
		//	//5,9,8,7
		//	a = vertices[an_element_iterator->at(8)-1]-vertices[an_element_iterator->at(4)-1];
		//	b = vertices[an_element_iterator->at(7)-1]-vertices[an_element_iterator->at(4)-1];
		//	c = vertices[an_element_iterator->at(6)-1]-vertices[an_element_iterator->at(4)-1];
		//	a_b_product.x = a.y*b.z-b.y*a.z;a_b_product.y = a.z*b.x-b.z*a.x;a_b_product.z = a.x*b.y-b.x*a.y;
		//	volume += 1.0/6.0 * fabs((a_b_product.x * c.x)+ (a_b_product.y * c.y)+(a_b_product.z * c.z));
		//	//5,2,9,7
		//	a = vertices[an_element_iterator->at(1)-1]-vertices[an_element_iterator->at(4)-1];
		//	b = vertices[an_element_iterator->at(8)-1]-vertices[an_element_iterator->at(4)-1];
		//	c = vertices[an_element_iterator->at(6)-1]-vertices[an_element_iterator->at(4)-1];
		//	a_b_product.x = a.y*b.z-b.y*a.z;a_b_product.y = a.z*b.x-b.z*a.x;a_b_product.z = a.x*b.y-b.x*a.y;
		//	volume += 1.0/6.0 * fabs((a_b_product.x * c.x)+ (a_b_product.y * c.y)+(a_b_product.z * c.z));
		//	//2,6,9,7
		//	a = vertices[an_element_iterator->at(5)-1]-vertices[an_element_iterator->at(1)-1];
		//	b = vertices[an_element_iterator->at(8)-1]-vertices[an_element_iterator->at(1)-1];
		//	c = vertices[an_element_iterator->at(6)-1]-vertices[an_element_iterator->at(1)-1];
		//	a_b_product.x = a.y*b.z-b.y*a.z;a_b_product.y = a.z*b.x-b.z*a.x;a_b_product.z = a.x*b.y-b.x*a.y;
		//	volume += 1.0/6.0 * fabs((a_b_product.x * c.x)+ (a_b_product.y * c.y)+(a_b_product.z * c.z));
		//	//9,6,10,7
		//	a = vertices[an_element_iterator->at(5)-1]-vertices[an_element_iterator->at(8)-1];
		//	b = vertices[an_element_iterator->at(9)-1]-vertices[an_element_iterator->at(8)-1];
		//	c = vertices[an_element_iterator->at(6)-1]-vertices[an_element_iterator->at(8)-1];
		//	a_b_product.x = a.y*b.z-b.y*a.z;a_b_product.y = a.z*b.x-b.z*a.x;a_b_product.z = a.x*b.y-b.x*a.y;
		//	volume += 1.0/6.0 * fabs((a_b_product.x * c.x)+ (a_b_product.y * c.y)+(a_b_product.z * c.z));
		//	//6,3,10,7
		//	a = vertices[an_element_iterator->at(2)-1]-vertices[an_element_iterator->at(5)-1];
		//	b = vertices[an_element_iterator->at(9)-1]-vertices[an_element_iterator->at(5)-1];
		//	c = vertices[an_element_iterator->at(6)-1]-vertices[an_element_iterator->at(5)-1];
		//	a_b_product.x = a.y*b.z-b.y*a.z;a_b_product.y = a.z*b.x-b.z*a.x;a_b_product.z = a.x*b.y-b.x*a.y;
		//	volume += 1.0/6.0 * fabs((a_b_product.x * c.x)+ (a_b_product.y * c.y)+(a_b_product.z * c.z));
		//	//8,9,10,7
		//	a = vertices[an_element_iterator->at(8)-1]-vertices[an_element_iterator->at(7)-1];
		//	b = vertices[an_element_iterator->at(9)-1]-vertices[an_element_iterator->at(7)-1];
		//	c = vertices[an_element_iterator->at(6)-1]-vertices[an_element_iterator->at(7)-1];
		//	a_b_product.x = a.y*b.z-b.y*a.z;a_b_product.y = a.z*b.x-b.z*a.x;a_b_product.z = a.x*b.y-b.x*a.y;
		//	volume += 1.0/6.0 * fabs((a_b_product.x * c.x)+ (a_b_product.y * c.y)+(a_b_product.z * c.z));
		//	//8,9,10,4
		//	a = vertices[an_element_iterator->at(8)-1]-vertices[an_element_iterator->at(7)-1];
		//	b = vertices[an_element_iterator->at(9)-1]-vertices[an_element_iterator->at(7)-1];
		//	c = vertices[an_element_iterator->at(3)-1]-vertices[an_element_iterator->at(7)-1];
		//	a_b_product.x = a.y*b.z-b.y*a.z;a_b_product.y = a.z*b.x-b.z*a.x;a_b_product.z = a.x*b.y-b.x*a.y;
		//	volume += 1.0/6.0 * fabs((a_b_product.x * c.x)+ (a_b_product.y * c.y)+(a_b_product.z * c.z));
		//
		//}

		
		
	} PY_CATCH;

	Py_Return;
}


static PyObject * exporter(PyObject *self, PyObject *args)
{
    PyObject* object;
    const char* filename;
    if (!PyArg_ParseTuple(args, "Os",&object,&filename))
        return NULL;

    PY_TRY {
        Py::List list(object);
        Base::Type meshId = Base::Type::fromName("Fem::FemMeshObject");
        for (Py::List::iterator it = list.begin(); it != list.end(); ++it) {
            PyObject* item = (*it).ptr();
            if (PyObject_TypeCheck(item, &(App::DocumentObjectPy::Type))) {
                App::DocumentObject* obj = static_cast<App::DocumentObjectPy*>(item)->getDocumentObjectPtr();
                if (obj->getTypeId().isDerivedFrom(meshId)) {
                    static_cast<FemMeshObject*>(obj)->FemMesh.getValue().write(filename);
                    break;
                }
            }
        }
    } PY_CATCH;

    Py_Return;
}

// ----------------------------------------------------------------------------

PyDoc_STRVAR(open_doc,
"open(string) -- Create a new document and a Mesh::Import feature to load the file into the document.");

PyDoc_STRVAR(inst_doc,
"insert(string|mesh,[string]) -- Load or insert a mesh into the given or active document.");

PyDoc_STRVAR(export_doc,
"export(list,string) -- Export a list of objects into a single file.");

/* registration table  */
struct PyMethodDef Fem_methods[] = {
    {"open"       ,open ,       METH_VARARGS, open_doc},
    {"insert"     ,importer,    METH_VARARGS, inst_doc},
    {"export"     ,exporter,    METH_VARARGS, export_doc},
    {"read"       ,read,        Py_NEWARGS,   "Read a mesh from a file and returns a Mesh object."},
	{"import_NASTRAN" , import_NASTRAN, Py_NEWARGS, "Import a Nastran Geometry Input Deck"},
    {NULL, NULL}  /* sentinel */
};
