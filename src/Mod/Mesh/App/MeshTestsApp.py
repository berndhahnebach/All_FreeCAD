#   (c) Juergen Riegel (juergen.riegel@web.de) 2007      LGPL

import FreeCAD, os, unittest,Mesh


#---------------------------------------------------------------------------
# define the functions to test the FreeCAD mesh module
#---------------------------------------------------------------------------

  
class MeshTopoTestCases(unittest.TestCase):
  def setUp(self):
		# set up a planar face with 18 triangles
    self.planarMesh = []
		for x in range(3):
			for y in range(3):
				self.planarMesh.append( [0.0 + x, 0.0 + y,0.0000] ) 
				self.planarMesh.append( [1.0 + x, 1.0 + y,0.0000] )
				self.planarMesh.append( [0.0 + x, 1.0 + y,0.0000] )
				self.planarMesh.append( [0.0 + x, 0.0 + y,0.0000] )
				self.planarMesh.append( [1.0 + x, 0.0 + y,0.0000] )
				self.planarMesh.append( [1.0 + x, 1.0 + y,0.0000] )


  def testCollapseFacetsSingle(self):
		for i in range(18):
			planarMeshObject = Mesh.mesh(planarMesh)
			planarMeshObject.CollapseFacets([i])

  def testCollapseFacetsMultible(self):
		planarMeshObject = Mesh.mesh(planarMesh)
		planarMeshObject.CollapseFacets(range(7))

	def testCollapseFacetsAll(self):
		planarMeshObject = Mesh.mesh(planarMesh)
		planarMeshObject.CollapseFacets(range(18))
			
    
  def tearDown(self):
    #closing doc
    FreeCAD.closeDocument("CreateTest")



      
