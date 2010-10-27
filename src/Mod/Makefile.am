#SUBDIRS=Part Mesh Points Raytracing Image Drawing Complete Draft Test TemplatePyMod
SUBDIRS=Points Complete Draft Test TemplatePyMod Web

#if HAVE_OPENCV
SUBDIRS += Image
#endif

#if HAVE_GTS
SUBDIRS += Mesh
#endif

if HAVE_OPENCASCADE
SUBDIRS += Part PartDesign Raytracing Drawing Sketcher
endif

if HAVE_EIGEN2
if HAVE_OPENCASCADE
SUBDIRS += Robot
endif
endif

#if HAVE_GTS
if HAVE_OPENCASCADE
SUBDIRS += ReverseEngineering MeshPart Fem
endif
#endif

#SUBDIRS += Sandbox

EXTRA_DIST = \
		__init__.py \
		CMakeLists.txt \
		mod.dox

