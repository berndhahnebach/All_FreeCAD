noinst_LTLIBRARIES=libsketchflat.la

libsketchflat_la_SOURCES = \
		assume.cpp \
		curve.cpp \
		derive.cpp \
		derived.h \
		draw.cpp \
		draw_constraint.cpp \
		draw_sketch.cpp \
		export.cpp \
		expr.cpp \
		expr.h \
		FCsimple.cpp \
		FCtext.cpp \
		FCutil.cpp \
		FCwindow.cpp \
		layer.cpp \
		loadsave.cpp \
		measure.cpp \
		newton.cpp \
		polygon.cpp \
		sketch_constraint.cpp \
		sketch.cpp \
		sketch.h \
		sketchflat.cpp \
		sketchflat.h \
		solve.cpp \
		ttf.cpp \
		ui.h \
		undoredo.cpp \
		util.cpp \
		COPYING.txt \
		README.txt

# set the include path found by configure
CPPFLAGS = -D_REENTRANT
AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/Mod/Sketcher/App \
		-I$(top_builddir)/src -I$(top_builddir)/src/Mod/Sketcher/App $(all_includes)

