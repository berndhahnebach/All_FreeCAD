SUBDIRS=sketchsolve_cp

lib_LTLIBRARIES=libSketcher.la Sketcher.la

BUILT_SOURCES=\
		ConstraintPy.cpp \
		SketchObjectPy.cpp \
		SketchObjectSFPy.cpp

libSketcher_la_BUILT=\
		ConstraintPy.h \
		SketchObjectPy.h \
		SketchObjectSFPy.h

libSketcher_la_SOURCES = \
		sketchflat/assume.cpp \
		sketchflat/constraint.cpp \
		sketchflat/curve.cpp \
		sketchflat/derive.cpp \
		sketchflat/derived.h \
		sketchflat/draw.cpp \
		sketchflat/draw_constraint.cpp \
		sketchflat/draw_sketch.cpp \
		sketchflat/export.cpp \
		sketchflat/expr.cpp \
		sketchflat/expr.h \
		sketchflat/FCsimple.cpp \
		sketchflat/FCtext.cpp \
		sketchflat/FCutil.cpp \
		sketchflat/FCwindow.cpp \
		sketchflat/layer.cpp \
		sketchflat/loadsave.cpp \
		sketchflat/measure.cpp \
		sketchflat/newton.cpp \
		sketchflat/polygon.cpp \
		sketchflat/sketch.cpp \
		sketchflat/sketch.h \
		sketchflat/sketchflat.cpp \
		sketchflat/sketchflat.h \
		sketchflat/solve.cpp \
		sketchflat/ttf.cpp \
		sketchflat/ui.h \
		sketchflat/undoredo.cpp \
		sketchflat/util.cpp \
		sketchflat/COPYING.txt \
		sketchflat/README.txt \
		\
		AppSketcherPy.cpp \
		Constraint.cpp \
		Constraint.h \
		ConstraintPyImp.cpp \
		SketchFlatInterface.cpp \
		SketchFlatInterface.h \
		SketchObject.cpp \
		SketchObject.h \
		SketchObjectPyImp.cpp \
		SketchObjectSF.cpp \
		SketchObjectSF.h \
		SketchObjectSFPyImp.cpp \
		PreCompiled.cpp \
		PreCompiled.h


# the library search path.
libSketcher_la_LDFLAGS = -L../../../Base -L../../../App -L../../../Mod/Part/App $(all_libraries) -L$(OCC_LIB) \
		-version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@
libSketcher_la_CPPFLAGS = -DSketcherAppExport=

libSketcher_la_LIBADD = \
		sketchsolve_cp/libsketchsolve.la \
		@BOOST_REGEX_LIB@ @BOOST_SYSTEM_LIB@ \
		-l@PYTHON_LIB@ \
		-lxerces-c \
		-lFreeCADBase \
		-lFreeCADApp \
		-lPart \
		-lTKernel \
		-lTKG2d \
		-lTKG3d \
		-lTKMath \
		-lTKSTEP \
		-lTKIGES \
		-lTKSTL \
		-lTKShHealing \
		-lTKXSBase \
		-lTKBool \
		-lTKBO \
		-lTKBRep \
		-lTKTopAlgo \
		-lTKGeomAlgo \
		-lTKGeomBase \
		-lTKOffset \
		-lTKPrim \
		-lTKHLR

%.cpp: %.xml $(top_srcdir)/src/Tools/generateTemplates/templateClassPyExport.py
	$(PYTHON) $(top_srcdir)/src/Tools/generate.py --outputPath $(@D) $<

#--------------------------------------------------------------------------------------
# Loader of libSketcher

Sketcher_la_SOURCES=\
		AppSketcher.cpp

# the library search path.
Sketcher_la_LDFLAGS = $(libSketcher_la_LDFLAGS) -module -avoid-version
Sketcher_la_CPPFLAGS = $(libSketcher_la_CPPFLAGS)

Sketcher_la_LIBADD   = \
		$(libSketcher_la_LIBADD) \
		-lSketcher

Sketcher_la_DEPENDENCIES = libSketcher.la

#--------------------------------------------------------------------------------------

# set the include path found by configure
AM_CXXFLAGS = -I$(OCC_INC) -I$(top_srcdir)/src -I$(top_builddir)/src $(all_includes)


libdir = $(prefix)/Mod/Sketcher

CLEANFILES = $(BUILT_SOURCES) $(libSketcher_la_BUILT)

EXTRA_DIST = \
		CMakeLists.txt \
		ConstraintPy.xml \
		SketchObjectPy.xml \
		SketchObjectSFPy.xml

