#   (c) Juergen Riegel (juergen.riegel@web.de) 2010      LGPL

import os, sys, unittest

from Sketcher import *
from Part import *
from FreeCAD import *

# set some constances for the constraints
StartPoint = 1
EndPoint = 2
MiddlePoint = 3


#---------------------------------------------------------------------------
# define the functions to test the FreeCAD Sketcher module
#---------------------------------------------------------------------------


class MeshTopoTestCases(unittest.TestCase):
	def setUp(self):
		# set up a planar face with 18 triangles
		self.doc = newDocument("Sketcher_Test")
		self.sketch = self.doc.addObject("Sketcher::SketchObject","Sketch")

	def testCreateGeom(self):
		# add geometry to the sketch
		self.sketch.Geometry = [Line(Vector(0,0,0),Vector(2,20,0)),Line(Vector(0,0,0),Vector(20,2,0))]
		
		# add constraints to the sketch
		self.sketch.Constraints = [Constraint('Vertical',0),Constraint('Horizontal',1)]
		
		# recompute (solving) the sketch
		self.doc.recompute()
		# add another constraint to tie the start points together
		l = self.sketch.Constraints
		l.append(Constraint('Coincident',0,StartPoint,1,StartPoint))
		self.sketch.Constraints = l
		
		# again recompute
		self.doc.recompute()

	def tearDown(self):
		#closing doc
		closeDocument("Sketcher_Test")


