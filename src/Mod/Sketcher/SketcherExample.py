# Example hwo to use the basic sketcher tools
from Sketcher import *
from Part import *
from FreeCAD import *

if(App.activeDocument() == None):App.newDocument()

f = App.activeDocument().addObject("Sketcher::SketchObject","Sketch")

f.Geometry = [Line(Vector(0,0,0),Vector(2,20,0)),Line(Vector(0,0,0),Vector(20,2,0))]

f.Constraints = [Constraint('Vertical',3),Constraint('Horizontal',4)]

App.activeDocument().recompute()

f.Geometry


