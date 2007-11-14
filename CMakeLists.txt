# cMake file for the FreeCAD build
# (c) 2007 Jürgen Riegel LGPL

PROJECT(FreeCADcmake)

# Recurse into the src subdirectories.  This does not actually
# cause another cmake executable to run.  The same process will walk through
# the project's entire directory structure.
SUBDIRS(src)
