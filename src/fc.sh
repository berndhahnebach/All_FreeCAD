#!/bin/bash

# You should have to change just next 2 (uncommented) lines.
OCCDIR=$HOME/software/OCC
export LD_LIBRARY_PATH=$OCCDIR/Linux/lib:$HOME/software/qextmdi.g++2.9/lib:/usr/local/lib:/usr/X11R6/lib
export XERCESCROOT=/usr/local
CASROOT=$OCCDIR
export CSF_MdtvFontDirectory=$OCCDIR/src/FontMFT/
export CSF_MdtvTexturesDirectory=$OCCDIR/src/Textures/
export CSF_UnitsDefinition=$OCCDIR/src/UnitsAPI/Units.dat
export CSF_UnitsLexicon=$OCCDIR/src/UnitsAPI/Lexi_Expr.dat
export CSF_PluginDefaults=$OCCDIR/src/XCAFResources
export CSF_StandardDefaults=$OCCDIR/src/StdResource
export CSF_GraphicShr=$OCCDIR/Linux/lib/libTKOpenGl.so

export TEMP=$HOME/temp

./freecad $1 $2 $3 $4 $5
