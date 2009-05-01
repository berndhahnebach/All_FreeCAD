
#***************************************************************************
#*                                                                         *
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License (GPL)            *
#*   as published by the Free Software Foundation; either version 2 of     *
#*   the License, or (at your option) any later version.                   *
#*   for detail see the LICENCE text file.                                 *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU Library General Public License for more details.                  *
#*                                                                         *
#*   You should have received a copy of the GNU Library General Public     *
#*   License along with this program; if not, write to the Free Software   *
#*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
#*   USA                                                                   *
#*                                                                         *
#***************************************************************************

"a vector math library for FreeCAD"

import math,FreeCAD
from FreeCAD import Vector

PREC = 4	# Precision of comparisions

def tup(first):
	"returns a tuple (x,y,z) with the vector coords"
	if isinstance(first,Vector):
		return (first.x,first.y,first.z)

def new(first, other):
	"new(Vector,Vector) - creates a new vector from first one to second one"
	if isinstance(first,Vector) and isinstance(other,Vector):
		return Vector(other.x-first.x, other.y-first.y, other.z-first.z)

def add(first, other):
	"add(Vector,Vector) - adds two vectors. OBSOLETE - use Vector.add()"
	if isinstance(first,Vector) and isinstance(other,Vector):
		return Vector(first.x+other.x, first.y+other.y, first.z+other.z)

def sub(first, other):
	'''
	sub(Vector,Vector) - subtracts second vector from first one.
	OBSOLETE - use Vector.sub()
	'''
	if isinstance(first,Vector) and isinstance(other,Vector):
		return Vector(first.x-other.x, first.y-other.y, first.z-other.z)

def neg(first):
	"neg(Vector) - returns an opposite (negative) vector"
	if isinstance(first,Vector):
		return Vector(-first.x, -first.y, -first.z)

def equals(first,other):
	'''returns True if both vectors are equal. The important point
	is that we round off a bit (default to 4 digits, which is what the Part module uses).
	'''
	if isinstance(first,Vector) and isinstance(other,Vector):
		if (round(first.x,PREC) == round(other.x,PREC)):
			if (round(first.y,PREC) == round(other.y,PREC)):
				if (round(first.z,PREC) == round(other.z,PREC)):
					return True
		return False

def scale(first,scalar):
	"scale(Vector,Float) - scales (multiplies) a vector by a factor"
	if isinstance(first,Vector):
		return Vector(first.x*scalar, first.y*scalar, first.z*scalar)

def length(first):
	"lengh(Vector) - gives vector length - OBSOLETE! use Vector.Length instead!"
	if isinstance(first,Vector):
		return math.sqrt(first.x*first.x + first.y*first.y + first.z*first.z)

def dist(first, other):
	"dist(Vector,Vector) - returns the distance between both points/vectors"
	if isinstance(first,Vector) and isinstance(other,Vector):
		return math.sqrt((first.x-other.x)*(first.x-other.x) + (first.y-other.y)*(first.y-other.y) + (first.z-other.z)*(first.z-other.z))

def normalized(first):
	"normalized(Vector) - returns a unit vector"
	if isinstance(first,Vector):
		l=length(first)
		if (l!=0):
			return Vector(first.x/l, first.y/l, first.z/l)
		else:
			return 0

def dotproduct(first, other):
	"dotproduct(Vector,Vector) - returns the dot product of both vectors"
	if isinstance(first,Vector) and isinstance(other,Vector):
		return (first.x*other.x + first.y*other.y + first.z*other.z)

def crossproduct(first, other=Vector(0,0,1)):
	'''
	crossproduct(Vector,Vector) - returns the cross product of both vectors.
	If only one is given, cross product is made with vertical axis,
	thus returning its perpendicular in XY plane
	'''
	if isinstance(first,Vector) and isinstance(other,Vector):
		return Vector(first.y*other.z - first.z*other.y, first.z*other.x - first.x*other.z, first.x*other.y - first.y*other.x)

def angle(first, other=Vector(1,0,0)):
	'''angle(Vector,Vector) - returns the angle in radians between the two vectors.
	If only one is given, angle is between the vector and the horizontal East direction
	'''
	if isinstance(first,Vector) and isinstance(other,Vector):
		dp=dotproduct(normalized(first),normalized(other))
		if (dp >= -1) and (dp <= 1):
			angle = math.acos(dotproduct(normalized(first),normalized(other)))
			#angle =  math.atan2(normalized(crossproduct(first,other)),dotproduct(first,other));
			# next line works only for 2D at the moment
			if ((first.x * other.y - first.y * other.x) < 0): angle = -angle
			return angle
		else:
			return 0

def project(first, other):
	"project(Vector,Vector): projects the first vector onto the second one"
	if isinstance(first,Vector) and isinstance(other,Vector):
		return scale(other, dotproduct(first,other)/dotproduct(other,other))

def rotate(first,angle):
	"rotate(Vector,Float): rotates the first Vector around the origin Z axis, at the given angle."
	if isinstance(first,Vector):
		return Vector(math.cos(angle)*first.x-math.sin(angle)*first.y,math.sin(angle)*first.x+math.cos(angle)*first.y,first.z)

def isNull(vector):
	'''isNull(vector): Tests if a vector is nul vector'''
	if round(vector.x,PREC)==0 and round(vector.y,PREC)==0 and round(vector.z,PREC)==0 :
		return True
	else :
		return False
