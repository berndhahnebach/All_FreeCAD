#SUBDIRS=Part Mesh Points Raytracing Image Drawing Complete Draft Test TemplatePyMod
SUBDIRS=Points Robot Complete Draft Test TemplatePyMod

#if HAVE_OPENCV
SUBDIRS += Image
#endif

#if HAVE_GTS
SUBDIRS += Mesh
#endif

if HAVE_OPENCASCADE
SUBDIRS += Part PartDesign Raytracing Drawing Sketcher
endif

#if HAVE_GTS
if HAVE_OPENCASCADE
SUBDIRS += ReverseEngineering MeshPart
endif
#endif

#SUBDIRS += Sandbox

EXTRA_DIST = \
		__init__.py \
		CMakeLists.txt

