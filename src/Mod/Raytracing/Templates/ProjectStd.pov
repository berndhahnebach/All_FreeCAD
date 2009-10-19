// Persistence of Vision Ray Tracer Scene Description File
// for FreeCAD (http://free-cad.sf.net)

#version 3.6;

#include "colors.inc"
#include "metals.inc"

// ----------------------------------------

global_settings {
  assumed_gamma 1.0
}  

// ----------------------------------------


sky_sphere {
  pigment {
    gradient y
    color_map {
      [0.0 rgb <0.6,0.7,1.0>]
      [0.7 rgb <0.0,0.1,0.8>]
    }
  }
}


// ----------------------------------------

plane {
  y, -1
  texture { pigment {rgb <0.0,0.0,0.0>} finish {ambient 1.0 reflection 0.05 specular 0.0} }
}

// Standard finish
#declare StdFinish = F_MetalA;

