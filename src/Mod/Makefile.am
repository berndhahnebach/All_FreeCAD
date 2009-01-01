#SUBDIRS=Part Mesh Points Raytracing Image Drawing Complete Draft Test TemplatePyMod
SUBDIRS=Points Image Complete Draft Test TemplatePyMod

if GTS_LIBRARY_FILES
SUBDIRS += Mesh
endif

if OCC_LIBRARY_FILES
SUBDIRS += Part Raytracing Drawing
endif

if GTS_LIBRARY_FILES
if OCC_LIBRARY_FILES
SUBDIRS += ReverseEngineering
endif
endif

EXTRA_DIST = \
		__init__.py

