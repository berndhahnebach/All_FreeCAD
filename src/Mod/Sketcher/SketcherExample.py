# Example hwo to use the basic sketcher tools
from Sketcher import *
from Part import *
from FreeCAD import *

# === low level constraint solving ===
# create a constraint sketch
sketch = Sketch()

# create some lines
l1 = Line(Vector(0,0,0),Vector(10,2,0))

# add the geometry to the sketch
sketch.addGeometry(l1)

# add constraints
sketch.addHorizontalConstraint(0,'h')

# solve the sketch
sketch.solve()

# get the result
sketch.Geometries[0].StartPoint
sketch.Geometries[0].EndPoint
