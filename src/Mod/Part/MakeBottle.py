import Part, math
from FreeCAD import Base

def makeBottle(myWidth, myHeight, myThickness):
	aPnt1=Base.Vector(-myWidth/2.,0,0)
	aPnt2=Base.Vector(-myWidth/2.,-myThickness/4.,0)
	aPnt3=Base.Vector(0,-myThickness/2.,0)
	aPnt4=Base.Vector(myWidth/2.,-myThickness/4.,0)
	aPnt5=Base.Vector(myWidth/2.,0,0)
	aPrismVec=Base.Vector(0,0,myHeight)
	
	aArcOfCircle = Part.Arc(Part.Circle,aPnt2,aPnt3,aPnt4)
	aSegment1=Part.Line(aPnt1,aPnt2)
	aSegment2=Part.Line(aPnt4,aPnt5)
	
	aEdge1=aSegment1.toShape()
	aEdge2=aArcOfCircle.toShape()
	aEdge3=aSegment2.toShape()
	aWire=Part.Wire(aEdge1,aEdge2,aEdge3)
	
	aTrsf=Base.Matrix()
	aTrsf.rotateX(math.pi)
	
	aMirroredWire=aWire.transform(aTrsf)
	myWireProfile=Part.Wire([aWire,aMirroredWire])
	
	myFaceProfile=Part.Face(myWireProfile)
	myBody=Part.makePrism(myFaceProfile,aPrismVec)
	
	return myBody

