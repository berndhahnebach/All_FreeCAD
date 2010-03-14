# Example hwo to use the basic sketcher tools
from Sketcher import *
from Part import *
from FreeCAD import *

# === low level constraint solving ===
# create a constraint sketch
sketch = Sketch()

# create some lines
l1 = Line(Vector(0,0,0),Vector(10,2,0))
#l2 = Line(Vector(0,0,0),Vector(10,0,0))

# add the geometry to the sketch
sketch.addGeometry(l1)
#sketch.addGeometry(l2)