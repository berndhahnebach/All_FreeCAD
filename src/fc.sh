#!/bin/bash
# You should have to change just next 2 (uncommented) lines.
OCCDIR=$HOME/OCC
LD_LIBRARY_PATH=$OCCDIR/Linux/lib:$HOME/software/qextmdi/lib
CASROOT=$OCCDIR
export CSF_MdtvFontDirectory=$OCCDIR/src/FontMFT/
export CSF_MdtvTexturesDirectory=$OCCDIR/src/Textures/
export CSF_UnitsDefinition=$OCCDIR/src/UnitsAPI/Units.dat
export CSF_UnitsLexicon=$OCCDIR/src/UnitsAPI/Lexi_Expr.dat
export CSF_PluginDefaults=$OCCDIR/src/XCAFResources
export CSF_StandardDefaults=$OCCDIR/src/StdResource
export CSF_GraphicShr=$OCCDIR/Linux/lib/libTKOpenGl.so
./freecad
