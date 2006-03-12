###################################################################
### Copyright (c) Berthold Grupp          2005                  ###
###                                                             ###
### Python Module for building solid regular geometric objects  ###
### Return value are list of vectors, 3 vectors define a facet  ###
###                                                             ###
### Samplecode for creating mesh:                               ###
### facets = Cube(3.0, 4.0, 5.0)                                ###
### m = Mesh.newMesh()                                          ###
### m.addFacets(facets)                                         ###
###################################################################

import math

def Sphere (radius, count):
    return Ellipsoid(radius, radius, count)

def Ellipsoid (lenX, lenY, count):
    polyline = []
    step = math.pi / count
    i = 0.0
    while (i < math.pi + step / 10.0):
        x = math.cos(i) * lenX
        y = math.sin(i) * lenY 
        polyline.append([x, y])
        i = i + step

    return RotationBody(polyline, count)    

def Cylinder (radius, len, count):
    return Cone(radius, radius, len, count)

def Cone (radius1, radius2, len, count):
    polyline = ([len, 0.0], [len, radius2], [0.0, radius1], [0.0, 0.0])
    return RotationBody(polyline, count)

def Toroid (radius1, radius2, count):
    polyline = []
    
    step = math.pi * 2.0 / count
    i = -math.pi
    while (i < math.pi +  step / 10.0):
        x = radius1 + math.cos(i) * radius2
        y = radius1 + math.sin(i) * radius2
        polyline.append([x, y])
        i = i + step

    return RotationBody(polyline, count)
         

# build a rotation body from a given (closed) polyline, rotation axis is the X-Axis
# parameter: polyline: list of tuple of 2 floats (2d vector)
def RotationBody (polyline, count):
    facets = []

    step = math.pi * 2.0 / count
    i = -math.pi;
    while (i < math.pi + step / 10.0):
        li = i + step
        for j in range(0, len(polyline) - 1):
            v1 = polyline[j]
            v2 = polyline[j+1]
            
            x1 = v1[0]
            y1 = v1[1] * math.cos(i)
            z1 = v1[1] * math.sin(i)
            x2 = v2[0]
            y2 = v2[1] * math.cos(i)
            z2 = v2[1] * math.sin(i)
            x3 = v1[0]
            y3 = v1[1] * math.cos(li)
            z3 = v1[1] * math.sin(li)
            x4 = v2[0]
            y4 = v2[1] * math.cos(li)
            z4 = v2[1] * math.sin(li)

            if (v1[1] != 0.0):            
                facets.append([x1, y1, z1])
                facets.append([x2, y2, z2])
                facets.append([x3, y3, z3])

            if (v2[1] != 0.0):
                facets.append([x2, y2, z2])
                facets.append([x4, y4, z4])
                facets.append([x3, y3, z3])
            
        i = i + step
        
    return facets;

def Cube (lenX, lenY, lenZ):
    hx = lenX / 2.0
    hy = lenY / 2.0
    hz = lenZ / 2.0
    
    facets = []
    
    facets.append([-hx, -hy, -hz])
    facets.append([hx, -hy, -hz])
    facets.append([hx, -hy, hz])

    facets.append([-hx, -hy, -hz])
    facets.append([hx, -hy, hz])
    facets.append([-hx, -hy, hz])

    facets.append([-hx, hy, -hz])
    facets.append([hx, hy, hz])
    facets.append([hx, hy, -hz])

    facets.append([-hx, hy, -hz])
    facets.append([-hx, hy, hz])
    facets.append([hx, hy, hz])

    facets.append([-hx, -hy, -hz])
    facets.append([-hx, hy, hz])
    facets.append([-hx, hy, -hz])

    facets.append([-hx, -hy, -hz])
    facets.append([-hx, -hy, hz])
    facets.append([-hx, hy, hz])

    facets.append([hx, -hy, -hz])
    facets.append([hx, hy, -hz])
    facets.append([hx, hy, hz])

    facets.append([hx, -hy, -hz])
    facets.append([hx, hy, hz])
    facets.append([hx, -hy, hz])

    facets.append([-hx, -hy, -hz])
    facets.append([-hx, hy, -hz])
    facets.append([hx, hy, -hz])

    facets.append([-hx, -hy, -hz])
    facets.append([hx, hy, -hz])
    facets.append([hx, -hy, -hz])

    facets.append([-hx, -hy, hz])
    facets.append([hx, hy, hz])
    facets.append([-hx, hy, hz])

    facets.append([-hx, -hy, hz])
    facets.append([hx, -hy, hz])
    facets.append([hx, hy, hz])
    
    return facets

def main ():
    Ellipsoid(5.0, 5.0, 10)
    
if __name__ == "__main__":
    main()
        