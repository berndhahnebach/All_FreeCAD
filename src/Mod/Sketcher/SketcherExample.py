# Example hwo to use the basic sketcher tools
from Sketcher import *
from Part import *
from FreeCAD import *

StartPoint = 0
EndPoint = 1
MiddlePoint = 2

# === low level sketch building & constraint solving ===
# create a constraint sketch
sketch = Sketch()

# create some lines
l1 = Line(Vector(0,0,0),Vector(10,2,0))
l2 = Line(Vector(0,0,0),Vector(2,10,0))

# add the geometry to the sketch
Line1 = sketch.addGeometry(l1)
Line2 = sketch.addGeometry(l2)

# add constraints
sketch.addHorizontalConstraint(Line1,'h')
sketch.addVerticalConstraint(Line2,'v')
# set a coincident constraint between the start points of both lines
sketch.addPointOnPointConstraint(Line1,StartPoint,Line2,StartPoint,'p')

# solve the sketch
sketch.solve()

# get the result
sketch.Geometries[0]
sketch.Geometries[1]
