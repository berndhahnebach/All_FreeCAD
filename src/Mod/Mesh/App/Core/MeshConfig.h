/***************************************************************************
 *   Copyright (c) 2005 Imetric 3D GmbH                                    *
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


#ifndef MESH_CONFIG_H
#define MESH_CONFIG_H

#include "../../../../Config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FC_OS_WIN32
# ifndef __WIN32__
#   define __WIN32__
# endif
#endif

#ifdef FC_OS_LINUX
# ifndef __LINUX__
#   define __LINUX__
# endif
#endif

/*
 * platform-specific-definitions for imports and exports.
 * platform-identification :
 */
#if !defined(__LINUX__) && !defined(__WIN32__) && !defined(__WIN16__)
# error Missing platform-specifier !
#endif

/*
 * general macro definitions
 */
#ifndef  MIN
# define MIN(A,B) ((A < B) ? (A) : (B))
#endif

#ifndef  MAX
# define MAX(A,B) ((A > B) ? (A) : (B))
#endif

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif


#ifdef __cplusplus
}
#endif

/*
 * general constant definitions
 */
#define FLOAT_EPS   1.0e-4f 

#ifndef  F_PI
# define F_PI  3.1415926f
#endif

#ifndef  D_PI
# define D_PI  3.141592653589793
#endif
  
#ifndef  FLOAT_MAX
# define FLOAT_MAX 1e30f
#endif

#ifndef  DOUBLE_MAX
# define DOUBLE_MAX 1.7976931348623157E+308    /* max decimal value of a "double"*/
#endif

#ifndef  DOUBLE_MIN
# define DOUBLE_MIN 2.2250738585072014E-308    /* min decimal value of a "double"*/
#endif

/*
 * macros to convert between angles
 */
#define RAD(D)    ((D) * D_PI / 180.0)
#define DEGREE(R) ((R) * 180.0 / D_PI) 

#endif  /* MESH_CONFIG_H  */ 

