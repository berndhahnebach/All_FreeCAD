/***************************************************************************
 *   Copyright (c) 2005 Werner Mayer <werner.wm.mayer@gmx.de>              *
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
#ifndef _PreComp_
# include <fcntl.h>
# include <TFunction_Logbook.hxx>
# include <ios>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Sequencer.h>
#include "FeatureMeshCurvature.h"
#include "MeshCurvature.h"
#include "MeshAlgos.h"

#include "Core/MeshIO.h"


using namespace Mesh;
using namespace MeshCore;

void FeatureMeshCurvature::initFeature(void)
{
  Base::Console().Log("FeatureMeshImport::InitLabel()\n");
  addProperty("Link","Source");
}

int FeatureMeshCurvature::execute(TFunction_Logbook& log)
{
  MeshFeature *pcFeat  = dynamic_cast<MeshFeature*>(getPropertyLink("Source"));
  if(!pcFeat || pcFeat->getStatus() != Valid)
    return 1;

  MeshWithProperty& rMesh = pcFeat->getMesh();
  MeshAlgos::calcVertexCurvature( &rMesh );

  MeshPropertyCurvature *prop = dynamic_cast<MeshPropertyCurvature*>(rMesh.Get("VertexCurvature") );
  if( prop && prop->isValid() )
  {
    std::map<MeshPropertyCurvature::Mode, std::string> modi;
    modi[MeshPropertyCurvature::MaxCurvature] = "Max. curvature";
    modi[MeshPropertyCurvature::MinCurvature] = "Min. curvature";
    modi[MeshPropertyCurvature::AvgCurvature] = "Avg. curvature";
    modi[MeshPropertyCurvature::GaussCurvature] = "Gaussian curvature";

    for ( std::map<MeshPropertyCurvature::Mode, std::string>::iterator mI = modi.begin(); mI != modi.end(); ++mI)
    {
      const char* mode = mI->second.c_str();
      printf("Mode: %s\n", mode);
      MeshPropertyColor* color = dynamic_cast<MeshPropertyColor*>(rMesh.Get( mode ) );
      if ( !color )
      {
        color = new MeshPropertyColor();
        rMesh.Add( color, mode );
      }

      std::vector<float> aValues = prop->getCurvature( mI->first );
      if ( aValues.empty() ) 
        return 1; // no values inside
      float fMin = *std::min_element( aValues.begin(), aValues.end() );/*-1.0f*/;
      float fMax = *std::max_element( aValues.begin(), aValues.end() );/* 1.0f*/;

      // histogram over all values
      std::map<int, int> aHistogram;
      for ( std::vector<float>::iterator it2 = aValues.begin(); it2 != aValues.end(); ++it2 )
      {
        int grp = (int)(10.0f*( *it2 - fMin )/( fMax - fMin ));
        aHistogram[grp]++;
      }

      float fRMin;
      for ( std::map<int, int>::iterator mIt = aHistogram.begin(); mIt != aHistogram.end(); ++mIt )
      {
        if ( (float)mIt->second / (float)aValues.size() > 0.05f )
        {
          fRMin = mIt->first * ( fMax - fMin )/10.0f + fMin;
          break;
        }
      }

      float fRMax;
      for ( std::map<int, int>::reverse_iterator rIt = aHistogram.rbegin(); rIt != aHistogram.rend(); ++rIt )
      {
        if ( (float)rIt->second / (float)aValues.size() > 0.05f )
        {
          fRMax = rIt->first * ( fMax - fMin )/10.0f + fMin;
          break;
        }
      }

      float fAbs = std::max<float>(fabs(fRMin), fabs(fRMax));
      fRMin = -fAbs;
      fRMax =  fAbs;
      Base::Console().Message("Type: %s: Min: %.2f, Max: %.2f, Range:[%.2f, %.2f]\n", mode, fMin, fMax, fRMin, fRMax);
      fMin = fRMin; fMax = fRMax;

      color->Color.clear();
      for ( std::vector<float>::iterator it = aValues.begin(); it != aValues.end(); ++it )
      {
        MeshPropertyColor::fColor fCol;
        float x = *it;

        // Lookup table (linear interpolating)
        if ( x < fMin )
        { fCol.r = 0.0f; fCol.g = 0.0f; fCol.b = 1.0f; }
        else if ( x < 0.75f*fMin+0.25f*fMax )
        { float s = 4.0f*(x-fMin)/(fMax-fMin);
          fCol.r = 0.0f; fCol.g = s; fCol.b = 1.0f; }
        else if ( x < 0.5f*fMin+0.5f*fMax )
        { float s = (4.0f*x-3.0f*fMin-fMax)/(fMax-fMin);
          fCol.r = 0.0f; fCol.g = 1.0f; fCol.b = 1.0f-s; }
        else if ( x < 0.25f*fMin+0.75f*fMax )
        { float s = (4.0f*x-2.0f*(fMin+fMax))/(fMax-fMin);
          fCol.r = s; fCol.g = 1.0f; fCol.b = 0.0f; }
        else if ( x < fMax )
        { float s = (4.0f*x-fMin-3.0f*fMax)/(fMax-fMin);
          fCol.r = 1.0f; fCol.g = 1.0f-s; fCol.b = 0.0f; }
        else
        { fCol.r = 1.0f; fCol.g = 0.0f; fCol.b = 0.0f; }
        color->Color.push_back( fCol );
      }
    }
  }
  else if ( prop )
  {
    Base::Console().Warning("Invalid property 'VertexCurvature' found.\n");
  }
  else
  {
    Base::Console().Warning("Property 'VertexCurvature' not found.\n");
  }

  return 0;
}

MeshWithProperty& FeatureMeshCurvature::getMesh()
{
  MeshFeature *pcFeat  = dynamic_cast<MeshFeature*>(getPropertyLink("Source"));
  if(!pcFeat || pcFeat->getStatus() != Valid)
    return MeshFeature::getMesh();

  return pcFeat->getMesh();
}
