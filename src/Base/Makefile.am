
lib_LTLIBRARIES=libFreeCADBase.la

BUILT_SOURCES=\
		BaseClassPy.cpp \
		BoundBoxPy.cpp \
		PersistancePy.cpp \
		VectorPy.cpp


libFreeCADBase_la_BUILT=\
		BaseClassPy.h \
		BoundBoxPy.h \
		PersistancePy.h \
		VectorPy.h

libFreeCADBase_la_SOURCES=\
		zipios/backbuffer.h \
		zipios/basicentry.cpp \
		zipios/basicentry.h \
		zipios/collcoll.cpp \
		zipios/collcoll.h \
		zipios/deflateoutputstreambuf.cpp \
		zipios/deflateoutputstreambuf.h \
		zipios/dircoll.cpp \
		zipios/dircoll.h \
		zipios/directory.cpp \
		zipios/directory.h \
		zipios/fcoll.cpp \
		zipios/fcoll.h \
		zipios/fcollexceptions.cpp \
		zipios/fcollexceptions.h \
		zipios/fileentry.cpp \
		zipios/fileentry.h \
		zipios/filepath.cpp \
		zipios/filepath.h \
		zipios/filterinputstreambuf.cpp \
		zipios/filterinputstreambuf.h \
		zipios/filteroutputstreambuf.cpp \
		zipios/filteroutputstreambuf.h \
		zipios/gzipoutputstream.cpp \
		zipios/gzipoutputstream.h \
		zipios/gzipoutputstreambuf.cpp \
		zipios/gzipoutputstreambuf.h \
		zipios/inflateinputstreambuf.cpp \
		zipios/inflateinputstreambuf.h \
		zipios/meta-iostreams.h \
		zipios/outputstringstream.h \
		zipios/simplesmartptr.h \
		zipios/virtualseeker.h \
		zipios/zipfile.cpp \
		zipios/zipfile.h \
		zipios/ziphead.cpp \
		zipios/ziphead.h \
		zipios/zipheadio.cpp \
		zipios/zipheadio.h \
		zipios/zipinputstream.cpp \
		zipios/zipinputstream.h \
		zipios/zipinputstreambuf.cpp \
		zipios/zipinputstreambuf.h \
		zipios/zipios_common.h \
		zipios/zipios-config.h \
		zipios/zipios_defs.h \
		zipios/zipoutputstreambuf.cpp \
		zipios/zipoutputstreambuf.h \
		zipios/zipoutputstream.cpp \
		zipios/zipoutputstream.h \
		PyCXX/Config.hxx \
		PyCXX/cxxextensions.c \
		PyCXX/cxx_extensions.cxx \
		PyCXX/cxxsupport.cxx \
		PyCXX/Exception.hxx \
		PyCXX/Extensions.hxx \
		PyCXX/IndirectPythonInterface.cxx \
		PyCXX/IndirectPythonInterface.hxx \
		PyCXX/Objects.hxx \
		BaseClass.cpp \
		BaseClass.h \
		BaseClassPyImp.cpp \
		BoundBoxPyImp.cpp \
		BoundBox.h \
		Builder3D.cpp \
		Builder3D.h \
		Console.cpp \
		Console.h \
		Doxygen.cpp \
		Exception.cpp \
		Exception.h \
		Factory.cpp \
		Factory.h \
		FileInfo.cpp \
		FileInfo.h \
		FileTemplate.cpp \
		FileTemplate.h \
		gzstream.cpp \
		gzstream.h \
		Handle.cpp \
		Handle.h \
		InputSource.cpp \
		InputSource.h \
		Interpreter.cpp \
		Interpreter.h \
		Matrix.cpp \
		Matrix.h \
		MemDebug.cpp \
		MemDebug.h \
		Observer.h \
		Parameter.cpp \
		Parameter.h \
		ParameterPy.cpp \
		Persistance.cpp \
		Persistance.h \
		PersistancePyImp.cpp \
		Placement.cpp \
		Placement.h \
		PreCompiled.cpp \
		PreCompiled.h \
		PyExport.cpp \
		PyExport.h \
		PyObjectBase.cpp \
		PyObjectBase.h \
		PyTools.c \
		PyTools.h \
		Reader.cpp \
		Reader.h \
		Rotation.cpp \
		Rotation.h \
		Sequencer.cpp \
		Sequencer.h \
		Stream.cpp \
		Stream.h \
		Swap.cpp \
		Swap.h \
		swigpyrun_1.3.25.h \
		swigpyrun_1.3.33.h \
		TimeInfo.cpp \
		TimeInfo.h \
		Type.cpp \
		Type.h \
		Tools2D.cpp \
		Tools2D.h \
		VectorPyImp.cpp \
		Vector3D.h \
		ViewProj.h \
		Writer.cpp \
		Writer.h \
		XMLTools.cpp \
		XMLTools.h

%.cpp: %.xml
	$(PYTHON) $(top_srcdir)/src/Tools/generate.py --outputPath $(@D) $<

libFreeCADBase_la_LIBADD   = \
		-l@PYTHON_LIB@ \
		-lboost_filesystem \
		-lxerces-c \
		-lz

# set the include path found by configure
INCLUDES= $(all_includes) -I$(top_srcdir)/src -I$(top_builddir)/src

# set additional defines (for cygwin only)
AM_CPPFLAGS=-DFCBase

CLEANFILES = $(BUILT_SOURCES) $(libFreeCADBase_la_BUILT)

# the library version number
libFreeCADBase_la_LDFLAGS = -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@

EXTRA_DIST = \
		FreeCADBase.vcproj \
		BaseClassPy.xml \
		BoundBoxPy.xml \
		PersistancePy.xml \
		VectorPy.xml

