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


#include "PreCompiled.h"

#include "Swap.h"

using namespace Mesh;

unsigned short Mesh::SwapOrder (void)
{
  unsigned short usDummy = 1;
  return *((char*) &usDummy) == 1 ? LOW_ENDIAN : HIGH_ENDIAN;
}

void Mesh::SwapVar (char&)
{
  return;
}

void Mesh::SwapVar (unsigned char&)
{
  return;
}

void Mesh::SwapVar (short& s)
{
  short sTmp;
  int i;

  for (i = 0; i < (int)sizeof (short); i++)
    *(((char*) &sTmp) + i) = *(((char*) &s) + sizeof (short) - i - 1);
  s = sTmp;
}

void Mesh::SwapVar (unsigned short& s)
{
  short sTmp;
  int i;

  for (i = 0; i < (int)sizeof (short); i++)
    *(((char*) &sTmp) + i) = *(((char*) &s) + sizeof (short) - i - 1);
  s = sTmp;
}

void Mesh::SwapVar (long& l)
{
  long lTmp;
  int i;

  for (i = 0; i < (int)sizeof (long); i++)
    *(((char*) &lTmp) + i) = *(((char*) &l) + sizeof (long) - i - 1);
  l = lTmp;
}

void Mesh::SwapVar (unsigned long& l)
{
  long lTmp;
  int i;

  for (i = 0; i < (int)sizeof (long); i++)
    *(((char*) &lTmp) + i) = *(((char*) &l) + sizeof (long) - i - 1);
  l = lTmp;
}

void Mesh::SwapVar (float& f)
{
  float fTmp;
  int i;

  for (i = 0; i < (int)sizeof (float); i++)
    *(((char*) &fTmp) + i) = *(((char*) &f) + sizeof (float) - i - 1);
  f = fTmp;
}

void Mesh::SwapVar (double& d)
{
  double dTmp;
  int i;

  for (i = 0; i < (int)sizeof (double); i++)
    *(((char*) &dTmp) + i) = *(((char*) &d) + sizeof (double) - i - 1);
  d = dTmp;
}


